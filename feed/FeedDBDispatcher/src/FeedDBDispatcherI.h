#ifndef FEED_DB_DISPATCHER_H_
#define FEED_DB_DISPATCHER_H_

#include <FeedDB.h>
#include "Singleton.h"
#include "TaskManager.h"
#include "IceLogger.h"

using namespace MyUtil;
using namespace xce::feed;
class FeedDBDispatcherI : public FeedDBDispatcher,
			public Singleton<FeedDBDispatcherI>,
			public Timer{
public:
	FeedDBDispatcherI() : Timer(1000){
		putCount = 0;
	}
	//void put(int userid, const FeedItem& item, const Ice::Current& = Ice::Current());
	void putBatch(const FeedItem& item, const MyUtil::IntSeq& userIds, const Ice::Current& = Ice::Current());
	//FeedItemSeq get(int userid, int begin, int count, const Ice::Current& = Ice::Current());

	void handle(){
		MCE_INFO("put count " << putCount);
		putCount = 0;
	}
	void incPut(){
		IceUtil::Mutex::Lock lock(_mutex);
		putCount++;
	}
private:
	IceUtil::Mutex _mutex;
	int putCount;
};

#endif
