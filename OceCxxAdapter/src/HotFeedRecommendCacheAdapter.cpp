#include "HotFeedRecommendCacheAdapter.h"
using namespace xce::socialgraph;

void HotFeedRecommendCacheAdapter::Set(int hostId, HotFeedSeq hotFeedList) {
	getProxy(hostId)->Set(hostId, hotFeedList);
}

HotFeedSeq HotFeedRecommendCacheAdapter::GetAndRemove(int hostId, int limit) {
	return getProxy(hostId)->GetAndRemove(hostId, limit);
}

HotFeedSeq HotFeedRecommendCacheAdapter::Get(int hostId, int limit) {
	return getProxy(hostId)->Get(hostId, limit);
}

void HotFeedRecommendCacheAdapter::Remove(int hostId, HotFeedSeq hotFeedList) {
	getProxy(hostId)->Remove(hostId, hotFeedList);
}
