#ifndef __XCE_IM_ICE__
#define __XCE_IM_ICE__

#include <Util.ice>
#include <TalkCommon.ice>
#include <TalkProxy.ice>

module xce
{
module talk 
{
	class Observer
	{
		bool update(MyUtil::Str2StrMap context);
	};
	sequence<Observer*> ObserverPool;
	dictionary<string, ObserverPool> ObserverPoolMap;

	interface IMStorm extends com::xiaonei::talk::DeliverInterface
	{
		void UserOnlineStatChange(int userid, int onlinestat, int onlinetype, MyUtil::IntSeq notifybuddys);
		void notify(string name, MyUtil::Str2StrMap context, int servercount, int channel);
		void subscribe(string name, Observer* o, int phoneservercount, int phoneserverindex);
		void unsubscribe(string name, Observer* o);
		
		void NotifyPhoneServerDisable();
		void NotifyPhoneServerEnalbe();


		ObserverPoolMap query(string name);		
	};
	class TestStorm{		
	};
};
};
#endif

