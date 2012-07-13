#ifndef ZKMANAGERFACTORY_H
#define ZKMANAGERFACTORY_H

#include "ZkManager.h"

namespace xce {
namespace distcache {

	class ZkManagerFactory {
		private:
			ZkManagerFactory() {
			}

		private:
			static ZkManagerFactory* instance_;
			
			static boost::shared_mutex lock_;
			
			static std::string defaultZkAddress_;

			std::map<std::string, ZkManager*> zkManagerMap_;
			
			boost::shared_mutex zkManagerMaplock_;
		
		public:
			static ZkManagerFactory* getInstance() {
				if( instance_ == NULL ) {
					boost::upgrade_lock<boost::shared_mutex> lock(lock_);
					boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock1(lock);
					if( instance_ == NULL ) {
						instance_ = new ZkManagerFactory;
					}
				}
				return instance_;
			}

			ZkManager* getZkManager(std::string zkAddress) {
				std::map<std::string, ZkManager*>::iterator findIt;
				if( zkAddress == "" ) {
					{
						boost::shared_lock<boost::shared_mutex> lock(zkManagerMaplock_);
						findIt = zkManagerMap_.find(defaultZkAddress_);
						if( findIt != zkManagerMap_.end() ){
							return findIt->second;
						}
					}
					{
						boost::upgrade_lock<boost::shared_mutex> lock(zkManagerMaplock_);
						boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock1(lock);
						findIt = zkManagerMap_.find(defaultZkAddress_);
						if( findIt != zkManagerMap_.end() ){
							
							return findIt->second;
						}
						ZkManager* zm = new ZkManager;
						if( zm->initialize(defaultZkAddress_) ) {
							zkManagerMap_.insert(make_pair(defaultZkAddress_, zm));
							return zm;
						}
						return NULL;
					}
				}else {
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
						ZkManager* zm = new ZkManager;
						if ( zm->initialize(zkAddress) ) {
							zkManagerMap_.insert(make_pair(zkAddress, zm));
							return zm;
						}
						return NULL;
					}
				}
			}
	};

	std::string ZkManagerFactory::defaultZkAddress_ = "UserZooKeeper1:2181,UserZooKeeper2:2181,UserZooKeeper3:2181,UserZooKeeper4:2181,UserZooKeeper5:2181/DistCache";
	ZkManagerFactory* ZkManagerFactory::instance_ = NULL; 
	boost::shared_mutex ZkManagerFactory::lock_; 
}
}
#endif
