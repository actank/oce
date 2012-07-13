#include "ClusterManager.h"
#include <iostream>
#include <boost/pointer_cast.hpp>

#include "ZkManagerFactory.h"

#include <LogWrapper.h>

using xce::distcache::ClusterManager;
using boost::shared_mutex;
using boost::upgrade_lock;
using boost::upgrade_to_unique_lock;
using std::vector;
using std::string;
using std::map;
using xce::distcache::node::ReadNodePtr;
using xce::distcache::node::ReadNode;
using xce::distcache::node::WriteNodePtr;
using xce::distcache::node::WriteNode;
using xce::distcache::node::MetaNodePtr;
using xce::distcache::node::MetaNode;
using xce::distcache::node::ItemPtr;
using xce::distcache::node::NodePtr;
using xce::distcache::node::NodeFactory;
using xce::distcache::node::Item;
using std::ostringstream;
using boost::dynamic_pointer_cast;
using xce::distcache::ClusterConfigPtr;
using xce::distcache::ClusterConfig;
using xce::distcache::managers_lock_;
using xce::distcache::managers_;


int ClusterManager::updateClusterConfig(zhandle_t* zk) {
	
	ClusterConfigPtr newconfig(new ClusterConfig);

	int block_size = init(newconfig, zk);
	
	if( block_size == -1 )
	{
		return block_size;
	}

	{
		boost::upgrade_lock<shared_mutex> lock(lock_);
		boost::upgrade_to_unique_lock<shared_mutex> uniqueLock(lock);
		config_ = newconfig;
	}

	return block_size;
}

void ClusterConfig::init(int block_size) {
	blockSize_ = block_size;
	for (int init = 0; init < block_size; ++init) {
		readOnlineNodes_.push_back(vector<ReadNodePtr>());
		readUpgradingNodes_.push_back(vector<ReadNodePtr>());
		readOfflineNodes_.push_back(vector<ReadNodePtr>());
	}
}

string ClusterConfig::str() {
	ostringstream buff;
	buff << "ClusterConfig [";
	buff << "blockSize=" << blockSize_ << ";";
	buff << "metaNodes=" << metaNodes_.size() << ";";
	buff << "writeNodes=" << writeNodes_.size() << ";";

	for (vector<vector<ReadNodePtr> >::iterator readOnlineNode = readOnlineNodes_.begin(); readOnlineNode != readOnlineNodes_.end(); ++readOnlineNode) {
		buff << "readOnlineNode [" << readOnlineNode->size();
	}
	return buff.str();
}

int ClusterManager::initialize(const std::string& name, const std::string& zkAddress) {
	
	name_ = name;

	zm_ = ZkManagerFactory::getInstance()->getZkManager(zkAddress);
	if( zm_ == NULL ) {
		MCE_WARN("ClusterManager::initialize zkManager initialize failed!");
		return -1;
	}
	zm_->registerCluster(name_,this);

	int block_size = updateClusterConfig(zm_->getZk());

	return block_size;
}

vector<ReadNodePtr> ClusterManager::getReadNodes(int block) {
	boost::shared_lock<shared_mutex> lock(lock_);
	return config_->readOnlineNodes_[block];
}

vector<ReadNodePtr> ClusterManager::getReadNodesOffline(int block) {
	boost::shared_lock<shared_mutex> lock(lock_);
	return config_->readOfflineNodes_[block];
}

vector<ReadNodePtr> ClusterManager::getReadNodesWithUpgrading(int block) {
	vector<ReadNodePtr> ret;
	vector<ReadNodePtr> online;
	vector<ReadNodePtr> upgrading;
	{
		boost::shared_lock<shared_mutex> lock(lock_);
		online = config_->readOnlineNodes_[block];
		upgrading = config_->readUpgradingNodes_[block];
	}
	ret.insert(ret.end(), online.begin(), online.end());
	ret.insert(ret.end(), upgrading.begin(), upgrading.end());
	return ret;
}

vector<MetaNodePtr> ClusterManager::getMetaNodes() {
	boost::shared_lock<shared_mutex> lock(lock_);
	return config_->metaNodes_;
}

