#ifndef _HOTFEEDRECOMMENDCACHE_ADAPTER_H_
#define _HOTFEEDRECOMMENDCACHE_ADAPTER_H_

#include "HotFeedRecommendCache.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace xce {
namespace socialgraph {
class HotFeedRecommendCacheAdapter : public MyUtil::ReplicatedClusterAdapterI<xce::socialgraph::HotFeedRecommendCachePrx>,
		public MyUtil::Singleton<HotFeedRecommendCacheAdapter> {
public:
	HotFeedRecommendCacheAdapter() :
		MyUtil::ReplicatedClusterAdapterI<xce::socialgraph::HotFeedRecommendCachePrx>("ControllerHotFeedRecommendCache", 120, 1000) {
	}

	void Set(int hostId, HotFeedSeq hotFeedList);
    HotFeedSeq GetAndRemove(int hostId, int limit);
    HotFeedSeq Get(int hostId, int limit);
    void Remove(int hostId, HotFeedSeq hotFeedList);
};
}
}

#endif
