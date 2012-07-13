#ifndef FEED_ASYNC_ICE
#define FEED_ASYNC_ICE

#include <RFeed.ice>
//#include <Util.ice>
module xce {
module feed {
dictionary<long,FeedReply> ReplyDict;
class FeedAsync{
	bool AddFeedIndex(FeedIndex idx);
	bool RemoveFeedIndex(MyUtil::LongSeq fids);

	bool AddFeedContent(FeedSeed seed);
	bool RemoveFeedContent(MyUtil::LongSeq fids);
	
	bool AddFeedMini(FeedSeed content);
	bool RemoveAllFeedMini(int uid);
	bool RemoveFeedMini(int user, int stype, long miniMerge);
	
	bool AddFeedSchool(int schoolid, FeedSeed content);
	bool AddFeedGroup(int group, FeedSeed content);
	
	bool ReplaceXML( MyUtil::LongSeq fids , string xml );
	bool UpdateReply(ReplyDict replys);
	
	bool CopyFeedContent(long oldfeedid, long newfeedid, int newtype,  string newprops);
	
};
};
};
#endif