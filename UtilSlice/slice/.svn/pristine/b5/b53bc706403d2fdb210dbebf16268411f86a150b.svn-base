#ifndef __SVC_DESCRIPTOR_ICE__
#define __SVC_DESCRIPTOR_ICE__

#include <Util.ice>

module MyUtil
{
	class SvcInstance
	{
		string name;
		int timestamp;
		MyUtil::Str2StrMap properties;
	};
	sequence<SvcInstance> SvcInstanceSeq;
	dictionary<string, SvcInstance> SvcInstanceMap;
	
	class SvcObserver
	{
		bool update(SvcInstance svci);
	};
	sequence<SvcObserver*> SvcObserverSeq;
	dictionary<string, SvcObserverSeq> SvcObserverMap;

	class SvcDescriptor
	{
		SvcInstance query(string name);
		void notify(string name);
		
		void subscribe(string name, SvcObserver* observer);
		void unsubscribe(string name, SvcObserver* observer);
	};
};

#endif

