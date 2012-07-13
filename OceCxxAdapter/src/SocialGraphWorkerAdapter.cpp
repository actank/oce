#include "SocialGraphWorkerAdapter.h"

using namespace xce::socialgraph;

void SocialGraphWorkerPrxData::Report(int userId, const Info& information) {
	getProxy(userId)->Report(information);
}

void SocialGraphWorkerPrxData::PreLoad(int userId, bool signal) {
	getProxy(userId)->PreLoad(userId, signal);
}

void SocialGraphWorkerPrxData::Rebuild(int userId) {
	getProxy(userId)->Rebuild(userId);
}

//-------------------------------------------------------------------------------------

void SocialGraphWorkerAdapter::Report(const std::string& endpoint, int userId, const Info& information) {
//	MCE_INFO("[SocialGraphWorkerAdapter] Report endpoint(" << endpoint << ") userId(" << userId << ")");
	IceUtil::RWRecMutex::RLock lock(init_flag_mutex_);
	std::map<std::string, ProxyTypePtr>::iterator fIt = proxyptr_map_.find(endpoint);

	if (fIt != proxyptr_map_.end()) {
//		MCE_INFO("[SocialGraphWorkerAdapter] Report != end endpoint(" << endpoint << ") userId(" << userId << ")");
		fIt->second->Report(userId, information);
		return;
	}
	
	lock.upgrade();

	pair<std::map<std::string, ProxyTypePtr>::iterator, bool> result =
			proxyptr_map_.insert(make_pair<std::string, ProxyTypePtr>(endpoint, new SocialGraphWorkerPrxData(endpoint)));

	//MCE_INFO("userId : " << userId);
	result.first->second->Report(userId, information);
}

void SocialGraphWorkerAdapter::PreLoad(const std::string& endpoint, int userId, bool signal) {
	IceUtil::RWRecMutex::RLock lock(init_flag_mutex_);
	std::map<std::string, ProxyTypePtr>::iterator fIt = proxyptr_map_.find(endpoint);

	if (fIt != proxyptr_map_.end()) {
		fIt->second->PreLoad(userId, signal);
		return;
	}
	
	lock.upgrade();

	pair<std::map<std::string, ProxyTypePtr>::iterator, bool> result =
			proxyptr_map_.insert(make_pair<std::string, ProxyTypePtr>(endpoint, new SocialGraphWorkerPrxData(endpoint)));

	result.first->second->PreLoad(userId, signal);
}

void SocialGraphWorkerAdapter::Rebuild(const std::string& endpoint, int userId) {
	IceUtil::RWRecMutex::RLock lock(init_flag_mutex_);
	std::map<std::string, ProxyTypePtr>::iterator fIt = proxyptr_map_.find(endpoint);

	if (fIt != proxyptr_map_.end()) {
		fIt->second->Rebuild(userId);
		return;
	}
	
	lock.upgrade();

	pair<std::map<std::string, ProxyTypePtr>::iterator, bool> result =
			proxyptr_map_.insert(make_pair<std::string, ProxyTypePtr>(endpoint, new SocialGraphWorkerPrxData(endpoint)));

	result.first->second->Rebuild(userId);
}
