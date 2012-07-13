#include "ZkManagerFactory.h"

namespace com {
namespace renren {
namespace tripod {

ZkManagerPtr ZkManagerFactory::getZkManager(std::string zkAddress) {
    MCE_INFO("ZkManagerFactory::getZkManager zkAddress " << zkAddress);
    std::map<std::string, ZkManagerPtr>::iterator findIt;
    {
        boost::shared_lock<boost::shared_mutex> lock(zkManagerMaplock_);
        findIt = zkManagerMap_.find(zkAddress);
        if( findIt != zkManagerMap_.end() ){
            return findIt->second;
        }
    }
    {
        boost::upgrade_lock<boost::shared_mutex> lock(zkManagerMaplock_);
        boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock1(lock);
        findIt = zkManagerMap_.find(zkAddress);
        if( findIt != zkManagerMap_.end() ){
            return findIt->second;
        }
        ZkManagerPtr zm(new ZkManager);
        if ( zm->initialize(zkAddress) ) {
            MCE_INFO("ZkManagerFactory::getZkManager init new ZkManager " << zkAddress << " success!");
            zkManagerMap_.insert(make_pair(zkAddress, zm));
            return zm;
        }
        MCE_WARN("ZkManagerFactory::getZkManager init new ZkManager " << zkAddress << "failed!");
        return ZkManagerPtr();
    }
}

}
}
}
