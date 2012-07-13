#ifndef _BUDDY_FRIENDS_CACHE_ICE_
#define _BUDDY_FRIENDS_CACHE_ICE_

#include <ObjectCache.ice>
module xce {
module buddy {
	class BuddyFriendsData {
		MyUtil::IntSeq friends; 
	};
  
	interface BuddyFriendsCacheManager {
		int getFriendCount(int hostId);
		MyUtil::Int2IntMap getFriendCounts(MyUtil::IntSeq hostIds);
		MyUtil::IntSeq getFriendList(int hostId,int begin,int limit);
		MyUtil::Int2IntSeqMap getFriendLists(MyUtil::IntSeq hostIds);
	 
    /***
    * to get friendsfriends of hostId from potentialFriends
    * author : min.shang@opi-corp.com
    ***/
    MyUtil::Int2IntMap isFriendsFriendBatch(int hostId, MyUtil::IntSeq potentialFriends);

		bool isFriendsFriend(int hostId, int guestId);
		
    MyUtil::IntSeq getCommonFriends(int hostId, int guestId);
    
    MyUtil::Str2IntSeqMap getCommonFriendsBatch( MyUtil::Int2IntMap idpairs );

    /** 
		 * BuddyFriendsLoader::load invoke this.
		 **/
		void load(int userId,Object o);
		/** 
		 * BuddyLogic invoke this.
		 **/
		void remove(int hostId,int guestId);
		
		bool isValid();
		void setValid(bool valid);
		void setData(MyUtil::ObjectResult datas);
		void add(int hostId,int guestId);
	};
	interface BuddyFriendsLoader 
	{
		/** 
		 * BuddyFriendsCache invoke this. 
		 **/
		void reload(int hostId);
		/** 
		 * BuddyLogic invoke this. 
		 * remove don't need sorting or filting. so Loader don't need Remove method
		 **/
		void load(int hostId,MyUtil::IntSeq buddies);
		void add(int hostId,int guestId);
	};
};
};

#endif
