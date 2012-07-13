#ifndef MSNFRIEND_ICE
#define MSNFRIEND_ICE

#include<Util.ice>

module xce
{
	module mfd
	{
		struct MSNFriend
		{
			int friendID;
			int weight;
		};
		sequence<MSNFriend> MSNFriendSeq;
		class MSNFriendManager
		{
			MSNFriendSeq getCommonFriend(int uid,int limit);
			MSNFriendSeq getFriendWithCommon(int uid,int limit); //包括一级和二级的MSN好友
		};

	};//mfd
};//xce






#endif
