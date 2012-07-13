#include "UniversityCacheReloaderAdapter.h"

using xce::universitycache::UniversityCacheReloaderAdapter;
using MyUtil::ReplicatedClusterAdapterI;
using xce::universitycache::UniversityCacheReloaderPrx;

UniversityCacheReloaderAdapter::UniversityCacheReloaderAdapter() : ReplicatedClusterAdapterI <UniversityCacheReloaderPrx>("ControllerUniversityCacheReloader", 120, 300) {
}

void UniversityCacheReloaderAdapter::AddUniversity(const UniversityCacheUnitSeq& data) {
	getProxy(0)->AddUniversity(data);
}

void UniversityCacheReloaderAdapter::RemoveUniversity(const UniversityCacheUnitSeq& data) {
	getProxy(0)->RemoveUniversity(data);
}

void UniversityCacheReloaderAdapter::Reload(const MyUtil::IntSeq& ids) {
	getProxy(0)->Reload(ids);
}
