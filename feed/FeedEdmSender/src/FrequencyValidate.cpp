#include "FrequencyValidate.h"
#include "QueryRunner.h"
#include "LogWrapper.h"
#include <boost/lexical_cast.hpp>
#include <string>
#include "Date.h"
#include "ServiceI.h"
namespace xce {
namespace edm {

const int kHourSecond = 60*60;
const std::string SendedTableName = "feed_edm_sended";
const std::string SendedTableNameSp = "feed_edm_sended_sp";
static const std::string FlushInfoTableName = "friend_send_count_info";
FrequencyCache::FrequencyCache() : weekly_load_sign_(true),daily_load_sign_(true),update_wday_(1) {
}

bool FrequencyCache::LoadFromDb() {
  _LoadWeeklyData();
  weekly_load_sign_ = false;
  _LoadDailyData();
  daily_load_sign_ = false;
  _LoadWday(); 
  TaskManager::instance().scheduleRepeated(new CacheCleaner(60));
  TaskManager::instance().scheduleRepeated(new CacheFlusher(1));
  return true; 
}

bool FrequencyCache::Add(const Ice::Int& user_id, SendType send_type, time_t update_time) {
  IceUtil::RWRecMutex::WLock lock(mutex_);
  UEMap::iterator ite = user_cache_.find(user_id);
  if (ite != user_cache_.end()) {
    switch (send_type) {
      case kWeekly:
        ite->second.send_flag_ |= 0x00000001;
        ite->second.weekly_send_time_ = update_time;
        break;
      case kDaily:
        ite->second.send_flag_ |= 0x00000002;
        ite->second.daily_send_time_ = update_time;
        break;
      default:
        // do nothing
        ;
    }
  } else {
    Entry entry;
    switch (send_type) {
      case kWeekly:
        entry.send_flag_ = 0x00000001;
        entry.weekly_send_time_ = update_time;
        break;
      case kDaily:
        entry.send_flag_ = 0x00000002;
        entry.daily_send_time_ = update_time;
        break;
      default:
        ;
    } 
    user_cache_.insert(std::make_pair<Ice::Int, Entry>(user_id, entry));
  }  
  return true;
}
Ice::Int FrequencyCache::CheckFrequencyLimit(const Ice::Int& user_id) {
  if (weekly_load_sign_) {
    return 0;
  } else if (daily_load_sign_) {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    UEMap::iterator ite = user_cache_.find(user_id);
    if (ite != user_cache_.end()) {
      if (ite->second.send_flag_ & 0x00000001) {
        return 0;
      } 
    } else {
      return 1;
    }
  } else {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    UEMap::iterator ite = user_cache_.find(user_id);
    if (ite != user_cache_.end()) {
      if (!(ite->second.send_flag_ & 0x00000001)) {
        return 1;
      } 
      if (!(ite->second.send_flag_ & 0x00000002)) {
        return 2;
      }
    } else {
      return 1;
    }
  }
  return 0;
}


void FrequencyCache::ClearCacheByUser(int userid){
  IceUtil::RWRecMutex::WLock lock(mutex_);
  UEMap::iterator ite = user_cache_.find(userid);
  if (ite != user_cache_.end()) {
		user_cache_.erase(ite);
		MCE_INFO("FrequencyCache::ClearCacheByUser --> clear success userid = " << userid);
		return;
	}	
}

void FrequencyCache::EvictCache() {
  IceUtil::RWRecMutex::WLock lock(mutex_);
  for (UEMap::iterator ite = user_cache_.begin(); ite != user_cache_.end(); ) {
    /*
    if (ite->second.send_flag_ & 0x00000001) {
      time_t now = time(NULL);
      if (now - ite->second.weekly_send_time_ >= kHourSecond * 24 * 7) {
        ite->second.send_flag_ &= 0xFFFFFFFE;
      } 
    }
    */
    if (ite->second.send_flag_ & 0x00000002) {
      time_t now = time(NULL);
      if (now - ite->second.daily_send_time_ >= kHourSecond * 24 ) {
        ite->second.send_flag_ &= 0xFFFFFFFD;
      }
    }
    if (!ite->second.send_flag_) {
      user_cache_.erase(ite++);
    } else {
      ++ite;
    }
  }
}

void FrequencyCache::FlushCache() {
  IceUtil::RWRecMutex::WLock lock(mutex_);  
  for (UEMap::iterator ite = user_cache_.begin(); ite != user_cache_.end(); ) {
    ite->second.send_flag_ &= 0xFFFFFFFE;
    if (ite->second.send_flag_ == 0) {
      user_cache_.erase(ite++);
    } else {
      ++ite;
    }
  }
  Date now_time(time(NULL));
  update_wday_ = now_time.wday();
  MCE_INFO("set wday to:" << update_wday_);
  _UpdateWday(update_wday_);
}
bool FrequencyCache::_UpdateWday(int wday) {
  std::string var_name = MyUtil::ServiceI::instance().getName() + "_last_update_time";
  com::xiaonei::xce::Statement sql;
  sql << "update " << FlushInfoTableName << " set count = " << update_wday_ << " where varname=" << mysqlpp::quote << var_name << mysqlpp::quote;
  try {
    com::xiaonei::xce::QueryRunner("feed_edm", com::xiaonei::xce::CDbWServer, FlushInfoTableName).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("set wday to db: " << FlushInfoTableName << "failed. err->" << e);
  }
  return true;  
}

bool FrequencyCache::_LoadWeeklyData() {
  time_t flush_time = _GetFlushTime();  
  /*time_t end = time(NULL);*/
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
  start_id = now_max_id > 20000000 ? now_max_id - 20000000 : 0;
  sql.clear();
  // load now data,every time read 1k
  while((now_max_id - start_id) > 1000) {
  	Ice::Long times = (now_max_id - start_id)/1000;
  	Ice::Long end_id = start_id + 1000;
  	for (Ice::Long runtimes = 0; runtimes < times; ++runtimes) {
  	  sql << "select userid,unix_timestamp(time) as time  from " << SendedTableName << " where id >= " << start_id << " and id < " << end_id;
  	  for(unsigned int retry = 0; retry< 3; ++retry) {
  	    try {
  	      mysqlpp::StoreQueryResult res =  com::xiaonei::xce::QueryRunner("feed_edm", com::xiaonei::xce::CDbRServer,SendedTableName).store(sql);
  	      for (unsigned i = 0;i < res.num_rows();++i) {
  	        mysqlpp::Row row = res.at(i);
  	        Ice::Int user_id = row["userid"];
  	        time_t time_stamp = (time_t)row["time"];
            /*
            if (difftime(end,time_stamp) < kHourSecond * 24 *7) {
  	          Add(user_id, kWeekly, time_stamp);
            }*/
            if(time_stamp > flush_time) {
              Add(user_id, kWeekly, time_stamp);
            }
  	      }
  	      break;
  	    } catch (Ice::Exception& e) {
  	      MCE_WARN("load unfit user from db " << SendedTableName << "failed once, err->" <<e);
  	      continue;
  	    }
  	  }
  	  start_id = end_id;
  	  end_id += 1000;
      sql.clear();
  	}
    // see now max id,because data group may group
    sql << "select max(id) as max_id from " << SendedTableName ;
    for(unsigned int retry = 0; retry< 3; ++retry) {
      try {
        mysqlpp::StoreQueryResult res =  com::xiaonei::xce::QueryRunner("feed_edm", com::xiaonei::xce::CDbRServer,SendedTableName).store(sql);
        mysqlpp::Row row = res.at(0);
        now_max_id = (Ice::Long)row["max_id"];
        break;
      } catch (Ice::Exception& e) {
        MCE_WARN("read max id from db " << SendedTableName << "failed, err->" << e);
        continue;
      }
    } 
    sql.clear();
  }
  // read left data
  sql << "select userid,unix_timestamp(time) as time  from " << SendedTableName << " where id between " << start_id << " and " << now_max_id;
  for(unsigned int retry = 0; retry < 3; ++retry) {
    try {
      mysqlpp::StoreQueryResult res =  com::xiaonei::xce::QueryRunner("feed_edm", com::xiaonei::xce::CDbRServer,SendedTableName).store(sql);
      for (unsigned i = 0;i < res.num_rows();++i) {
        mysqlpp::Row row = res.at(i);
        Ice::Int user_id = row["userid"];
        time_t time_stamp = (time_t)row["time"];
        /*
        if (difftime(end,time_stamp) < kHourSecond * 24 *7) {
          Add(user_id, kWeekly,time_stamp); 
        }*/
        if(time_stamp > flush_time) {
          Add(user_id, kWeekly, time_stamp);
        }
      }
      break;
    } catch (Ice::Exception& e) {
      MCE_WARN("load unfit user from db " << SendedTableName << "failed, err->" << e);
      continue;
    }
  }
  MCE_INFO("load weekly history date done");
  return true;
}

bool FrequencyCache::_LoadDailyData() {
  time_t end = time(NULL);
  Ice::Long start_id = 0;
  com::xiaonei::xce::Statement sql;
  Ice::Long now_max_id = 0;
  sql << "select max(id) as max_id from " << SendedTableNameSp ;
  for(unsigned int retry = 0; retry < 3; ++retry) {
    try {
      mysqlpp::StoreQueryResult res =  com::xiaonei::xce::QueryRunner("feed_edm", com::xiaonei::xce::CDbRServer,SendedTableNameSp).store(sql);
      MCE_INFO("rows num:" << res.num_rows());
      if(res.num_rows() == 0) {
        MCE_INFO("load dailyly history date done");
        return true;
      }
      mysqlpp::Row row = res.at(0);
      now_max_id = boost::lexical_cast<Ice::Long> ((mysqlpp::Null<string,
        mysqlpp::NullIsZero>)row["max_id"]); 
      break;
    } catch (Ice::Exception& e) {
      MCE_WARN("read start id from db " << SendedTableNameSp << "failed, err->" << e);
      continue;
    }
  }
  sql.clear();
  start_id = now_max_id > 20000000 ? now_max_id - 20000000 : 0;
  while((now_max_id - start_id) > 1000) {
  	Ice::Long times = (now_max_id - start_id)/1000;
  	Ice::Long end_id = start_id + 1000;
  	for (Ice::Long runtimes = 0; runtimes < times; ++runtimes) {
  	  sql << "select userid,unix_timestamp(time) as time  from " << SendedTableNameSp << " where id >= " << start_id << " and id < " << end_id;
  	  for(unsigned int retry = 0; retry< 3; ++retry) {
  	    try {
  	      mysqlpp::StoreQueryResult res =  com::xiaonei::xce::QueryRunner("feed_edm", com::xiaonei::xce::CDbRServer,SendedTableNameSp).store(sql);
  	      for (unsigned i = 0;i < res.num_rows();++i) {
  	        mysqlpp::Row row = res.at(i);
  	        Ice::Int user_id = row["userid"];
  	        time_t time_stamp = (time_t)row["time"];
            if (difftime(end, time_stamp)< kHourSecond * 24) {
  	          Add(user_id, kDaily, time_stamp);
            }
  	      }
  	      break;
  	    } catch (Ice::Exception& e) {
  	      MCE_WARN("Load unfit user from db " << SendedTableNameSp << "failed once, err->" <<e);
  	      continue;
  	    }
  	  }
  	  start_id = end_id;
  	  end_id += 1000;
      sql.clear();
  	}
    sql << "select max(id) as max_id from " << SendedTableNameSp ;
    for(unsigned int retry = 0; retry< 3; ++retry) {
      try {
        mysqlpp::StoreQueryResult res =  com::xiaonei::xce::QueryRunner("feed_edm", com::xiaonei::xce::CDbRServer,SendedTableNameSp).store(sql);
        mysqlpp::Row row = res.at(0);
        now_max_id = (Ice::Long)row["max_id"];
        break;
      } catch (Ice::Exception& e) {
        MCE_WARN("read max id from db " << SendedTableNameSp << "failed, err->" << e);
        continue;
      }
    } 
    sql.clear();
  }
  sql << "select userid,unix_timestamp(time) as time  from " << SendedTableNameSp << " where id between " << start_id << " and " << now_max_id;
  for(unsigned int retry = 0; retry < 3; ++retry) {
    try {
      mysqlpp::StoreQueryResult res =  com::xiaonei::xce::QueryRunner("feed_edm", com::xiaonei::xce::CDbRServer,SendedTableNameSp).store(sql);
      for (unsigned i = 0;i < res.num_rows();++i) {
        mysqlpp::Row row = res.at(i);
        Ice::Int user_id = row["userid"];
        time_t time_stamp = (time_t)row["time"];
        if (difftime(end, time_stamp)< kHourSecond * 24) {
          Add(user_id, kDaily, time_stamp);
        }
      }
      break;
    } catch (Ice::Exception& e) {
      MCE_WARN("Load unfit user from db " << SendedTableNameSp << "failed, err->" << e);
      continue;
    }
  }
  MCE_INFO("load dailyly history date done");
  return true;
}
bool FrequencyCache::_LoadWday() {
  std::string var_name = MyUtil::ServiceI::instance().getName() + "_last_update_time";
  com::xiaonei::xce::Statement sql;
  sql << "select count from " << FlushInfoTableName << " where varname=" << mysqlpp::quote << var_name << mysqlpp::quote;
  try {
    mysqlpp::StoreQueryResult res =  com::xiaonei::xce::QueryRunner("feed_edm", com::xiaonei::xce::CDbRServer, FlushInfoTableName).store(sql);
    if(res.num_rows() != 0) {
      update_wday_ = boost::lexical_cast<Ice::Int>((mysqlpp::Null<string,mysqlpp::NullIsZero>)res.at(0)["count"]);
      MCE_INFO("set update wday:" << update_wday_);
    }
  } catch (Ice::Exception& e) {
    MCE_WARN("read count from db: " << FlushInfoTableName << "failed. err->" << e);
  }
  return true;
}
time_t FrequencyCache::_GetFlushTime() {
  time_t now = time(NULL);
  tm tm_set;
  localtime_r(&now,&tm_set);
  int change;
  if (tm_set.tm_wday >= update_wday_) {
    change = tm_set.tm_wday - update_wday_;
  } else {
    change = 7 + tm_set.tm_wday - update_wday_;
  }
  tm_set.tm_mday -= change;
  tm_set.tm_min = 0;
  tm_set.tm_sec = 0;
  return  mktime(&tm_set);
}
void FrequencyCache::CacheCleaner::handle() {
  if (FrequencyCache::instance().load_sign()) {
    return;
  }
  FrequencyCache::instance().EvictCache();  
}

time_t FrequencyCache::CacheFlusher::last_update_time_ = time(NULL);
FrequencyCache::CacheFlusher::CacheFlusher(Ice::Int s) : Timer(s*1000) {
}
void FrequencyCache::CacheFlusher::handle() {
  if (FrequencyCache::instance().load_sign()) {
    return;
  }
  time_t now = time(NULL);
  // 90秒内不重复执行
  if (difftime(now,last_update_time_) < 90) {
    return;
  }
  tm now_tm;
  localtime_r(&now,&now_tm);
  if (now_tm.tm_wday == FrequencyCache::instance().update_wday() && now_tm.tm_hour == 0 && now_tm.tm_min == 0) {
    MCE_INFO("start flush cache");
    FrequencyCache::instance().FlushCache();
    last_update_time_ = time(NULL);
  }
}

void FrequencyCacheLoadTask::handle() {
  FrequencyCache::instance().LoadFromDb();
}


}
}
