#include "SocialFeedCacheAdapter.h"

using namespace xce::socialgraph;

void SocialFeedCacheAdapter::set(int hostId, const FeedSeq& feedList) {
	getProxy(hostId)->set( hostId, feedList );
}

void SocialFeedCacheAdapter::remove(int hostId, const MyUtil::IntSeq& friendIds, const MyUtil::IntSeq& newCommonIds) {
	getProxy(hostId)->remove( hostId, friendIds, newCommonIds );
}

FeedSeq SocialFeedCacheAdapter::get(int hostId, int limit) {
	return getProxy(hostId)->get( hostId, limit );
}
