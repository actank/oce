#ifndef _BUDDY_BYONLINETIME_CACHE_ICE_
#define _BUDDY_BYONLINETIME_CACHE_ICE_

#include <ObjectCache.ice>

module xce {
module buddy {
	struct OnlineNote {
		int userId; 
		int onlineId; 
	};
	sequence<OnlineNote> OnlineNoteSeq;

	struct OnlineTime {
		int userId; 
		int time; 
	};
	sequence<OnlineTime> OnlineTimeSeq;

	class BuddyByOnlineTimeData {
		OnlineTimeSeq onlinetime;
	};
	interface BuddyByOnlineTimeCacheManager {
		int getFriendCount(int hostId);
		MyUtil::Int2IntMap getFriendCounts(MyUtil::IntSeq hostIds);
		MyUtil::IntSeq getFriendList(int hostId,int limit);
		MyUtil::IntSeq getFriendListN(int hostId,int begin,int limit);
		MyUtil::Int2IntSeqMap getFriendLists(MyUtil::IntSeq hostIds);
		/** 
		 * BuddyByOnlineTimeLoader::load invoke this.
		 * BuddyByOnlineTimeLoader::reload invoke this.
		 **/
		void load(int userId,Object o);
		/** 
		 * BuddyLogic invoke this.
		 **/
                void remove(int hostId,int guestId);
		/** 
		 * BuddyByOnlineTimeLoader::online invoke this.
		 **/
		void notifySomebodyOnline(int userId,int onlineId); 
		void notifyOnlineBatch(OnlineNoteSeq notes); 
		
		bool isValid();
		void setValid(bool valid);
		void setData(MyUtil::ObjectResult buddyData);
	};
	interface BuddyByOnlineTimeLoader 
	{
		/** 
		 * BuddyByOnlineTimeCache invoke this. 
		 **/
		void reload(int hostId);
		/** 
		 * BuddyByIdLoader invoke this. 
		 **/
                void load(MyUtil::Int2IntSeqMap lists );
		/** 
		 * OnlineEventMediator invoke this. 
		 **/
		void online(int theOnlineOne);
	};
};
};

#endif
