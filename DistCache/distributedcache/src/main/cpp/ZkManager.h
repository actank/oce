#ifndef ZKMANAGER_H
#define ZKMANAGER_H

#include <zookeeper/zookeeper.h>

#include <boost/thread/thread.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <set>
namespace xce {
namespace distcache {

#define DEFAULT_ZKADDRESS "UserZooKeeper1:2181,UserZooKeeper2:2181,UserZooKeeper3:2181,UserZooKeeper4:2181,UserZooKeeper5:2181/DistCache"

	const char* state2String(int state);

	const char* watcherEvent2String(const int ev);

	void ZkWatcher(zhandle_t *zzh, int type, int state, const char *path, void *watcherCtx);

	class ClusterManager;

	class ZkManager {
	
		private:
			std::map<std::string, ClusterManager*> clusterMap_;
			
			boost::shared_mutex clusterLock_;
			
			zhandle_t *zk_;
			
			boost::mutex zkLock_;
			boost::condition zkCdt_;
		
			std::set<std::string> reNotifyClusterSet_;

			boost::mutex notifySetLock_;
			boost::condition notifySetCdt_;

			std::string zkAddress_;

			boost::thread* reNotifyThread_;
		public:
			
			static void static_run(ZkManager* zm) {
				zm->run();
			}
	
		public:

			void run();

			void startAndDetach() {
				reNotifyThread_ = new boost::thread(boost::bind(static_run, this));
				reNotifyThread_->detach();
			}

		private:
			void close();
			
			std::string getClusterName(std::string path);
			
			bool notifyChange(std::string clusterName);
			
			std::set<std::string> notifyChangeAll();

			void addFailCluster( std::string clusterName );

			void addFailCluster( std::set<std::string> clusterNameSet );

		public:
			
			bool initialize(std::string zkAddress);
			
			void notifyZkLock();
			
			void registerCluster(std::string cluserName, ClusterManager* clusterManager);

			void notifyHandle(std::string path);
			
			bool reInitZk();

			zhandle_t* getZk();

	};

	static std::map<zhandle_t*, ZkManager*> managers_;

	static boost::shared_mutex managers_lock_;
}
}
#endif
