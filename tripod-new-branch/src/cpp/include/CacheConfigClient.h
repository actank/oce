#ifndef _TRIPOD_CACHECONFIGCLIENT_H__
#define _TRIPOD_CACHECONFIGCLIENT_H__

#include <TripodData.h>

#include <Common.h>
#include <ConfigClient.h>
#include <HashHolder.hpp>
#include <MemcachedClientPool.h>

namespace com {
namespace renren {
namespace tripod {

class CacheConfigClient : public ConfigClient {
    friend class ConfigClientFactory;
    typedef boost::shared_ptr<HashHolder<MemcachedClientPoolPtr> > MemcachedClientPoolHashHolderPtr;
    protected:
    CacheConfigClient(const std::string& zkAddress);

    public:
    virtual bool update(const ConfigDataPtr& newConfig);

    public:
    std::map<MemcachedClientPoolPtr, KeySeq> getPrxToKeysMap(const KeySeq& keys, const std::string& namespaceId);
    MemcachedClientPoolPtr getPrx(const std::string& key, const std::string& namespaceId);

    std::map<std::string, MemcachedClientPoolPtr> getOriginalPrxMap();

    KeySeq getFilterKeys(const KeySeq& keys, const std::vector<std::string>& prxVec, const std::string& namespaceId);

    private:
    std::map<std::string, MemcachedClientPoolHashHolderPtr> hashHolderMap_;
    boost::shared_mutex hashHolderMaplock_;

};

}
}
}
#endif
