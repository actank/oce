#include "DistUserApiCacheReloaderI.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "util/cpp/TimeCost.h"

using xce::userapicache::UserApiCacheUpdateOldData;
using xce::userapicache::UserApiCacheUpdateOldDataPtr;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&xce::distcache::userapicache::DistUserApiCacheReloaderI::instance(), service.createIdentity(xce::distcache::userapicache::USER_API_CACHE_RELOADER, ""));
  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("DistUserApiCacheReloader.Mod");
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("DistUserApiCacheReloader.Interval", 5);
  xce::serverstate::ServerStateSubscriber::instance().initialize("ControllerDistUserApiCacheReloader", &xce::distcache::userapicache::DistUserApiCacheReloaderI::instance(), mod, interval);
  xce::distcache::userapicache::DistUserApiCacheAdapter::instance().initialize();
}

bool xce::distcache::userapicache::DistUserApiCacheReloaderI::isValid(const Ice::Current&) {
  IceUtil::RWRecMutex::RLock lock(validmutex_);
  return valid_;
}

void xce::distcache::userapicache::DistUserApiCacheReloaderI::setValid(bool valid, const Ice::Current& current) {
  ostringstream oss;
  oss << "DistUserApiCacheReloaderI::setValid -> " << valid << " " << current.con->toString().substr(current.con->toString().find("remote",25));
  MyUtil::TimeCost tc = MyUtil::TimeCost::create(oss.str(), MyUtil::TimeCost::LOG_LEVEL_INFO);
  IceUtil::RWRecMutex::WLock lock(validmutex_);
  valid_ = valid;
  if (valid_) {
    xce::serverstate::ServerStateSubscriber::instance().setStatus(1);
  } else {
    xce::serverstate::ServerStateSubscriber::instance().setStatus(0);
  }
}

void xce::distcache::userapicache::DistUserApiCacheReloaderI::reload(const MyUtil::IntSeq& ids, const Ice::Current& current) {
  ostringstream oss;
  oss << "DistUserApiCacheReloaderI::reload -> size: "<<ids.size()<< " " << current.con->toString().substr(current.con->toString().find("remote",25));
  MyUtil::TimeCost tc = MyUtil::TimeCost::create(oss.str(), MyUtil::TimeCost::LOG_LEVEL_INFO);
  oss<<"ids = ";
  MyUtil::IntSeq tempids;
  for (MyUtil::IntSeq::const_iterator i=ids.begin(); i!=ids.end();++i){
    if (*i > 0) {
      tempids.push_back(*i);
      oss<<*i<<" ";
    }
  }
  MCE_DEBUG(oss.str());
  MyUtil::TaskManager::instance().execute(new ReloadTask(tempids));
}

void xce::distcache::userapicache::DistUserApiCacheReloaderI::update(const xce::userapicache::UserApiCacheUpdateOldDataPtr& data, const Ice::Current& current) {
  ostringstream oss;
  oss << "DistUserApiCacheReloaderI::update -> id: "<<data->userid <<" "<< current.con->toString().substr(current.con->toString().find("remote",25));
  MyUtil::TimeCost tc = MyUtil::TimeCost::create(oss.str(), MyUtil::TimeCost::LOG_LEVEL_INFO);

  MyUtil::TaskManager::instance().execute(new UpdateTask(data));
}

