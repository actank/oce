#include "ConfigManagerFactory.h"

namespace com {
namespace renren {
namespace tripod {

ConfigManagerPtr ConfigManagerFactory::getConfigManager(std::string zkAddress) {
    MCE_INFO("ConfigManagerFactory::getConfigManager zkAddress " << zkAddress);
    std::map<std::string, ConfigManagerPtr>::iterator findIt;
    {
        boost::shared_lock<boost::shared_mutex> lock(configManagerMaplock_);
        findIt = configManagerMap_.find(zkAddress);
        if( findIt != configManagerMap_.end() ){
            return findIt->second;
        }
    }
    {
        boost::upgrade_lock<boost::shared_mutex> lock(configManagerMaplock_);
        boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock1(lock);
        findIt = configManagerMap_.find(zkAddress);
        if( findIt != configManagerMap_.end() ){
            return findIt->second;
        }
        ConfigManagerPtr cm(new ConfigManager);
        if( cm->initialize(zkAddress)) {
            MCE_INFO("ConfigManagerFactory::getConfigManager init new ConfigManager " << zkAddress << " success!");
            configManagerMap_.insert(make_pair(zkAddress, cm));
            return cm;
        }
        MCE_INFO("ConfigManagerFactory::getConfigManager init new ConfigManager " << zkAddress << " failed!");
        return ConfigManagerPtr();
    }
}

}
}
}
