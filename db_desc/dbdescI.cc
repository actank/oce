#include <boost/lexical_cast.hpp>

#include "base/common.h"
#include "base/logging.h"
#include "base/asyncall.h"
#include "arch/service.h"

#include <mysql++/mysql++.h>
//#include <ssqls.h>

#include "dbdesc/dbdescI.h"

// ugly namespace
using namespace xce;
using xce::LogMessage;

namespace com { namespace xiaonei { namespace xce {

DbDescriptorI::DbDescriptorI() {
    init_db_ = new DbServer;
    Init();
}

void DbDescriptorI::Init() {
  if (Service::Adapter())
    ApplyConfig(Service::Adapter()->getCommunicator()->getProperties());

  reload();
}

void DbDescriptorI::ApplyConfig(const Ice::PropertiesPtr& props) {
  // TODO: user servo_name_
  init_db_->database = props->getPropertyWithDefault("Server.init_database", "beijing");
  init_db_->host = props->getPropertyWithDefault("Server.init_host", "10.2.18.202");
  init_db_->user = props->getPropertyWithDefault("Server.init_user", "test");
  init_db_->password = props->getPropertyWithDefault("Server.init_password", "test");
  init_db_->port = boost::lexical_cast<int>(props->getPropertyWithDefault("Server.int_port", "3306"));
}

static void throw_error(const std::string &err) {
    com::xiaonei::xce::XceException le(err);
    throw (le);
}

// 有类型为容器的成员，故不能使用编译器生成==
static bool IsEqual(const DbInstance &a, const DbInstance &b) {

#define CHECKPART(name) { if (a.name != b.name)  \
  return false; }

  //assert(a.name == b.name);
  CHECKPART(timestamp)
  CHECKPART(type);

  CHECKPART(rservers.size());
  CHECKPART(routes.size());

  if (a.wserver) {
    CHECKPART(wserver->type);
    CHECKPART(wserver->database);
    CHECKPART(wserver->host);
    CHECKPART(wserver->port);
    CHECKPART(wserver->charset);
    CHECKPART(wserver->user);
    CHECKPART(wserver->password);
  }

