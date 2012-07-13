
#include "ip_area_cache.h"
#include <boost/asio/ip/address_v4.hpp>
#include <arpa/inet.h>

#include "ServiceI.h"
#include "QueryRunner.h"
#include "AdGateConfig.h"

using namespace xce::ad;
using namespace std;
using namespace com::xiaonei::xce;
using namespace MyUtil;

void IpAreaCacheReloadTask::handle() {
  IpAreaCache::instance().Reload();
}

IpAreaCache::IpAreaCache() : default_area_("0086000000000000") {
}

IpAreaCache::~IpAreaCache() {
}

bool IpAreaCache::Reload() {
  CacheMap cache;

  // 加载 data_schemas 表信息
  {
    Statement sql;

    sql << "SELECT id, start, end, area_code FROM ad_ip";

    try {
      mysqlpp::StoreQueryResult res = QueryRunner(GateConfig::instance().DB(), CDbRServer).store(sql);
      
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

  return true;
}

string IpAreaCache::GetIpArea(const string & ip) const {
  return GetIpArea(ntohl(inet_addr(ip.c_str())));
}

string IpAreaCache::GetIpArea(unsigned int ip) const {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  CacheMap::const_iterator i = cache_.find(ip);
  if (i != cache_.end()) {
    return i->second;
  }

  MCE_INFO("IpAreaCache::GetIpArea() --> ip = " << ip << " get failed!");
  return default_area_;
}

