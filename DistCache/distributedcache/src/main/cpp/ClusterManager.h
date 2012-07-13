#ifndef CLUSTERMANAGER_H
#define CLUSTERMANAGER_H

#include <zookeeper/zookeeper.h>
#include <boost/thread/shared_mutex.hpp>
#include <boost/lexical_cast.hpp>
#include "Node.h"

#include "ZkManager.h"

namespace xce {
namespace distcache {

class BufferContainer {
	public:
		BufferContainer() {
			buflen = sizeof(buffer);
			memset(buffer, 0, buflen);
		}
	public:
		char buffer[1048576];
		int buflen;
};
typedef boost::shared_ptr<BufferContainer> BufferContainerPtr;

class ClusterConfig {
public:
	void init(int block_size);
	std::string str();
	int blockSize_;
	std::vector<xce::distcache::node::MetaNodePtr> metaNodes_;
	std::vector<xce::distcache::node::WriteNodePtr> writeNodes_;
	std::vector<xce::distcache::node::WriteNodePtr> writeNodesOffline_;
	std::vector<std::vector<xce::distcache::node::ReadNodePtr> > readOnlineNodes_;
	std::vector<std::vector<xce::distcache::node::ReadNodePtr> > readUpgradingNodes_;
	std::vector<std::vector<xce::distcache::node::ReadNodePtr> > readOfflineNodes_;
};

typedef boost::shared_ptr<ClusterConfig> ClusterConfigPtr;

class ClusterManager;

typedef boost::shared_ptr<ClusterManager> ClusterManagerPtr;

class ZkManager;

class ClusterManager {
public:
	int initialize(const std::string& name, const std::string& zkAddress);
	std::vector<xce::distcache::node::ReadNodePtr> getReadNodes(int block);
	std::vector<xce::distcache::node::ReadNodePtr> getReadNodesWithUpgrading(int block);
	std::vector<xce::distcache::node::ReadNodePtr> getReadNodesOffline(int block);
	std::vector<xce::distcache::node::MetaNodePtr> getMetaNodes();
	std::vector<xce::distcache::node::WriteNodePtr> getWriteNodes();
	std::vector<xce::distcache::node::WriteNodePtr> getWriteNodesOffline();
	int getNumBlocks();
	bool allocId2Block(long id, int block);

	int updateClusterConfig( zhandle_t* zk );
	
	bool setStatus( const std::string& nodeName, const NodeStatusEnum status );

private:

	boost::shared_mutex lock_;
	ClusterConfigPtr config_;
	ZkManager* zm_;
	std::string name_;

	int init(const ClusterConfigPtr& config, zhandle_t* zk);
};

}
}

#endif
