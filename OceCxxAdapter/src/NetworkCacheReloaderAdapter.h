#ifndef NETWORKCACHERELOADERADAPTER_H_
#define NETWORKCACHERELOADERADAPTER_H_

#include <NetworkCache.h>
#include <AdapterI.h>

namespace xce {
namespace networkcache {

class NetworkCacheReloaderAdapter : public MyUtil::ReplicatedClusterAdapterI<NetworkCacheReloaderPrx>, public MyUtil::Singleton<NetworkCacheReloaderAdapter>{
public:
	NetworkCacheReloaderAdapter();
	void ReloadNetwork(const MyUtil::IntSeq& ids);
};

}
}

#endif
