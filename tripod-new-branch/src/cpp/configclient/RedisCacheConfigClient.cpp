
#include <Common.h>
#include <ConfigData.h>
#include <RedisCacheConfigClient.h>

namespace com {
namespace renren {
namespace tripod {

RedisCacheConfigClient::RedisCacheConfigClient(const std::string& zk_address): ConfigClient(zk_address, TRIPOD_MANAGER) {
//RedisCacheConfigClient::RedisCacheConfigClient(const std::string& zk_address): ConfigClient(zk_address, TRIPOD_CACHE) {

}

bool RedisCacheConfigClient::update(const ConfigDataPtr& new_config) {
  MCE_INFO("RedisCacheConfigClient::update");
  std::map<std::string, ConfigDataPtr>::const_iterator findIt = new_config->childrenDatas_.find(TRIPOD_CACHE);
  if(findIt == new_config->childrenDatas_.end()) {
      MCE_WARN("RedisCacheConfigClient::update no cache in config!");
      return false;
  }
  for(std::map<std::string, ConfigDataPtr>::const_iterator it = findIt->second->childrenDatas_.begin();
          it != findIt->second->childrenDatas_.end(); ++it) {
      std::map<std::string, RedisClientPoolPtr> redis_client_pools;
      for(std::map<std::string, ConfigDataPtr>::const_iterator iIt = it->second->childrenDatas_.begin();
              iIt != it->second->childrenDatas_.end(); ++iIt) { 
          std::vector<std::string> parts;
          boost::algorithm::split(parts, iIt->second->name_, boost::algorithm::is_any_of(":"));
          if(parts.size() != 2) {
              std::ostringstream oss;
              oss << "RedisCacheConfigClient::update Exception the config string" << iIt->second->name_
                  << " not match the format: <host>:<port>";
              MCE_WARN(oss.str());
              return false;
          }
          std::string address = parts[0];
          int port = 0 ;
          try {
            port = boost::lexical_cast<int>(parts[1]); 

          } catch (...) {
             MCE_WARN("RedisCacheConfigClient::update Exception the config string when cast port to int,  <host>:<port> " 
                       << iIt->second->name_);
            continue;
          }

          RedisClientPoolPtr redis_pool(new RedisClientPool(address, port));
          redis_client_pools.insert(std::make_pair(iIt->second->name_, redis_pool));
      }
      RedisClientPoolHashHolderPtr holder(new HashHolder<RedisClientPoolPtr>);
      holder->renew(redis_client_pools);
      {
        boost::upgrade_lock<boost::shared_mutex> lock(hash_holder_map_lock_);
        boost::upgrade_to_unique_lock<boost::shared_mutex> unique_lock1(lock);
        hash_holder_map_[it->second->name_] = holder;
      }
  }
  return true;
}


std::map<RedisClientPoolPtr, KeySeq> RedisCacheConfigClient::GetPrxToKeysMap(const KeySeq& keys, const std::string& namespace_id) {
  RedisClientPoolHashHolderPtr holder;
  {
    boost::shared_lock<boost::shared_mutex> lock(hash_holder_map_lock_);
    std::map<std::string, RedisClientPoolHashHolderPtr>::const_iterator findIt = hash_holder_map_.find(namespace_id);
    if(findIt == hash_holder_map_.end()) {
        return std::map<RedisClientPoolPtr, KeySeq>();
    }
    holder = findIt->second;
  }
  return holder->get(keys);
}

RedisClientPoolPtr RedisCacheConfigClient::GetPrx(const std::string& key, const std::string& namespace_id) {
  RedisClientPoolHashHolderPtr holder;
  {
    boost::shared_lock<boost::shared_mutex> lock(hash_holder_map_lock_);
    std::map<std::string, RedisClientPoolHashHolderPtr>::const_iterator findIt = hash_holder_map_.find(namespace_id);
    if(findIt == hash_holder_map_.end()) {
      return RedisClientPoolPtr();
    }
    holder = findIt->second;
  }
  return holder->get(key);
}


}}} // com::renren::tripod

