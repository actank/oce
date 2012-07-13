#ifndef _FRIEND_COUNT_CACHE_ICE_
#define _FRIEND_COUNT_CACHE_ICE_

#include <ObjectCache.ice>
module xce {
module buddy {
	class FriendCountCacheData{
		int count;
	};
	interface FriendCountCacheManager {
		int getFriendCount(int id);
		MyUtil::Int2IntMap getFriendCounts(MyUtil::IntSeq ids);
		void setFriendCount(int id, int count);
		void setFriendCounts(MyUtil::Int2IntMap counts);
		void dec(int id);
		void reload(int id);
	};
};
};

#endif
