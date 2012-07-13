#include "PUKFeedCacheAdapter.h"

using namespace xce::socialgraph;

void PUKFeedCacheAdapter::Set(int hostId, PUKFeedSeq feedList) {
	getProxy(hostId)->Set(hostId, feedList);
}

PUKFeedSeq PUKFeedCacheAdapter::Get(int hostId, int limit) {
	return getProxy(hostId)->Get(hostId, limit);
}

PUKFeedSeq PUKFeedCacheAdapter::GetAndRemove(int hostId, int limit) {
	return getProxy(hostId)->GetAndRemove(hostId, limit);
}

void PUKFeedCacheAdapter::Remove(int hostId, PUKFeedSeq feedList) {
	return getProxy(hostId)->Remove(hostId, feedList);
}
