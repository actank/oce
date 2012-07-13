#ifndef _TRIPOD_REDIS_CACHE_CONFIG_CLIENT_H__
#define _TRIPOD_REDIS_CACHE_CONFIG_CLIENT_H__
#include <TripodData.h>

#include <Common.h>
#include <ConfigClient.h>
#include <HashHolder.hpp>
#include <RedisClientPool.h>

namespace com {
namespace renren {
namespace tripod {

class RedisCacheConfigClient : public ConfigClient {
 public:
  virtual bool update(const ConfigDataPtr& new_config);
  std::map<RedisClientPoolPtr, KeySeq> GetPrxToKeysMap(const KeySeq& keys, const std::string& namespace_id);
  RedisClientPoolPtr GetPrx(const std::string& key, const std::string& namespace_id);
 protected:
  RedisCacheConfigClient(const std::string& zk_address);
 private:
  friend class ConfigClientFactory;
  typedef boost::shared_ptr<HashHolder<RedisClientPoolPtr> > RedisClientPoolHashHolderPtr;
  // key:namespace
  std::map<std::string, RedisClientPoolHashHolderPtr> hash_holder_map_;
  boost::shared_mutex hash_holder_map_lock_;
};

}}} //end com::renren::tripod


#endif

