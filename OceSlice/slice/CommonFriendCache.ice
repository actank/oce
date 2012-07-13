#ifndef _COMMON_FRIEND_CACHE_ICE_
#define _COMMON_FRIEND_CACHE_ICE_

#include <Util.ice>
module xce {
	module buddy {
		struct CommonFriendEntry{
			MyUtil::IntSeq common;
			MyUtil::IntSeq uncommon;
		};
		dictionary<int,CommonFriendEntry> CommonFriendEntryMap;
		
		struct CommonFriendObject {
			int uid;
			CommonFriendEntryMap dataMap;
		};

		struct CommonFriendParam {
			int uid;
			MyUtil::GlobalId gid;
			int cbegin;
			int climit;
			int ubegin;
			int ulimit;
		};    

		interface CommonFriendCacheManager {
			// Get friend ids of uid that are the fans of pid
			// if begin < 0 or limit == -1 or limit == 0, return empty sequence
			// if begin > size, return empty sequence
			// if limit = -1, return sequence[begin, end)
			// if begin + limit > size, return sequence[begin, end)
			// if begin + limit < size, return sequence[begin, begin + limit)
			MyUtil::IntSeq GetCommon(int uid, MyUtil::GlobalId gid, int begin, int limit);

			// Get friend ids of uid that are not the fans of pid 
			// if begin < 0 or limit == -1 or limit == 0, return empty sequence
			// if begin > size, return empty sequence
			// if limit = -1, return sequence[begin, end)
			// if begin + limit > size, return sequence[begin, end)
			// if begin + limit < size, return sequence[begin, begin + limit)
			MyUtil::IntSeq GetUnCommon(int uid, MyUtil::GlobalId gid, int begin, int limit);

			// Get Both CommonFans and UnCommonFans sequences
			CommonFriendEntry GetCommonAndUnCommon(CommonFriendParam param);

			//Save Data to  Cache
			bool SetData(CommonFriendObject pcf);
		
      void add(MyUtil::GlobalId gid, int uid);
     
      void remove(MyUtil::GlobalId gid, int uid);
      
      int getCount(MyUtil::GlobalId gid);

      bool hasRelation(MyUtil::GlobalId gid, int uid);
    };

	};
};

#endif
