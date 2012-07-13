#ifndef __MEMCACHE_CLIENT_H__
#define __MEMCACHE_CLIENT_H__

#include <set>
#include <libmemcached/memcached.h>
#include "Singleton.h"
#include "Evictor.h"
#include "AdStruct.h"

namespace xce {
namespace ad {

class MemcacheClient : public MyUtil::Singleton<MemcacheClient> {
public:
  MemcacheClient();
  virtual ~MemcacheClient();
  void config();
  bool Init();
  bool Filter(int uid, std::set<AdGroupPtr>& groups);
  bool append(const string& key, const string& value);
  bool get(const string& key, std::set<Ice::Long>& value);

private:
  bool init_;
  SizedEvictor<int, std::set<Ice::Long> > local_cache_;
  IceUtil::RWRecMutex mutex_;
};

} // end namespace ad
} // end namespace xce

#endif //__MEMCACHE_CLIENT_H__
