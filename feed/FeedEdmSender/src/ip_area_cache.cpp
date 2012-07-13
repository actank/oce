
#include "ip_area_cache.h"
#include <boost/asio/ip/address_v4.hpp>
#include <arpa/inet.h>

#include "ServiceI.h"
#include "QueryRunner.h"

using namespace xce::ad;
using namespace std;
using namespace com::xiaonei::xce;
using namespace MyUtil;

static const char * kIpDbInstance = "ade_db";
static const string kCodeDbInstance = "useradscache_university_info";
static const string kCodeAreaDbName = "ad_china_area";

void IpAreaCacheReloadTask::handle() {
  IpAreaCache::instance().Reload();
}

IpAreaCache::IpAreaCache() : default_area_("") {
}

IpAreaCache::~IpAreaCache() {
}

bool IpAreaCache::Reload() {
  StrMap cache_code2name;
  CacheMap cache_ip2name;
  CacheMap cache_ip2province;
  // 加载 data_schemas 表信息
  {
    Statement sql;
    sql << "SELECT area_code, name FROM " << kCodeAreaDbName;
    for (unsigned int retry = 0; retry < 3; ++retry) {
      try {
        mysqlpp::StoreQueryResult res = QueryRunner(kCodeDbInstance, CDbRServer).store(sql);
        if (!res) {
          MCE_WARN("select area_code table error");
          return false;
        }

        MCE_DEBUG("loaded code size : " << res.num_rows());
        for (size_t i = 0; i < res.num_rows(); ++i) {
          mysqlpp::Row row = res.at(i);
          string code = string("0086") + (string)row["area_code"];
          string name = (string)row["name"];
          cache_code2name.insert(std::make_pair(code,name));
        }
        cache_code2name.insert(std::make_pair("0001000000000000","美国"));
        cache_code2name.insert(std::make_pair("0002000000000000","加拿大"));
        
        break;
      } catch (Ice::Exception& e) {
        MCE_WARN("load db err : " << e);
        if(retry < 2) {
          continue;
        } else {
          return false;
        }
      } catch (...) {
        MCE_WARN("load db unknown err.");
        if(retry < 2) {
          continue;
        } else {
          return false;
        }
      }
    }
    sql.clear();
    sql << "SELECT id, start, end, area_code FROM ad_ip";

    try {
      mysqlpp::StoreQueryResult res = QueryRunner(kIpDbInstance, CDbRServer).store(sql);
      
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
        string area_name = "";
        string province_name = "";
        StrMap::const_iterator ite = cache_code2name.find(area_code);
        if (ite != cache_code2name.end()) {
          area_name = ite->second;
        }
        string province_area_code = area_code.replace(6,10,"0000000000");
        StrMap::const_iterator province_ite = cache_code2name.find(province_area_code);
        if (province_ite != cache_code2name.end()) {
          province_name = province_ite->second;
        }
        if (start < end) {
          cache_ip2name[RangeKey<unsigned int>(start, end)] = area_name;
          cache_ip2province[RangeKey<unsigned int>(start, end)] = province_name;
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

  MCE_INFO("area code ip range load success. item count = " << cache_ip2name.size());

  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    cache_.swap(cache_ip2name);
    province_cache_.swap(cache_ip2province);
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

  return default_area_;
}
string IpAreaCache::GetIpArea2P(const string & ip) const {
  return GetIpArea2P(ntohl(inet_addr(ip.c_str())));
}

string IpAreaCache::GetIpArea2P(unsigned int ip) const {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  CacheMap::const_iterator i = province_cache_.find(ip);
  if (i != province_cache_.end()) {
    return i->second;
  }

  return default_area_;
}
