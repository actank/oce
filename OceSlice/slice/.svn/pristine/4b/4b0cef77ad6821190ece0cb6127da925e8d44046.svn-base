#ifndef FEED_ASYNCDB_ICE
#define FEED_ASYNCDB_ICE

#include <RFeed.ice>
//#include <Util.ice>
module xce {
module feed {
class FeedAsyncDb{
	bool AddFeedIndex(FeedIndex idx);
	bool RemoveFeedIndex(MyUtil::LongSeq fids);

	bool AddFeedContent(FeedSeed seed);
	bool RemoveFeedContent(MyUtil::LongSeq fids);
	
	bool AddFeedMini(FeedSeed content);
	bool RemoveAllFeedMini(int uid);
	bool RemoveFeedMini(int user, int stype, long miniMerge);
	
	bool AddFeedSchool(int schoolid, FeedSeed content);
	bool AddFeedGroup(int group, FeedSeed content);
};
};
};
#endif