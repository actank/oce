#include <Common.h>
#include <CacheManagerClient.h>
#include <ProducerManagerClient.h>
#include <CacheClient.h>
#include <RedisCacheClient.h>
#include <ClientFactory.h>

namespace com {
namespace renren {
namespace tripod { 

CacheManagerClientPtr ClientFactory::getCacheManagerClient(const std::string& zkAddress) {
    MCE_INFO("ClientFactory::getCacheManagerClient zkAddress " << zkAddress);
    std::map<std::string, CacheManagerClientPtr>::iterator findIt;
    {
        boost::shared_lock<boost::shared_mutex> lock(cacheManagerClientMaplock_);
        findIt = cacheManagerClientMap_.find(zkAddress);
        if( findIt != cacheManagerClientMap_.end() ){
            return findIt->second;
        }
    }
    {
        boost::upgrade_lock<boost::shared_mutex> lock(cacheManagerClientMaplock_);
        boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock1(lock);
        findIt = cacheManagerClientMap_.find(zkAddress);
        if( findIt != cacheManagerClientMap_.end() ){
            return findIt->second;
        }
        CacheManagerClientPtr cmc(new CacheManagerClient(zkAddress));
        cacheManagerClientMap_.insert(make_pair(zkAddress, cmc));
        MCE_INFO("ClientFactory::getCacheManagerClient init new cachemanagerclient for zkAddress " << zkAddress);
        return cmc;
    }
}

ProducerManagerClientPtr ClientFactory::getProducerManagerClient(const std::string& zkAddress) {
    MCE_INFO("ClientFactory::getProducerManagerClient zkAddress " << zkAddress);
    std::map<std::string, ProducerManagerClientPtr>::iterator findIt;
    {
        boost::shared_lock<boost::shared_mutex> lock(producerManagerClientMaplock_);
        findIt = producerManagerClientMap_.find(zkAddress);
        if( findIt != producerManagerClientMap_.end() ){
            return findIt->second;
        }
    }
    {
        boost::upgrade_lock<boost::shared_mutex> lock(producerManagerClientMaplock_);
        boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock1(lock);
        findIt = producerManagerClientMap_.find(zkAddress);
        if( findIt != producerManagerClientMap_.end() ){
            return findIt->second;
        }
        ProducerManagerClientPtr pmc(new ProducerManagerClient(zkAddress));
        producerManagerClientMap_.insert(make_pair(zkAddress, pmc));
        MCE_INFO("ClientFactory::getProducerManagerClient init new cachemanagerclient for zkAddress " << zkAddress);
        return pmc;
    }
}

CacheClientPtr ClientFactory::getCacheClient(const std::string& zkAddress) {
    MCE_INFO("ClientFactory::getCacheClient zkAddress " << zkAddress);
    std::map<std::string, CacheClientPtr>::iterator findIt;
    {
        boost::shared_lock<boost::shared_mutex> lock(cacheClientMaplock_);
        findIt = cacheClientMap_.find(zkAddress);
        if( findIt != cacheClientMap_.end() ){
            return findIt->second;
        }
    }
    {
        boost::upgrade_lock<boost::shared_mutex> lock(cacheClientMaplock_);
        boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock1(lock);
        findIt = cacheClientMap_.find(zkAddress);
        if( findIt != cacheClientMap_.end() ){
            return findIt->second;
        }
        CacheClientPtr cc(new CacheClient(zkAddress));
        cacheClientMap_.insert(make_pair(zkAddress, cc));
        MCE_INFO("ClientFactory::getCacheClient init new cachemanagerclient for zkAddress " << zkAddress);
        return cc;
    }
}

RedisCacheClientPtr ClientFactory::GetRedisCacheClient(const std::string& zk_address) {
  MCE_INFO("ClientFactory::GetRedisCacheClient() zk_address:" + zk_address);
  std::map<std::string, RedisCacheClientPtr>::iterator find_it;
  {
      boost::shared_lock<boost::shared_mutex> lock(redis_cache_client_map_lock_);
      find_it = redis_cache_client_map_.find(zk_address);
      if( find_it != redis_cache_client_map_.end() ){
          return find_it->second;
      }
  }
  {
      boost::upgrade_lock<boost::shared_mutex> lock(redis_cache_client_map_lock_);
      boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock1(lock);
      find_it = redis_cache_client_map_.find(zk_address);
      if( find_it != redis_cache_client_map_.end() ){
          return find_it->second;
      }
      RedisCacheClientPtr rcc(new RedisCacheClient(zk_address));
      redis_cache_client_map_.insert(std::make_pair(zk_address, rcc));
      MCE_INFO("ClientFactory::getRedisCacheClient init new RedisCacheClient for zk_address " << zk_address);
      return rcc;
  }
}

    
}
}
}
