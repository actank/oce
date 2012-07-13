#ifndef _TRIPOD_CACHECLIENT_H__
#define _TRIPOD_CACHECLIENT_H__

#include <TripodData.h>

#include <Common.h>
#include <ConfigClientFactory.h>
#include <HashHolder.hpp>
#include <MemcachedClientPool.h>

namespace com {
namespace renren {
namespace tripod {

const int MEM_FLAG_BYTEARR = 4096;

class CacheClient {
    friend class ClientFactory;
    protected:
    CacheClient(const std::string& zkAddress);

    public:
    DataMap get(const KeySeq& keys, const std::string& namespaceId, const std::string& businessId, const long timeout);
    
    bool set(const std::string& key, const Data& value, const std::string& namespaceId, const std::string& businessId, int expiredTime, const long timeout);
    
    bool set(const DataMap& data, const std::string& namespaceId, const std::string& businessId, int expiredTime, const long timeout);
    
    bool remove(const std::string& key, const std::string& namespaceId, const std::string& businessId, const long timeout);
    
    bool remove(const KeySeq& keys, const std::string& namespaceId, const std::string& businessId, const long timeout);

    private:
    DataMap get(const MemcachedClientPoolPtr& mcpp, const KeySeq& keys, const std::string& businessId, const long timeout);

    std::string getBigKey(const std::string& key, const std::string& businessId);
    std::string getKey(const std::string& bigKey, const std::string& businessId);

    CacheConfigClientPtr getConfig();

    private:
    CacheConfigClientPtr config_;
    boost::shared_mutex configMutex_;
    std::string zkAddress_;

};

}
}
}
#endif
