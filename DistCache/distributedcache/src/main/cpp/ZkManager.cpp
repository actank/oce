#include "ZkManager.h"

#include "ClusterManager.h"

namespace xce {
namespace distcache {

	const char* state2String(int state){
	    switch(state){
	    case 0:
	        return "ZOO_CLOSED_STATE";
	    case 1:
	        return "ZOO_CONNECTING_STATE";
	    case 2:
	        return "ZOO_ASSOCIATING_STATE";
	    case 3:
	        return "ZOO_CONNECTED_STATE";
	    case -112:
	        return "ZOO_EXPIRED_SESSION_STATE";
	    case -113:
	        return "ZOO_AUTH_FAILED_STATE";
	    }
	    return "INVALID_STATE";
	}
	
	const char* watcherEvent2String(const int ev){
	    switch(ev){
	    case 0:
	        return "ZOO_ERROR_EVENT";
	    case 1:
	        return "ZOO_CREATED_EVENT";
	    case 2:
	        return "ZOO_DELETED_EVENT";
	    case 3:
	        return "ZOO_CHANGED_EVENT";
	    case 4:
	        return "ZOO_CHILD_EVENT";
	    case -1:
	        return "ZOO_SESSION_EVENT";
	    case -2:
	        return "ZOO_NOTWATCHING_EVENT";
	    }
	    return "INVALID_EVENT";
	}
	
	
	void ZkWatcher(zhandle_t *zzh, int type, int state, const char *path, void *watcherCtx) {
		MCE_WARN( "ZkWatcher type = "<<watcherEvent2String(type)<<" state = "<<state2String(state)<<" path = "<<path );
	
		if( type == ZOO_SESSION_EVENT )
		{
			
			if( state == ZOO_CONNECTED_STATE )
			{
				boost::shared_lock<boost::shared_mutex> lock(managers_lock_);
				managers_[zzh]->notifyZkLock();
				managers_[zzh]->notifyHandle("");
				return;
			}
		
			if( state == ZOO_EXPIRED_SESSION_STATE )
			{
				ZkManager* tempPtr = 0;
				{
					boost::shared_lock<boost::shared_mutex> lock(managers_lock_);
					tempPtr = managers_[zzh];
				}
				
				if( tempPtr )
				{
					while( tempPtr->reInitZk() == false )
					{
						sleep(1000);
					}
				}else
				{
					MCE_WARN("ZkWatcher can not reinitzk!");
				}
				
				{
					boost::upgrade_lock<boost::shared_mutex> lock(managers_lock_);
					boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock1(lock);
					std::map<zhandle_t*,ZkManager*>::iterator findIt = managers_.find( zzh );
					if( findIt != managers_.end() )
					{
						managers_.erase(managers_.find(zzh));
					}
				}
				return;
			}
		}
	
		{
			boost::shared_lock<boost::shared_mutex> lock(managers_lock_);
			managers_[zzh]->notifyHandle(path);
		}
		
	}

	bool ZkManager::initialize(std::string zkAddress) {
		if( zkAddress == "" ) {
			zkAddress_ = DEFAULT_ZKADDRESS;
		}else {
			zkAddress_ = zkAddress;
		}
		
		{
			boost::unique_lock<boost::mutex> lock( zkLock_ );
	
			{
				boost::upgrade_lock<boost::shared_mutex> lock(managers_lock_);
				boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock1(lock); 
				
				zk_ = zookeeper_init(zkAddress_.c_str(), xce::distcache::ZkWatcher, 10000, 0, 0, 0);
		
				if( zk_ == NULL )
				{
					MCE_WARN("ZkManager::initZk can not initZk!");
					return false;
				}
				managers_[zk_] = this;
			}
	
			zkCdt_.wait( lock );
		}

		startAndDetach();

		return true;
	}

	void ZkManager::notifyZkLock() {
		zkCdt_.notify_all();
	}

	void ZkManager::close() {
		boost::unique_lock<boost::mutex> lock( zkLock_ );
		if (zk_ != NULL) {
			zookeeper_close(zk_);
		}
	}
	
