#ifndef _FRIENDFINDER_BYACTIVEUSER_ICE_
#define _FRIENDFINDER_BYACTIVEUSER_ICE_

#include <Util.ice>

module xce {
module friendfinder {

interface FriendFinderByActiveUser {
	void setBitMap(MyUtil::Int2IntMap loginMap);
	void setNegtiveList(MyUtil::IntSeq userList);
	void clearUserLoginCache();

	MyUtil::Int2IntSeqMap getRecommend(int userId, int limit);
	MyUtil::Int2IntSeqMap getRecommendRandom(int userId, int limit);
	MyUtil::IntSeq getWriteRecommendRandom(int userId, int limit);
	MyUtil::IntSeq getFriendsRecommendRandom(int userId, int limit);
	MyUtil::IntSeq getRecommendFriendList(int userId, int friendId, int begin, int limit);
	void removeRecommendFriends(int userId, int friendId, MyUtil::IntSeq ids); 

	MyUtil::IntSeq getActiveUser(int userId, MyUtil::IntSeq userList);
};

};
};

#endif
