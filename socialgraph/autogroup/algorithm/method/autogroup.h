#ifndef _AUTOGROUP_H_
#define _AUTOGROUP_H_

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <set>
#include <boost/shared_ptr.hpp>

#include "OceCxxAdapter/src/BuddyByIdReplicaAdapter.h"
#include "socialgraph/autogroup/algorithm/method/impl/friendsgraph.h"
#include "socialgraph/autogroup/algorithm/util/util.h"
#include "base/ptime.h"
#include "item.pb.h"

namespace xce {
namespace socialgraph {

//分组结果统计
struct GroupStats {
	GroupStats() {
		userid_ = 0;
		friendsnum_ = 0;
		group_num_ = 0;
		groupednum_ = 0;
		ungroupednum_ = 0;
		groupedrate_ = 0;
	}

	int userid_;
	int friendsnum_;
	int group_num_;
	int groupednum_;
	int ungroupednum_;
	float groupedrate_;
};

//------------------------------------------------------------

class AutoGroup {
public:
	AutoGroup() {}
	virtual ~AutoGroup() {}

	virtual void Grouping(int userid);

	//需要子类实现的函数
	virtual void GetGroupResult(Items& result) = 0;
	virtual GroupStats GetGroupStats() = 0;
	virtual void Display() = 0;

protected:
	virtual void CreateGroup() = 0;
	virtual void CreateGroupTags() = 0;
	virtual void CreateStats() = 0;

	//钩子，控制运行条件,子类可自行实现
	virtual bool hook() {
		return true;
	}

	int userid_;
	std::vector<int> friendlist_;
	boost::shared_ptr<FriendsGraph> friendgraphptr_;

private:
	void Initialize(int userid);
	void CreateGraph();
};

typedef boost::shared_ptr<AutoGroup> AutoGroupPtr;

}
}

#endif
