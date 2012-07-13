#ifndef NETWORKCACHEADAPTER_H_
#define NETWORKCACHEADAPTER_H_

#include <NetworkCache.h>
#include <AdapterI.h>

namespace xce {
namespace networkcache {

class NetworkCacheAdapter : public MyUtil::ReplicatedClusterAdapterI<NetworkCacheManagerPrx>, public MyUtil::Singleton<NetworkCacheAdapter>{
public:
	NetworkCacheAdapter();
	void setData(const MyUtil::ObjectResultPtr& data);
};

}
}

#endif
