#ifndef USER_ONLINE_ICE
#define USER_ONLINE_ICE

#include <Util.ice>

module mop
{
module hi
{
module svc
{
module online
{
    class OnlineUser
    {
    	int userId;
		int onlineStat; // 0: off line			2: web on line 			1: im on line  			3: web and im on line
    	int timestamp;
    };
    sequence<OnlineUser> OnlineUserSeq;
	dictionary<int, OnlineUser> OnlineUserMap;
	
	interface OnlineManager
	{
		void setStat(int userId, int onlineType, bool online);
		void setStats(MyUtil::IntSeq userIds, int onlineType, bool online);
		
		int size(int onlineType);
		
		int getStat(int userId);
		MyUtil::IntSeq getStats(int begin, int limit, int onlineType);
		
		MyUtil::Int2IntMap checkStats(MyUtil::IntSeq userIds);
    };

};
};
};
};

#endif

