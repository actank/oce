#include "NetworkCacheReloaderAdapter.h"

using xce::networkcache::NetworkCacheReloaderAdapter;
using MyUtil::ReplicatedClusterAdapterI;
using xce::networkcache::NetworkCacheReloaderPrx;

NetworkCacheReloaderAdapter::NetworkCacheReloaderAdapter() : ReplicatedClusterAdapterI <NetworkCacheReloaderPrx>("ControllerNetworkCacheReloader", 120, 300) {
}

void NetworkCacheReloaderAdapter::ReloadNetwork(const MyUtil::IntSeq& ids) {
	getProxy(0)->ReloadNetwork(ids);
}
