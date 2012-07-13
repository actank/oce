#ifndef _PUKFEED_CACHE_ADAPTER_H_
#define	_PUKFEED_CACHE_ADAPTER_H_ 

#include "PUKFeedCache.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace xce {
namespace socialgraph {
class PUKFeedCacheAdapter : public MyUtil::ReplicatedClusterAdapterI<xce::socialgraph::PUKFeedCachePrx>,
		public MyUtil::Singleton<PUKFeedCacheAdapter> {
public:
	PUKFeedCacheAdapter() :
		MyUtil::ReplicatedClusterAdapterI<xce::socialgraph::PUKFeedCachePrx>("ControllerPUKFeedCache", 120, 1000) {
	}
	
	void Set(int hostId, PUKFeedSeq feedList);
    PUKFeedSeq Get(int hostId, int limit);
    PUKFeedSeq GetAndRemove(int hostId, int limit);
    void Remove(int hostId, PUKFeedSeq feedList);
};
}
}

#endif
