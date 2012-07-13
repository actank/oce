#ifndef CLUSTERMANAGERFACTORY_H
#define CLUSTERMANAGERFACTORY_H

#include "ClusterManager.h"

namespace xce {
namespace distcache {

	class ClusterManagerFactory {
		private:
			ClusterManagerFactory(){
			}

		private:
			static ClusterManagerFactory* instance_;
			
			static boost::shared_mutex lock_;

			std::map<std::string, ClusterManager*> clusterManagerMap_;
			
			boost::shared_mutex clusterManagerMaplock_;
		
		public:
			static ClusterManagerFactory* getInstance() {
				if( instance_ == NULL ) {
					boost::upgrade_lock<boost::shared_mutex> lock(lock_);
					boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock1(lock);
					if( instance_ == NULL ) {
						instance_ = new ClusterManagerFactory;
					}
				}
				return instance_;
			}

			ClusterManager* getClusterManager(std::string clusterName, std::string zkAddress) {
				std::map<std::string, ClusterManager*>::iterator findIt;
				{
					boost::shared_lock<boost::shared_mutex> lock(clusterManagerMaplock_);
					findIt = clusterManagerMap_.find(clusterName+zkAddress);
					if( findIt != clusterManagerMap_.end() ){
						return findIt->second;
					}
				}
				{
					boost::upgrade_lock<boost::shared_mutex> lock(clusterManagerMaplock_);
					boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock1(lock);
					findIt = clusterManagerMap_.find(clusterName+zkAddress);
					if( findIt != clusterManagerMap_.end() ){
						
						return findIt->second;
					}
					ClusterManager* cm = new ClusterManager;
					if( cm->initialize( clusterName, zkAddress) != -1 ) {
						clusterManagerMap_.insert(make_pair(clusterName+zkAddress, cm));
						return cm;
					}
					return NULL;
				}
			}
	};

	ClusterManagerFactory* ClusterManagerFactory::instance_ = NULL;
	boost::shared_mutex ClusterManagerFactory::lock_;
}
}

#endif
