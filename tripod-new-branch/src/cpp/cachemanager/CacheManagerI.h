#ifndef _TRIPOD_CACHEMANAGER_I_H__
#define _TRIPOD_CACHEMANAGER_I_H__

#include <ServiceI.h>
#include <TempMutexT.h>
#include <Monitor/client/service/ServiceMonitor.h>
#include <ClientFactory.h>
#include "TripodCacheManager.h"

namespace com {
namespace renren {
namespace tripod {

const std::string DEFAULT_ZKADDRESS = "UserZooKeeper1:2181/Tripod";

class CacheManagerI : virtual public CacheManager, public MyUtil::Singleton<CacheManagerI> {
    public:
        CacheManagerI();

        virtual ~CacheManagerI();

        virtual DataMap get(const KeySeq& keys, const std::string& namespaceId, const std::string& businessId, 
                long expiredTime, bool useMaster, const Ice::Current& = Ice::Current());

        virtual bool load(const KeySeq& keys, const std::string& namespaceId, const std::string& businessId, 
                long expiredTime, bool useMaster, const Ice::Current& = Ice::Current());

        virtual bool deleteCache(const KeySeq& keys, const std::string& namespaceId, const std::string& businessId, const Ice::Current& = Ice::Current());

        void setZkAddress(const std::string& zkAddress);

        virtual bool loadListCache(const std::string& key, const std::string& namespace_id, const std::string& business_id, bool use_master,const Ice::Current& = Ice::Current());

        virtual bool deleteListCache(const std::string& key, const std::string& namespace_id, const std::string& business_id, const Ice::Current& = Ice::Current());

    private:
        ProducerManagerClientPtr getProducer();
        
        CacheClientPtr getCache();

    private:
        xce::tempmutext::TempMutexManagerT<std::string> tempMutexManager_;
        ProducerManagerClientPtr producer_;
        CacheClientPtr cache_;
        IceUtil::Mutex producerMutex_;
        IceUtil::Mutex cacheMutex_;
        std::string zkAddress_;

 private:
  RedisCacheClientPtr GetRedisCache();

 private:
  IceUtil::Mutex redis_cache_mutex_;
  RedisCacheClientPtr redis_cache_;
};

}
}
}
#endif

