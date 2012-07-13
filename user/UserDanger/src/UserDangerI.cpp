#include <XceStormAdapter.h>
#include "UserDangerI.h"
#include <util/cpp/TimeCost.h>
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include <boost/lexical_cast.hpp>
#include "Monitor/client/service/ServiceMonitor.h"
#include "util/cpp/InvokeMonitor.h"

using xce::storm::XceStormAdapter;
using boost::lexical_cast;
using MyUtil::Str2StrMap;
using MyUtil::InvokeClient;
using xce::userdanger::UserDangerManagerI;
using xce::userdanger::UserDangerResultHandler;
using xce::userdanger::Int2Bool;
using xce::userdanger::IntSeq;
using xce::userdanger::Int2IntSeq;
using MyUtil::TimeCost;
using xce::serverstate::ServerStateSubscriber;
using com::xiaonei::xce::Statement;
using com::xiaonei::xce::CDbRServer;
using com::xiaonei::xce::CDbWServer;
using com::xiaonei::xce::QueryRunner;
using xce::userdanger::DangerSet;
using xce::userdanger::DangerSetPtr;
using xce::userdanger::FIELD_USERID;
using xce::userdanger::FIELD_TYPE;
using xce::userdanger::BitSet;
using xce::userdanger::RBSet;
using xce::userdanger::RBSetPtr;
using xce::userdanger::SetPtr;
using xce::userdanger::Updater;
using xce::userdanger::UpdateTask;
using MyUtil::TaskManager;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&UserDangerManagerI::instance(), service.createIdentity("M", ""));
  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("UserDanger.Mod");
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserDanger.Interval", 5);
  ServerStateSubscriber::instance().initialize("ControllerUserDanger", &UserDangerManagerI::instance(), mod, interval, new MyUtil::OceChannel, "5000", "5000", "1048576");
  MyUtil::ServiceMonitorManager::instance().start();
  XceStormAdapter::instance().subscribe("USER_DANGER_UPDATER", new Updater);
  TaskManager::instance().config(10, MyUtil::ThreadPoolConfig(1, 1));
  UserDangerManagerI::instance().setValid(true);
}

bool Updater::update(const MyUtil::Str2StrMap& context, const Ice::Current& e) {
  InvokeClient ic = InvokeClient::create(e);
  TaskManager::instance().execute(new UpdateTask(context));
  return true;
}

void UpdateTask::handle() {
  int id = boost::lexical_cast<int>(context_["ID"]);
  int type = boost::lexical_cast<int>(context_["TYPE"]);
  int operation = boost::lexical_cast<bool>(context_["OPERATION"]);
  UserDangerManagerI::instance().update(id, type, operation);
}

BitSet::BitSet() {
  for (int i=0;i<STATUS_CACHE_SIZE;++i) {
    _set.reset(i);
  }
}

void BitSet::add(int id) {
  _set.set(id);
}

void BitSet::remove(int id) {
  _set.reset(id);
}

bool BitSet::has(int id) const{
  return _set[id];
}

int BitSet::size() const{
  return _set.count();
}

void RBSet::add(int id) {
  _set.insert(id);
}

void RBSet::remove(int id) {
  _set.erase(id);
}

bool RBSet::has(int id) const{
  return _set.count(id) <= 0 ? false : true;
}

int RBSet::size() const{
  return _set.size();
}

void UserDangerManagerI::setTypeOn(int userid, int type, const Ice::Current& current) {
  ostringstream msg;
  msg << "userid: " << userid << " type: " << type;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  Statement sql;
  sql << "INSERT IGNORE INTO " << TABLE_USER_DANGER << " ( " << FIELD_USERID << " , " << FIELD_TYPE << " ) VALUES ( " << userid << " , " << type << " )";
  QueryRunner(DB_USER_DANGER, CDbWServer).execute(sql);
  Str2StrMap m;
  m["ID"] = boost::lexical_cast<string>(userid);
  m["TYPE"] = boost::lexical_cast<string>(type);
  m["OPERATION"] = boost::lexical_cast<string>(true);
  XceStormAdapter::instance().notify("USER_DANGER_UPDATER", m);
}

void UserDangerManagerI::update(int id, int type, bool operation) {
  MCE_INFO("UserDangerManagerI::update id: " << id << " type: " <<type << " operation: " << operation);
  DangerSetPtr dsp = getDangerSet(type);
  if (!dsp) {
    DangerSetPtr ods=NULL;
    dsp = setDangerSet(type, new DangerSet(type), ods);
  }
  if (operation) {
    dsp->setOn(id);
  } else {
    dsp->setOff(id);
  }
}

IntSeq UserDangerManagerI::getUserIds(int type, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current, "type: " + type, InvokeClient::INVOKE_LEVEL_INFO);
  DangerSetPtr dsp = getDangerSet(type);
  return dsp->getIds();
}