  for (size_t i = 0; i < a.rservers.size(); ++i) {
    CHECKPART(rservers[i]->type);
    CHECKPART(rservers[i]->database);
    CHECKPART(rservers[i]->host);
    CHECKPART(rservers[i]->port);
    CHECKPART(rservers[i]->charset);
    CHECKPART(rservers[i]->user);
    CHECKPART(rservers[i]->password);
  }
  for (size_t i = 0; i < a.routes.size(); ++i) {
    CHECKPART(routes[i]->expression);
    CHECKPART(routes[i]->instance);
  }
  return true;
#undef CHECKPART
}

bool DbDescriptorI::IsEqualInstance(const DbInstancePtr &instance) {
  ASSERT(instance);

  boost::mutex::scoped_lock lock(guard_);

  DbInstancePtr mem_inst;
  DbInstanceMap::iterator pos = dbs_.find(instance->name);
  if (pos != dbs_.end()) {
    mem_inst = pos->second;
  }

  if (mem_inst.get() == 0)
    return false;

  return IsEqual(*instance, *mem_inst);
}

void BuildDbInstanceRouter(mysqlpp::Connection& mysql_, DbInstancePtr instance) {
  mysqlpp::Query query = mysql_.query();
  query << "select * from route where instance_name = " << mysqlpp::quote << instance->name;
  mysqlpp::StoreQueryResult res = query.store();
  if (res) {
    for (size_t i = 0; i < res.num_rows(); ++i) {
      RoutePtr route = new Route;
      route->expression = res[i]["expression"].c_str();
      route->instance= res[i]["instance"].c_str();
      instance->routes.push_back(route);
    }
  } else
      throw_error ("database query route table fail!");
}

void BuildDbInstanceSingler(mysqlpp::Connection& mysql_, DbInstancePtr instance) {
    mysqlpp::Query query = mysql_.query();
    query << "select * from server where instance_name = " << mysqlpp::quote << instance->name;
    mysqlpp::StoreQueryResult res = query.store();
    if (res) {
      for (size_t i = 0; i < res.num_rows(); ++i) {
        DbServerPtr serv = new DbServer;
        serv->type = res[i]["type"].c_str();
        serv->database= res[i]["database"].c_str();
        serv->host= res[i]["host"].c_str();
        serv->port= boost::lexical_cast<int>(res[i]["port"]);
        serv->user= res[i]["user"].c_str();
        serv->password= res[i]["password"].c_str();
        serv->charset= res[i]["charset"].c_str();
        std::string wrflag= res[i]["wrflag"].c_str();
        int flag;
        try {
          flag = boost::lexical_cast<int>(wrflag);
        } catch (...) {
          if (wrflag == "w" || wrflag == "W") {
            flag = CDbWServer;
          } else if (wrflag == "r" || wrflag == "R") {
            flag = CDbRServer;
          } else if (wrflag == "rw" || wrflag == "wr" || wrflag == "RW"
            || wrflag == "WR" || wrflag == "Rw" || wrflag == "Wr") {
              flag = CDbWRServer;
          }
        }

        if (flag & CDbWServer) {
          instance->wserver = serv;
        }
        if (flag & CDbRServer) {
          instance->rservers.push_back(serv);
        }
      }
    } else
        throw_error ("database query singler table fail!");
}

void DbDescriptorI::reload(const Ice::Current&) {
  // debug add
  std::cout << "Begin:reload()" << std::endl;
#if 0
  DbInstancePtr inst = new DbInstance;

  inst->name = "common";
  inst->type = "singler";
  inst->timestamp = 1033;
    DbServerPtr serv = new DbServer;
    serv->type = "mysql";
    serv->database= "fb";
    serv->host= "dbcore1";
    serv->port= 1036;
    serv->user= "lqh";
    serv->password= "xxx";
    serv->charset= "utf-8";
    int flag;
    flag = CDbWServer;
    //flag = CDbRServer;

    if (flag & CDbWServer) {
      inst->wserver = serv;
    }
    if (flag & CDbRServer) {
      inst->rservers.push_back(serv);
    }
  Notify("common", inst);
  return;
#endif

  mysqlpp::Connection mysql_(false);
  {
    // 1 访问 init_db_ 是需要加锁的
    boost::mutex::scoped_lock lock(guard_);
    //try {
    if (!mysql_.connect(init_db_->database.c_str(),
            init_db_->host.c_str(),
            init_db_->user.c_str(),
            init_db_->password.c_str(),
            init_db_->port)) {
        // 数据库连接失败，服务就应该直接退出
        // TODO:
        throw_error ("database connect fail!");
        return;
    } 
  }

  std::string instance_str = "select * from instance";
  mysqlpp::Query query = mysql_.query(instance_str);
  mysqlpp::StoreQueryResult res = query.store();
  if (res) {
    for (size_t i = 0; i < res.num_rows(); ++i) {
      DbInstancePtr instance = new DbInstance;

      instance->name = res[i]["name"].c_str();
      instance->type = res[i]["type"].c_str();
      instance->timestamp = boost::lexical_cast<long>(res[i]["timestamp"]);
      if (instance->type == CDbInstanceRouter) {
        BuildDbInstanceRouter(mysql_, instance);
      } else {
        BuildDbInstanceSingler(mysql_, instance);
      }
      if (IsEqualInstance(instance))
          continue;
      // TODO: 旧版代码重新赋值了，不太明白
      // instance->timestamp = time(NULL);

      {
        // 2 赋值也需要锁
         boost::mutex::scoped_lock lock(guard_);
         dbs_[instance->name] = instance;
      }
      std::cout << "Notify:reload():" << instance->name << std::endl;
      Notify(instance->name, instance);
    }
  } else
      throw_error ("database query instance table fail!");
}

void NotifyToProxy(DbObserverPrx ob, DbInstancePtr db) {
  std::cout << "UpdateToProxy" << db->name << std::endl;
  ob->updated(db);
}

void DbDescriptorI::Notify(std::string const& name, DbInstancePtr db) {
  observer_set to;
  {
    boost::mutex::scoped_lock lock(guard_);
    to = observers_[name];

    // 支持订阅 *
    observer_set star = observers_["*"];
    to.insert(star.begin(), star.end());
  }

  // 重试3次，每次间隔 60秒
  for (observer_set::const_iterator i=to.begin(); i!=to.end(); ++i) {
    SurePost(boost::bind(NotifyToProxy, *i, db), 60 * 1000, 3);
  }
}

DbInstanceSeq DbDescriptorI::getDbInstances(const Ice::Current&) {
  LOG_F(VERBOSE) << "";
  DbInstanceSeq seq;
  return seq;
}

DbInstancePtr DbDescriptorI::getDbInstance(const std::string& name, const Ice::Current&) {
  LOG_F(VERBOSE) << name;
  boost::mutex::scoped_lock lock(guard_);
  DbInstanceMap::const_iterator i = dbs_.find(name);
  if (dbs_.end() != i)
    return i->second;
  return DbInstancePtr();
}

void DbDescriptorI::subscribe(const std::string& name, const DbObserverPrx& prx, const Ice::Current&) {
  LOG_F(VERBOSE) << name << " prx:" << prx;
  // TODO: prx is valid?
  boost::mutex::scoped_lock lock(guard_);
  observers_[name].insert(prx);
}

void DbDescriptorI::unsubscribe(const std::string& name, const DbObserverPrx& prx, const Ice::Current&) {
  LOG_F(VERBOSE) << name << " prx:" << prx;
  boost::mutex::scoped_lock lock(guard_);

  observer_by_name::iterator i = observers_.find(name);
  if (observers_.end() == i)
    return;

  i->second.erase(prx);
}

DbObserverMap
DbDescriptorI::query(const std::string& name, const Ice::Current&) {
  LOG_F(VERBOSE) << name;
  DbObserverMap result;
  return result;
}

bool IsEmpty(const DbServerPtr& ds) {
  return !ds || ds->host.empty() || ds->database.empty();
}

bool DbInstanceAdd(DbInstancePtr& con, const DbInstancePtr& db) {
  if (!IsEmpty(db->wserver))
    con->wserver = db->wserver;

  if (!db->rservers.empty()) {
    con->rservers.insert(con->rservers.end()
      , db->rservers.begin(), db->rservers.end());
  }

  if (!db->routes.empty()) {
    con->routes.insert(con->routes.end()
      , db->routes.begin(), db->routes.end());
  }
  return true;
}

bool DbInstanceRemove(DbInstancePtr& con, const DbInstancePtr& db) {
  // 删除一样的
  for(DbServerSeq::const_iterator i=db->rservers.begin(); i!=db->rservers.end(); ++i) {
    for(DbServerSeq::iterator j=con->rservers.begin(); j!=con->rservers.end(); ++j)
      if (*j == *i) {
        con->rservers.erase(j);
        break;
      }
  }

  for(RouteSeq::const_iterator i=db->routes.begin(); i!=db->routes.end(); ++i) {
    for(RouteSeq::iterator j=con->routes.begin(); j!=con->routes.end(); ++j)
      if (*j == *i) {
        con->routes.erase(j);
        break;
      }
  }

  return true;
}

// 2
bool DbDescriptorI::update(const std::string& name, const std::string& action
    , const DbInstancePtr& db, const Ice::Current&) {
  LOG_F(VERBOSE) << name << " " << action;
  if ("add" == action) {
    // TODO: check db valid
    boost::mutex::scoped_lock lock(guard_);
    DbInstanceAdd(dbs_[name], db);
  }
  else if ("remove" == action) {
    boost::mutex::scoped_lock lock(guard_);
    DbInstanceRemove(dbs_[name], db);
  }
  else if ("update" == action) {
    boost::mutex::scoped_lock lock(guard_);
    dbs_[name] = db;
  }
  return true;
}

} } }

BEGIN_SERVANT_MAP("DbDescriptor")
  SERVANT_ENTRY(DbDescriptor, &xce::ServantFactory<com::xiaonei::xce::DbDescriptorI>::CreateServant
    , &xce::ServantFactory<com::xiaonei::xce::DbDescriptorI>::Destroy)
END_SERVANT_MAP()

