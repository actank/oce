#ifndef _HOTFEED_RECOMMENDCACHE_ICE_
#define _HOTFEED_RECOMMENDCACHE_ICE_

#include <Util.ice>

module xce {
module socialgraph {

struct HotFeed {
	int userId;
	long feedId;
	int feedType;
};
sequence<HotFeed> HotFeedSeq;

interface HotFeedRecommendCache {
	void Set(int hostId, HotFeedSeq hotFeedList);
	HotFeedSeq GetAndRemove(int hostId, int limit);
	HotFeedSeq Get(int hostId, int limit);
	void Remove(int hostId, HotFeedSeq hotFeedList);
};

};
};

#endif 
