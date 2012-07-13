#include "MiscEdmSenderCache.h"
#include "QueryRunner.h"
#include "ServiceI.h"
namespace xce {
namespace edm {
static const int SecondOfDay = 60*60*24;
static const std::string SendCountInfoTableName = "friend_send_count_info";
static const std::string SendedTableName = "feed_edm_sended_misc";
MiscEdmSenderCache::MiscEdmSenderCache() : init_flag_(false) {
  friend_ = new TypeInfo(kFriend,0,"_friend_count",500000,1,0x00000001,0x0000000F,0xFFFFFFF0,0,1,SecondOfDay);
  addfriend_ = new TypeInfo(kAddFriend,0,"_addfriend_count",500000,2,0x00000010,0x000000F0,0xFFFFFF0F,4,2,SecondOfDay);
  mayknow_= new TypeInfo(kMayKnow,0,"_mayknow_count",500000,2,0x00000100,0x00000F00,0xFFFFF0FF,8,3,SecondOfDay);
  focusfriend_ = new TypeInfo(kFocusFriend,0,"_focusfriend_count",500000,1,0x00001000,0x0000F000,0xFFFF0FFF,12,4,SecondOfDay*28);
}
MiscEdmSenderCache::~MiscEdmSenderCache() {
  if (friend_ != 0) {
    delete friend_;
  }
  if (addfriend_ != 0) {
    delete addfriend_;
  }
  if (mayknow_ != 0) {
    delete mayknow_;
  }
  if (focusfriend_ != 0) {
    delete focusfriend_;
  }
}

void MiscEdmSenderCache::Initialize() {
  MCE_INFO("MiscEdmSenderCache start initialize");
  friend_->var_name_ = MyUtil::ServiceI::instance().getName() + friend_->var_name_;
  addfriend_->var_name_ = MyUtil::ServiceI::instance().getName() + addfriend_->var_name_;
  mayknow_->var_name_ = MyUtil::ServiceI::instance().getName() + mayknow_->var_name_;
  focusfriend_->var_name_ = MyUtil::ServiceI::instance().getName() + focusfriend_->var_name_;
  friend_->count_ = _ReadCountFromDB(friend_);
  addfriend_->count_ = _ReadCountFromDB(addfriend_);
  mayknow_->count_ = _ReadCountFromDB(mayknow_);
  focusfriend_->count_ = _ReadCountFromDB(focusfriend_);
  LoadFromDb();
  init_flag_ = true; 
  TaskManager::instance().scheduleRepeated(new MiscEdmSendCountTimer(60));
  TaskManager::instance().scheduleRepeated(new FriendCacheCleaner(SecondOfDay));
  TaskManager::instance().scheduleRepeated(new AddFriendCacheCleaner(SecondOfDay));
  TaskManager::instance().scheduleRepeated(new MayKnowCacheCleaner(SecondOfDay));
  TaskManager::instance().scheduleRepeated(new FocusFriendCacheCleaner(SecondOfDay*7));
  MCE_INFO("MiscEdmSenderCache start end");
}
bool MiscEdmSenderCache::LoadFromDb() {
  IceUtil::RWRecMutex::WLock lock(mutex_);
  time_t end = time(NULL);
  Ice::Long start_id = 0;
  com::xiaonei::xce::Statement sql;
  // get end id
  Ice::Long now_max_id = 0;
  sql << "select max(id) as max_id from " << SendedTableName ;
  for(unsigned int retry = 0; retry < 3; ++retry) {
    try {
      mysqlpp::StoreQueryResult res =  com::xiaonei::xce::QueryRunner("feed_edm", com::xiaonei::xce::CDbRServer,SendedTableName).store(sql);
      if(res.num_rows() == 0) {
        return true;
      }
      mysqlpp::Row row = res.at(0);
      now_max_id = boost::lexical_cast<Ice::Long> ((mysqlpp::Null<string,mysqlpp::NullIsZero>)row["max_id"]);
      break;
    } catch (Ice::Exception& e) {
      MCE_WARN("read start id from db " << SendedTableName << "failed, err->" << e);
      continue;
    }
  }
  sql.clear();
  start_id = now_max_id > 25000000 ? now_max_id - 25000000 : 0;
  Ice::Long times = (now_max_id - start_id)/1000;
  Ice::Long end_id = (now_max_id - start_id)>1000? start_id + 1000 : now_max_id;
  for (Ice::Long runtimes = 0; runtimes <= times; ++runtimes) {
    sql << "select user_id,type,unix_timestamp(time) as time from " << SendedTableName << " where id >= " << start_id << " and id < " << end_id;
    for(unsigned int retry = 0; retry< 3; ++retry) {
      try {
        mysqlpp::StoreQueryResult res =  com::xiaonei::xce::QueryRunner("feed_edm", com::xiaonei::xce::CDbRServer,SendedTableName).store(sql);
        for (unsigned i = 0;i < res.num_rows();++i) {
          mysqlpp::Row row = res.at(i);
          Ice::Int user_id = row["user_id"];
          Ice::Int type = row["type"];
          time_t time_stamp = (time_t)row["time"];
          TypeInfoPtr type_info = _GetTypeInfoFromDbType(type);
          if(type_info && (difftime(end,time_stamp) < type_info->period_)) {
            Add(user_id, type_info->misc_type_, true);
          }
        }
        break;
      } catch (Ice::Exception& e) {
        MCE_WARN("load unfit user from db " << SendedTableName << "failed once, err->" <<e);
        continue;
      }
    }
    start_id = end_id;
    end_id = (now_max_id - end_id)>1000 ? end_id + 1000 : now_max_id; 
    sql.clear();
  }
  MCE_INFO("load misc history date done");
  return true;

}
void MiscEdmSenderCache::Add(const Ice::Int& user_id, MiscType misc_type,bool load) {
  IceUtil::RWRecMutex::WLock lock(mutex_);
  TypeInfoPtr type_info = _GetTypeInfo(misc_type);
  if(type_info == 0) {
    return;
  } 
  IUMap::iterator ite = cache_.find(user_id);
  if (ite != cache_.end()) {
    ite->second += type_info->mask_;
  } else {
    unsigned int count = type_info->mask_;
    cache_.insert(std::make_pair<Ice::Int,unsigned int>(user_id,count));
  }
  if (!load) {
    ++type_info->count_;
    _InsertSendRecord(user_id,type_info->type_inDb_);
  }
}
bool MiscEdmSenderCache::EvictCache(MiscType misc_type) {
  IceUtil::RWRecMutex::WLock lock(mutex_);
  MCE_INFO("start evict cache");
  TypeInfoPtr type_info = _GetTypeInfo(misc_type);
  if(type_info == 0) {
    return false;
  }
  for(IUMap::iterator ite = cache_.begin(); ite != cache_.end();) {
    ite->second &= type_info->set_mask_;
    if (ite->second == 0x00000000) {
      cache_.erase(ite++);
    } else {
      ++ite;
    }
  } 
  return true;
}
bool MiscEdmSenderCache::OverCount(const Ice::Int& user_id, MiscType misc_type) {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  if (!init_flag_) {
    return true;
  }
  return _OverMaxCount(misc_type)|_OverMaxUserCount(user_id, misc_type);
}

TypeInfoPtr MiscEdmSenderCache::_GetTypeInfo(MiscType misc_type) {
  switch(misc_type) {
    case kFriend:
      return friend_;
    case kAddFriend:
      return addfriend_;
    case kMayKnow:
      return mayknow_;
    case kFocusFriend:
      return focusfriend_;
    default:
        return 0;
  }
}

TypeInfoPtr MiscEdmSenderCache::_GetTypeInfoFromDbType(int db_type) {
  switch(db_type) {
    case 1:
      return friend_;
    case 2:
      return addfriend_;
    case 3:
      return mayknow_;
    case 4:
      return focusfriend_;
    default:
        return 0;
  }
}

int MiscEdmSenderCache::_ReadCountFromDB(TypeInfoPtr type_info) {
  com::xiaonei::xce::Statement sql;
  sql << "select count from " << SendCountInfoTableName << " where varname=" << mysqlpp::quote << type_info->var_name_ << mysqlpp::quote;
  try {
    mysqlpp::StoreQueryResult res =  com::xiaonei::xce::QueryRunner("feed_edm", com::xiaonei::xce::CDbRServer, SendCountInfoTableName).store(sql);
    mysqlpp::Row row = res.at(0);
    int count = row["count"];
    return count;
  } catch (Ice::Exception& e) {
    MCE_WARN("read count from db: " << SendCountInfoTableName << "failed. err->" << e);
  }
  return 0;
}
bool MiscEdmSenderCache::_OverMaxCount(MiscType misc_type) {
  TypeInfoPtr type_info = _GetTypeInfo(misc_type);
  if (type_info == 0) {
    return true;
  }
  return type_info->count_ >= type_info->max_count_;
}
bool MiscEdmSenderCache::_OverMaxUserCount(const Ice::Int& user_id, MiscType misc_type) {
  IUMap::iterator ite = cache_.find(user_id);
  if (ite != cache_.end()) {
    TypeInfoPtr type_info = _GetTypeInfo(misc_type);
    if (type_info == 0) {
      return true;
    }
    if((ite->second & type_info->read_mask_) >> type_info->right_shift_ >= type_info->user_send_limit_) {
      return true;
    }
  }
  return false;
}
void MiscEdmSenderCache::_ResetCountToDB(MiscType misc_type) {
  time_t now = time(NULL);
  tm now_tm;
  localtime_r(&now,&now_tm);
  com::xiaonei::xce::Statement sql;
  TypeInfoPtr type_info = _GetTypeInfo(misc_type);
  if (type_info == 0) {
    return;
  }
  if (now_tm.tm_hour == 10 && (now_tm.tm_min == 0||now_tm.tm_min == 1)) {
    sql << "update " << SendCountInfoTableName << " set count = 0 where varname=" << mysqlpp::quote << type_info->var_name_ << mysqlpp::quote;
    try {
      com::xiaonei::xce::QueryRunner("feed_edm", com::xiaonei::xce::CDbWServer, SendCountInfoTableName).store(sql);
    } catch (Ice::Exception& e) {
      MCE_WARN("reset focusfriend count to db: " << SendCountInfoTableName << "failed. err->" << e);
    }
    type_info->count_ = 0; 
  } else {
    sql << "update " << SendCountInfoTableName << " set count = " << type_info->count_ << " where varname=" << mysqlpp::quote << type_info->var_name_ << mysqlpp::quote;
    MCE_INFO("update count of varname:" << type_info->var_name_<< " to count:" << type_info->count_ << ",misc_type:" << misc_type);
    try {
      com::xiaonei::xce::QueryRunner("feed_edm", com::xiaonei::xce::CDbWServer, SendCountInfoTableName).store(sql);
    } catch (Ice::Exception& e) {
      MCE_WARN("set focusfriend count to db: " << SendCountInfoTableName << "failed. err->" << e);
    }
  }
}
void MiscEdmSenderCache::_InsertSendRecord(const Ice::Int& user_id, int type) {
  com::xiaonei::xce::Statement sql;
  sql << "insert into " << SendedTableName << "(user_id,type) values(" << user_id << "," << type << ");";
  try {
    com::xiaonei::xce::QueryRunner("feed_edm", com::xiaonei::xce::CDbWServer, SendCountInfoTableName).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("insert misc send record to db: " << SendCountInfoTableName << "failed. err->" << e);
  } 
}
void MiscEdmSenderCache::MiscEdmSendCountTimer::handle() {
  MiscEdmSenderCache::instance()._ResetCountToDB(kFriend);
  MiscEdmSenderCache::instance()._ResetCountToDB(kAddFriend);
  MiscEdmSenderCache::instance()._ResetCountToDB(kMayKnow);
  MiscEdmSenderCache::instance()._ResetCountToDB(kFocusFriend);
}    

void MiscEdmSenderCache::FriendCacheCleaner::handle() {
  MiscEdmSenderCache::instance().EvictCache(kFriend);
}
void MiscEdmSenderCache::AddFriendCacheCleaner::handle() {
  MiscEdmSenderCache::instance().EvictCache(kAddFriend);
}
void MiscEdmSenderCache::MayKnowCacheCleaner::handle() {
  MiscEdmSenderCache::instance().EvictCache(kMayKnow);
}
void MiscEdmSenderCache::FocusFriendCacheCleaner::handle() {
  MiscEdmSenderCache::instance().EvictCache(kFocusFriend);
}

void MiscEdmSenderCacheLoadTask::handle() {
  MiscEdmSenderCache::instance().Initialize();
}
  
}
}
