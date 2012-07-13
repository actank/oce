#ifndef __USER_COUNT_ICE__
#define __USER_COUNT_ICE__

#include <Util.ice>
#include <ObjectCache.ice>

module xce
{
module usercount
{
	const int COnlineFriends=0;
	const int CPoke=1;
	const int CClassInvite=2;
	const int CGuestRequest=3;
	const int CFriendMemoRequest=4;
	const int CNewMessage=5;
	const int CImLeaveword=6;
	const int CTeamInvite=7;
	const int CTribeRequest=8;
	const int CFriendCount=9;
	const int CRegApply=10;
	const int CRecommendedFriend=11;	
	const int CNotify=12;
	const int CEventInvite=13;
	const int CAppInvite=14;
	const int CPhotoTagInvite=15;
	const int CSurveyInvite=16;
	const int CGameInvite=17;
	const int CTestInvite=18;
	const int CCallInvite=19;
	const int CNewRequest=20;
	const int CContactRequest=21;
	const int COfferMSNContact=22;
	const int CTotalTypeSize=23;
	
	class UserCounter
	{
		MyUtil::IntSeq types;
		MyUtil::IntSeq values;
	};
	
	interface UserCountReader
	{
		void inc(int userId, int type, int step);
		void dec(int userId, int type, int step);
		void set(int userId, int type, int value);
		int get(int userId, int type);
		UserCounter getAll(int userId);
	 	void setData(MyUtil::ObjectResult data);
	}; 
	
	interface UserCountWriter
	{
		void inc(int userId, int type, int step);
		void dec(int userId, int type, int step);
		void set(int userId, int type, int value);
		void reload(int userId);
	}; 


	interface UserCountManager
	{
		void inc(int userId, int type, int step);
		void dec(int userId, int type, int step);
		int get(int userId, int type);
		void set(int userId, int type, int value);
		UserCounter getAll(int userId);
		void reload(int userId);
	}; 
};
};

#endif

