#ifndef _SORTBY_FRIENDRANK_UTIL_H_
#define _SORTBY_FRIENDRANK_UTIL_H_

#include <FriendRankCacheAdapter.h>
#include <TaskManager.h>

namespace sortbyrank {

const int TASK_LEVEL_SORTBY_FRIENDRANK = 2;

//-----------------------------------------------------------------------------------

struct RankComp {
    bool operator()(const std::pair<int, int>& p1, const std::pair<int, int>& p2) {
        return p1.second > p2.second;
    }   
};                       

//-----------------------------------------------------------------------------------

class SortByFriendRankUtil {
public:
	SortByFriendRankUtil() {}
	void SortTargetByFriendRank(std::map<int, std::vector<int> >& target);
};

//-----------------------------------------------------------------------------------

class SortByFriendRankTask : public MyUtil::Task {
public:
	SortByFriendRankTask(int userId, std::vector<int>& idList) : _userId(userId), _idList(idList) {}
	virtual void handle();

private:
	int _userId;
	std::vector<int>& _idList;
};

}

#endif
