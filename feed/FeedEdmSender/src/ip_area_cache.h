#ifndef _AD_IP_AREA_CACHE_H_
#define _AD_IP_AREA_CACHE_H_

#include <string>
#include <map>
#include <IceUtil/RWRecMutex.h>

#include "Singleton.h"
#include "TaskManager.h"
#include "range_key.h"

namespace xce {
namespace ad {

using namespace  MyUtil;

class IpAreaCache : public MyUtil::Singleton<IpAreaCache> {
public:
  IpAreaCache();
  ~IpAreaCache();

  bool Reload();
  string GetIpArea(unsigned int ip) const;
  string GetIpArea(const string & ip) const;
  string GetIpArea2P(unsigned int ip) const;
  string GetIpArea2P(const string & ip) const;
private:

  mutable IceUtil::RWRecMutex mutex_;
  typedef map<string,string> StrMap;
  typedef map<RangeKey<unsigned int>, string> CacheMap;
  CacheMap cache_;
  CacheMap province_cache_;
  string default_area_;
};

class IpAreaCacheReloadTask : public Task {
public:
  IpAreaCacheReloadTask () {}
  virtual void handle();
};

}
}

#endif // _AD_IP_AREA_CACHE_H_

