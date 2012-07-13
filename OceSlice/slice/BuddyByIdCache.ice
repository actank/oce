#ifndef _BUDDY_BYID_CACHE_ICE_
#define _BUDDY_BYID_CACHE_ICE_

#include <ObjectCache.ice>
module xce {
module buddy {
	class BuddyByIdData {
		MyUtil::IntSeq friends; 
	};
	interface BuddyByIdCacheManager {
		int getFriendCount(int hostId);
		MyUtil::Int2IntMap getFriendCounts(MyUtil::IntSeq hostIds);
		MyUtil::IntSeq getFriendList(int hostId,int limit);
		MyUtil::IntSeq getFriendListN(int hostId,int begin,int limit);
		MyUtil::Int2IntSeqMap getFriendLists(MyUtil::IntSeq hostIds);
		/** 
		 * BuddyByIdLoader::load invoke this.
		 **/
		void load(int userId,Object o);
		/** 
		 * BuddyLogic invoke this.
		 **/
		void remove(int hostId,int guestId);
		
		bool isValid();
		void setValid(bool valid);
		void setData(MyUtil::ObjectResult datas);
	};
	interface BuddyByIdLoader 
	{
		/** 
		 * BuddyByIdCache invoke this. 
		 **/
		void reload(int hostId);
		/** 
		 * BuddyLogic invoke this. 
		 * remove don't need sorting or filting. so Loader don't need Remove method
		 **/
		void load(int hostId,MyUtil::IntSeq buddies);
		void add(int hostId,int guestId);
		void statusUpdate(int userId,int status);
		int filter(int userId,MyUtil::IntSeq buddies);
		MyUtil::ByteSeq getStatusMinMax(int minId, int maxId);
	};

  interface DistBuddyByIdCacheReloader
  {
    void reload(MyUtil::IntSeq userIds);
    void add(int host,int guest);
    void remove(int host, int guest);
    void statusUpdate(int userId,int status);
    bool isValid();
    void setValid(bool valid);
  };
};
};

#endif
