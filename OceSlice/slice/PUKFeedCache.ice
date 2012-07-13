#ifndef _PUKFEED_CACHE_ICE_ 
#define _PUKFEED_CACHE_ICE_

#include <Util.ice>

module xce {
module socialgraph {

struct PUKFeed {
	int friendId;
	int commonId;
};
sequence<PUKFeed> PUKFeedSeq;

interface PUKFeedCache {
	void Set(int hostId, PUKFeedSeq feedList);
	PUKFeedSeq Get(int hostId, int limit);
	PUKFeedSeq GetAndRemove(int hostId, int limit);
	void Remove(int hostId, PUKFeedSeq feedList);
};

};
};

#endif
