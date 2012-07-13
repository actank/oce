#ifndef __XCE_NOTIFY_ICE__
#define __XCE_NOTIFY_ICE__

#include <Util.ice>

module xce
{
module notify
{
	class Observer
	{
		bool update(MyUtil::Str2StrMap context);
	};
	sequence<Observer*> ObserverPool;
	dictionary<string, ObserverPool> ObserverPoolMap;

	class NotifyStorm
	{
		void notify(string name, MyUtil::Str2StrMap context);
		
		void subscribe(string name, Observer* o);
		void unsubscribe(string name, Observer* o);

		ObserverPoolMap query(string name);		
	};
	class TestStorm{		
	};
};
};
#endif

