#ifndef _SOCIALFEEDCACHE_ICE_
#define _SOCIALFEEDCACHE_ICE_

#include <Util.ice>

module xce {
module socialgraph {
struct Feed {
	MyUtil::IntSeq friendIds;
	MyUtil::IntSeq newCommonIds;
};
sequence<Feed> FeedSeq;

interface SocialFeedCache {
	void set(int hostId, FeedSeq feedList);
	void remove(int hostId, MyUtil::IntSeq friendIds, MyUtil::IntSeq newCommonIds);
	FeedSeq get(int hostId, int limit);
};

};
};

#endif
