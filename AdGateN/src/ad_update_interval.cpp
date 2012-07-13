#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <mysql++.h>
#include "AdGateI.h"
#include "ad_update_interval.h"
#include "AdGateConfig.h"

using namespace xce::ad;
using namespace com::xiaonei::xce;
using namespace std;
using namespace MyUtil;

void AdUpdateIntervalReloadTimer::handle() {
  AdUpdateIntervalCache::instance().Reload();
  TaskManager::instance().schedule(new AdUpdateIntervalReloadTimer());
}

void AdUpdateIntervalCache::Reload() {
  string s = "select config_key, value from config";
  map<string, pair<int, int> > url_update_interval;
  try {
    mysqlpp::Connection conn("jebe_ads", GateConfig::instance().flush_db().c_str(), "ad", "adjb###");
    mysqlpp::Query q(&conn,true,s.c_str());
    mysqlpp::StoreQueryResult res = q.store();
    MCE_INFO("AdUpdateIntervalCache::Reload --> res.size() = " << res.size());
    
    if (!res) {
      MCE_WARN("select table update_interval error.");
      return;
    }

    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      string url(row["config_key"]); 
      if (boost::starts_with(url, "http://")) {
        url = url.substr(strlen("http://"));
      }
      string value(row["value"]); 
      int update_interval = 40;
      int max_update_count = 10;
      vector<string> tokens;
      boost::algorithm::split_regex(tokens, value, boost::regex(","));
      if(tokens.size() == 2) {
        update_interval = boost::lexical_cast<int>(tokens[0]);
        max_update_count = boost::lexical_cast<int>(tokens[1]);
      }
      MCE_DEBUG("AdUpdateIntervalCache::Reload --> url = " << url << ", update_interval = " << update_interval << ", max_update_count = " << max_update_count);
      url_update_interval.insert(make_pair(url, make_pair(update_interval, max_update_count)));
    }
  } catch (std::exception& e) {
    MCE_WARN("load db update_interval err : " << e.what());
    return;
  } catch (...) {
    MCE_WARN("load db update_interval unknown err.");
    return;
  }

  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    url_update_interval_.swap(url_update_interval);
  }
}

pair<int, int> AdUpdateIntervalCache::GetUpdateInterval(const string & url) {
  string u;
  size_t pos = url.find_first_of('?');
  if (pos != string::npos) {
    u = url.substr(0, pos);
  } else {
    u = url;
  }
 
  if (boost::starts_with(u, "http://")) {
    u = u.substr(strlen("http://"));
  }
 
  map<string, pair<int, int> >::const_iterator it;
  {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    while(true) {
      MCE_DEBUG("AdUpdateIntervalCache::GetUpdateInterval --> u = " << u);
      it = url_update_interval_.find(u);
      if (it != url_update_interval_.end()) {
        MCE_DEBUG("got update config: u = " << u << ", time(s) = " << it->second.first << ", times = " << it->second.second);
        return it->second;
      }
      pos = u.find_last_of('/');
      if (pos == string::npos) {
        break;
      }
      u = u.substr(0, pos);
    }

    //默认刷新策略
    it = url_update_interval_.find("default");
    if (it != url_update_interval_.end()) {
      MCE_DEBUG("got default update config: time(s) = " << it->second.first << ", times = " << it->second.second);
      return it->second;
    }
  }
  MCE_DEBUG("no update config in u = " << u);
  return make_pair(60, 4);
};

