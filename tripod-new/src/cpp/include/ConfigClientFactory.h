#ifndef _TRIPOD_CONFIGCLIENTFACTORY_H__
#define _TRIPOD_CONFIGCLIENTFACTORY_H__

#include <Common.h>
#include <Singleton.hpp>

namespace com {
namespace renren {
namespace tripod {

class ConfigClientFactory : public Singleton<ConfigClientFactory> {
    public:
        CacheManagerConfigClientPtr getCacheManagerConfigClient(const std::string& zkAddress);

        ProducerManagerConfigClientPtr getProducerManagerConfigClient(const std::string& zkAddress);

        CacheConfigClientPtr getCacheConfigClient(const std::string& zkAddress);

  RedisCacheConfigClientPtr GetRedisCacheConfigClient(const std::string& zk_address);

    private:
        std::map<std::string, CacheManagerConfigClientPtr> cacheManagerConfigClientMap_;

        boost::shared_mutex cacheManagerConfigClientMaplock_;

        std::map<std::string, ProducerManagerConfigClientPtr> producerManagerConfigClientMap_;

        boost::shared_mutex producerManagerConfigClientMaplock_;

        std::map<std::string, CacheConfigClientPtr> cacheConfigClientMap_;

        boost::shared_mutex cacheConfigClientMaplock_;

  std::map<std::string, RedisCacheConfigClientPtr> redis_cache_config_client_map_;
  
  boost::shared_mutex redis_cache_config_client_map_lock_;
};

}
}
}
#endif
