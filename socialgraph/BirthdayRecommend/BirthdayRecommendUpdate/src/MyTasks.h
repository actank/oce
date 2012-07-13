#ifndef _MY_TASKS_H_
#define _MY_TASKS_H_

#include "DBHelper.h"
#include "TaskManager.h"
#include <FriendRankCacheAdapter.h>
#include <BirthdayRecommendCacheAdapter.h>

extern xce::dbhelper::DBHelper* dbhelper;

//namespace xce {
//namespace task {

const int TASK_LEVEL_READ_FRIENDS = 1;
const int TASK_LEVEL_FLITER_BLOCK = 2;
const int TASK_LEVEL_SET_CACHE = 3;

//--------------------------------------------------------------------------------------------

struct RankComp {
	bool operator()(const std::pair<int, int>& p1, const std::pair<int, int>& p2) {
		return p1.second > p2.second;
	}
};

//--------------------------------------------------------------------------------------------

class ReadFriendTask : public MyUtil::Task {
public:
	ReadFriendTask(int userId, std::vector<int>& friends, ::Ice::Int level = TASK_LEVEL_READ_FRIENDS) :
		MyUtil::Task(level), _userId(userId), _friends(friends) {
	}
	virtual void handle(); 

private:
	int _userId;
	std::vector<int>& _friends;
};

//--------------------------------------------------------------------------------------------

class FliterBlockTask : public MyUtil::Task {
public:
	FliterBlockTask(int userId, std::vector<int>& birthdayRecommendList, ::Ice::Int level = TASK_LEVEL_FLITER_BLOCK) :
		MyUtil::Task(level), _userId(userId), _birthdayRecommendList(birthdayRecommendList) {
	}		
	virtual void handle();

private:
	int _userId;
	std::vector<int>& _birthdayRecommendList;
};

//--------------------------------------------------------------------------------------------

class SetCacheAndSortTask : public MyUtil::Task {
public:
	SetCacheAndSortTask(int userId, std::vector<int>& birthdayRecommendList, ::Ice::Int level = TASK_LEVEL_SET_CACHE) :
		MyUtil::Task(level), _userId(userId), _birthdayRecommendList(birthdayRecommendList) {
	}
	virtual void handle();

private:
	int _userId;
	std::vector<int>& _birthdayRecommendList;	
};

//}
//}

#endif
