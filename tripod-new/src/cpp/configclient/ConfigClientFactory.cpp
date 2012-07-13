#include <Common.h>
#include <CacheManagerConfigClient.h>
#include <ProducerManagerConfigClient.h>
#include <CacheConfigClient.h>
#include <ConfigClientFactory.h>
#include <CacheConfigClient.h>
#include <RedisCacheConfigClient.h>

namespace com {
namespace renren {
namespace tripod { 

CacheManagerConfigClientPtr ConfigClientFactory::getCacheManagerConfigClient(const std::string& zkAddress) {
    MCE_INFO("ConfigClientFactory::getCacheManagerConfigClient zkAddress" << zkAddress);
    std::map<std::string, CacheManagerConfigClientPtr>::iterator findIt;
    {
        boost::shared_lock<boost::shared_mutex> lock(cacheManagerConfigClientMaplock_);
        findIt = cacheManagerConfigClientMap_.find(zkAddress);
        if( findIt != cacheManagerConfigClientMap_.end() ){
            return findIt->second;
        }
    }
    {
        boost::upgrade_lock<boost::shared_mutex> lock(cacheManagerConfigClientMaplock_);
        boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock1(lock);
        findIt = cacheManagerConfigClientMap_.find(zkAddress);
        if( findIt != cacheManagerConfigClientMap_.end() ){
            return findIt->second;
        }
        CacheManagerConfigClientPtr cmc(new CacheManagerConfigClient(zkAddress));
        if ( cmc->initialize() ) {
            MCE_INFO("ConfigClientFactory::getCacheManagerConfigClient init for zkAddress" << zkAddress << " success!");
            cacheManagerConfigClientMap_.insert(make_pair(zkAddress, cmc));
            return cmc;
        }
        MCE_INFO("ConfigClientFactory::getCacheManagerConfigClient init for zkAddress" << zkAddress << " failed!");
        return CacheManagerConfigClientPtr();
    }
}

ProducerManagerConfigClientPtr ConfigClientFactory::getProducerManagerConfigClient(const std::string& zkAddress) {
    MCE_INFO("ConfigClientFactory::getProducerManagerConfigClient zkAddress" << zkAddress);
    std::map<std::string, ProducerManagerConfigClientPtr>::iterator findIt;
    {
        boost::shared_lock<boost::shared_mutex> lock(producerManagerConfigClientMaplock_);
        findIt = producerManagerConfigClientMap_.find(zkAddress);
        if( findIt != producerManagerConfigClientMap_.end() ){
            return findIt->second;
        }
    }
    {
        boost::upgrade_lock<boost::shared_mutex> lock(producerManagerConfigClientMaplock_);
        boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock1(lock);
        findIt = producerManagerConfigClientMap_.find(zkAddress);
        if( findIt != producerManagerConfigClientMap_.end() ){
            return findIt->second;
        }
        ProducerManagerConfigClientPtr pmc(new ProducerManagerConfigClient(zkAddress));
        if ( pmc->initialize() ) {
            MCE_INFO("ConfigClientFactory::getProducerManagerConfigClient init for zkAddress" << zkAddress << " success!");
            producerManagerConfigClientMap_.insert(make_pair(zkAddress, pmc));
            return pmc;
        }
        MCE_INFO("ConfigClientFactory::getProducerManagerConfigClient init for zkAddress" << zkAddress << " failed!");
        return ProducerManagerConfigClientPtr();
    }
}

CacheConfigClientPtr ConfigClientFactory::getCacheConfigClient(const std::string& zkAddress) {
    MCE_INFO("ConfigClientFactory::getCacheConfigClient zkAddress" << zkAddress);
    std::map<std::string, CacheConfigClientPtr>::iterator findIt;
    {
        boost::shared_lock<boost::shared_mutex> lock(cacheConfigClientMaplock_);
        findIt = cacheConfigClientMap_.find(zkAddress);
        if( findIt != cacheConfigClientMap_.end() ){
            return findIt->second;
        }
    }
    {
        boost::upgrade_lock<boost::shared_mutex> lock(cacheConfigClientMaplock_);
        boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock1(lock);
        findIt = cacheConfigClientMap_.find(zkAddress);
        if( findIt != cacheConfigClientMap_.end() ){
            return findIt->second;
        }
        CacheConfigClientPtr cc(new CacheConfigClient(zkAddress));
        if ( cc->initialize() ) {
            MCE_INFO("ConfigClientFactory::getCacheConfigClient init for zkAddress" << zkAddress << " success!");
            cacheConfigClientMap_.insert(make_pair(zkAddress, cc));
            return cc;
        }
        MCE_INFO("ConfigClientFactory::getCacheConfigClient init for zkAddress" << zkAddress << " failed!");
        return CacheConfigClientPtr();
    }
}

RedisCacheConfigClientPtr ConfigClientFactory::GetRedisCacheConfigClient(const std::string& zk_address) {
  MCE_INFO("ConfigClientFactory::GetRedisCacheConfigClient zk_address" << zk_address);
  MCE_DEBUG("===============ConfigClientFactory::GetRedisCacheConfigClient zk_address" << zk_address);
  std::map<std::string, RedisCacheConfigClientPtr>::iterator findIt;
  {
      boost::shared_lock<boost::shared_mutex> lock(redis_cache_config_client_map_lock_);
      findIt = redis_cache_config_client_map_.find(zk_address);
      if( findIt != redis_cache_config_client_map_.end() ){
          return findIt->second;
      }
  }
  {
      boost::upgrade_lock<boost::shared_mutex> lock(redis_cache_config_client_map_lock_);
      boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock1(lock);
      findIt = redis_cache_config_client_map_.find(zk_address);
      if( findIt != redis_cache_config_client_map_.end() ){
          return findIt->second;
      }
      RedisCacheConfigClientPtr cc(new RedisCacheConfigClient(zk_address));
      if ( cc->initialize() ) {
          MCE_INFO("liyong ConfigClientFactory::GetRedisCacheConfigClient init for zk_address" << zk_address << " success!");
          redis_cache_config_client_map_.insert(make_pair(zk_address, cc));
          return cc;
      }
      MCE_INFO("liyong ConfigClientFactory::GetRedisCacheConfigClient init for zk_address" << zk_address << " failed!");
      return RedisCacheConfigClientPtr();
  }
}
    
}
}
}
