#ifndef __XCE_STORM_ICE__
#define __XCE_STORM_ICE__

#include <Util.ice>

module xce
{
module storm
{
	class Observer
	{
		bool update(MyUtil::Str2StrMap context);
	};
	sequence<Observer*> ObserverPool;
	dictionary<string, ObserverPool> ObserverPoolMap;

	class TopicManager
	{
		void notify(string name, MyUtil::Str2StrMap context);
		
		void subscribe(string name, Observer* o);
		void unsubscribe(string name, Observer* o);

		ObserverPoolMap query(string name);		
	};
};
};
#endif

