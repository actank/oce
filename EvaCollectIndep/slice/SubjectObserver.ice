#ifndef __SUBJECT_OBSERVER_ICE__
#define __SUBJECT_OBSERVER_ICE__

module MyUtil
{
	class SbjState
	{
	};
	
	class SbjSubscriber
	{
		SbjState opera(SbjState state);
	};
	sequence<SbjSubscriber*> SbjSubscriberSeq;
	dictionary<string, SbjSubscriberSeq> SbjSubscriberMap;
	
	class SbjTopic
	{
		string name;
		int timestamp;
	};
	dictionary<string, SbjTopic> SbjTopicMap;
	sequence<SbjState> SbjStateSeq;

	class SubjectObserver
	{
		SbjTopic getSbjTopic(string name);
		SbjTopic query(string name);
		
		void subscribe(string name, SbjSubscriber* subscriber);
		void unsubscribe(string name, SbjSubscriber* subscriber);
		void notifyall(string name, SbjState state);
		SbjStateSeq fetch(string name, SbjState state);
		

		SbjTopicMap topics;
		SbjSubscriberMap subscribers;
	};
};

#endif

