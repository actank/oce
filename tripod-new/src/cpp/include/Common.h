#ifndef _TRIPOD_COMMON_H__
#define _TRIPOD_COMMON_H__

#include <zookeeper/zookeeper.h>

#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include <string>
#include <TripodLogger.hpp>

namespace com {
namespace renren {
namespace tripod {

const std::string TRIPOD_MANAGER = "Manager";
const std::string TRIPOD_PRODUCER = "Producer";
const std::string TRIPOD_CACHE = "Cache";
const std::string TRIPOD_PRODUCER_CC = "cc";
const std::string TRIPOD_PRODUCER_JAVA = "java";

class ZkManager;
typedef boost::shared_ptr<ZkManager> ZkManagerPtr;

class ZkManagerFactory;
typedef boost::shared_ptr<ZkManagerFactory> ZkManagerFactoryPtr;

class ConfigManager;
typedef boost::shared_ptr<ConfigManager> ConfigManagerPtr;

class ConfigManagerFactory;
typedef boost::shared_ptr<ConfigManagerFactory> ConfigManagerFactoryPtr;

class ConfigData;
typedef boost::shared_ptr<ConfigData> ConfigDataPtr;

class ConfigClient;
typedef boost::shared_ptr<ConfigClient> ConfigClientPtr;

class ConfigClientFactory;
typedef boost::shared_ptr<ConfigClientFactory> ConfigClientFactoryPtr;

class CacheManagerConfigClient;
typedef boost::shared_ptr<CacheManagerConfigClient> CacheManagerConfigClientPtr;

class ProducerManagerConfigClient;
typedef boost::shared_ptr<ProducerManagerConfigClient> ProducerManagerConfigClientPtr;

class CacheConfigClient;
typedef boost::shared_ptr<CacheConfigClient> CacheConfigClientPtr;

class RedisCacheConfigClient;
typedef boost::shared_ptr<RedisCacheConfigClient> RedisCacheConfigClientPtr;

class MemcachedClientPool;
typedef boost::shared_ptr<MemcachedClientPool> MemcachedClientPoolPtr;

class RedisClientPool;
typedef boost::shared_ptr<RedisClientPool> RedisClientPoolPtr;

class ClientFactory;
typedef boost::shared_ptr<ClientFactory> ClientFactoryPtr;

class CacheManagerClient;
typedef boost::shared_ptr<CacheManagerClient> CacheManagerClientPtr;

class ProducerManagerClient;
typedef boost::shared_ptr<ProducerManagerClient> ProducerManagerClientPtr;

class CacheClient;
typedef boost::shared_ptr<CacheClient> CacheClientPtr;

class RedisCacheClient;
typedef boost::shared_ptr<RedisCacheClient> RedisCacheClientPtr;
	
class TripodClient;
typedef boost::shared_ptr<TripodClient> TripodClientPtr;

}
}
}

#endif
