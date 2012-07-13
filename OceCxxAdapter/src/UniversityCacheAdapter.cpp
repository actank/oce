#include "UniversityCacheAdapter.h"

using xce::universitycache::UniversityCacheAdapter;
using MyUtil::ReplicatedClusterAdapterI;
using xce::universitycache::UniversityCacheManagerPrx;

UniversityCacheAdapter::UniversityCacheAdapter() : ReplicatedClusterAdapterI <UniversityCacheManagerPrx>("ControllerUniversityCache", 120, 300) {
}

void UniversityCacheAdapter::AddUniversity(const UniversityCacheUnitSeq& data) {
	getProxy(0)->AddUniversity(data);
}

void UniversityCacheAdapter::RemoveUniversity(const UniversityCacheUnitSeq& data) {
	getProxy(0)->RemoveUniversity(data);
}

void UniversityCacheAdapter::setData(const MyUtil::ObjectResultPtr& data) {
	getProxy(0)->setData(data);
}

bool UniversityCacheAdapter::IsSameUniversity(int hostid, int guestid) {
	return getProxy(0)->IsSameUniversity(hostid, guestid);
}
