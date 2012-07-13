#include "Monitor.h"
#include "CacheConfigClient.h"
#include "ProducerManagerConfigClient.h"
#include "CacheManagerConfigClient.h"

void MyUtil::initialize() {
    MyUtil::ServiceI& service = MyUtil::ServiceI::instance();

  std::string zkAddress = service.getCommunicator()->getProperties()->getPropertyWithDefault("ZkAddress", com::renren::tripod::DEFAULT_ZKADDRESS);
  long missPer = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("MissPercent", 20)/100;
  com::renren::tripod::Monitor::instance().setZkAddress(zkAddress);
  com::renren::tripod::Monitor::instance().setMissPer(missPer);
	
  service.getAdapter()->add(&com::renren::tripod::Monitor::instance(), service.createIdentity("M", ""));

  com::renren::tripod::Monitor::instance().start().detach();

  MyUtil::ServiceMonitorManager::instance().start();
}

namespace com {
namespace renren {
namespace tripod {

Monitor::Monitor() {
}

Monitor::~Monitor() {
}

void Monitor::setZkAddress(const std::string& zkAddress) {
    zkAddress_ = zkAddress;
}

void Monitor::setMissPer(long missPer) {
    missPer_ = missPer;
}

void Monitor::run() {
    while(true) {
        sleep(10);
        MCE_DEBUG("Monitor::run check start");
        std::ostringstream os;
        {
            std::map<std::string, ProducerManagerPrx> prxs = getProducer();
            if(prxs.empty()) {
                os<<"No Producer in config \r";
            }
            for(std::map<std::string, ProducerManagerPrx>::const_iterator it = prxs.begin(); it != prxs.end(); ++it) {
                try {
                    it->second->ice_ping();
                }catch(Ice::Exception& e) {
                    os<<"ProducerManagerPrx : "<<it->second->ice_toString()<<" Ice::Exception "<<e.what()<<"\r";
                }catch(...) {
                    os<<"ProducerManagerPrx : "<<it->second->ice_toString()<<" UnknownException \r";
                }
            }
        }
        {
            std::map<std::string, CacheManagerPrx> prxs = getCacheManager();
            if(prxs.empty()) {
                os<<"No CacheManager in config!\r";
            }
            for(std::map<std::string, CacheManagerPrx>::const_iterator it = prxs.begin(); it != prxs.end(); ++it) {
                try {
                    it->second->ice_ping();
                }catch(Ice::Exception& e) {
                    os<<"CacheManagerPrx : "<<it->second->ice_toString()<<" Ice::Exception "<<e.what()<<"\r";
                }catch(...) {
                    os<<"CacheManagerPrx : "<<it->second->ice_toString()<<" UnknownException \r";
                }
            }
        }
        {
            std::map<std::string, MemcachedClientPoolPtr> prxs = getCache();
            if(prxs.empty()) {
                os<<"No Cache in config!\r";
            }
            std::map<std::string, std::map<std::string, std::string> > memStats;
            for(std::map<std::string, MemcachedClientPoolPtr>::const_iterator it = prxs.begin(); it != prxs.end(); ++it) {
                std::map<std::string, std::map<std::string, std::string> > stats;
                memcache::Memcache* mem = it->second->borrowItem();
                try {
                    mem->getStats(stats);
                    if(stats.empty()) {
                        os<<"CachePrx : "<<it->first<<" UnknownException \r";
                    }else {
                        memStats.insert(make_pair(it->first, stats.begin()->second));
                    }
                }catch(...) {
                    os<<"CachePrx : "<<it->first<<" UnknownException \r";
                }
                it->second->returnItem(mem);
            }
            std::string memStr = checkStats(memStats);
            if(memStr.length() != 0) {
                os<<memStr;
            }
        }
        if(os.str().length()) {
            logAndAlert(os.str());
        }
        MCE_DEBUG("Monitor::run check end");
    }
}

std::string Monitor::checkStats(const std::map<std::string, std::map<std::string, std::string> > stats) {
    if(lastMemStats_.empty()) {
        return std::string();
    }

    std::ostringstream os;
    
    for(std::map<std::string, std::map<std::string, std::string> >::const_iterator it = stats.begin(); it != stats.end(); ++it) {
        std::map<std::string, std::map<std::string, std::string> >::const_iterator findIt = lastMemStats_.find(it->first);
        if(findIt == lastMemStats_.end()) {
            continue;
        }
      
        long miss = 0;
        std::map<std::string, std::string>::const_iterator sFindIt = it->second.find("get_misses");
        if(sFindIt != it->second.end()) {
            long newL = boost::lexical_cast<long>(sFindIt->second);
            sFindIt = findIt->second.find("get_misses");
            if(sFindIt != findIt->second.end()) {
                long oldL = boost::lexical_cast<long>(sFindIt->second);
                miss = newL - oldL;
            }
        }
        
        long cmdGet = 0;
        sFindIt = it->second.find("cmd_get");
        if(sFindIt != it->second.end()) {
            long newL = boost::lexical_cast<long>(sFindIt->second);
            sFindIt = findIt->second.find("cmd_get");
            if(sFindIt != findIt->second.end()) {
                long oldL = boost::lexical_cast<long>(sFindIt->second);
                cmdGet = newL - oldL;
            }
        }
        
        long evictions = 0;
        sFindIt = it->second.find("evictions");
        if(sFindIt != it->second.end()) {
            evictions = boost::lexical_cast<long>(sFindIt->second);
        }

        if(miss && cmdGet) {
            if((miss / cmdGet) > missPer_) {
                os<<"CachePrx : "<<it->first<<" missed too much, missper "<<miss/cmdGet<<" maxper "<<missPer_<<"\r";
            }
        }
        if(evictions) {
            os<<"CachePrx : "<<it->first<<" eviction occurs, evictions "<<evictions<<" \r";
        }
    }
    return os.str();
}

void Monitor::logAndAlert(const std::string& str) {
    MCE_WARN(str);
}

std::map<std::string, ProducerManagerPrx> Monitor::getProducer() {
    if(producer_ == NULL) {
        IceUtil::Mutex::Lock lock(producerMutex_);
        producer_ = ConfigClientFactory::getInstance()->getProducerManagerConfigClient(zkAddress_);
    }
    return producer_->getOriginalPrxMap();
}

std::map<std::string, CacheManagerPrx> Monitor::getCacheManager() {
    if(cacheManager_ == NULL) {
        IceUtil::Mutex::Lock lock(cacheManagerMutex_);
        cacheManager_ = ConfigClientFactory::getInstance()->getCacheManagerConfigClient(zkAddress_);
    }
    return cacheManager_->getOriginalPrxMap();
}

std::map<std::string, MemcachedClientPoolPtr> Monitor::getCache() {
    if(cache_ == NULL) {
        IceUtil::Mutex::Lock lock(cacheMutex_);
        cache_ = ConfigClientFactory::getInstance()->getCacheConfigClient(zkAddress_);
    }
    return cache_->getOriginalPrxMap();
}


}
}
}