void UserDangerManagerI::setTypeOff(int userid, int type, const Ice::Current& current) {
  ostringstream msg;
  msg << "userid: " << userid << " type: " << type;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  Statement sql;
  sql << "DELETE FROM " << TABLE_USER_DANGER << " WHERE " << FIELD_USERID << " = " << userid << " AND " << FIELD_TYPE << " = " << type;
  QueryRunner(DB_USER_DANGER, CDbWServer).execute(sql);
  Str2StrMap m;
  m["ID"] = boost::lexical_cast<string>(userid);
  m["TYPE"] = boost::lexical_cast<string>(type);
  m["OPERATION"] = boost::lexical_cast<string>(false);
  XceStormAdapter::instance().notify("USER_DANGER_UPDATER", m);
}

bool UserDangerManagerI::isType(int userid, int type, const Ice::Current& current) {
  ostringstream msg;
  msg << "userid: " << userid << " type: " << type;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
  DangerSetPtr dsp = getDangerSet(type);
  if (dsp) {
    return dsp->getType(userid);
  }
  return false;
}

Int2Bool UserDangerManagerI::isTypes(const IntSeq& userids,int type, const Ice::Current& current) {
  ostringstream msg;
  msg << "size: " << userids.size() << " type: " << type;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
  DangerSetPtr dsp = getDangerSet(type);
  if (dsp) {
    return dsp->getTypes(userids);
  }
  Int2Bool result;
  for (IntSeq::const_iterator i = userids.begin(); i != userids.end(); ++i) {
    result.insert(make_pair<int,bool>(*i, false));
  }
  return result;
}

bool UserDangerManagerI::isValid(const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current, "");
  IceUtil::RWRecMutex::RLock lock(_validMutex);
  return _valid;
}

void UserDangerManagerI::setValid(bool valid, const Ice::Current& current) {
  ostringstream oss;
  oss << "valid: " << valid;
  InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);
  IceUtil::RWRecMutex::WLock lock(_validMutex);
  _valid = valid;
  if (_valid) {
    ServerStateSubscriber::instance().setStatus(1);
  } else {
    ServerStateSubscriber::instance().setStatus(0);
  }
}

Int2IntSeq UserDangerManagerI::getClientCache(const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_DEBUG);
  IceUtil::RWRecMutex::RLock lock(_dangersMutex);
  Int2IntSeq result;
  for (map<int, DangerSetPtr>::iterator i = _dangers.begin(); i != _dangers.end(); ++i) {
    if (i->second->size() <= CLIENT_SIZE) {
      result.insert(make_pair<int, IntSeq>(i->first, i->second->getIds()));
    }
  }
  return result;
}

DangerSetPtr UserDangerManagerI::getDangerSet(int type) {
  IceUtil::RWRecMutex::RLock lock(_dangersMutex);
  map<int, DangerSetPtr>::iterator i = _dangers.find(type);
  if (i == _dangers.end()) {
    return 0;
  }
  return i->second;
}

DangerSetPtr UserDangerManagerI::setDangerSet(int type, const DangerSetPtr& ds, DangerSetPtr& ods) {
  MCE_INFO("UserDangerManagerI::setDangerSet type: " << type);
  IceUtil::RWRecMutex::WLock lock(_dangersMutex);
  ods=_dangers[type];
  _dangers[type] = ds;
  return ds;
}

void UserDangerManagerI::preload() {
  int beginid = 0;
  do {
    Statement sql;
    sql << "SELECT * FROM " << TABLE_USER_DANGER << " WHERE id > " << beginid << " ORDER BY id" << " LIMIT " << LOAD_SIZE;
    vector<pair<int, int> > data;
    QueryRunner(DB_USER_DANGER, CDbRServer).query(sql, UserDangerResultHandler(beginid, data));
    if (data.size() <= 0) {
      break;
    }
    for (vector<pair<int, int> >::iterator i = data.begin(); i != data.end(); ++i) {
      DangerSetPtr dsp = getDangerSet(i->second);
      if (dsp == 0) {
        DangerSetPtr ods=NULL;
        dsp = setDangerSet(i->second, new DangerSet(i->second), ods);
      }
      if (!dsp->getType(i->first)) {
        dsp->setOn(i->first);
      }
    }
    MCE_INFO("UserDangerManagerI::preload to " << beginid);
  } while (true);
  MCE_INFO("UserDangerManagerI::preload done.");
}