vector<WriteNodePtr> ClusterManager::getWriteNodes() {
	boost::shared_lock<shared_mutex> lock(lock_);
	return config_->writeNodes_;
}

vector<WriteNodePtr> ClusterManager::getWriteNodesOffline() {
	boost::shared_lock<shared_mutex> lock(lock_);
	return config_->writeNodesOffline_;
}

int ClusterManager::getNumBlocks() {
	boost::shared_lock<shared_mutex> lock(lock_);
	return config_->blockSize_;
}

bool ClusterManager::allocId2Block(long id, int block) {
	vector<MetaNodePtr> metaNodes = getMetaNodes();
	for (int x = 0; x < (int)metaNodes.size(); ++x) {
		try {
			metaNodes.at(x)->alloc(ItemPtr(new Item(block, id)));
			return true;
		} catch (...) {
			MCE_WARN( "ClusterManager::allocId2Block exception block: " << block << " id: " << id );
		}
	}
	return false;
}

int ClusterManager::init(const ClusterConfigPtr& config, zhandle_t* zk) {

	int block_size = -1;

	try
	{
		int watch = 1;

		BufferContainerPtr bc( new BufferContainer ); 

		ostringstream oss;
		oss << "/"<< name_ ;
		int rc = 0;
		rc = zoo_exists(zk, oss.str().c_str(), watch, 0);
		
		if (rc != ZOK) {
			//To do
			MCE_WARN( "ClusterManager::init zoo_exists failed because "<<zerror(rc)<< __FILE__<<__LINE__ );
			return -1;
		}
		
		ostringstream oss1;
		oss1 << "/"<< name_ << "/info";
	
		rc = zoo_exists(zk, oss1.str().c_str(), watch, 0);
		if (rc != ZOK) {
			//To do
			MCE_WARN( "ClusterManager::init zoo_exists failed because "<<zerror(rc)<< __FILE__<<__LINE__ );
			return -1;
		}
		rc = zoo_get(zk, oss1.str().c_str(), watch, bc->buffer, &(bc->buflen), 0);
		
		if (rc != ZOK) {
			//To do
			MCE_WARN( "ClusterManager::init zoo_get failed because "<<zerror(rc)<< __FILE__<<__LINE__ );
			return -1;
		}
		string cluster_info_data(bc->buffer, bc->buffer + bc->buflen);
		ClusterInfo cluster_info;
		cluster_info.ParseFromString(cluster_info_data);
		block_size = cluster_info.block_size();
		config->init(block_size);
	
		ostringstream oss2;
		oss2 << "/"  << name_;
		struct String_vector node_names;
		
		rc = zoo_exists(zk, oss2.str().c_str(), watch, 0); 
		if (rc != ZOK) {
			//To do
			MCE_WARN( "ClusterManager::init zoo_exists failed because "<<zerror(rc)<< __FILE__<<__LINE__ );
			return -1;
		}
		
		rc = zoo_get_children(zk, oss2.str().c_str(), watch, &node_names);
		
		if (rc != ZOK) {
			//To do
			MCE_WARN( "ClusterManager::init zoo_get failed because "<<zerror(rc)<< __FILE__<<__LINE__ );
			return -1;
		}
		for (int node_name = 0; node_name < node_names.count; ++node_name) {
			if (!strcmp("info", node_names.data[node_name])) {
				continue;
			}
			ostringstream oss3;
			oss3 << "/"<< name_ << "/" << node_names.data[node_name] << "/info";
			bc->buflen = sizeof(bc->buffer);
			
			rc = zoo_exists(zk, oss3.str().c_str(), watch, 0);
			if (rc != ZOK) {
				//To do
				MCE_WARN( "ClusterManager::init zoo_exists failed because "<<zerror(rc)<< __FILE__<<__LINE__ );
				return -1;
			}
			
			rc = zoo_get(zk, oss3.str().c_str(), watch, bc->buffer, &(bc->buflen), 0);
			
			if (rc != ZOK) {
				//To do
				MCE_WARN( "ClusterManager::init zoo_get failed because "<<zerror(rc)<< __FILE__<<__LINE__ );
				return -1;
			}
		
			string node_info_data(bc->buffer, bc->buffer + bc->buflen);
			NodeInfo node_info;
			node_info.ParseFromString(node_info_data);
			ostringstream oss4;
			oss4 << "/"<< name_ << "/" << node_names.data[node_name] << "/status";
			bc->buflen = sizeof(bc->buffer);
			
			rc = zoo_exists(zk, oss4.str().c_str(), watch, 0);
			if (rc != ZOK) {
				//To do
				MCE_WARN( "ClusterManager::init zoo_exists failed because "<<zerror(rc)<< __FILE__<<__LINE__ );
				return -1;
			}
			rc = zoo_get(zk, oss4.str().c_str(), watch, bc->buffer, &(bc->buflen), 0);
			
			if (rc != ZOK) {
				//To do
				MCE_WARN( "ClusterManager::init zoo_get failed because "<<zerror(rc)<< __FILE__<<__LINE__ );
				return -1;
			}
			string node_status_data(bc->buffer, bc->buffer + bc->buflen);
			NodeStatus node_status;
			node_status.ParseFromString(node_status_data);
	
			NodeCategoryEnum category = node_info.category();
			NodeStatusEnum status = node_status.status();
			NodeTypeEnum type = node_info.type();
			string node_config = node_info.config();
			NodePtr node = NodeFactory::create(category, type, node_names.data[node_name], node_config);
			if (category == META) {
				config->metaNodes_.push_back(dynamic_pointer_cast<MetaNode>(node));
			} else if (category == READ) {
				ostringstream oss5;
				oss5 << "/"<< name_ << "/" << node_names.data[node_name] << "/blocks";
				struct String_vector blocks;
				
				rc = zoo_exists(zk, oss5.str().c_str(), watch, 0);
				if (rc != ZOK) {
					//To do
					MCE_WARN( "ClusterManager::init zoo_exists failed because "<<zerror(rc)<< __FILE__<<__LINE__ );
					return -1;
				}
			
				rc = zoo_get_children(zk, oss5.str().c_str(), watch, &blocks);
				
				if (rc != ZOK) {
					//To do
					MCE_WARN( "ClusterManager::init zoo_get_children failed because "<<zerror(rc)<< __FILE__<<__LINE__ );
					return -1;
				}
				for (int blockStr = 0; blockStr < blocks.count; ++blockStr) {
					int block = boost::lexical_cast<int>(blocks.data[blockStr]);
					if (status == ONLINE) {
						ReadNodePtr fu = dynamic_pointer_cast<ReadNode>(node);
						config->readOnlineNodes_[block].push_back(dynamic_pointer_cast<ReadNode>(fu));
					} else if (status == UPGRADING) {
						config->readUpgradingNodes_[block].push_back(dynamic_pointer_cast<ReadNode>(node));
					} else if (status == OFFLINE) {
						config->readOfflineNodes_[block].push_back(dynamic_pointer_cast<ReadNode>(node));
					}
				}
			} else if (category == WRITE) {
				if(status == ONLINE)
				{
					config->writeNodes_.push_back(dynamic_pointer_cast<WriteNode>(node));
				}else if(status == OFFLINE)
				{
					config->writeNodesOffline_.push_back(dynamic_pointer_cast<WriteNode>(node));
				}
			}
		}
	}catch(...)
	{
		MCE_WARN( "ClusterManager::updateClusterConfig can not init config "<< __FILE__<<__LINE__ );
		return -1;
	}
	return block_size;
}

bool ClusterManager::setStatus( const std::string& nodeName, const NodeStatusEnum status )
{
	boost::upgrade_lock<shared_mutex> lock(lock_);
	boost::upgrade_to_unique_lock<shared_mutex> uniqueLock(lock);
	
	ostringstream osPath;
	osPath << "/"<< name_ << "/" << nodeName << "/status";

	NodeStatus nodeStatus;

	nodeStatus.set_status( status );

	std::string dataStr = nodeStatus.SerializeAsString();

	int rc = 0;

	rc = zoo_set(zm_->getZk(), osPath.str().c_str(), dataStr.c_str(), dataStr.length(), -1);

	if( rc != ZOK )
	{
		MCE_WARN( "ClusterManager::setStatus zoo_set failed because "<<zerror(rc)<< __FILE__<<__LINE__ );
		return false;
	}

	return true;
}

