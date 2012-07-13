#include "CacheManagerI.h"
#include "CacheClient.h"
#include "RedisCacheClient.h"
#include "ProducerManagerClient.h"

void MyUtil::initialize() {
    MyUtil::ServiceI& service = MyUtil::ServiceI::instance();

  std::string zkAddress = service.getCommunicator()->getProperties()->getPropertyWithDefault("ZkAddress", com::renren::tripod::DEFAULT_ZKADDRESS);
  com::renren::tripod::CacheManagerI::instance().setZkAddress(zkAddress);
	
  service.getAdapter()->add(&com::renren::tripod::CacheManagerI::instance(), service.createIdentity("M", ""));
 
  MyUtil::ServiceMonitorManager::instance().start();
}

namespace com {
namespace renren {
namespace tripod {

CacheManagerI::CacheManagerI() {
}

CacheManagerI::~CacheManagerI() {
}

void CacheManagerI::setZkAddress(const std::string& zkAddress) {
    zkAddress_ = zkAddress;
}

ProducerManagerClientPtr CacheManagerI::getProducer() {
    if(producer_ == NULL) {
        IceUtil::Mutex::Lock lock(producerMutex_);
        producer_ = ClientFactory::getInstance()->getProducerManagerClient(zkAddress_);
    }
    return producer_;
}

CacheClientPtr CacheManagerI::getCache() {
    if(cache_ == NULL) {
        IceUtil::Mutex::Lock lock(cacheMutex_);
        cache_ = ClientFactory::getInstance()->getCacheClient(zkAddress_);
    }
    return cache_;
}

RedisCacheClientPtr CacheManagerI::GetRedisCache() {
  if (redis_cache_ == NULL) {
    IceUtil::Mutex::Lock lock(redis_cache_mutex_);
    redis_cache_ = ClientFactory::getInstance()->GetRedisCacheClient(zkAddress_);
  }
  return redis_cache_;
}

DataMap CacheManagerI::get(const KeySeq& keys, const std::string& namespaceId, const std::string& businessId, 
                long expiredTime, bool useMaster, const Ice::Current& current) {
    std::ostringstream os;
    os<<"Key : ";
    for(KeySeq::const_iterator it = keys.begin(); it != keys.end(); ++it) {
        os<<*it<<" ";
    }
    os<<" namespaceId :"<<namespaceId;
    os<<" businessId :"<<businessId;
    os<<" expiredTime :"<<expiredTime;
    os<<" useMaster :"<<useMaster;
    MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, os.str(), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
    
    ProducerManagerClientPtr producer = getProducer();
    if(producer == NULL) {
        return DataMap();
    }

    KeySeq inputKeys(keys.begin(), keys.end());
    KeySeq lockedKeys;
    KeySeq failedLockedKeys;
    DataMap res;

    while(true) {
        xce::tempmutext::Locks<std::string> locks(&tempMutexManager_, inputKeys, lockedKeys, failedLockedKeys);
        if(!lockedKeys.empty()) {
            DataMap singleRes = producer->produce(keys, businessId, useMaster, 1000);
            if(!singleRes.empty()) {
                res.insert(singleRes.begin(), singleRes.end());
            }
        
            CacheClientPtr cache = getCache();
            if(cache != NULL) {
                for(DataMap::const_iterator it = singleRes.begin(); it != singleRes.end(); ++it) {
                    cache->set(it->first, it->second, namespaceId, businessId, expiredTime, 1000);
                }
            }
            if(failedLockedKeys.empty()) {
                break;
            }
        }
        inputKeys.swap(failedLockedKeys);
        lockedKeys.clear();
        failedLockedKeys.clear();
    }

    return res;
}

bool CacheManagerI::load(const KeySeq& keys, const std::string& namespaceId, const std::string& businessId, 
                long expiredTime, bool useMaster, const Ice::Current& current) {
    std::ostringstream os;
    os<<"Key : ";
    for(KeySeq::const_iterator it = keys.begin(); it != keys.end(); ++it) {
        os<<*it<<" ";
    }
    os<<" namespaceId :"<<namespaceId;
    os<<" businessId :"<<businessId;
    os<<" expiredTime :"<<expiredTime;
    os<<" useMaster :"<<useMaster;
    MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, os.str(), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
    ProducerManagerClientPtr producer = getProducer();
    if(producer == NULL) {
        return false;
    }
    
    bool sucFlag = true;

    KeySeq inputKeys(keys.begin(), keys.end());
    KeySeq lockedKeys;
    KeySeq failedLockedKeys;
    DataMap res;
    
    while(true) {
        xce::tempmutext::Locks<std::string> locks(&tempMutexManager_, inputKeys, lockedKeys, failedLockedKeys);
        if(!lockedKeys.empty()) {
            DataMap singleRes = producer->produce(keys, businessId, useMaster, 1000);
            if(!singleRes.empty()) {
                res.insert(singleRes.begin(), singleRes.end());
            }
        
            CacheClientPtr cache = getCache();
            if(cache != NULL) {
                for(DataMap::const_iterator it = singleRes.begin(); it != singleRes.end(); ++it) {
                    sucFlag &= cache->set(it->first, it->second, namespaceId, businessId, expiredTime, 1000);
                }
            }
            if(failedLockedKeys.empty()) {
                break;
            }
        }
        inputKeys.swap(failedLockedKeys);
        lockedKeys.clear();
        failedLockedKeys.clear();
    }

    return sucFlag;
}

bool CacheManagerI::deleteCache(const KeySeq& keys, const std::string& namespaceId, const std::string& businessId, const Ice::Current& current) {
    std::ostringstream os;
    os<<"Key : ";
    for(KeySeq::const_iterator it = keys.begin(); it != keys.end(); ++it) {
        os<<*it<<" ";
    }
    os<<" namespaceId :"<<namespaceId;
    os<<" businessId :"<<businessId;
    MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, os.str(), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
    CacheClientPtr cache = getCache();
    if(cache == NULL) {
        return false;
    }
    
    bool sucFlag = true;

    KeySeq inputKeys(keys.begin(), keys.end());
    KeySeq lockedKeys;
    KeySeq failedLockedKeys;
    DataMap res;
    
    while(true) {
        xce::tempmutext::Locks<std::string> locks(&tempMutexManager_, inputKeys, lockedKeys, failedLockedKeys);
        if(!lockedKeys.empty()) {
            for(KeySeq::const_iterator it = keys.begin(); it != keys.end(); ++it) {
                sucFlag &= cache->remove(*it, namespaceId, businessId, 1000);
            }
            if(failedLockedKeys.empty()) {
                break;
            }
        }
        inputKeys.swap(failedLockedKeys);
        lockedKeys.clear();
        failedLockedKeys.clear();
    }

    return sucFlag;
}

bool CacheManagerI::deleteListCache(const std::string& key, 
                                    const std::string& namespace_id, 
                                    const std::string& business_id, 
                                    const Ice::Current& current) {
  std::ostringstream otem;
  otem << "CacheManageI::deleteListCache() key:" << key
       << "\tnamespace_id:" << namespace_id 
       << "\tbusiness_id:" << business_id;
  MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, otem.str(), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
  RedisCacheClientPtr cache = GetRedisCache();
  if (cache == NULL) {
    MCE_WARN("GetRedisCache() return NULL");
    return false;
  }
  return cache->Remove(key, namespace_id, business_id);
}

bool CacheManagerI::loadListCache(const std::string& key, const std::string& namespace_id, const std::string& business_id, bool use_master, const Ice::Current& current) {
  std::ostringstream otem;
  otem << "CacheManageI::loadListCache() key:" << key
       << "\tnamespace_id:" << namespace_id 
       << "\tbusiness_id:" << business_id;
  MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, otem.str(), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
  ProducerManagerClientPtr producer = getProducer();
  if(producer == NULL) {
      return false;
  }
  KeySeq keys;
  keys.push_back(key);
  DataMap data = producer->produce(keys, business_id, use_master, 0);
  if (data.empty()) {
    return true;
  } 
  std::string list_key;
  StrList list_value;
  DataMap::const_iterator iter = data.begin();
  list_key = iter->second;
  for (; iter != data.end(); ++iter) {
    list_value.push_back(iter->first);
  }
  RedisCacheClientPtr redis_client = GetRedisCache(); 
  redis_client->Set(list_key, list_value, namespace_id, business_id);
  return true;
}

}}} // end com::renren::tripod