	bool ZkManager::reInitZk() {
		close();
		
		boost::unique_lock<boost::mutex> lock( zkLock_ );
		
		{
			boost::upgrade_lock<boost::shared_mutex> lock(managers_lock_);
			boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock1(lock); 
			
			zk_ = zookeeper_init(zkAddress_.c_str(), xce::distcache::ZkWatcher, 10000, 0, 0, 0);
		
			if( zk_ == NULL )
			{
				MCE_WARN("ZkManager::reinitZk can not initZk!");
				return false;
			}
			managers_[zk_] = this;
		}
		
		return true;
	}

	void ZkManager::registerCluster( std::string clusterName, ClusterManager* clusterManager ) {
		boost::upgrade_lock<boost::shared_mutex> lock(clusterLock_);
		boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock1(lock);
		clusterMap_.insert(make_pair(clusterName, clusterManager));
	}

	void ZkManager::notifyHandle(std::string path) {
		if( path == "" ){
			std::set<std::string> failClusterSet = notifyChangeAll();
			if( failClusterSet.size() > 0 ) {
				addFailCluster( failClusterSet );
			}
		}else { 
			std::string clusterName = getClusterName(path);
			if( notifyChange( clusterName ) == false ) {
				addFailCluster( clusterName );
			}
		}
	}

	std::string ZkManager::getClusterName( std::string path ) {
		std::vector<std::string> part;
		boost::algorithm::split( part, path, boost::algorithm::is_any_of("/") );
		return part.at(1);
	}

	bool ZkManager::notifyChange( std::string clusterName ) {
		boost::shared_lock<boost::shared_mutex> lock(clusterLock_);
		std::map<std::string, ClusterManager*>::const_iterator findIt = clusterMap_.find( clusterName );
		if( findIt != clusterMap_.end() ) {
			{
				boost::unique_lock<boost::mutex> lock( zkLock_ );
				if( findIt->second->updateClusterConfig(zk_) != -1) {
					return true;
				}
			}
		}else {
			MCE_WARN("ZkManager::notifyChange no cluster " << clusterName << " in clusterMap_!");
		}
		return false;
	}
	
	std::set<std::string> ZkManager::notifyChangeAll() {
		boost::shared_lock<boost::shared_mutex> lock(clusterLock_);

		std::set<std::string> failSet;
		{
			boost::unique_lock<boost::mutex> lock( zkLock_ );
			for( std::map<std::string, ClusterManager*>::const_iterator it = clusterMap_.begin(); it != clusterMap_.end() ; ++ it ) {
				if( it->second->updateClusterConfig( zk_ ) == -1 ) {
					failSet.insert( it->first );
				}
			}
		}
		
		return failSet;
	}

	void ZkManager::addFailCluster( std::string clusterName ) {
		boost::unique_lock<boost::mutex> lock( notifySetLock_ );
		reNotifyClusterSet_.insert( clusterName );
		notifySetCdt_.notify_all();
	}
	
	void ZkManager::addFailCluster( std::set<std::string> clusterNameSet ) {
		boost::unique_lock<boost::mutex> lock( notifySetLock_ );
		reNotifyClusterSet_.insert( clusterNameSet.begin(), clusterNameSet.end() );
		notifySetCdt_.notify_all();
	}

	void ZkManager::run() {
		while( true ){
			std::set<std::string> clusterSet;
			std::set<std::string> failClusterSet;
			{
				boost::unique_lock<boost::mutex> lock( notifySetLock_ );
				if( reNotifyClusterSet_.size() == 0 ) {
					notifySetCdt_.wait( lock );
				}
				clusterSet.swap( reNotifyClusterSet_ );
			}
			for ( std::set<std::string>::const_iterator it = clusterSet.begin(); it != clusterSet.end(); ++it ) {
				if( notifyChange( *it ) == false ) {
					failClusterSet.insert( *it );
				}
			}
			if( failClusterSet.size() > 0 ) {
				addFailCluster( failClusterSet );
			}
			sleep( 1000 );
		}
	}

	zhandle_t* ZkManager::getZk() {
		boost::unique_lock<boost::mutex> lock( zkLock_ );
		return zk_;
	}
}
}
