#ifndef _AUTOGROUPWITHFREE_H_
#define _AUTOGROUPWITHFREE_H_
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <math.h>
#include <boost/shared_ptr.hpp>

#include "OceCxxAdapter/src/BuddyByIdReplicaAdapter.h"
#include "OceCxxAdapter/src/UserNameAdapter.h"

#include "socialgraph/autogroup/algorithm/method/autogroup.h"
#include "socialgraph/autogroup/algorithm/method/impl/friendsgraph.h"
#include "socialgraph/autogroup/algorithm/method/impl/genericsortutil.h"
#include "socialgraph/autogroup/algorithm/method/impl/group.h"

namespace xce {
namespace socialgraph {

const float THRESHOLD1 = 0.66f;
const float THRESHOLD2 = 0.8f;
const float THRESHOLD3 = 0.5;
const int MEMBERS_THRESHOLD = 2;
//决定如何删除组成员的策略
enum Strategy {
	HUMAN = 0,
	AUTO
};

//------------------------------------------------------------

class AutoGroupWithFree : public AutoGroup {
public:
	AutoGroupWithFree() {
		userid_ = 0;
	}
	virtual ~AutoGroupWithFree() {}

	void GetGroupResult(Items& result);
	//决定程序执行的条件	
	bool hook() {
		if (friendlist_.empty() || 2000 < friendlist_.size()) {
			grouped_stats_.userid_ = userid_;
			grouped_stats_.friendsnum_ = (int)friendlist_.size();
			return false;	
		}
		return true;
	}

	void Display(); 
	GroupStats GetGroupStats();


protected:
	void CreateGroup();
	void CreateGroupTags();
	void CreateStats();

private:
	void CreateClique();
	void SortMembersOfIndexGroup();
	void AddFriendsIntoGroup();
	void DeleteMembersFromGroup(Strategy strategy, const std::map<int, std::vector<int> >& backupmap);
	void UniteSimilarGroup();
	void RefineIndexGroup();
	void CreateUngrouped();
	void CreateRealGroup();

	std::map<int, std::vector<int> > indexgroup_;
	std::map<int, std::vector<int> > group_members_;
	GroupStats grouped_stats_;
};

}
}

#endif
