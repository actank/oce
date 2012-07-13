#include <Common.h>
#include <ProducerManagerConfigClient.h>
#include <ProducerManagerClient.h>

namespace com {
namespace renren {
namespace tripod {

ProducerManagerClient::ProducerManagerClient(const std::string& zkAddress) : zkAddress_(zkAddress) {
}

DataMap ProducerManagerClient::produce(const KeySeq& keys, const std::string& businessId, bool useMaster, const long timeout) {
    std::string useWhich = "";
    std::string newUseWhich = "";
    {
        boost::shared_lock<boost::shared_mutex> lock(prxToccOrjavaMapMutex_);
        std::map<std::string, std::string>::const_iterator findIt = prxToccOrjavaMap_.find(businessId);
        if(findIt != prxToccOrjavaMap_.end()) {
            useWhich = findIt->second;
        }
    }

    DataMap res;

    if(useWhich == TRIPOD_PRODUCER_JAVA) {
        try {
            res = produceUseJava(keys, businessId, useMaster, timeout);
        }catch (TripodProducerNotSupportedException& e) {
            TRIPOD_WARN("ProducerManagerClient::produce use cc instead of java, because java not supported! businessId " + businessId);
            try {
                res = produceUseCc(keys, businessId, useMaster, timeout);
                newUseWhich = TRIPOD_PRODUCER_CC;
            }catch (Ice::Exception& e) {
                std::ostringstream os;
                os << "ProducerManagerClient::produce use cc but get Exception " << e.what() << " businessId " << businessId;
                TRIPOD_WARN(os.str());
            }
        }catch (Ice::Exception& e1) {
            std::ostringstream os;
            os << "ProducerManagerClient::produce use java but get Exception " << e1.what() << " businessId " << businessId;
            TRIPOD_WARN(os.str());
        }
    }

    if(useWhich == TRIPOD_PRODUCER_CC) {
        try {
            res = produceUseCc(keys, businessId, useMaster, timeout);
        }catch (TripodProducerNotSupportedException& e) {
            TRIPOD_WARN("ProducerManagerClient::produce use java instead of cc, because cc not supported! businessId " + businessId);
            try {
                res = produceUseJava(keys, businessId, useMaster, timeout);
                newUseWhich = TRIPOD_PRODUCER_JAVA;
            }catch (Ice::Exception& e) {
                std::ostringstream os;
                os << "ProducerManagerClient::produce use java but get Exception " << e.what() << " businessId " << businessId;
                TRIPOD_WARN(os.str());
            }
        }catch (Ice::Exception& e1) {
            std::ostringstream os;
            os << "ProducerManagerClient::produce use cc but get Exception " << e1.what() << " businessId " << businessId;
            TRIPOD_WARN(os.str());
        }
    }

    if(useWhich.empty()) {
        TRIPOD_INFO("ProducerManagerClient::produce try with cc and java businessId " + businessId);
        try {
            res = produceUseCc(keys, businessId, useMaster, timeout);
            newUseWhich = TRIPOD_PRODUCER_CC;
        }catch (TripodProducerNotSupportedException& e) {
            TRIPOD_WARN("ProducerManagerClient::produce use java instead of cc, because cc not supported! businessId " + businessId);
            try {
                res = produceUseJava(keys, businessId, useMaster, timeout);
                newUseWhich = TRIPOD_PRODUCER_JAVA;
            }catch (Ice::Exception& e) {
                std::ostringstream os;
                os << "ProducerManagerClient::produce use java but get Exception " << e.what() << " businessId " << businessId;
                TRIPOD_WARN(os.str());
            }
        }catch (Ice::Exception& e1) {
            std::ostringstream os;
            os << "ProducerManagerClient::produce use cc but get Exception " << e1.what() << " businessId " << businessId;
            TRIPOD_WARN(os.str());
        }
    }

    if(useWhich != newUseWhich && !newUseWhich.empty()) {
        boost::upgrade_lock<boost::shared_mutex> lock(prxToccOrjavaMapMutex_);
        boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock1(lock);
        prxToccOrjavaMap_[businessId] = newUseWhich;
    }

    return res;
}

KeySeq ProducerManagerClient::produceKeys(const std::string& beginKey, int limit, const std::string& businessId, bool hotKey, const long timeout) {
    std::string useWhich = "";
    std::string newUseWhich = "";
    {
        boost::shared_lock<boost::shared_mutex> lock(prxToccOrjavaMapMutex_);
        std::map<std::string, std::string>::const_iterator findIt = prxToccOrjavaMap_.find(businessId);
        if(findIt != prxToccOrjavaMap_.end()) {
            useWhich = findIt->second;
        }
    }

    KeySeq res;

    if(useWhich == TRIPOD_PRODUCER_JAVA) {
        try {
            res = produceUseJava(beginKey, limit, businessId, hotKey, timeout);
        }catch (TripodProducerNotSupportedException& e) {
            TRIPOD_WARN("ProducerManagerClient::produceKeys use cc instead of java, because java not supported! businessId " + businessId);
            try {
                res = produceUseCc(beginKey, limit, businessId, hotKey, timeout);
                newUseWhich = TRIPOD_PRODUCER_CC;
            }catch (Ice::Exception& e) {
                std::ostringstream os;
                os << "ProducerManagerClient::produceKeys use cc but get Exception " << e.what() << " businessId " << businessId;
                TRIPOD_WARN(os.str());
            }
        }catch (Ice::Exception& e1) {
            std::ostringstream os;
            os << "ProducerManagerClient::produceKeys use java but get Exception " << e1.what() << " businessId " << businessId;
            TRIPOD_WARN(os.str());
        }
    }

    if(useWhich == TRIPOD_PRODUCER_CC) {
        try {
            res = produceUseCc(beginKey, limit, businessId, hotKey, timeout);
        }catch (TripodProducerNotSupportedException& e) {
            TRIPOD_WARN("ProducerManagerClient::produceKeys use java instead of cc, because cc not supported! businessId " + businessId);
            try {
                res = produceUseJava(beginKey, limit, businessId, hotKey, timeout);
                newUseWhich = TRIPOD_PRODUCER_JAVA;
            }catch (Ice::Exception& e) {
                std::ostringstream os;
                os << "ProducerManagerClient::produceKeys use java but get Exception " << e.what() << " businessId " << businessId;
                TRIPOD_WARN(os.str());
            }
        }catch (Ice::Exception& e1) {
            std::ostringstream os;
            os << "ProducerManagerClient::produceKeys use cc but get Exception " << e1.what() << " businessId " << businessId;
            TRIPOD_WARN(os.str());
        }
    }

    if(useWhich.empty()) {
        TRIPOD_INFO("ProducerManagerClient::produceKeys try with cc and java businessId " + businessId);
        try {
            res = produceUseCc(beginKey, limit, businessId, hotKey, timeout);
            newUseWhich = TRIPOD_PRODUCER_CC;
        }catch (TripodProducerNotSupportedException& e) {
            TRIPOD_WARN("ProducerManagerClient::produceKeys use java instead of cc, because cc not supported! businessId " + businessId);
            try {
                res = produceUseJava(beginKey, limit, businessId, hotKey, timeout);
                newUseWhich = TRIPOD_PRODUCER_JAVA;
            }catch (Ice::Exception& e) {
                std::ostringstream os;
                os << "ProducerManagerClient::produceKeys use java but get Exception " << e.what() << " businessId " << businessId;
                TRIPOD_WARN(os.str());
            }
        }catch (Ice::Exception& e1) {
            std::ostringstream os;
            os << "ProducerManagerClient::produceKeys use cc but get Exception " << e1.what() << " businessId " << businessId;
            TRIPOD_WARN(os.str());
        }
    }

    if(useWhich != newUseWhich && !newUseWhich.empty()) {
        boost::upgrade_lock<boost::shared_mutex> lock(prxToccOrjavaMapMutex_);
        boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock1(lock);
        prxToccOrjavaMap_[businessId] = newUseWhich;
    }

    return res;
}


DataMap ProducerManagerClient::produceUseJava(const KeySeq& keys, const std::string& businessId, bool useMaster, const long timeout) {

    DataMap res;
   
    ProducerManagerConfigClientPtr config = getConfig();
    if(config == NULL) {
        return res;
    }
    
    std::map<ProducerManagerPrx, KeySeq> prxMap = config->getJavaPrxToKeysMap(keys);

    for(std::map<ProducerManagerPrx, KeySeq>::const_iterator it = prxMap.begin(); it != prxMap.end(); ++it) {
        try {
            DataMap singleRes = it->first->ice_twoway()->ice_timeout(timeout)->produce(it->second, businessId, useMaster);
            res.insert(singleRes.begin(), singleRes.end());
        }catch(Ice::Exception& e){
            std::ostringstream os;
            os << "ProducerManagerClient::produceUseJava Ice::Exception " << e.what() << " businessId " << businessId;
            TRIPOD_WARN(os.str());
        }catch(...){
            std::ostringstream os;
            os << "ProducerManagerClient::produceUseJava UnKnownException businessId " << businessId;
            TRIPOD_WARN(os.str());
        }

    }
    return res;
}

DataMap ProducerManagerClient::produceUseCc(const KeySeq& keys, const std::string& businessId, bool useMaster, const long timeout) {
    DataMap res;
    
    ProducerManagerConfigClientPtr config = getConfig();
    if(config == NULL) {
        return res;
    }

    std::map<ProducerManagerPrx, KeySeq> prxMap = config->getCcPrxToKeysMap(keys);

    for(std::map<ProducerManagerPrx, KeySeq>::const_iterator it = prxMap.begin(); it != prxMap.end(); ++it) {
        try {
            DataMap singleRes = it->first->ice_twoway()->ice_timeout(timeout)->produce(it->second, businessId, useMaster);

            res.insert(singleRes.begin(), singleRes.end());
        }catch(Ice::Exception& e){
            std::ostringstream os;
            os << "ProducerManagerClient::produceUseCc Ice::Exception " << e.what() << " businessId " << businessId;
            TRIPOD_WARN(os.str());
        }catch(...){
            std::ostringstream os;
            os << "ProducerManagerClient::produceUseCc UnKnownException businessId " << businessId;
            TRIPOD_WARN(os.str());
        }
    }
    return res;
}

KeySeq ProducerManagerClient::produceUseJava(const std::string& beginKey, int limit, const std::string& businessId, bool hotKey, const long timeout) {

    KeySeq res;
   
    ProducerManagerConfigClientPtr config = getConfig();
    if(config == NULL) {
        return res;
    }
    
    KeySeq keys;
    keys.push_back(beginKey);
    
    std::map<ProducerManagerPrx, KeySeq> prxMap = config->getJavaPrxToKeysMap(keys);

    for(std::map<ProducerManagerPrx, KeySeq>::const_iterator it = prxMap.begin(); it != prxMap.end(); ++it) {
        try {
            KeySeq singleRes = it->first->ice_twoway()->ice_timeout(timeout)->produceKeys(beginKey, limit, businessId, hotKey);

            res.insert(res.end(), singleRes.begin(), singleRes.end());
        }catch(Ice::Exception& e){
            std::ostringstream os;
            os << "ProducerManagerClient::produceKeysUseJava Ice::Exception " << e.what() << " businessId " << businessId;
            TRIPOD_WARN(os.str());
        }catch(...){
            std::ostringstream os;
            os << "ProducerManagerClient::produceKeysUseJava UnKnownException businessId " << businessId;
            TRIPOD_WARN(os.str());
        }
    }
    return res;
}

KeySeq ProducerManagerClient::produceUseCc(const std::string& beginKey, int limit, const std::string& businessId, bool hotKey, const long timeout) {
    KeySeq res;
    
    ProducerManagerConfigClientPtr config = getConfig();
    if(config == NULL) {
        return res;
    }

    KeySeq keys;
    keys.push_back(beginKey);

    std::map<ProducerManagerPrx, KeySeq> prxMap = config->getCcPrxToKeysMap(keys);

    for(std::map<ProducerManagerPrx, KeySeq>::const_iterator it = prxMap.begin(); it != prxMap.end(); ++it) {
        try {
            KeySeq singleRes = it->first->ice_twoway()->ice_timeout(timeout)->produceKeys(beginKey, limit, businessId, hotKey);

            res.insert(res.end(), singleRes.begin(), singleRes.end());
        }catch(Ice::Exception& e){
            std::ostringstream os;
            os << "ProducerManagerClient::produceKeysUseCc Ice::Exception " << e.what() << " businessId " << businessId;
            TRIPOD_WARN(os.str());
        }catch(...){
            std::ostringstream os;
            os << "ProducerManagerClient::produceKeysUseCc UnKnownException businessId " << businessId;
            TRIPOD_WARN(os.str());
        }
    }
    return res;
}

ProducerManagerConfigClientPtr ProducerManagerClient::getConfig() {
    if(config_ == NULL) {
        boost::upgrade_lock<boost::shared_mutex> lock(configMutex_);
        boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock1(lock);
        config_ = ConfigClientFactory::getInstance()->getProducerManagerConfigClient(zkAddress_);
    }
    return config_;
}

}
}
}
