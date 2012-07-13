#ifndef __FEED_SYNC_DB_ICE__
#define __FEED__SYNC_DB_ICE__

#include <Util.ice>
#include <RFeed.ice>


module xce
{
module feed
{
	class FeedSyncDb{
		void replaceFeedReply(MyUtil::LongSeq feedIds, FeedReply reply);
	};
	
};

};

#endif 
