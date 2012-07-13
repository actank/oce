#ifndef COUNT_CACHE_ICE
#define COUNT_CACHE_ICE

#include <Util.ice>

module mop
{
	module hi
	{
		module svc
		{
			module count
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

				class Counter
				{
					MyUtil::IntSeq values;
				};

				class CountManager
				{
					void inc(int id, int step, int type);
					void dec(int id, int step, int type);
					int get(int id, int type);
					Counter getAll(int id);
					void reload(int id);
				};
			};
		};
	};
};

#endif
