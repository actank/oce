#ifndef _MY_TASKS_H_
#define _MY_TASKS_H_

#include "TaskManager.h"
#include "HotFeedRecommendCacheAdapter.h"
#include "NegtiveListCacheAdapter.h"

namespace tasks {

static const int TASK_LEVEL_SET_CACHE = 5;

//********************************************************************************************
class AppendSingleRelationAndSetCacheTask : public MyUtil::Task {
public:
	AppendSingleRelationAndSetCacheTask(int ownerId, const std::vector<int>& negtiveFriendList, 
		::Ice::Int level = TASK_LEVEL_SET_CACHE):
		MyUtil::Task(level), _ownerId(ownerId), _negtiveFriendList(negtiveFriendList) {
	}

	virtual void handle();

private:
	int _ownerId;
	std::vector<int> _negtiveFriendList;
};

}

#endif
