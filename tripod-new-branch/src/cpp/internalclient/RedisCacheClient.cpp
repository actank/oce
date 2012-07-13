#include <RedisCacheClient.h>

#include <boost/foreach.hpp>

#include <Common.h>
#include <RedisCacheConfigClient.h>
#include <RedisClientPool.h>


namespace com {
namespace renren {
namespace tripod {

RedisCacheClient::RedisCacheClient(const std::string& zk_address) : zk_address_(zk_address) {
  TRIPOD_INFO("RedisCacheClient::RedisCacheClient zk_address " + zk_address_);
}

RedisCacheConfigClientPtr RedisCacheClient::GetConfig() {
  if(config_ == NULL) {
    boost::upgrade_lock<boost::shared_mutex> lock(config_mutex_);
    boost::upgrade_to_unique_lock<boost::shared_mutex> unique_lock1(lock);
    config_ = ConfigClientFactory::getInstance()->GetRedisCacheConfigClient(zk_address_);
  }
  return config_;
}


StrList RedisCacheClient::GetList(const std::string& key,  
                              int begin, 
                              int limit,
                              const std::string& namespace_id, 
                              const std::string& business_id) {
  StrList res;
  RedisCacheConfigClientPtr config = GetConfig();
  if (!config) {
    MCE_DEBUG("RedisCacheClient::get config is NULL, namespace_id " + namespace_id + " business_id " + business_id);
    return res;
  }
  RedisClientPoolPtr cache_pool = config->GetPrx(key, namespace_id);
  std::string cache_key = GetBigKey(key, business_id);
  RedisClient* redis_client = cache_pool->BorrowItem();
  if (!redis_client) {
    MCE_WARN("RedisCacheClient::Get() not enough connection to redis server, business_id:" << business_id);
    return res;
  }
  res = redis_client->GetList(cache_key, begin, limit);
  cache_pool->ReturnItem(redis_client);
  return res;
}

IntList RedisCacheClient::GetIntList(const std::string& key,  
                              int begin, 
                              int limit,
                              const std::string& namespace_id, 
                              const std::string& business_id) {
  IntList res;
  RedisCacheConfigClientPtr config = GetConfig();
  if (!config) {
    MCE_DEBUG("RedisCacheClient::get config is NULL, namespace_id " + namespace_id + " business_id " + business_id);
    return res;
  }
  RedisClientPoolPtr cache_pool = config->GetPrx(key, namespace_id);
  std::string cache_key = GetBigKey(key, business_id);
  RedisClient* redis_client = cache_pool->BorrowItem();
  if (!redis_client) {
    MCE_WARN("RedisCacheClient::Get() not enough connection to redis server, business_id:" << business_id);
    return res;
  }
  res = redis_client->GetIntList(cache_key, begin, limit);
  cache_pool->ReturnItem(redis_client);
  return res;
}

bool RedisCacheClient::Set(const std::string& key, const StrList& value,
                           const std::string& namespace_id, const std::string& business_id) {
  RedisCacheConfigClientPtr config = GetConfig();
  if (!config) {
    MCE_DEBUG("RedisCacheClient::get config is NULL, namespace_id " + namespace_id + " business_id " + business_id);
    return false;
  }
  RedisClientPoolPtr cache_pool = config->GetPrx(key, namespace_id);
  std::string cache_key = GetBigKey(key, business_id);
  RedisClient* redis_client = cache_pool->BorrowItem();
  if (!redis_client) {
    MCE_WARN("RedisCacheClient::Get() not enough connection to redis server, business_id:" << business_id);
    return false;
  }
  bool flag = redis_client->SetList(cache_key, value);
  cache_pool->ReturnItem(redis_client);
  return flag;
  
}

bool RedisCacheClient::Remove(const std::string& key, const std::string& namespace_id, const std::string& business_id) {
  RedisCacheConfigClientPtr config = GetConfig();
  if (!config) {
    MCE_DEBUG("RedisCacheClient::get config is NULL, namespace_id " + namespace_id + " business_id " + business_id);
    return false;
  }
  RedisClientPoolPtr cache_pool = config->GetPrx(key, namespace_id);
  std::string cache_key = GetBigKey(key, business_id);
  RedisClient* redis_client = cache_pool->BorrowItem();
  if (!redis_client) {
    MCE_WARN("RedisCacheClient::Get() not enough connection to redis server, business_id:" << business_id);
    return false;
  }
  bool flag = redis_client->RemoveList(cache_key);
  cache_pool->ReturnItem(redis_client);
  return flag;
  
}


}}} // end com::renren::tripod

