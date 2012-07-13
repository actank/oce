#include <Common.h>
#include <CacheManagerConfigClient.h>
#include <CacheManagerClient.h>

#include <boost/foreach.hpp>

namespace com {
namespace renren {
namespace tripod {

CacheManagerClient::CacheManagerClient(const std::string& zkAddress) : zkAddress_(zkAddress) {
}

DataMap CacheManagerClient::get(const KeySeq& keys, const std::string& namespaceId, 
        const std::string& businessId, long expiredTime, bool useMaster, const long timeout) {
    DataMap res;
   
    CacheManagerConfigClientPtr config = getConfig();
    if(config == NULL) {
        TRIPOD_WARN("CacheManagerClient::get config is NULL, namespaceId " + namespaceId + " businessId " + businessId);
        return res;
    }
    
    std::map<CacheManagerPrx, KeySeq> prxMap = config->getPrxToKeysMap(keys);

    for(std::map<CacheManagerPrx, KeySeq>::const_iterator it = prxMap.begin(); it != prxMap.end(); ++it) {
        try {
            DataMap singleRes = it->first->ice_twoway()->ice_timeout(timeout)->get(it->second, namespaceId, businessId, expiredTime, useMaster);
            res.insert(singleRes.begin(), singleRes.end());
        }catch(Ice::Exception& e) {
            std::ostringstream os;
            os << "CacheManagerClient::get Ice::Exception " << e.what() << " namespaceId " << namespaceId << " businessId " << businessId;
            TRIPOD_WARN(os.str());
        }catch(...) {
            std::ostringstream os;
            os << "CacheManagerClient::get UnKnownException namespaceId " << namespaceId << " businessId " << businessId;
            TRIPOD_WARN(os.str());
        }
    }
    return res;
}

bool CacheManagerClient::load(const KeySeq& keys, const std::string& namespaceId, 
        const std::string& businessId, long expiredTime, bool useMaster, const long timeout) {
   
    CacheManagerConfigClientPtr config = getConfig();
    if(config == NULL) {
        TRIPOD_WARN("CacheManagerClient::load config is NULL, namespaceId " + namespaceId + " businessId " + businessId);
        return false;
    }
    
    std::map<CacheManagerPrx, KeySeq> prxMap = config->getPrxToKeysMap(keys);

    bool flag = true;
    bool inFlag = false;

    for(std::map<CacheManagerPrx, KeySeq>::const_iterator it = prxMap.begin(); it != prxMap.end(); ++it) {
        inFlag = true;
        try {
            flag &= it->first->ice_twoway()->ice_timeout(timeout)->load(it->second, namespaceId, businessId, expiredTime, useMaster);
        }catch(Ice::Exception& e) {
            std::ostringstream os;
            os << "CacheManagerClient::load Ice::Exception " << e.what() << " namespaceId " << namespaceId << " businessId " << businessId;
            TRIPOD_WARN(os.str());
        }catch(...) {
            std::ostringstream os;
            os << "CacheManagerClient::load UnKnownException namespaceId " << namespaceId << " businessId " << businessId;
            TRIPOD_WARN(os.str());
        }
    }
    return flag && inFlag;
}

bool CacheManagerClient::deleteCache(const KeySeq& keys, const std::string& namespaceId, 
        const std::string& businessId, const long timeout) {

    CacheManagerConfigClientPtr config = getConfig();
    if(config == NULL) {
        return false;
    }
    
    std::map<CacheManagerPrx, KeySeq> prxMap = config->getPrxToKeysMap(keys);

    bool flag = true;
    bool inFlag = false;

    for(std::map<CacheManagerPrx, KeySeq>::const_iterator it = prxMap.begin(); it != prxMap.end(); ++it) {
        inFlag = true;
        try {
            flag &= it->first->ice_twoway()->ice_timeout(timeout)->deleteCache(it->second, namespaceId, businessId);
        }catch(Ice::Exception& e) {
            std::ostringstream os;
            os << "CacheManagerClient::deleteCache Ice::Exception " << e.what() << " namespaceId " << namespaceId << " businessId " << businessId;
            TRIPOD_WARN(os.str());
        }catch(...) {
            std::ostringstream os;
            os << "CacheManagerClient::deleteCache UnKnownException namespaceId " << namespaceId << " businessId " << businessId;
            TRIPOD_WARN(os.str());
        }
    }
    return flag && inFlag;
}

CacheManagerConfigClientPtr CacheManagerClient::getConfig() {
    if(config_ == NULL) {
        boost::upgrade_lock<boost::shared_mutex> lock(configMutex_);
        boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock1(lock);
        config_ = ConfigClientFactory::getInstance()->getCacheManagerConfigClient(zkAddress_);
    }
    return config_;
}

bool CacheManagerClient::LoadListCache(const std::string& key, const std::string& namespace_id, const std::string& biz_id, bool use_master) {
  CacheManagerConfigClientPtr config = getConfig();
  if(config == NULL) {
      TRIPOD_WARN("CacheManagerClient::LoadListCache config is NULL, namespaceId " + namespace_id + " businessId " + biz_id);
      MCE_WARN("CacheManagerClient::LoadListCache config is NULL, namespaceId " + namespace_id + " businessId " + biz_id);
      return false;
  }
  
  KeySeq keys;
  keys.push_back(key);

  std::map<CacheManagerPrx, KeySeq> prx_map = config->getPrxToKeysMap(keys);
  std::map<CacheManagerPrx, KeySeq>::const_iterator iter = prx_map.begin();
  for (; iter != prx_map.end(); ++iter) {
    const KeySeq& seq = iter->second;
    BOOST_FOREACH(const std::string& k, seq) {
      iter->first->loadListCache(k, namespace_id, biz_id, use_master);
    }   
  }
}

bool CacheManagerClient::DeleteListCache(const std::string& key, const std::string& namespace_id, const std::string& biz_id) {
  CacheManagerConfigClientPtr config = getConfig();
  if(config == NULL) {
      TRIPOD_WARN("CacheManagerClient::DeleteListCache config is NULL, namespaceId " + namespace_id + " businessId " + biz_id);
      MCE_WARN("CacheManagerClient::DeleteListCache config is NULL, namespaceId " + namespace_id + " businessId " + biz_id);
      return false;
  }
  
  KeySeq keys;
  keys.push_back(key);

  std::map<CacheManagerPrx, KeySeq> prx_map = config->getPrxToKeysMap(keys);
  std::map<CacheManagerPrx, KeySeq>::const_iterator iter = prx_map.begin();
  for (; iter != prx_map.end(); ++iter) {
    const KeySeq& seq = iter->second;
    BOOST_FOREACH(const std::string& k, seq) {
      iter->first->deleteListCache(k, namespace_id, biz_id);
    }   
  }
  return true;
}

}
}
}
