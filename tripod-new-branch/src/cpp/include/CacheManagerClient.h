#ifndef _TRIPOD_CACHEMANAGERCLIENT_H__
#define _TRIPOD_CACHEMANAGERCLIENT_H__

#include <TripodCacheManager.h>

#include <Common.h>
#include <HashHolder.hpp>
#include <ConfigClientFactory.h>
#include <Ice/Ice.h>

namespace com {
namespace renren {
namespace tripod {

class CacheManagerClient {
    friend class ClientFactory;
    protected:
    CacheManagerClient(const std::string& zkAddress);

    public:
    DataMap get(const KeySeq& keys, const std::string& namespaceId, const std::string& businessId, long expiredTime, bool useMaster, const long timeout);

    bool load(const KeySeq& keys, const std::string& namespaceId, const std::string& businessId, long expiredTime, bool useMaster, const long timeout);

    bool deleteCache(const KeySeq& keys, const std::string& namespaceId, const std::string& businessId, const long timeout);

    private:

    CacheManagerConfigClientPtr getConfig();

    private:
    CacheManagerConfigClientPtr config_;
    boost::shared_mutex configMutex_;
    std::string zkAddress_;

 public:
  bool LoadListCache(const std::string& key, const std::string& namespace_id, const std::string& biz_id, bool use_master);
  bool DeleteListCache(const std::string& key, const std::string& namespace_id, const std::string& biz_id);

};

}
}
}
#endif
