#ifndef __FEED_CACHEN_TASKS_H__
#define __FEED_CACHEN_TASKS_H__
#include "TaskManager.h"
#include "RFeed.h"
#include "FeedCacheI.h"

using namespace std;
using namespace xce::feed;

class RejectTask: public MyUtil::Task {
public:
	RejectTask(const int uid, const ::xce::feed::FeedItemSeq & fis):
			Task(), _uid(uid), _fis(fis) { }	
	
	virtual void handle();
private:
	int _uid;
	::xce::feed::FeedItemSeq  _fis;
		

};
#endif
