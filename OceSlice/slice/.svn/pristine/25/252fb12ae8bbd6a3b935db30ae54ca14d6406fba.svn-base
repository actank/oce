#ifndef _BUDDY_BYNAME_CACHE_ICE_
#define _BUDDY_BYNAME_CACHE_ICE_

#include <ObjectCache.ice>

module xce {
module buddy {
	struct NameNote{
		int id;
		string name;
	};
	sequence<NameNote> NameNoteSeq;
	class BuddyByNameData {
		NameNoteSeq names;
	};
	interface BuddyByNameCacheManager {
		int getFriendCount(int hostId);
		MyUtil::Int2IntMap getFriendCounts(MyUtil::IntSeq hostIds);
		MyUtil::IntSeq getFriendList(int hostId,int limit);
		MyUtil::IntSeq getFriendListN(int hostId,int begin, int limit);
		MyUtil::Int2IntSeqMap getFriendLists(MyUtil::IntSeq hostIds);
		/** 
		 * BuddyByNameLoader::load invoke this.
		 * BuddyByNameLoader::reload invoke this.
		 * BuddyByNameLoader::rename invoke this.
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
	interface BuddyByNameLoader 
	{
		/** 
		 * BuddyByNameCache invoke this. 
		 **/
		void reload(int hostId);
		/** 
		 * BuddyByIdLoader invoke this. 
		 **/
                void load(MyUtil::Int2IntSeqMap lists );
		/** 
		 * UserLogic invoke this. 
		 **/
		void rename(int theRenameOne,string name);
	};
};
};

#endif
