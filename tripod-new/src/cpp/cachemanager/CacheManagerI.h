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

//const std::string DEFAULT_ZKADDRESS = "UserZooKeeper1:2181/Tripod";
const std::string DEFAULT_ZKADDRESS = "10.22.206.32:2181/Tripod";

class CacheManagerI : virtual public CacheManager, public MyUtil::Singleton<CacheManagerI> {
    public:
        CacheManagerI();

        virtual ~CacheManagerI();

        virtual DataMap get(const KeySeq& keys, const std::string& namespaceId, const std::string& businessId, 
                int expiredTime, bool useMaster, const Ice::Current& = Ice::Current());

        virtual bool load(const KeySeq& keys, const std::string& namespaceId, const std::string& businessId, 
                int expiredTime, bool useMaster, const Ice::Current& = Ice::Current());

        virtual bool deleteCache(const KeySeq& keys, const std::string& namespaceId, const std::string& businessId, const Ice::Current& = Ice::Current());

        void setZkAddress(const std::string& zkAddress);

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

 public:
  bool loadListCache(const std::string&, const std::string&, const std::string&, bool, const Ice::Current& = Ice::Current());
  bool deleteListOrHashCache(const std::string&, const std::string&, const std::string&, const Ice::Current& = Ice::Current());

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

