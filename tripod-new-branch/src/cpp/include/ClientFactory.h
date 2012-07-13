#ifndef _TRIPOD_CLIENTFACTORY_H__
#define _TRIPOD_CLIENTFACTORY_H__

#include <Common.h>
#include <Singleton.hpp>

namespace com {
namespace renren {
namespace tripod {

class ClientFactory : public Singleton<ClientFactory> {
    public:
        CacheManagerClientPtr getCacheManagerClient(const std::string& zkAddress);

        ProducerManagerClientPtr getProducerManagerClient(const std::string& zkAddress);

        CacheClientPtr getCacheClient(const std::string& zkAddress);

        RedisCacheClientPtr GetRedisCacheClient(const std::string& zk_address);

    private:
        std::map<std::string, CacheManagerClientPtr> cacheManagerClientMap_;

        boost::shared_mutex cacheManagerClientMaplock_;

        std::map<std::string, ProducerManagerClientPtr> producerManagerClientMap_;

        boost::shared_mutex producerManagerClientMaplock_;

        std::map<std::string, CacheClientPtr> cacheClientMap_;

        boost::shared_mutex cacheClientMaplock_;

        std::map<std::string, RedisCacheClientPtr> redis_cache_client_map_;

        boost::shared_mutex redis_cache_client_map_lock_;
};

}
}
}
#endif