xce::distcache::userapicache::UserApiCacheMemDataPtr xce::distcache::userapicache::UpdateTask::GetNewData() {
  xce::distcache::userapicache::UserApiCacheMemDataPtr data = xce::distcache::userapicache::DistUserApiCacheAdapter::instance().get(data_->userid);
  if(data == NULL) 
  {
    MyUtil::IntSeq ids;
    ids.push_back(data_->userid);
    xce::distcache::userapicache::DistUserApiCacheReloaderI::instance().addDelayReloadIds(ids);
    return NULL;
  }
  MyUtil::Str2StrMap::const_iterator i;
  switch (data_->table) {
    case xce::userapicache::TABLEBORN:
      i = data_->data.find("BIRTH_DAY");
      if (i != data_->data.end()) {
        data->set_birthday(boost::lexical_cast<int>(i->second));
      }
      i = data_->data.find("GENDER");
      if (i != data_->data.end()) {
        data->set_gender(i->second);
      }
      i = data_->data.find("BIRTH_YEAR");
      if (i != data_->data.end()) {
        data->set_birthyear(boost::lexical_cast<int>(i->second));
      }
      i = data_->data.find("BIRTH_MONTH");
      if (i != data_->data.end()) {
        data->set_birthmonth(boost::lexical_cast<int>(i->second));
      }
      i = data_->data.find("HOME_CITY");
      if (i != data_->data.end()) {
        data->set_homecity(i->second);
      }
      i = data_->data.find("HOME_PROVINCE");
      if (i != data_->data.end()) {
        data->set_homeprovince(i->second);
      }
      break;
    case xce::userapicache::TABLESTAGE:
      i = data_->data.find("STAGE");
      if (i != data_->data.end()) {
        data->set_stage(boost::lexical_cast<int>(i->second));
      }
      i = data_->data.find("UNIV");
      if (i != data_->data.end()) {
        data->set_univ(boost::lexical_cast<int>(i->second));
      }
      break;
    case xce::userapicache::TABLESTATE:
      i = data_->data.find("LEVEL");
      if (i != data_->data.end()) {
        data->set_level(boost::lexical_cast<int>(i->second));
      }
      i = data_->data.find("STATE");
      if (i != data_->data.end()) {
        data->set_state(boost::lexical_cast<int>(i->second));
      }
      break;
    case xce::userapicache::TABLENAMES:
      i = data_->data.find("NAME");
      if (i != data_->data.end()) {
        data->set_name(i->second);
      }
      break;
    case xce::userapicache::TABLERIGHT:
      i = data_->data.find("AUTH");
      if (i != data_->data.end()) {
        data->set_auth(boost::lexical_cast<int>(i->second));
      }
      break;
    case xce::userapicache::TABLEPASSPORT:
      i = data_->data.find("STATUS");
      if (i != data_->data.end()) {
        data->set_status(boost::lexical_cast<int>(i->second));
      }
      break;
    case xce::userapicache::TABLEURL:
      i = data_->data.find("TINYURL");
      if (i != data_->data.end()) {
        data->set_tinyurl(i->second);
      }
      i = data_->data.find("MAINURL");
      if (i != data_->data.end()) {
        data->set_mainurl(i->second);
      }
      i = data_->data.find("HEADURL");
      if (i != data_->data.end()) {
        data->set_headurl(i->second);
      }
      break;
    case xce::userapicache::TABLECONFIG:
      i = data_->data.find("BASIC_CONFIG");
      if (i != data_->data.end()) {
        data->set_basicconfig(boost::lexical_cast<int>(i->second));
      }
      i = data_->data.find("STATUS_CONFIG");
      if (i != data_->data.end()) {
        data->set_statusconfig(boost::lexical_cast<int>(i->second));
      }
      i = data_->data.find("PROFILE_PRIVACY");
      if (i != data_->data.end()) {
        data->set_profileprivacy(i->second);
      }
      break;
    default:
      MCE_WARN("UpdateTask::GetNewData -> Oh, no!");
  }
  return data;
}

  void xce::distcache::userapicache::UpdateTask::handle() {
    if(xce::distcache::userapicache::DistUserApiCacheReloaderI::instance().getFilter()->filter(data_->userid))
    {
      xce::distcache::userapicache::UserApiCacheMemDataPtr datad = GetNewData();
      if(datad)
      {
        try
        {
          xce::distcache::userapicache::DistUserApiCacheAdapter::instance().set(datad);
        }catch(...)
        {
          MCE_WARN("UpdateTask::handle set distcache Exception: id = " << data_->userid);
        }
      }
      xce::distcache::userapicache::DistUserApiCacheReloaderI::instance().getFilter()->remove(data_->userid);
    }else
    {
      MyUtil::IntSeq ids;
      ids.push_back(data_->userid);
      MCE_WARN("UpdateTask::handle muticall id = " << data_->userid);
      xce::distcache::userapicache::DistUserApiCacheReloaderI::instance().addDelayReloadIds(ids);
    }
  }

