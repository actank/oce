/*
 * cluster.cc
 *
 *  Created on: 2011-4-11
 *      Author: guanghe.ge
 */

#include "cluster.h"

using namespace com::renren::tripod;

void Cluster::Initialize(const std::string& communitor_addr,
		const std::string& cluster_name) {
	try {
		communicator_ = CommunicatorFactory::Create(communitor_addr);
		ParseCluster();
	} catch (...) {

	}
}

DataPtr Cluster::Read(long id) {

}

void Cluster::Write(long id, DataPtr data) {

}

void Cluster::Delete(long id) {

}

void Cluster::Update(const std::string& path) {

}

void Cluster::ParseCluster() {

}

std::string Cluster::Key(long id) {
	std::ostringstream oss;
	oss << cluster_name_ << "_" << NodeIndex(id);
	return oss.str();
}

int Cluster::NodeIndex(long id) {
	return 0 != block_size_ ? id % block_size_ : -1;
}

NodePtr Cluster::ChooseNode(const std::vector<NodePtr>& nodes) {

}
