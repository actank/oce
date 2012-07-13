#include <iostream>
#include <boost/pointer_cast.hpp>

#include <LogWrapper.h>

#include "ZkManager.h"
#include "ZkManagerFactory.h"
#include "ConfigClient.h"
#include "ConfigData.h"

#include "ConfigManager.h"

namespace com {
namespace renren {
namespace tripod {

BufferContainer::BufferContainer() {
    buflen = sizeof(buffer);
    memset(buffer, 0, buflen);
}

ConfigManager::ConfigManager() {
}

bool ConfigManager::initialize(const std::string& zkAddress) {
    MCE_INFO("ConfigManager::initialize zkAddress " << zkAddress);
    zm_ = ZkManagerFactory::getInstance()->getZkManager(zkAddress);
    if( zm_ == NULL ) {
        MCE_WARN("ConfigManager::initialize zkManager initialize failed!");
        return false;
    }

    return updateConfig(zm_->getZk());
}

bool ConfigManager::updateConfig(zhandle_t* zk) {
    MCE_INFO("ConfigManager::updateConfig");

    ConfigDataPtr newConfigData = getConfigData();

    bool updateFlag = false;

    bool sucFlag = init(newConfigData, zk, updateFlag); 

    if(updateFlag && sucFlag) {
        MCE_INFO("ConfigManager::updateConfig set newconfig");
        boost::upgrade_lock<boost::shared_mutex> lock(lock_);
        boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
        configData_ = newConfigData;
    }

    if(updateFlag) {
        MCE_INFO("ConfigManager::updateConfig update clients");
        boost::shared_lock<boost::shared_mutex> lock(clientsLock_);
        for(std::vector<ConfigClientPtr>::const_iterator it = clients_.begin(); it != clients_.end(); ++it) {
            (*it)->notifyHandle();
        }
    }
    
    MCE_INFO("ConfigManager::updateConfig end sucFlag " << sucFlag << " updateFlag " << updateFlag);

    return sucFlag;
}

void ConfigManager::registerClient(const ConfigClientPtr& cc) {
    MCE_INFO("ConfigManager::registerClient");
    boost::upgrade_lock<boost::shared_mutex> lock(clientsLock_);
    boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
    clients_.push_back(cc);
}

ConfigDataPtr ConfigManager::getConfigData() {
    MCE_INFO("ConfigManager::getConfigData");
    boost::shared_lock<boost::shared_mutex> lock(lock_);
    if(configData_ != NULL) {
        return configData_->getData();
    }else
    {
        MCE_WARN("ConfigManager::getConfigData configdata_ is NULL");
        return ConfigDataPtr();
    }
}

bool ConfigManager::init(ConfigDataPtr& config, zhandle_t* zk, bool& updateFlag) {
    MCE_INFO("ConfigManager::init");

    updateFlag = false;

    if(config == NULL) {
        MCE_INFO("ConfigManager::init first time config null");
        ConfigDataPtr managerConfig(new ConfigData);
        managerConfig->name_ = TRIPOD_MANAGER;
        ConfigDataPtr producerConfig(new ConfigData);
        producerConfig->name_ = TRIPOD_PRODUCER;
        ConfigDataPtr cacheConfig(new ConfigData);
        cacheConfig->name_ = TRIPOD_CACHE;
        updateFlag = true;
        bool tempFlag = false;
        bool sucFlag = updateManagerConfig(managerConfig, zk, false, tempFlag) && updateProducerConfig(producerConfig, zk, false, tempFlag) && updateCacheConfig(cacheConfig, zk, false, tempFlag);
        if(sucFlag) {
            config = ConfigDataPtr(new ConfigData);
            config->childrenDatas_.insert(make_pair(TRIPOD_MANAGER, managerConfig));
            config->childrenDatas_.insert(make_pair(TRIPOD_PRODUCER, producerConfig));
            config->childrenDatas_.insert(make_pair(TRIPOD_CACHE, cacheConfig));
        }
        MCE_INFO("ConfigManager::init sucFlag " << sucFlag);
        return sucFlag;
    }

    std::map<std::string, ConfigDataPtr>::const_iterator findIt = config->childrenDatas_.find(TRIPOD_MANAGER);

    if(findIt == config->childrenDatas_.end()) {
        MCE_WARN("ConfigManager::init no cachemanager in newconfig");
        return false;
    }

    bool updateManagerFlag = false;
    bool sucManagerFlag = updateManagerConfig(findIt->second, zk, true, updateManagerFlag);

    if(sucManagerFlag == false) {
        MCE_WARN("ConfigManager::init updateManagerConfig failed");
        return false;
    }

    findIt = config->childrenDatas_.find(TRIPOD_PRODUCER);

    if(findIt == config->childrenDatas_.end()) {
        MCE_WARN("ConfigManager::init no producermanager in newconfig");
        return false;
    }

    bool updateProducerFlag = false;
    bool sucProducerFlag = updateProducerConfig(findIt->second, zk, true, updateProducerFlag);

    if(sucProducerFlag == false) {
        MCE_WARN("ConfigManager::init updateProducerConfig failed");
        return false;
    }

    findIt = config->childrenDatas_.find(TRIPOD_CACHE);

    if(findIt == config->childrenDatas_.end()) {
        MCE_WARN("ConfigManager::init no cache in newconfig");
        return false;
    }

    bool updateCacheFlag = false;
    bool sucCacheFlag = updateCacheConfig(findIt->second, zk, true, updateCacheFlag);

    if(sucCacheFlag == false) {
        MCE_WARN("ConfigManager::init updateCacheConfig failed");
        return false;
    }

    updateFlag = updateCacheFlag | updateProducerFlag | updateManagerFlag;
    MCE_INFO("ConfigManager::init updateCacheFlag " << updateCacheFlag 
            << " updateManagerFlag " << updateManagerFlag 
            << " updateProducerFlag " << updateProducerFlag 
            << " updateFlag " << updateFlag);
    MCE_INFO("ConfigManager::init sucCacheFlag " << sucCacheFlag
            << " sucProducerFlag " << sucProducerFlag
            << " sucManagerFlag " << sucManagerFlag);

    return sucCacheFlag && sucProducerFlag && sucManagerFlag; 
}

bool ConfigManager::updateManagerConfig(const ConfigDataPtr& config, zhandle_t* zk, bool checkVersion, bool& updateFlag) {
    MCE_INFO("ConfigManager::updateManagerConfig");

    updateFlag =false;

    try
    {
        int watch = 1;

        BufferContainerPtr bc( new BufferContainer ); 

        std::ostringstream oss;
        oss << "/"<<TRIPOD_MANAGER;
        int rc = 0;
        rc = zoo_exists(zk, oss.str().c_str(), watch, 0);

        if (rc != ZOK) {
            MCE_WARN( "ConfigManager::updateManagerConfig zoo_exists failed because "<<zerror(rc)<< __FILE__<<__LINE__ );
            return false;
        }

        rc = zoo_get(zk, oss.str().c_str(), watch, bc->buffer, &(bc->buflen), 0);

        if (rc != ZOK) {
            MCE_WARN( "ConfigManager::updateManagerConfig zoo_get failed because "<<zerror(rc)<< __FILE__<<__LINE__ );
            return false;
        }
        std::string version(bc->buffer, bc->buffer + bc->buflen);
        if(checkVersion) {
            if(config->version_ == version) {
                MCE_INFO("ConfigManager::updateManagerConfig version is same");
                return true;
            }
        }

        config->name_ = TRIPOD_MANAGER;
        config->version_ = version;

        bool sucChildrenFlag = getChildrenDatas(config, zk, oss);

        if(!sucChildrenFlag) {
            MCE_WARN("ConfigManager::updateManagerConfig getChildrenDatas failed");
            return false;
        }
    }catch(...)
    {
        MCE_WARN( "ConfigManager::updateManagerConfig can not init config "<< __FILE__<<__LINE__ );
        return false;
    }

    updateFlag = true;
    return true;
}

bool ConfigManager::updateProducerConfig(const ConfigDataPtr& config, zhandle_t* zk, bool checkVersion, bool& updateFlag) {
    MCE_INFO("ConfigManager::updateProducerConfig");

    updateFlag =false;

    try
    {
        int watch = 1;

        BufferContainerPtr bc( new BufferContainer ); 

        std::ostringstream oss;
        oss << "/"<<TRIPOD_PRODUCER<<"/"<<TRIPOD_PRODUCER_CC;
        
        int rc = zoo_exists(zk, oss.str().c_str(), watch, 0);

        if (rc != ZOK) {
            MCE_WARN( "ConfigManager::updateProducerConfig zoo_exists failed because "<<zerror(rc)<< __FILE__<<__LINE__ );
            updateFlag =false;
            return false;
        }

        rc = zoo_get(zk, oss.str().c_str(), watch, bc->buffer, &(bc->buflen), 0);

        if (rc != ZOK) {
            MCE_WARN( "ConfigManager::updateProducerConfig zoo_get failed because "<<zerror(rc)<< __FILE__<<__LINE__ );
            updateFlag =false;
            return false;
        }

        std::string ccVersion(bc->buffer, bc->buffer + bc->buflen);
        if(checkVersion) {
            MCE_INFO("ConfigManager::updateProducerConfig cc need check version");
            std::map<std::string, ConfigDataPtr>::const_iterator findIt = config->childrenDatas_.find(TRIPOD_PRODUCER_CC);
            if(findIt == config->childrenDatas_.end()) {
                MCE_WARN("ConfigManager::updateProducerConfig no cc");
                updateFlag =false;
                return false;
            }
            if(findIt->second->version_ != ccVersion) {
                updateFlag = true;
                findIt->second->version_ = ccVersion;
                bool sucCCFlag = getChildrenDatas(findIt->second, zk, oss);
                if(sucCCFlag == false) {
                    MCE_WARN("ConfigManager::updateProducerConfig get cc childrendatas failed");
                    updateFlag =false;
                    return false;
                }
            }
        }else {
            MCE_INFO("ConfigManager::updateProducerConfig cc do not need check version");
            ConfigDataPtr newConfig(new ConfigData);
            newConfig->version_ = ccVersion;
            bool sucCCFlag = getChildrenDatas(newConfig, zk, oss);
            if(sucCCFlag == false) {
                MCE_WARN("ConfigManager::updateProducerConfig get cc childrendatas failed");
                updateFlag =false;
                return false;
            }
            config->childrenDatas_.insert(make_pair(TRIPOD_PRODUCER_CC, newConfig));
            updateFlag = true;
        }

        std::ostringstream oss1;
        oss1 << "/"<<TRIPOD_PRODUCER<<"/"<<TRIPOD_PRODUCER_JAVA;
        
        rc = zoo_exists(zk, oss1.str().c_str(), watch, 0);

        if (rc != ZOK) {
            MCE_WARN( "ConfigManager::updateProducerConfig zoo_exists failed because "<<zerror(rc)<< __FILE__<<__LINE__ );
            updateFlag =false;
            return false;
        }

        rc = zoo_get(zk, oss1.str().c_str(), watch, bc->buffer, &(bc->buflen), 0);

        if (rc != ZOK) {
            MCE_WARN( "ConfigManager::updateProducerConfig zoo_get failed because "<<zerror(rc)<< __FILE__<<__LINE__ );
            updateFlag =false;
            return false;
        }

        std::string javaVersion(bc->buffer, bc->buffer + bc->buflen);
        if(checkVersion) {
            MCE_INFO("ConfigManager::updateProducerConfig java need check version");
            std::map<std::string, ConfigDataPtr>::const_iterator findIt = config->childrenDatas_.find(TRIPOD_PRODUCER_JAVA);
            if(findIt == config->childrenDatas_.end()) {
                MCE_WARN("ConfigManager::updateProducerConfig no java");
                updateFlag =false;
                return false;
            }
            if(findIt->second->version_ != javaVersion) {
                updateFlag = true;
                findIt->second->version_ = javaVersion;
                bool sucJavaFlag = getChildrenDatas(findIt->second, zk, oss1);
                if(sucJavaFlag == false) {
                    updateFlag =false;
                    MCE_WARN("ConfigManager::updateProducerConfig get java childrendatas failed");
                    return false;
                }
            }
        }else {
            MCE_INFO("ConfigManager::updateProducerConfig java do not need check version");
            ConfigDataPtr newConfig(new ConfigData);
            newConfig->version_ = javaVersion;
            bool sucJavaFlag = getChildrenDatas(newConfig, zk, oss1);
            if(sucJavaFlag == false) {
                MCE_WARN("ConfigManager::updateProducerConfig get java childrendatas failed");
                updateFlag =false;
                return false;
            }
            config->childrenDatas_.insert(make_pair(TRIPOD_PRODUCER_JAVA, newConfig));
            updateFlag = true;
        }

    }catch(...)
    {
        MCE_WARN( "ConfigManager::updateProducerConfig can not init config "<< __FILE__<<__LINE__ );
        updateFlag =false;
        return false;
    }

    return true;
}

bool ConfigManager::updateCacheConfig(const ConfigDataPtr& config, zhandle_t* zk, bool checkVersion, bool& updateFlag) {
    MCE_INFO("ConfigManager::updateCacheConfig");

    int watch = 1;
    updateFlag =false;

    try
    {

        BufferContainerPtr bc( new BufferContainer ); 

        std::ostringstream oss;
        oss << "/"<<TRIPOD_CACHE;
        
        int rc = zoo_exists(zk, oss.str().c_str(), watch, 0);

        if (rc != ZOK) {
            MCE_WARN( "ConfigManager::updateCacheConfig zoo_exists failed because "<<zerror(rc)<< __FILE__<<__LINE__ );
            updateFlag = false;
            return false;
        }

        ConfigDataPtr childrenConfig(new ConfigData);

        bool sucChildrenFlag = getChildrenDatas(childrenConfig, zk, oss, true);

        if(sucChildrenFlag == false) {
            MCE_WARN("ConfigManager::updateCacheConfig get namespaces failed");
            updateFlag = false;
            return false;
        }

        if(checkVersion) {
            MCE_INFO("ConfigManager::updateCacheConfig need check version");
            std::map<std::string, ConfigDataPtr> oldChild;
            oldChild.swap(config->childrenDatas_);
            for(std::map<std::string, ConfigDataPtr>::const_iterator it = childrenConfig->childrenDatas_.begin(); it != childrenConfig->childrenDatas_.end(); ++it) {
                std::map<std::string, ConfigDataPtr>::const_iterator findIt = oldChild.find(it->first);
                if(findIt != oldChild.end()) {
                    if(it->second->version_ != findIt->second->version_) {
                        updateFlag = true;
                        findIt->second->version_ = it->second->version_;
                        std::ostringstream oss1;
                        oss1<<oss.str()<<"/"<<it->first;
                        bool sucChildrenFlag = getChildrenDatas(findIt->second, zk, oss1); 
                        if(sucChildrenFlag == false) {
                            MCE_WARN("ConfigManager::updateCacheConfig get namespace " << findIt->first << " children failed");
                            updateFlag = false;
                            return false;
                        }
                    }
                    config->childrenDatas_.insert(make_pair(findIt->first, findIt->second));
                } else {
                    ConfigDataPtr newConfig(new ConfigData);
                    newConfig->name_ = it->second->name_;
                    newConfig->version_ = it->second->version_;
                    
                    std::ostringstream oss1;
                    oss1<<oss.str()<<"/"<<it->first;
                    bool sucChildrenFlag = getChildrenDatas(newConfig, zk, oss1); 
                    if(sucChildrenFlag == false) {
                        MCE_WARN("ConfigManager::updateCacheConfig get namespace " << findIt->first << " children failed");
                        updateFlag = false;
                        return false;
                    }
                    config->childrenDatas_.insert(make_pair(newConfig->name_, newConfig));
                }
            }
        }else {
            MCE_INFO("ConfigManager::updateCacheConfig do not need check version");
            config->childrenDatas_.clear();
            for(std::map<std::string, ConfigDataPtr>::const_iterator it = childrenConfig->childrenDatas_.begin(); it != childrenConfig->childrenDatas_.end(); ++it) {
                std::ostringstream oss1;
                oss1<<oss.str()<<"/"<<it->first;
                
                ConfigDataPtr newConfig(new ConfigData);
                newConfig->name_ = it->second->name_;
                newConfig->version_ = it->second->version_;

                bool sucGrandsonFlag = getChildrenDatas(newConfig, zk, oss1);
                if(sucGrandsonFlag == false) {
                    MCE_WARN("ConfigManager::updateCacheConfig get namespace " << it->first << " children failed");
                    updateFlag = false;
                    return false;
                }
                config->childrenDatas_.insert(make_pair(it->first, newConfig));
            }
            updateFlag = true;
        }
    }catch(...)
    {
        MCE_WARN( "ConfigManager::updateCacheConfig can not init config "<< __FILE__<<__LINE__ );
        updateFlag = false;
        return false;
    }

    return true;
}

bool ConfigManager::getChildrenDatas(const ConfigDataPtr& config, zhandle_t* zk, const std::ostringstream& path, bool needVersion) {
    MCE_INFO("ConfigManager::getChildrenDatas path " << path.str() << " needVersion " << needVersion);
    struct String_vector nodeNames;
    int noWatch = 0;	
    int rc = zoo_get_children(zk, path.str().c_str(), noWatch, &nodeNames);

    if (rc != ZOK) {
        MCE_WARN( "ConfigManager::getChildrenDatas zoo_get_children failed because "<<zerror(rc)<< __FILE__<<__LINE__ );
        return false;
    }

    config->childrenDatas_.clear();

    for (int i = 0; i < nodeNames.count; ++i) {
        ConfigDataPtr newConfig(new ConfigData);
        newConfig->name_ = nodeNames.data[i];

        if(needVersion) {
            BufferContainerPtr bc( new BufferContainer ); 

            std::ostringstream oss;
            oss << path.str() << "/" << newConfig->name_; 

            int rc = zoo_get(zk, oss.str().c_str(), 1, bc->buffer, &(bc->buflen), 0); 
            if (rc != ZOK) {
                MCE_WARN( "ConfigManager::getChildrenDatas zoo_get failed because "<<zerror(rc)<< __FILE__<<__LINE__ );
                return false;
            } 
            newConfig->version_ = std::string(bc->buffer, bc->buffer + bc->buflen);
        }
        config->childrenDatas_.insert(make_pair(newConfig->name_, newConfig));
    }

    return true;
}

};
};
};

