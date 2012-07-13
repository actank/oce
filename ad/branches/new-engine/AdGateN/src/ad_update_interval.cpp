#include "ad_update_interval.h"

#include <boost/algorithm/string/predicate.hpp>

#include "QueryRunner.h"
#include "AdGateI.h"

using namespace xce::ad;
using namespace com::xiaonei::xce;
using namespace std;
using namespace MyUtil;

void AdUpdateIntervalReloadTask::handle() {
  AdUpdateIntervalCache::instance().Reload();
}

void AdUpdateIntervalCache::Reload() {
  Statement sql;
  sql << "SELECT url, update_interval, max_update_count FROM ad_update_interval";

  map<string, pair<int, int> > url_update_interval;
  try {
    MCE_INFO("AdUpdateIntervalCache::Reload() --> db_instance = " << db_instance);
    mysqlpp::StoreQueryResult res = QueryRunner(db_instance, CDbRServer).store(sql);
    
    if (!res) {
      MCE_WARN("select table update_interval error.");
      return;
    }

    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      string url(row["url"].c_str()); 
      if (boost::starts_with(url, "http://")) {
        url = url.substr(strlen("http://"));
      }
      url_update_interval.insert(make_pair(url, make_pair((int)row["update_interval"], (int)row["max_update_count"])));
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
      it = url_update_interval_.find(u);
      if (it != url_update_interval_.end()) {
        return it->second;
      }
      pos = u.find_last_of('/');
      if (pos == string::npos) {
        break;
      }
      u = u.substr(0, pos);
    }
  }
  return make_pair(60, 4);
};

