#ifndef _AUTOGROUPWITHEXISTGROUP_H_
#define _AUTOGROUPWITHEXISTGROUP_H_

#include <iostream>
#include <map>
#include <set>
#include <math.h>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>

#include "OceCxxAdapter/src/BuddyByIdReplicaAdapter.h"
#include "OceCxxAdapter/src/BuddyGroupAdapter.h"
#include "OceCxxAdapter/src/UserNameAdapter.h"

#include "socialgraph/autogroup/algorithm/method/autogroup.h"
#include "socialgraph/autogroup/algorithm/method/impl/friendsgraph.h"
#include "socialgraph/autogroup/algorithm/util/util.h"

#include "base/ptime.h"

namespace xce {
namespace socialgraph {

const static int REMAINNUM = 3;

//------------------------------------------------------------------------
//每个好友与每个分组的关系
struct RelationItem {
	RelationItem() {
		id_ = 0;
		index_ = 0;
		relation_ = 0;
	}

	int id_;
	int index_;
	int relation_;
};

//------------------------------------------------------------------------

struct RelationComp {
	bool operator()(const RelationItem& r1, const RelationItem& r2) {
		return r1.relation_ > r2.relation_;
	}
};

//------------------------------------------------------------------------

class AutoGroupWithExistGroup : public AutoGroup {
public:
	AutoGroupWithExistGroup() {
		userid_ = 0;
	}
	virtual ~AutoGroupWithExistGroup() {}

	void GetGroupResult(Items& result);
	GroupStats GetGroupStats();
	void Display(); 

	//好友数不满足条件不进行计算
	bool hook() {
		if (friendlist_.empty() || 2000 < friendlist_.size()) {
			grouped_stats_.userid_ = userid_;
			grouped_stats_.friendsnum_ = (int)friendlist_.size();
			return false;	
		}
		return true;
	}

protected:
	void CreateGroup();
	void CreateGroupTags();
	void CreateStats();

private:
	void GetIdIndex();
	void GetExistGroup();
	void GetUngroupedMems();
	void CreateGroupRecommend();
	void GetMembersIndex(const std::vector<int>& idlist, std::vector<int>& indexlist);
	int GetRelationCount(int index, const std::vector<int>& indexlist);

	std::vector<int> ungrouped_friends_;
	GroupStats grouped_stats_;
	std::map<int, int> id_index_;
	std::map<std::string, std::vector<int> > group_members_;
	std::map<std::string, std::vector<int> > group_recommends_;
};

}
}

#endif