void xce::distcache::userapicache::ReloadTask::handle() {
  std::vector<int> finalIds;
  std::vector<int> filterIds = xce::distcache::userapicache::DistUserApiCacheReloaderI::instance().getFilter()->filter(ids_, finalIds);

  MyUtil::ObjectResultPtr res = xce::distcache::userapicache::DistUserApiCacheReloaderI::instance().GetObjectResult(finalIds);

  if(!res->data.empty())
  {
    try
    {
      xce::distcache::userapicache::DistUserApiCacheAdapter::instance().set(res);
    }catch(...)
    {
      ostringstream os;
      os<<"ReloadTask::handle set distcache Exception: ids = ";
      for(std::vector<int>::const_iterator it = finalIds.begin(); it != finalIds.end(); ++it)
      {
        os<<*it<<" ";
      }
      MCE_WARN(os.str());
    }
  }
  xce::distcache::userapicache::DistUserApiCacheReloaderI::instance().getFilter()->remove(finalIds);
  if(filterIds.size() > 0)
  {
    ostringstream os;
    os<<"ReloadTask::handle muticall ids = ";
    for(std::vector<int>::const_iterator it = filterIds.begin(); it != filterIds.end(); ++it)
    {
      os<<*it<<" ";
    }
    MCE_WARN(os.str());
    xce::distcache::userapicache::DistUserApiCacheReloaderI::instance().addDelayReloadIds(filterIds);
  }
}


MyUtil::ObjectResultPtr xce::distcache::userapicache::DistUserApiCacheReloaderI::GetObjectResult(const MyUtil::IntSeq& ids) {
  MyUtil::ObjectResultPtr result = new MyUtil::ObjectResult;
  GetUserPassport(ids, result);
  GetUserState(ids, result);
  GetUserRight(ids, result);
  GetUserConfig(ids, result);
  GetUserNames(ids, result);
  GetUserUrl(ids, result);
  GetUserBorn(ids, result);
  GetUserStage(ids, result);
  return result;
}

void xce::distcache::userapicache::DistUserApiCacheReloaderI::GetUserPassport(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr& data) {
  com::xiaonei::xce::Statement sql;
  sql << "SELECT id, status FROM user_passport WHERE id IN (";
  for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
    if (it != ids.begin()) {
      sql << ", ";
    }
    sql << *it;
  }
  sql << ")";
  com::xiaonei::xce::QueryRunner("userapicache_user_passport",com::xiaonei::xce::CDbRServer).query(sql, BatchUserPassportResultHandler(data));
}

void xce::distcache::userapicache::DistUserApiCacheReloaderI::GetUserState(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr& data) {
  com::xiaonei::xce::Statement sql;
  sql << "SELECT id, state,level FROM user_state WHERE id IN (";
  for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
    if (it != ids.begin()) {
      sql << ",";
    }
    sql << *it;
  }
  sql << ")";
  com::xiaonei::xce::QueryRunner("userapicache_user_state",com::xiaonei::xce::CDbRServer).query(sql, BatchUserStateResultHandler(data));
}

void xce::distcache::userapicache::DistUserApiCacheReloaderI::GetUserBorn(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr& data) {
  com::xiaonei::xce::Statement sql;
  sql << "SELECT id,gender,birth_year,birth_month,birth_day,home_province, home_city FROM user_born WHERE id IN (";
  for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
    if (it != ids.begin()) {
      sql << ",";
    }
    sql << *it;
  }
  sql << ")";
  com::xiaonei::xce::QueryRunner("userapicache_user_born",com::xiaonei::xce::CDbRServer).query(sql, BatchUserBornResultHandler(data));
}

