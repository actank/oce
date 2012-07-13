#ifndef _BUDDY_BYADDTIME_CACHE_ICE_
#define _BUDDY_BYADDTIME_CACHE_ICE_

#include <ObjectCache.ice>
module xce {
module buddy {
	struct BuddyByAddTimeNode {
		int userId;
		int time;
	};
	sequence<BuddyByAddTimeNode> BuddyByAddTimeNodeSeq;
	class BuddyByAddTimeDataN {
		MyUtil::IntSeq friends; 
		MyUtil::IntSeq times; 
	};
	class BuddyByAddTimeData {
		MyUtil::IntSeq friends; 
	};
	interface BuddyByAddTimeCacheManager {
		int getFriendCount(int hostId);
		MyUtil::Int2IntMap getFriendCounts(MyUtil::IntSeq hostIds);
		MyUtil::IntSeq getFriendList(int hostId,int limit);
		MyUtil::Int2IntSeqMap getFriendLists(MyUtil::IntSeq hostIds);
		MyUtil::IntSeq getFriendListN(int hostId,int begin,int limit);
		BuddyByAddTimeNodeSeq getFriendListWithinTime(int hostId,long beginTime,long endTime);
		BuddyByAddTimeNodeSeq getFriendListAndTime(int hostId,int limit);
		/** 
		 * BuddyByAddTimeLoader::load invoke this.
		 * BuddyByAddTimeLoader::reload invoke this.
		 **/
		void load(int userId,Object o);
		/** 
		 * BuddyLogic invoke this.
		 **/
                void remove(int hostId,int guestId);

		bool isValid();
		void setValid(bool valid);
		void setData(MyUtil::ObjectResult buddyData);
	};
	interface BuddyByAddTimeLoader 
	{
		/** 
		 * BuddyByAddTimeCache invoke this. 
		 **/
		void reload(int hostId);
		/** 
		 * BuddyByIdLoader invoke this. 
		 **/
                void load(MyUtil::Int2IntSeqMap lists );
	};
};
};

#endif
