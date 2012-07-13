#ifndef __ONLINE_ICE__
#define __ONLINE_ICE__

#include <Util.ice>

module xce
{
module online
{
	const int CStatOffline = 0;
	const int CStatOnline = 1;

	class OnlineManager
	{
	
		/* 用户作出了任何表示其活动的行为
		*/
		void UserActivity(MyUtil::IntSeq userids);

		/* @brief 返回一组用户的状态(是否在线) 
		*/
		MyUtil::Int2IntMap UserOnlineStat(MyUtil::IntSeq userids);
	};

};
};

#endif
