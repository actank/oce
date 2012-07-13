#ifndef _MUTUAL_FRIENDS_CACHE_ICE_
#define _MUTUAL_FRIENDS_CACHE_ICE_

#include <Util.ice>
#include <ObjectCache.ice>
module xce {
module buddy {
	//保存粉丝数组的缓存
	class FansObject {
		MyUtil::IntSeq fans;
	};

//	//如果实时计算CPU消耗较大，以后可以把好友数缓存一份
//	//保存共同好友
//	struct MutualFriendsEntry {
//		MyUtil::IntSeq mutualFriends;
//	};
//	//保存由gid到共同好友的映射关系
//	dictionary<long, MutualFriendsEntry> MutualFriendsMap;
//	//对每个用户id，保存一份所有好友的缓存和一个共同好友的map；
//	//考虑到共同好友调用量较大，而数据量较小，会缓存用户与每个应用的共同好友数据；
//	//为了节省空间，非共同通过计算而得，而不做缓存，所以使用的时候调用量不应个很多。
//	struct MutualFriendsObject {
//		MyUtil::IntSeq allFriends;
//		MutualFriendsEntry dataMap;
//	};

	interface MutualFriendsCacheManager {

		// begin >= 0, limit >== -1, userId >= 0
		// 1. 以上三者，一个条件不满足，则返回空
		// 2. 取得的结果是[begin, ...)，不超过limit个id
		// 3. limit = -1 返回 [begin, end)
		// Get friend ids of uid that are the fans of gid
		MyUtil::IntSeq getMutualFriends(int uid, MyUtil::GlobalId gid, int begin, int limit);
		// Get friend ids of uid that are not the fans of gid
		MyUtil::IntSeq getUnMutualFriends(int uid, MyUtil::GlobalId gid, int begin, int limit);
//		MyUtil::Object getMutualFriendsObject(int uid, MyUtil::GlobalId gid, int begin, int limit);

		int getMutualFriendsCount(int uid, MyUtil::GlobalId gid);
		int getUnMutualFriendsCount(int uid, MyUtil::GlobalId gid);
		int getFansCount(MyUtil::GlobalId gid);
		bool isFans(int uid, MyUtil::GlobalId gid);

		// Get all fans sequence of gid
		MyUtil::IntSeq getFans(MyUtil::GlobalId gid, int begin, int limit);

		bool isValid();
		void setValid(bool valid);
		void add(int uid, MyUtil::GlobalId gid);
		void remove(int uid, MyUtil::GlobalId gid);

		//由MutualFriendsLoader调用
		void setData(MyUtil::ObjectResult datas);
	};

	interface MutualFriendsLoader
	{
		//由MutualFriendsCacheManager调用
		void reload(long gid);
	};
};
};

#endif