void UserDangerManagerI::reload() {
  int beginid = 0;
  map<int, DangerSetPtr> ds;
  do {
    Statement sql;
    sql << "SELECT * FROM " << TABLE_USER_DANGER << " WHERE id > " << beginid << " ORDER BY id" << " LIMIT " << LOAD_SIZE;
    vector<pair<int, int> > data;
    QueryRunner(DB_USER_DANGER, CDbRServer).query(sql, UserDangerResultHandler(beginid, data));
    if (data.size() <= 0) {
      break;
    }
    for (vector<pair<int, int> >::iterator i = data.begin(); i != data.end(); ++i) {
      map<int, DangerSetPtr>::iterator ii = ds.find(i->second);
      if (ii == ds.end()) {
        ds[i->second] = new DangerSet(i->second);
      }
      ds[i->second]->setOn(i->first);
    }
    MCE_INFO("UserDangerManagerI::reload to " << beginid);
  } while (true);
  for (map<int, DangerSetPtr>::iterator i = ds.begin(); i != ds.end(); ++i) {
    DangerSetPtr ods=NULL;
    setDangerSet(i->first, i->second,ods);
  }
}

void UserDangerManagerI::check() {
  IceUtil::RWRecMutex::RLock lock(_dangersMutex);
  for (map<int, DangerSetPtr>::iterator i = _dangers.begin(); i != _dangers.end(); ++i) {
    i->second->check();
  }
}

void UserDangerManagerI::run() {
  while (true) {
    try {
      reload();
      check();
      sleep(60);
    } catch (const std::exception& e) {
      MCE_WARN("UserDangerManagerI::run " << e.what());
    } catch (...) {
      MCE_WARN("UserDangerManagerI::run unknown exception.");
    }
  }
}

bool UserDangerResultHandler::handle(mysqlpp::Row& row) const {
  int id = row["id"];
  _largeid = id > _largeid ? id : _largeid;
  int uid = row[FIELD_USERID.c_str()];
  int type = row[FIELD_TYPE.c_str()];
  _data.push_back(make_pair<int, int>(uid, type));
  return true;
}

void DangerSet::setOn(int userid) {
  IceUtil::RWRecMutex::WLock lock(_mutex);
  getSet()->add(userid);
  _wflag = true;
  _lastwritten = time(0);
}

void DangerSet::setOff(int userid) {
  IceUtil::RWRecMutex::WLock lock(_mutex);
  getSet()->remove(userid);
  _wflag = true;
  _lastwritten = time(0);
}

bool DangerSet::getType(int userid) {
  IceUtil::RWRecMutex::RLock lock(_mutex);
  SetPtr s = getSet();
  if (s->size() <= CLIENT_SIZE) {
    MCE_WARN("DangerSet::getType type: " << _type << " size < "<<CLIENT_SIZE);
  }
  return getSet()->has(userid) > 0 ? true : false;
}

Int2Bool DangerSet::getTypes(const IntSeq& userids) {
  IceUtil::RWRecMutex::RLock lock(_mutex);
  Int2Bool result;
  for (IntSeq::const_iterator i = userids.begin(); i != userids.end(); ++i) {
    if (getSet()->has(*i) > 0) {
      result.insert(make_pair<int, bool>(*i,true));
    } else {
      result.insert(make_pair<int, bool>(*i,false));
    }
  }
  return result;
}

void DangerSet::check() {
  IceUtil::RWRecMutex::RLock lock(_mutex);
  SetPtr s = getSet();
  if (s->type() == RB_SET_TYPE && s->size() > CHANGE_BITSET_SIZE) {
    SetPtr r = new BitSet;
    for (set<int>::iterator i = RBSetPtr::dynamicCast(s)->_set.begin(); i != RBSetPtr::dynamicCast(s)->_set.end(); ++i) {
      r->add(*i);
    }
    setSet(r);
  }
  if (getSet()->type() == BIT_SET_TYPE && getSet()->size() < CHANGE_RBSET_SIZE) {
    SetPtr r = new RBSet;
    for (int i = 0; i < STATUS_CACHE_SIZE; ++i) {
      if (getSet()->has(i)) {
        r->add(i);
      }
    }
    setSet(r);
  }
}

IntSeq DangerSet::getIds() const {
  IceUtil::RWRecMutex::RLock lock(_mutex);
  SetPtr s = getSet();
  if (s->size() > CLIENT_SIZE || s->type() == BIT_SET_TYPE) {
    return IntSeq();
  }
  IntSeq result;
  for (set<int>::iterator i = RBSetPtr::dynamicCast(s)->_set.begin(); i != RBSetPtr::dynamicCast(s)->_set.end(); ++i) {
    result.push_back(*i);
  }
  return result;
}

int DangerSet::size() const {
  IceUtil::RWRecMutex::RLock lock(_mutex);
  return getSet()->size();
}

SetPtr DangerSet::getSet() const {
  IceUtil::RWRecMutex::RLock lock(_setmutex);
  return _set;
}

void DangerSet::setSet(const SetPtr& s) {
  IceUtil::RWRecMutex::WLock lock(_setmutex);
  _set = s;
}
