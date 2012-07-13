#ifndef _SORTBY_FRIENDRANK_UTIL_H_
#define _SORTBY_FRIENDRANK_UTIL_H_

#include "OceCxxAdapter/src/FriendRankCacheAdapter.h"
#include "UtilCxx/src/TaskManager.h"

#include "socialgraph/socialgraphutil/calculator/component.h"

namespace xce {
namespace socialgraph {

const int TASK_LEVEL_SORTBY_FRIENDRANK = 2;

//-----------------------------------------------------------------------------------
struct OriginalRankComp {
    bool operator()(const xce::socialgraph::RankData& d1, const xce::socialgraph::RankData& d2) {
        return d1.weight > d2.weight;
    }   
};                       

struct RankComp {
    bool operator()(const std::pair<int, int>& p1, const std::pair<int, int>& p2) {
        return p1.second > p2.second;
    }   
};                       

//-----------------------------------------------------------------------------------

class SortByFriendRankUtil: public Component {
public:
	SortByFriendRankUtil() {}
	void Load() { }
	void SortTargetByFriendRank(std::map<int, std::vector<int> >& target);
};

//-----------------------------------------------------------------------------------
//按亲密度排序
class SortByFriendRankTask : public MyUtil::Task {
public:
	SortByFriendRankTask(int userId, std::vector<int>& idList) : _userId(userId), _idList(idList) {}
	virtual void handle();

private:
	int _userId;
	std::vector<int>& _idList;
};

}
}

#endif
