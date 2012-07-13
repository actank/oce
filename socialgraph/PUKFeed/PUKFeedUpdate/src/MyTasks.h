#ifndef _MY_TASKS_H_
#define _MY_TASKS_H_

#include "TaskManager.h"
#include "PUKFeedCacheAdapter.h"

#include "ReadFriendListUtil.h"
#include "PUKFeedItem.h"
#include "PUKFeedDBHelper.h"

extern readfriend::ReadFriendListUtil* g_readfriend_ptr;

namespace tasks {

static const int TASK_LEVEL_CREATE_FEED = 1;

//---------------------------------------------------------------

struct PUKFeedComp {
	bool operator() (const PUKFeedItem& p1, const PUKFeedItem& p2) {
		return p1.GetSharesCount() > p2.GetSharesCount();	
	}
};

//---------------------------------------------------------------

class CreateFeedAndSetCacheTask : public MyUtil::Task {
public:
	CreateFeedAndSetCacheTask(int ownerId, const std::vector<int>& recommendList, 
		const std::map<int, std::set<int> >& buddyApplyMap, ::Ice::Int level = TASK_LEVEL_CREATE_FEED):
		MyUtil::Task(level), _ownerId(ownerId), _recommendList(recommendList), _buddyApplyMap(buddyApplyMap) {
	}  

	virtual void handle();

private:
	PUKFeedItem GetMostValuePUKFeed(int friendId, const std::vector<int>& ownerFriends);

	void PrintMap(const map<int, vector<int> >& resource) {
		for (map<int, vector<int> >::const_iterator iter = resource.begin();
				iter != resource.end(); ++iter) {
			cout << iter->first << " : " << endl;
			for (vector<int>::const_iterator it = iter->second.begin();
					it != iter->second.end(); ++it) {
					cout << *it << " ";
			}
			cout << endl;
		}
	}

	int _ownerId;
	std::vector<int> _recommendList;
	const std::map<int, std::set<int> >& _buddyApplyMap;
};

}

#endif