void xce::distcache::userapicache::DistUserApiCacheReloaderI::GetUserStage(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr& data) {
  com::xiaonei::xce::Statement sql;
  sql << "SELECT id,stage,univ FROM user_stage WHERE id IN (";
  for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
    if (it != ids.begin()) {
      sql << ",";
    }
    sql << *it;
  }
  sql << ")";
  com::xiaonei::xce::QueryRunner("userapicache_user_stage",com::xiaonei::xce::CDbRServer).query(sql, BatchUserStageResultHandler(data));
}

void xce::distcache::userapicache::DistUserApiCacheReloaderI::GetUserNames(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr& data) {
  com::xiaonei::xce::Statement sql;
  sql << "SELECT id, name FROM user_names WHERE id IN (";
  for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
    if (it != ids.begin()) {
      sql << ",";
    }
    sql << *it;
  }
  sql << ")";
  com::xiaonei::xce::QueryRunner("userapicache_user_name",com::xiaonei::xce::CDbRServer).query(sql, BatchUserNamesResultHandler(data));
}

void xce::distcache::userapicache::DistUserApiCacheReloaderI::GetUserRight(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr& data) {
  com::xiaonei::xce::Statement sql;
  sql << "SELECT id,auth FROM user_right WHERE id IN (";
  for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
    if (it != ids.begin()) {
      sql << ",";
    }
    sql << *it;
  }
  sql << ")";
  com::xiaonei::xce::QueryRunner("userapicache_user_right",com::xiaonei::xce::CDbRServer).query(sql, BatchUserRightResultHandler(data));
}

void xce::distcache::userapicache::DistUserApiCacheReloaderI::GetUserConfig(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr& data) {
  com::xiaonei::xce::Statement sql;
  sql << "SELECT id,basic_config,status_config,profile_privacy FROM user_config WHERE id IN (";
  for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
    if (it != ids.begin()) {
      sql << ",";
    }
    sql << *it;
  }
  sql << ")";
  com::xiaonei::xce::QueryRunner("userapicache_user_config",com::xiaonei::xce::CDbRServer).query(sql, BatchUserConfigResultHandler(data));
}

void xce::distcache::userapicache::DistUserApiCacheReloaderI::GetUserUrl(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr& data) {
  com::xiaonei::xce::Statement sql;
  sql << "SELECT id,mainurl,headurl,tinyurl FROM user_url WHERE id IN (";
  for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
    if (it != ids.begin()) {
      sql << ",";
    }
    sql << *it;
  }
  sql << ")";
  com::xiaonei::xce::QueryRunner("userapicache_user_url",com::xiaonei::xce::CDbRServer).query(sql, BatchUserUrlResultHandler(data));
}

bool xce::distcache::userapicache::BatchUserPassportResultHandler::handle(mysqlpp::Row& row) const {
  int id = (int) row["id"];
  MyUtil::ObjectMap::iterator it = _result->data.find(id);
  if (it == _result->data.end()) {
    xce::distcache::userapicache::UserApiCacheMemDataPtr obj = new UserApiCacheMemData;
    obj->set_id(id);
    obj->setUserPassportProperties(row, "status");
    _result->data.insert(make_pair(id, obj));
  } else {
    MCE_WARN("BatchUserPassportResultHandler::handle -> Invalid Id: " << id);
  }
  return true;
}

bool xce::distcache::userapicache::BatchUserNamesResultHandler::handle(mysqlpp::Row& row) const {
  int id = (int) row["id"];
  MyUtil::ObjectMap::iterator it = _result->data.find(id);
  if (it == _result->data.end()) {
    MCE_WARN("BatchUserNamesResultHandler::handle -> user_names has userid = "<<id <<" but user_passport not");
  } else {
    xce::distcache::userapicache::UserApiCacheMemDataPtr::dynamicCast(it->second)->setNameProperties(row, "name");
  }
  return true;
}

