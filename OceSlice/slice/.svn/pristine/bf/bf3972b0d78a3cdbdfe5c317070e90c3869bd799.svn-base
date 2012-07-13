#ifndef _FRIEND_DIST_ICE
#define _FRIEND_DIST_ICE

#include <Util.ice>

module mop
{
module hi
{
module svc
{
module model
{
	class FriendCount
	{
		int networkId;
		int count;
	};
	sequence<FriendCount> FriendCountSeq;
	
	class FriendDistWithTotal{
		FriendCountSeq networks;
		int totalCount;
	};

	interface FriendDistManager
	{
		MyUtil::Int2IntMap getFriendDist(int userId);
		FriendDistWithTotal getFriendDistWithTotal(int userId);
		int getFriendDistByUniv(int userId, int universityId);
		void rebuildFriendDist(int userId);
		int getTotalCount(int userId);
	};


};  
};  
};  
};  
#endif
    
    
    
    
    
    
    
    
    
    
    
