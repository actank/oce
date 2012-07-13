#include "NetworkCacheAdapter.h"

using xce::networkcache::NetworkCacheAdapter;
using MyUtil::ReplicatedClusterAdapterI;
using xce::networkcache::NetworkCacheManagerPrx;

NetworkCacheAdapter::NetworkCacheAdapter() : ReplicatedClusterAdapterI <NetworkCacheManagerPrx>("ControllerNetworkCache", 120, 300) {
}

void NetworkCacheAdapter::setData(const MyUtil::ObjectResultPtr& data) {
	getProxy(0)->setData(data);
}
