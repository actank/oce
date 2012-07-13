#ifndef FEED_LRU_ICE
#define FEED_LRU_ICE

#include <Util.ice>
module xce {
module feed {

class LruItem{
	int key;
	int value;
};

class FeedLru{
	//void 
	void Add(int key);
	int Get(int key);	
	void GetInteraction(long fid);	
	void RunTask(int start);	
	void Schedule(int seconds);
	void InsertDb(int uid,int feature);
	void SelectDb(int uid);
};

};
};
#endif
