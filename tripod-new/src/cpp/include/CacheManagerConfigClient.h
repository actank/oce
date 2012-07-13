#ifndef _TRIPOD_CACHEMANAGERCONFIGCLIENT_H__
#define _TRIPOD_CACHEMANAGERCONFIGCLIENT_H__

#include <TripodCacheManager.h>

#include <Common.h>
#include <HashHolder.hpp>
#include <ConfigClient.h>
#include <Ice/Ice.h>

namespace com {
namespace renren {
namespace tripod {

class CacheManagerConfigClient : public ConfigClient {
    friend class ConfigClientFactory;
    protected:
    CacheManagerConfigClient(const std::string& zkAddress);

    public:
    virtual bool update(const ConfigDataPtr& newConfig);

    public:

    std::map<CacheManagerPrx, KeySeq> getPrxToKeysMap(const KeySeq& keys);
    
    std::map<std::string, CacheManagerPrx> getOriginalPrxMap();

    private:
    Ice::CommunicatorPtr ic_;

    HashHolder<CacheManagerPrx> hashHolder_;

};

}
}
}
#endif
