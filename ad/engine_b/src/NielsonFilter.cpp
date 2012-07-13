#include <boost/lexical_cast.hpp>
#include <connection.h>
#include <query.h>
#include "IceLogger.h"
#include "AdConfig.h"
#include "NielsonFilter.h"

namespace xce {
namespace ad {
//using namespace std;
using namespace mysqlpp;

bool NielsonFilter::Filter(const UserProfile& userProfile, set<AdGroupPtr>& groups) {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  MCE_DEBUG("NielsonFilter::Filter() --> uid = " << userProfile.id() << ", groups.size() = " << groups.size());
  bool result = true;
  if(groups.empty()) {
    MCE_DEBUG("NielsonFilter::Filter() --> uid = " << userProfile.id() << ", no group");
    return result;
  }
  int uid_tail = userProfile.id()%10;
  if(uid_tails_.find(uid_tail) == uid_tails_.end()) { 
    uid_tail = userProfile.id()%100;
    if(uid_tails_.find(uid_tail) == uid_tails_.end()) {
      MCE_DEBUG("NielsonFilter::Filter() --> uid = " << userProfile.id() << ", no filter rule, ok");
      return result;
    }
  }
  map<int, pair<int, bool> >::iterator rule = permissions_.find(uid_tail);
  if(rule != permissions_.end()) {
    int ad_type = rule->second.first;
    bool permission_flag = rule->second.second;
    MCE_DEBUG("NielsonFilter::Filter() --> uid = " << userProfile.id() << ", uid_tail = " << uid_tail << ", ad_type = " << ad_type << ", permission_flag = " << permission_flag);
    if(permission_flag) { //仅允许ad_type类型的广告
      for(set<AdGroupPtr>::iterator it = groups.begin(); it != groups.end();) {
        int group_ad_type = (*it)->ad_type();
        MCE_DEBUG("NielsonFilter::Filter() --> group_id = " << (*it)->group_id() << ", group_ad_type = " << group_ad_type);
        if(group_ad_type != 0 && group_ad_type != ad_type) {
          result = false;
          MCE_DEBUG("NielsonFilter::Filter() --> group_id = " << (*it)->group_id() << " be erased");
          groups.erase(it++);
        } else {
          ++it;
        }
      }
    } else { //不允许ad_type类型的广告, 如果是3, 表示1,2都不允许
      for(set<AdGroupPtr>::iterator it = groups.begin(); it != groups.end();) {
        int group_ad_type = (*it)->ad_type();
        MCE_DEBUG("NielsonFilter::Filter() --> group_id = " << (*it)->group_id());
        if(group_ad_type == ad_type) {
          result = false;
          MCE_DEBUG("NielsonFilter::Filter() --> group_id = " << (*it)->group_id() << " be erased");
          groups.erase(it++);
        } else if(ad_type == 3 && group_ad_type != 0) {
          result = false;
          MCE_DEBUG("NielsonFilter::Filter() --> group_id = " << (*it)->group_id() << " be erased");
          groups.erase(it++);
        } else {
          ++it;
        }
      }
    }
  }
  
  return result;
}

bool NielsonFilter::Filter(int uid, Ice::Long group_id, int& permission_ad_type) {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  int uid_tail = uid%10;
  if(uid_tails_.find(uid_tail) == uid_tails_.end()) { 
    uid_tail = uid%100;
    if(uid_tails_.find(uid_tail) == uid_tails_.end()) {
      MCE_DEBUG("NielsonFilter::Filter() --> uid = " << uid << ", no filter rule, ok");
      return false;
    }
  }
  if(groups_.find(group_id) == groups_.end()) {
    return false;
  }

  map<int, pair<int, bool> >::iterator rule = permissions_.find(uid_tail);
  if(rule != permissions_.end()) {
    int ad_type = rule->second.first;
    bool permission_flag = rule->second.second;
    MCE_DEBUG("NielsonFilter::Filter() --> uid = " << uid << ", uid_tail = " << uid_tail << ", ad_type = " << ad_type << ", permission_flag = " << permission_flag);
    if(permission_flag) { //仅允许ad_type类型的广告
      permission_ad_type = ad_type; 
      return true;
    } else { //不允许ad_type类型的广告, 如果是3, 表示1,2都不允许
      if(ad_type == 3) {
        permission_ad_type = 0; 
        return true;
      } else {
        if(2 == ad_type) {
          permission_ad_type = 1;
        } else {
          permission_ad_type = 2;
        }
        return true;
      }
    }
  
  }
  return false;//不受限制
}

bool NielsonFilter::Init() {
  try{
    string db_host = EngineConfig::instance().NielsonDB();
    MCE_INFO("NielsonFilter::Init() --> NielsonDB == " << db_host);
    map<int, pair<int, bool> > temp_permissions;
    set<int> uid_tails;
    {
      Connection conn("jebe_ads", db_host.c_str(), "ad", "adjb###");
      //Connection conn("engine", "10.3.16.119", "ad", "rrad");
      string s("select * from nielson_test where begin_time <= now() and end_time >= now();");
      Query q(&conn,true,s.c_str());
      StoreQueryResult result = q.store();
      for(int i = 0; i < result.size(); i++) {
        Row row = result.at(i);
        int uid = (int)row["uid_tail"];
        int ad_type = (int)row["ad_type"];
        bool permission = (bool)row["permission"];
        uid_tails.insert(uid);
        temp_permissions[uid] = pair<int, bool>(ad_type, permission);
        MCE_DEBUG("NielsonFilter::Init() --> uid_tail = " << uid << ", ad_type = " << ad_type << ", permission = " << permission);
      }
      MCE_DEBUG("NielsonFilter::Init() --> permissions.size() = " << temp_permissions.size());
    }
    set<Ice::Long> groups;
    {
      Connection conn("jebe_ads", db_host.c_str(), "ad", "adjb###");
      string s("select adgroup_id from nielsen_group where begin_time <= now() and end_time >= now();");
      Query q(&conn,true,s.c_str());
      StoreQueryResult result = q.store();
      for(int i = 0; i < result.size(); i++) {
        Row row = result.at(i);
        Ice::Long group_id = (Ice::Long)row["adgroup_id"];
        groups.insert(group_id);
        MCE_DEBUG("NielsonFilter::Init() --> group = " << group_id);
      }
      MCE_DEBUG("NielsonFilter::Init() --> groups.size() = " << groups.size());
    }
    {
      IceUtil::RWRecMutex::WLock lock(mutex_);
      permissions_.swap(temp_permissions);
      groups_.swap(groups);
      uid_tails_.swap(uid_tails);
    }
    return true;
  } catch(...) {
    MCE_WARN("NielsonFilter::Init() --> failed!!!");
  }
  return false;
}

void NielsonFilter::ReloadTimer::handle() {
  NielsonFilter::instance().Init();
  TaskManager::instance().schedule(new ReloadTimer());
}

}
}
