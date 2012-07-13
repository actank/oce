#ifndef _TRIPOD_MONITOR_H__
#define _TRIPOD_MONITOR_H__

#include <ServiceI.h>
#include <Monitor/client/service/ServiceMonitor.h>
#include <TripodProducerManager.h>
#include <TripodCacheManager.h>
#include <ConfigClientFactory.h>

namespace com {
namespace renren {
namespace tripod {

const std::string DEFAULT_ZKADDRESS = "UserZooKeeper1:2181/Tripod";

class Monitor : public Ice::Object, public IceUtil::Thread, public MyUtil::Singleton<Monitor> {
    public:
        Monitor();

        virtual ~Monitor();

        void setZkAddress(const std::string& zkAddress);

        void run();

        void setMissPer(long missPer);

    private:

        std::map<std::string, ProducerManagerPrx> getProducer();
        std::map<std::string, CacheManagerPrx> getCacheManager();
        std::map<std::string, MemcachedClientPoolPtr> getCache();

        void logAndAlert(const std::string& str);

        std::string checkStats(const std::map<std::string, std::map<std::string, std::string> > stats);
    
    private:
        ProducerManagerConfigClientPtr producer_;
        CacheConfigClientPtr cache_;
        CacheManagerConfigClientPtr cacheManager_;
        IceUtil::Mutex producerMutex_;
        IceUtil::Mutex cacheMutex_;
        IceUtil::Mutex cacheManagerMutex_;
        std::string zkAddress_;

        std::map<std::string, std::map<std::string, std::string> > lastMemStats_;

        float missPer_;

};

}
}
}
#endif

