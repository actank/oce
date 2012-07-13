#ifndef _CHECKER_ICE_
#define _CHECKER_ICE_

#include <Util.ice>

module xce {
module check{

	struct Entry
	{
		string path;
        	string step;
        	string replica;
        	string key;
        	string checksum;	
	};

	sequence<Entry> EntrySeq;

	interface CheckerManager{
	
		void add(string path, string step, string replica, string key, string checksum);
		void notify(string path, string step, string replica, string key,string checksum);
		
		void addOne(Entry one);
		void notifyOne(Entry one);
		void addBatch(EntrySeq batch);
		void notifyBatch(EntrySeq batch);

		bool reloadConfig();
	};
};
};

#endif
