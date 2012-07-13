#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/lexical_cast.hpp>

#include <Common.h>
#include <CacheConfigClient.h>
#include <CacheClient.h>

namespace com {
namespace renren {
namespace tripod {

CacheClient::CacheClient(const std::string& zkAddress) : zkAddress_(zkAddress) {
    TRIPOD_INFO("CacheClient::CacheClient zkAddress " + zkAddress);
}

DataMap CacheClient::get(const KeySeq& keys, const std::string& namespaceId, const std::string& businessId, const long timeout) {
    
    DataMap res;
  
    CacheConfigClientPtr config = getConfig();
    if(config == NULL) {
        TRIPOD_WARN("CacheClient::get config is NULL, namespaceId " + namespaceId + " businessId " + businessId);
        return res;
    }

    std::map<MemcachedClientPoolPtr, KeySeq> clientMap = config->getPrxToKeysMap(keys, namespaceId);


    for(std::map<MemcachedClientPoolPtr, KeySeq>::const_iterator it = clientMap.begin(); it != clientMap.end(); ++it) {
        DataMap singleRes = get(it->first, it->second, businessId, timeout);
        res.insert(singleRes.begin(), singleRes.end());
    }

    return res;
}

bool CacheClient::set(const std::string& key, const std::string& value, const std::string& namespaceId,
        const std::string& businessId, long expiredTime, const long timeout) {
    if(key.empty()) {
        TRIPOD_WARN("CacheClient::set key is empty, namespaceId " + namespaceId + " businessId " + businessId);
        return false;
    }

    CacheConfigClientPtr config = getConfig();
    if(config == NULL) {
        TRIPOD_WARN("CacheClient::set config is NULL, namespaceId " + namespaceId + " businessId " + businessId);
        return false;
    }

    MemcachedClientPoolPtr mcpp = config->getPrx(key, namespaceId);
    if(mcpp == NULL) {
        TRIPOD_WARN("CacheClient::set no prx for namespaceId " + namespaceId + " businessId " + businessId);
        return false;
    }
    
    memcache::Memcache* conn = mcpp->borrowItem();
    if (!conn) {
        TRIPOD_WARN("CacheClient::set there is not enough connections, namespaceId " + namespaceId + " businessId " + businessId);
        return false;
    }

    conn->setBehavior(MEMCACHED_BEHAVIOR_RCV_TIMEOUT, timeout);
    conn->setBehavior(MEMCACHED_BEHAVIOR_SND_TIMEOUT, timeout);

    bool flag = conn->set(getBigKey(key, businessId), std::vector<char>(value.begin(), value.end()), expiredTime, MEM_STR_FLAG);

    mcpp->returnItem(conn);

    return flag;
}

bool CacheClient::remove(const std::string& key, const std::string& namespaceId, const std::string& businessId, const long timeout) {
    if(key.empty()) {
        return false;
    }

    CacheConfigClientPtr config = getConfig();
    if(config == NULL) {
        TRIPOD_WARN("CacheClient::remove config is NULL, namespaceId " + namespaceId + " businessId " + businessId);
        return false;
    }

    MemcachedClientPoolPtr mcpp = config->getPrx(key, namespaceId);
    if(mcpp == NULL) {
        TRIPOD_WARN("CacheClient::remove no prx for namespaceId " + namespaceId + " businessId " + businessId);
        return false;
    }
    
    memcache::Memcache* conn = mcpp->borrowItem();
    if (!conn) {
        TRIPOD_WARN("CacheClient::remove there is not enough connections, namespaceId " + namespaceId + " businessId " + businessId);
        return false;
    }
    
    conn->setBehavior(MEMCACHED_BEHAVIOR_RCV_TIMEOUT, timeout);

    bool flag = conn->remove(getBigKey(key, businessId));

    mcpp->returnItem(conn);

    return flag;
}

DataMap CacheClient::get(const MemcachedClientPoolPtr& mcpp, const KeySeq& keys, const std::string& businessId, const long timeout) {
    std::vector<std::string> bigKeys(keys.size());

    for (KeySeq::const_iterator it = keys.begin(); it != keys.end(); ++it) {
        std::string bigKey = getBigKey(*it, businessId);
        bigKeys.push_back(bigKey);
    } 

    DataMap res; 

    memcache::Memcache* conn = mcpp->borrowItem();
    if (!conn) {
        TRIPOD_WARN("CacheClient::get there is not enough connections, businessId " + businessId);
        return res;
    }

    if (!conn->mget(bigKeys))
    {
        mcpp->returnItem(conn);
        return res;
    }

    conn->setBehavior(MEMCACHED_BEHAVIOR_RCV_TIMEOUT, timeout);
    
    char *return_value = NULL;
    char return_key[MEMCACHED_MAX_KEY];
    size_t return_key_length;
    size_t return_value_length;
    unsigned int flags;
    memcached_return rc; 
    while(true) {
        return_value = memcached_fetch(&(conn->getImpl()), return_key, &return_key_length, &return_value_length, &flags, &rc);
        if(rc == MEMCACHED_END){
            break;
        }
        return_key[return_key_length]=0;
        if(return_value!=0){
            res[getKey(return_key, businessId)] = std::string(return_value, return_value + return_value_length);
        }

        if (return_value != NULL) {
            free( return_value );
            return_value = NULL;
        }
    }
    mcpp->returnItem(conn);
    return res;
}

std::string CacheClient::getBigKey(const std::string& key, const std::string& businessId) {
    return businessId + "_" + key;
}

std::string CacheClient::getKey(const std::string& bigKey, const std::string& businessId) {
    return bigKey.substr(businessId.length()+1, bigKey.length());
}

CacheConfigClientPtr CacheClient::getConfig() {
    if(config_ == NULL) {
        boost::upgrade_lock<boost::shared_mutex> lock(configMutex_);
        boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock1(lock);
        config_ = ConfigClientFactory::getInstance()->getCacheConfigClient(zkAddress_);
    }
    return config_;
}

}
}
}
