#ifndef __ACTIVE_USER_ICE__
#define __ACTIVE_USER_ICE__

#include <Util.ice>

//"

module mop
{
module hi
{
module svc
{
module model
{	
    class ActiveUserManager
    {
    	MyUtil::Int2IntMap checkActiveUsers(MyUtil::IntSeq userIds);
    	MyUtil::IntSeq filterActiveUsers(MyUtil::IntSeq userIds);
    	
    	void addActiveUsers(MyUtil::IntSeq userIds);
    	void removeActiveUsers(MyUtil::IntSeq userIds);
    	int getActiveUserSize();
    };
};
};
};
};
#endif
