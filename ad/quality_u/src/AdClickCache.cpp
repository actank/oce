#include <cmath>
#include "AdClickCache.h"
#include "QueryRunner.h"
#include "FeedMemcProxy/client/user_profile_client.h"

namespace xce {
namespace ad {
using namespace MyUtil;
using namespace std;
using namespace boost;
using namespace com::xiaonei::xce;

const string DB_INSTANCE = "ads_db";

map<int, double> AdClickStat::AgeFactors() {
  if (!age_factors_.empty()) {
    return age_factors_;
  }
  int total_clicks = 0;
  map<int, int> age_clicks;
  const static int kMaxAge = 99;
  const static int AGE_SECTIONS[] = {18, 22, 25, 29, 35, 45, 60, 100};
  for(size_t i = 0; i < sizeof(AGE_SECTIONS) / sizeof(AGE_SECTIONS[0]); ++i) {
    age_clicks[AGE_SECTIONS[i]] = 0;
  }

  for(map<int, int>::const_iterator it = user_clicks_.begin(); it != user_clicks_.end(); ++it) {
    int uid = it->first;
    if (uid <= 0 || uid % 10 < 5 || uid % 10 == 8) {
      continue;
    }
    string serialized;
    if(!UserProfileClient::instance().GetSerialized(uid, &serialized)) {
      continue;
    }
    UserProfile profile;
    profile.ParseFromString(serialized);
    int age = profile.age() < kMaxAge ? profile.age() : kMaxAge;
    map<int, int>::iterator it = age_clicks.lower_bound(age);
    if (it != age_clicks.end()) {
      ++it->second;
      ++total_clicks;
    }
  }
    
  stringstream ss;
  ss.precision(3);
  ss.setf(ios::fixed,ios::floatfield);
  ss << "ad " << gid_ << " click age ";

  if (total_clicks > 400) {
    for(map<int, int>::iterator it = age_clicks.begin(); it != age_clicks.end(); ++it) {
      age_factors_[it->first] = (double)(it->second) / (double)total_clicks;
      ss << setw(6) << it->first << "-" << setw(6) << it->second << "-" << setw(7) << (double)(it->second) / total_clicks << " ";
    }
    ss << " total_clicks=" << setw(6) << total_clicks;
  } else {
    age_factors_[-1] = 0.0;
  }
  MCE_DEBUG(ss.str());
  return age_factors_;
}

double AdClickStat::GenderFactor() {
  if (gender_factor_ >= 0.0) {
    return gender_factor_;
  }

  long male_count = 0, female_count = 0;
  for(map<int, int>::const_iterator it = user_clicks_.begin(); it != user_clicks_.end(); ++it) {
    int uid = it->first;
    // if (uid <= 0 || uid % 10 < 5 || uid % 10 == 8) {
    // if (uid <= 0 || uid % 10 < 5) {
    if (uid <= 0) {
      continue;
    }
    string serialized;
    if(!UserProfileClient::instance().GetSerialized(uid, &serialized)) {
      continue;
    }
    UserProfile profile;
    profile.ParseFromString(serialized);
    if (profile.gender() == 1) {
      male_count += it->second;
    } else {
      female_count += it->second;
    }
  }
  double factor = 0.0;
  if (male_count + female_count >= 40) {
    factor = (double)male_count / (double)(male_count + female_count);
  }
  MCE_DEBUG("ad " << gid_ << " male_click=" << male_count << " female_click=" << female_count << " gender_factor=" << factor);
  gender_factor_ = factor;
  return factor;
}

bool AdClickCache::Reload() {
  MCE_INFO("to load click info ...");
  {
    IceUtil::Mutex::Lock lock(mutex_);
    if (loading_) {
      MCE_INFO("loading...");
      return false;
    }
  }
  loading_ = true;

  Statement sql;
  sql << "select group_id, user_id from ad_user_click WHERE time > FROM_UNIXTIME(" << time(NULL) - 3 * 24 * 60 * 60 << ")";

  map<Ice::Long, AdClickStatPtr> ad_user_clicks;
  bool load_success = false;
  try {
    mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer, "adn").store(sql);
    if (res && res.num_rows() > 0) {
      for(size_t i = 0; i < res.num_rows(); ++i) {
        mysqlpp::Row row = res.at(i);
        Ice::Long group_id = (Ice::Long)row["group_id"];
      
        AdClickStatMap::iterator group_it = ad_user_clicks.find(group_id);
        if (group_it == ad_user_clicks.end()) {
          AdClickStatPtr p = new AdClickStat(group_id);
          p->AddUserClick((int)row["user_id"], 1);
          ad_user_clicks.insert(make_pair(group_id, p));
        } else {
          group_it->second->AddUserClick((int)row["user_id"], 1);
        }
      }
      load_success = true;
      MCE_INFO("ad_user_clicks.size()=" << ad_user_clicks.size());
    }
  } catch (std::exception& e) {
    MCE_WARN("load ad_user_click error: " << e.what());
  } catch (...) {
    MCE_WARN("load ad_user_click error!");
  }
  if (load_success) {
    IceUtil::Mutex::Lock lock(mutex_);
    ad_user_clicks.swap(ad_user_clicks_);
  }
  loading_ = false;
  MCE_INFO("finish loading click info ...");
  return load_success;
}


double AdClickCache::GenderFactor(Ice::Long gid) {
  AdClickStatPtr ad_click_stat;
  {
    IceUtil::Mutex::Lock lock(mutex_);
    AdClickStatMap::iterator it = ad_user_clicks_.find(gid);
    if (it != ad_user_clicks_.end()) {
      ad_click_stat = it->second;
    }
  }
  if (ad_click_stat) {
    return ad_click_stat->GenderFactor();
  }
  return 0.0;
}

map<int, double> AdClickCache::AgeFactors(Ice::Long gid) {
  AdClickStatPtr ad_click_stat;
  {
    IceUtil::Mutex::Lock lock(mutex_);
    AdClickStatMap::iterator it = ad_user_clicks_.find(gid);
    if (it != ad_user_clicks_.end()) {
      ad_click_stat = it->second;
    }
  }
  if (ad_click_stat) {
    return ad_click_stat->AgeFactors();
  }
  return map<int, double>();
}

void AdClickCache::handle() {
  Reload();
}

}
}