bool xce::distcache::userapicache::BatchUserRightResultHandler::handle(mysqlpp::Row& row) const {
  int id = (int) row["id"];
  MyUtil::ObjectMap::iterator it = _result->data.find(id);
  if (it == _result->data.end()) {
    MCE_WARN("BatchUserRightResultHandler::handle -> user_right has userid = "<<id <<" but user_passport not");
  } else {
    xce::distcache::userapicache::UserApiCacheMemDataPtr::dynamicCast(it->second)->setRightProperties(row, "auth");
  }
  return true;
}

bool xce::distcache::userapicache::BatchUserStateResultHandler::handle(mysqlpp::Row& row) const {
  int id = (int) row["id"];
  MyUtil::ObjectMap::iterator it = _result->data.find(id);
  if (it == _result->data.end()) {
    MCE_WARN("BatchUserStateResultHandler::handle -> user_state has userid = "<<id <<" but user_passport not");
  } else {
    xce::distcache::userapicache::UserApiCacheMemDataPtr::dynamicCast(it->second)->setStateProperties(row, "state", "level");
  }
  return true;
}

bool xce::distcache::userapicache::BatchUserUrlResultHandler::handle(mysqlpp::Row& row) const {
  int id = (int) row["id"];
  MyUtil::ObjectMap::iterator it = _result->data.find(id);
  if (it == _result->data.end()) {
    MCE_WARN("BatchUserUrlResultHandler::handle -> user_url has userid = "<<id <<" but user_passport not");
  } else {
    xce::distcache::userapicache::UserApiCacheMemDataPtr::dynamicCast(it->second)->setUrlProperties(row, "mainurl","headurl","tinyurl");
  }
  return true;
}

bool xce::distcache::userapicache::BatchUserConfigResultHandler::handle(mysqlpp::Row& row) const {
  int id = (int) row["id"];
  MyUtil::ObjectMap::iterator it = _result->data.find(id);
  if (it == _result->data.end()) {
    MCE_WARN("BatchUserConfigResultHandler::handle -> user_config has userid = "<<id <<" but user_passport not");
  } else {
    xce::distcache::userapicache::UserApiCacheMemDataPtr::dynamicCast(it->second)->setConfigProperties(row, "basic_config", "status_config", "profile_privacy");
  }
  return true;
}

bool xce::distcache::userapicache::BatchUserBornResultHandler::handle(mysqlpp::Row& row) const {
  int id = (int) row["id"];
  MyUtil::ObjectMap::iterator it = _result->data.find(id);
  if (it == _result->data.end()) {
    MCE_WARN("BatchUserBornResultHandler::handle -> user_born has userid = "<<id <<" but user_passport not");
  } else {
    xce::distcache::userapicache::UserApiCacheMemDataPtr::dynamicCast(it->second)->setBornProperties(row, "gender","birth_year", "birth_month", "birth_day", "home_province","home_city");
  }
  return true;

}

bool xce::distcache::userapicache::BatchUserStageResultHandler::handle(mysqlpp::Row& row) const {
  int id = (int) row["id"];
  MyUtil::ObjectMap::iterator it = _result->data.find(id);
  if (it == _result->data.end()) {
    MCE_WARN("BatchUserStageResultHandler::handle -> user_stage has userid = "<<id <<" but user_passport not");
  } else {
    xce::distcache::userapicache::UserApiCacheMemDataPtr::dynamicCast(it->second)->setStageProperties(row, "univ", "stage");
  }
  return true;

}

void xce::distcache::userapicache::DelayReloadThread::add(const MyUtil::IntSeq& ids)
{
  IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
  _idSet.insert(ids.begin(), ids.end());
  if(_idSet.size() >= 100)
  {
    _mutex.notify();
  }
}

void xce::distcache::userapicache::DelayReloadThread::run()
{
  while(true)
  {
    MyUtil::IntSeq ids;
    {
      IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
      if(_idSet.empty())
      {
        _mutex.timedWait(IceUtil::Time::seconds(1));
      }
      if(_idSet.empty())
      {
        continue;
      }
      ids.insert(ids.end(),_idSet.begin(), _idSet.end());
      _idSet.clear();
    }
    MyUtil::TaskManager::instance().execute(new ReloadTask(ids));
  }
}


