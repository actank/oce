/*
 * Communicator.cc
 *
 *  Created on: 2011-4-11
 *      Author: guanghe.ge
 */

#include "communicator.h"

using namespace com::renren::tripod;

void ZooKeeperWatcher(zhandle_t *zzh, int type, int state, const char *path,
		void *watcherCtx) {
	if (type == ZOO_SESSION_EVENT) {

		if (state == ZOO_CONNECTED_STATE) {
			boost::unique_lock<boost::shared_mutex> lock(ready_mutex_);
			ready_ = true;
			return;
		}

		if (state == ZOO_EXPIRED_SESSION_STATE) {
			MCE_WARN("communicator ZooKeeperWatcher ZOO_EXPIRED_SESSION_STATE");
			return;
		}
	}

	//other case notify all.
	std::map<std::string, CommunicatorPtr> cs = CommunicatorFactory::getAll();

	std::string info(path);
	for (std::map<std::string, CommunicatorPtr>::iterator it = cs.begin(); it
			!= cs.end(); ++it) {
		it->second->NotifyObservers(info);
	}
}

CommunicatorPtr CommunicatorFactory::Create(
		const std::string& communicator_addr) {
	{
		boost::shared_lock<boost::shared_mutex> lock(mutex_);
		CommunicatorPtr result = communicators_[communicator_addr];
		if (result) {
			return result;
		}
	}
	try {
		boost::unique_lock<boost::shared_mutex> lock(mutex_);
		CommunicatorPtr result = communicators_[communicator_addr];
		if (result) {
			return result;
		}
		result = CommunicatorPtr(new Communicator());
		{
			boost::unique_lock<boost::shared_mutex> lock(ready_mutex_);
			ready_ = false;
		}
		result->Initialize(communicator_addr);
		communicators_[communicator_addr] = result;
		return result;
	} catch (...) {
		MCE_WARN("CommunicatorFactory::Create, " << " addr= "
				<< communicator_addr);
	}
}

void Communicator::Initialize(const std::string& Communicator_addr) {
	try {
		handle_ = zookeeper_init(Communicator_addr.c_str(), &ZooKeeperWatcher,
				10000, 0, 0, 0);
		{
			boost::shared_lock<boost::shared_mutex> lock(ready_mutex_);
			while (!ready_) {
				MCE_INFO("ZooKeeperCommunicator::Initialize, not ready!");
				sleep(2);
			}
		}
	} catch (...) {
		MCE_WARN("ZooKeeperCommunicator::Initialize Exception");
	}
}

void Communicator::Register(ObserverPtr observer) {
	try {
		RegisterObserver(observer);
	} catch (...) {
		MCE_WARN("ZooKeeperCommunicator::Register Exception");
	}
}

void Communicator::UnRegister(ObserverPtr observer) {
	try {
		RemoveObserver(observer);
	} catch (...) {
		MCE_WARN("ZooKeeperCommunicator::Register Exception");
	}
}

std::string Communicator::GetData(const std::string& path) {
	try {
		BufferContainerPtr bc(new BufferContainer);
		int rc =
				zoo_get(handle_, path.c_str(), 0, bc->buffer, &(bc->buflen), 0);
		std::string data(bc->buffer, bc->buffer + bc->buflen);
		return data;
	} catch (...) {
		MCE_WARN("ZooKeeperCommunicator::GetData Exception");
	}
}

std::vector<std::string> Communicator::GetChild(const std::string& path) {
	try {
		struct String_vector node_names;
		int rc = zoo_get_children(handle_, path.c_str(), 0, &node_names);
		std::vector<std::string> children;
		for (int i = 0; i < node_names.count; i++) {
			std::string child(node_names.data[i]);
			children.push_back(child);
		}
		return children;
	} catch (...) {
		MCE_WARN("ZooKeeperCommunicator::GetChild Exception");
	}
}

//ClusterPtr ZooKeeperCommunicator::ParseCluster(const std::string& name) {
//	try {
//		ClusterPtr result = ClusterPtr(new Cluster());
//		int rc = 0;
//		int watch = 1;
//		BufferContainerPtr bc(new BufferContainer);
//
//		//read all children of {name} cluster.
//		std::ostringstream oss_cluster;
//		oss_cluster << "/" << name;
//		struct String_vector node_names;
//		rc = zoo_get_children(handle_, oss_cluster.str().c_str(), watch,
//				&node_names);
//
//		for (int node_name = 0; node_name < node_names.count; ++node_name) {
//			/*read info of {name} cluster, include block_size and replicas.*/
//			if (!strcmp("info", node_names.data[node_name])) {
//				std::ostringstream oss_info;
//				oss_info << "/" << name << "/" << "info";
//				rc = zoo_get(handle_, oss_info.str().c_str(), watch,
//						bc->buffer, &(bc->buflen), 0);
//				std::string cluster_info_data(bc->buffer, bc->buffer
//						+ bc->buflen);
//
//				ClusterInfo cluster_info;
//				cluster_info.ParseFromString(cluster_info_data);
//				int block_size = cluster_info.blocksize();
//				result->SetBlockSize(block_size);
//				continue;
//			}
//
//			/*if child is a node.*/
//
//			//read info of node.
//			std::ostringstream oss_node_info;
//			oss_node_info << "/" << name << "/" << node_names.data[node_name]
//					<< "/info";
//			bc->buflen = sizeof(bc->buffer);
//			rc = zoo_get(handle_, oss_node_info.str().c_str(), watch,
//					bc->buffer, &(bc->buflen), 0);
//			std::string node_info_data(bc->buffer, bc->buffer + bc->buflen);
//			NodeInfo node_info;
//			node_info.ParseFromString(node_info_data);
//
//			//read status of node.
//			std::ostringstream oss_node_status;
//			oss_node_status << "/" << name << "/" << node_names.data[node_name]
//					<< "/status";
//			bc->buflen = sizeof(bc->buffer);
//			rc = zoo_get(handle_, oss_node_status.str().c_str(), watch,
//					bc->buffer, &(bc->buflen), 0);
//			std::string node_status_data(bc->buffer, bc->buffer + bc->buflen);
//			NodeStatus node_status;
//			node_status.ParseFromString(node_status_data);
//
//			//read blocks of node.
//			std::ostringstream oss_node_blocks;
//			oss_node_blocks << "/" << name << "/" << node_names.data[node_name]
//					<< "/blocks";
//			struct String_vector blocks;
//			rc = zoo_get_children(handle_, oss_node_blocks.str().c_str(),
//					watch, &blocks);
//
//			//parse info of node and add cluster.
//			NodeCategoryEnum category = node_info.category();
//			NodeStatusEnum status = node_status.status();
//			NodeTypeEnum type = node_info.type();
//			std::string node_config = node_info.config();
//			result->AddNode(category, type, node_names.data[node_name],
//					node_config, blocks);
//
//		}
//		return result;
//	} catch (...) {
//		MCE_WARN("ZooKeeperCommunicator::ParseCluster Exception");
//	}
//}
