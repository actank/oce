
#include "ip_area_cache.h"
#include <boost/asio/ip/address_v4.hpp>
#include <arpa/inet.h>

#include "ServiceI.h"
#include "QueryRunner.h"
#include "AdGateI.h"

using namespace xce::ad;
using namespace std;
using namespace com::xiaonei::xce;
using namespace MyUtil;

void IpAreaCacheReloadTask::handle() {
  IpAreaCache::instance().Reload();
}

IpAreaCache::IpAreaCache() : default_area_("0086000000000000"), ready_(false) {
}

IpAreaCache::~IpAreaCache() {
}

bool IpAreaCache::Reload() {
  CacheMap cache;

  // 加载 data_schemas 表信息
  MCE_INFO("IpAreaCache::Reload() --> db_instance = " << db_instance);
  {
    Statement sql;

    sql << "SELECT id, start, end, area_code FROM ad_ip";

    try {
      mysqlpp::StoreQueryResult res = QueryRunner(db_instance, CDbRServer).store(sql);
      
      if (!res) {
        MCE_WARN("select table ad_ip error.");
        return false;
      }

      MCE_DEBUG("loaded ad_ip size : " << res.num_rows());
      for (size_t i = 0; i < res.num_rows(); ++i) {
        mysqlpp::Row row = res.at(i);
        unsigned int start = (unsigned int)row["start"];
        unsigned int end = (unsigned int)row["end"];
        string area_code = row["area_code"].c_str();
        if (start < end) {
          cache[RangeKey<unsigned int>(start, end)] = area_code;
        } else {
          MCE_WARN("load ip range " << (int)row["id"] << ':' << start << '-' << end << " error");
        }
      }
    } catch (std::exception& e) {
      MCE_WARN("load db err : " << e.what());
      return false;
    } catch (...) {
      MCE_WARN("load db unknown err.");
      return false;
    }
  }

  MCE_INFO("area code ip range load success. item count = " << cache.size());

  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    cache_.swap(cache);
  }

  ready_ = true;

  return true;
}

string IpAreaCache::GetIpArea(const string & ip) const {
  return GetIpArea(ntohl(inet_addr(ip.c_str())));
}

string IpAreaCache::GetIpArea(unsigned int ip) const {
  if (ready_) {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    CacheMap::const_iterator i = cache_.find(ip);
    if (i != cache_.end()) {
      return i->second;
    }
  }

  return default_area_;
}

