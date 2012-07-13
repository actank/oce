#ifndef FEED_SINK_ICE
#define FEED_SINK_ICE

#include <RFeed.ice>

module xce {
module feed {


class FeedSink{
    void AddFeed(FeedItem item, FeedConfig config, MyUtil::Int2IntMap id2weight);
	void SetRead(int uid, int stype, long merge);
	void SetReadById(int uid,long feedId);
	void SetReadAll(int uid);
};

};
};
#endif