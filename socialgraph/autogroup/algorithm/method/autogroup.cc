#include "socialgraph/autogroup/algorithm/method/autogroup.h"

namespace xce {
namespace socialgraph {

//主分组函数，需要子类实现其中的某些函数
void AutoGroup::Grouping(int userid) {
	std::ostringstream oss;
	oss << "[AutoGroup] Grouping userid(" << userid << ")";
	PTIME(pt, oss.str(), true, 0);

	Initialize(userid);	

	if (hook()) {
		CreateGraph();

		CreateGroup();

		CreateGroupTags();

		CreateStats();
	}
}

//初始化创建好友关系
void AutoGroup::Initialize(int userid) {
	userid_ = userid;

	try {
		friendlist_ = xce::buddy::adapter::BuddyByIdCacheAdapter::instance().getFriendListAsc(userid_, -1);
	} catch (Ice::Exception e) {
		std::cout << "[AutoGroup] Initialize userid(" << userid_ << ") Ice::Exception " << e.what() << std::endl;
	} catch (...) {
		std::cout << "[AutoGroup] Initialize userid(" << userid_ << ") unknown::Exception" << std::endl;
	}
}

//创建好友关系图
void AutoGroup::CreateGraph() {
	friendgraphptr_ = boost::shared_ptr<FriendsGraph>(new FriendsGraph((int)friendlist_.size()));

	std::map<int, std::vector<int> > ff_map;
	try {
		ff_map = xce::buddy::adapter::BuddyByIdCacheAdapter::instance().getFriendLists(friendlist_);
	} catch (Ice::Exception e) {
		std::cout << "[AutoGroup] CreateGraph BuddyByIdCacheAdapter userid(" << userid_ << ") Ice::Exception " << e.what() << std::endl;
	} catch (...) {
		std::cout << "[AutoGroup] CreateGraph BuddyByIdCacheAdapter userid(" << userid_ << ") unknown::Exception" << std::endl;
	}
	for (int i = 0; i < friendgraphptr_->friendsnum_; ++i) {
		std::map<int, std::vector<int> >::iterator fit;
		try {
			fit = ff_map.find(friendlist_.at(i));
		} catch (std::exception e) {
			std::cout << "[AutoGroup] CreateGraph map userid(" << userid_ << ") std::exception " << e.what() << std::endl;
		} catch (...) {
			std::cout << "[AutoGroup] CreateGraph map userid(" << userid_ << ") unknown::Exception " << std::endl;
		}

		if (fit != ff_map.end()) {
			for (int j = i + 1; j < friendgraphptr_->friendsnum_; ++j) {
				std::vector<int>::iterator fsameit = lower_bound(fit->second.begin(), fit->second.end(), friendlist_.at(j));
				if (fsameit != fit->second.end() && *fsameit == friendlist_.at(j)) {
					friendgraphptr_->graph_[i][j] = 1;
					friendgraphptr_->graph_[j][i] = 1;
				}	
			}
		}
	}
}

}
}
