#include "AdConf.h"

#include <fstream>
#include <boost/regex.hpp>
// #include <boost/algorithm/string.hpp>

#include "LogWrapper.h"
#include "QueryRunner.h"

using namespace com::xiaonei::xce;

const static char * kDbInstance = "ad_db";

namespace xce {
namespace ad {

const char * kSepRegex = "\\^SEP\\^";

void AdConf::ParseRelatedPosts(const string & src) {
  static boost::regex re(kSepRegex);
  boost::sregex_token_iterator i(src.begin(), src.end(), re, -1);
  boost::sregex_token_iterator j;
  unsigned count = 0;
  string title;
  string url;
  while(i != j) {
    if (count % 2 == 0) {
      title = *i++;
    } else {
      url = *i++;
      related_posts_.push_back(make_pair(title, url));
    }
    count++;
  }
}

void AdConfCache::Reload() {
  Statement sql;
  mysqlpp::StoreQueryResult res;

  sql << "select id, pic_url, pic_src, message1, message2, join_button, join_title, not_join_title, related_posts "\
         ", UNIX_TIMESTAMP(start_time) AS start_time, UNIX_TIMESTAMP(end_time) AS end_time "\
         " from ad_hot_promotion where start_time < now() and end_time > now()";
  try {
    res = QueryRunner(kDbInstance, CDbRServer, "ad_hot_promotion").store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("load table ad_hot_promotion : " << e);
  } catch (std::exception& e) {
    MCE_WARN("load table ad_hot_promotion : " << e.what());
  }

  if (!res) {
    MCE_WARN("reload from ad_hot_promotion error.");
    return;
  }

  AdConfMap ad_cache;
  set<int> ad_ids;
  try {
    for(size_t i = 0; i < res.size(); ++i) {
      AdConfPtr config = new AdConf((Ice::Int)res.at(i)["id"]);
      config->pic_url_ = res.at(i)["pic_url"].c_str();
      config->pic_src_ = res.at(i)["pic_src"].c_str();
      config->message1_ = res.at(i)["message1"].c_str();
      config->message2_ = res.at(i)["message2"].c_str();
      config->join_button_ = res.at(i)["join_button"].c_str();
      config->join_title_ = res.at(i)["join_title"].c_str();
      config->not_join_title_ = res.at(i)["not_join_title"].c_str();

      config->start_time_ = (time_t)res.at(i)["start_time"];
      config->end_time_ = (time_t)res.at(i)["end_time"];
      if (res.at(i)["related_posts"].is_null()) {
        config->ParseRelatedPosts(res.at(i)["related_posts"].c_str());
      }

      ad_cache.insert(make_pair(config->id_, config));
      ad_ids.insert(config->id_);
    }
  } catch (std::exception& e) {
    MCE_WARN("process db result error : " << e.what());
  }

  MCE_INFO("reload ad success. count=" << ad_cache.size());

  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    ad_cache_.swap(ad_cache);
    ad_ids_.swap(ad_ids);
  }
}

set<int> AdConfCache::GetAdIds() const {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  return ad_ids_;
}

AdConfPtr AdConfCache::Get(Ice::Int ad_id) const {
  {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    AdConfMap::const_iterator it = ad_cache_.find(ad_id);
    if (it != ad_cache_.end()) {
      return it->second;
    }
  }
  MCE_WARN("AdHotGateI::getAdConf NULL. ad id=" << ad_id);
  return 0;
}

}
}
