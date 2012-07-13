/*
 * Adapter.cc
 *
 *  Created on: 2011-4-11
 *      Author: guanghe.ge
 */

#include "adapter.h"

using namespace com::renren::tripod;

void Adapter::Initialize() {
	try {
//		registry_ -> Initialize(registry_addr_);
//		registry_ -> Register(name_);
	} catch (...) {

	}
}

DataPtr Adapter::Read(const std::string& key) {
	try {
//		ClusterPtr cluster = registry_ -> GetCluster(name_);
//		if (NULL != cluster) {
//			return cluster->Read(key);
//		}
	} catch (...) {

	}
}

bool Adapter::Write(const std::string& key, DataPtr data, bool exist, bool sync) {
	try {
		//TODO exist write Meta and sync function
//		ClusterPtr cluster = registry_ -> GetCluster(name_);
//		if (NULL != cluster) {
//			cluster->Write(key, data);
//		}

		return true;
	} catch (...) {

	}
	return false;
}
