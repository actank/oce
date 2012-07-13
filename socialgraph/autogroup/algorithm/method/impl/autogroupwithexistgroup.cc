#include "socialgraph/autogroup/algorithm/method/impl/autogroupwithexistgroup.h"

namespace xce {
namespace socialgraph {

void AutoGroupWithExistGroup::CreateGroup() {
	//创建用户id索引
	GetIdIndex();

	//获取已存在分组
	GetExistGroup();

	//获取为分组用户
	GetUngroupedMems();
	
	//根据好友关系创建分组推荐
	CreateGroupRecommend();
}

void AutoGroupWithExistGroup::CreateGroupTags() {
}

void AutoGroupWithExistGroup::CreateStats() {
	grouped_stats_.userid_ = userid_;
	grouped_stats_.friendsnum_ = friendgraphptr_->friendsnum_;
	grouped_stats_.group_num_ = (int)group_members_.size();
	grouped_stats_.groupednum_ = (int)group_recommends_.size();
	grouped_stats_.ungroupednum_ = 100;
}

//显示分组结果
void AutoGroupWithExistGroup::Display() {
	std::cout << std::endl;
	for (std::map<std::string, std::vector<int> >::iterator iter = group_recommends_.begin();
			iter != group_recommends_.end(); ++iter) {
		std::map<std::string, std::vector<int> >::iterator fit = group_members_.find(iter->first);
		if (fit == group_members_.end()) {
			continue;
		}
		
		std::cout << iter->first << ":" << std::endl;
		for (std::vector<int>::iterator it = fit->second.begin();
				it != fit->second.end(); ++it) {
			xce::adapter::username::UserNameInfoPtr nameptr = xce::adapter::username::UserNameAdapter::instance().getUserName(*it);
			std::cout << nameptr->name() << " ";
		}
		std::cout << std::endl;

		std::cout << "recommend:" << std::endl;
		std::cout << "{ ";
		for (std::vector<int>::iterator it = iter->second.begin();
				it != iter->second.end(); ++it) {
			xce::adapter::username::UserNameInfoPtr nameptr = xce::adapter::username::UserNameAdapter::instance().getUserName(*it);
			std::cout << nameptr->name() << " ";
		}
		std::cout << "}";
		std::cout << std::endl;
		std::cout << std::endl;
	}
}

void AutoGroupWithExistGroup::GetIdIndex() {
	int friendnum = 0;
	for (std::vector<int>::iterator iter = friendlist_.begin();
			iter != friendlist_.end(); ++iter) {
		id_index_.insert(make_pair<int, int>(*iter, friendnum++));
	}

}

void AutoGroupWithExistGroup::GetExistGroup() {
	std::vector<std::string> groupnamelist = mop::hi::svc::adapter::BuddyGroupAdapter::instance().getGroupList(userid_);
	
	BOOST_FOREACH (std::string name, groupnamelist) {
		const std::vector<int>& mems = mop::hi::svc::adapter::BuddyGroupAdapter::instance().getBuddyList(userid_, name, 0, -1);
		group_members_.insert(make_pair<std::string, std::vector<int> >(name, mems));
	}

}

void AutoGroupWithExistGroup::GetUngroupedMems() {
	ungrouped_friends_ = friendlist_;
	for (std::map<std::string, std::vector<int> >::iterator iter = group_members_.begin();
			iter != group_members_.end(); ++iter) {
		std::vector<int> tmp_ungroup_mem;
		for (std::vector<int>::iterator it = ungrouped_friends_.begin();
				it != ungrouped_friends_.end(); ++it) {
			std::vector<int>::iterator fit = lower_bound(iter->second.begin(), iter->second.end(), *it);
			if (fit == iter->second.end() || *it != *fit) {
				tmp_ungroup_mem.push_back(*it);
			}
		}
		ungrouped_friends_.swap(tmp_ungroup_mem);
	}
}

void AutoGroupWithExistGroup::CreateGroupRecommend() {
	std::vector<int> ungroup_member_index;
	GetMembersIndex(ungrouped_friends_, ungroup_member_index);	
	
	for (std::map<std::string, std::vector<int> >::iterator iter = group_members_.begin();
			iter != group_members_.end(); ++iter) {
		std::vector<int> exist_member_index;
		GetMembersIndex(iter->second, exist_member_index);

		std::vector<RelationItem> itemlist;
		for (int i = 0; i < (int)ungroup_member_index.size(); ++i) {
			RelationItem item;
			item.index_ = ungroup_member_index.at(i);
			item.id_ = ungrouped_friends_.at(i);
			item.relation_ = GetRelationCount(ungroup_member_index.at(i), exist_member_index);
			itemlist.push_back(item);
		}
		
		sort(itemlist.begin(), itemlist.end(), RelationComp());
		int left_size = (int)itemlist.size() > REMAINNUM ? REMAINNUM : (int)itemlist.size();

		int threshold = 0;
		//好友数>2时threshold为好友数的1/3
		if ((int)iter->second.size() > 2) {
			threshold = (int)(iter->second.size()/3);
		} else {
			threshold = 1;
		}

		//暂时去掉推送好友数量限制
		//itemlist.resize(left_size);
		for (std::vector<RelationItem>::iterator it = itemlist.begin();
				it != itemlist.end(); ++it) {
			if (it->relation_ >= threshold) {
				group_recommends_[iter->first].push_back(it->id_);
			}
		}
	}
}

void AutoGroupWithExistGroup::GetMembersIndex(const std::vector<int>& idlist, std::vector<int>& indexlist) {
	for (std::vector<int>::const_iterator iter = idlist.begin();
			iter != idlist.end(); ++iter) {
		std::map<int, int>::iterator fit = id_index_.find(*iter);
		if (fit != id_index_.end()) {
			indexlist.push_back(fit->second);	
		}
	}
}

int AutoGroupWithExistGroup::GetRelationCount(int index, const std::vector<int>& indexlist) {
	int count = 0;
	for (std::vector<int>::const_iterator iter = indexlist.begin();
			iter != indexlist.end(); ++iter) {
		if (1 == friendgraphptr_->graph_[index][*iter]) {
			++count;
		}
	}

	return count;
}

GroupStats AutoGroupWithExistGroup::GetGroupStats() {
	return grouped_stats_;
}

void AutoGroupWithExistGroup::GetGroupResult(Items& result) {
	if (group_recommends_.empty()) {
		return;
	}

	for (std::map<std::string, std::vector<int> >::iterator iter = group_recommends_.begin();
			iter != group_recommends_.end(); ++iter) {
		Item* item = result.add_items();
		item->set_id(userid_);
		item->set_message(iter->first);
		for (std::vector<int>::iterator it = iter->second.begin();
				it != iter->second.end(); ++it) {
			item->add_fields(*it);
		}
	}

	if (0 < result.items_size()) {
		result.set_timestamp(time(NULL));
	}
}

}
}
