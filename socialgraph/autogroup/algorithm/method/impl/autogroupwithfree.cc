#include "socialgraph/autogroup/algorithm/method/impl/autogroupwithfree.h"

namespace xce {
namespace socialgraph {

void AutoGroupWithFree::CreateGroup() {
	std::cout << "[AutoGroupWithFree] Grouping userid(" << userid_ << ")" << std::endl;

	//按用户好友关系排序
	GenericSortUtil::SortFriendsOrderByRelationNum(friendgraphptr_, friendlist_);
	
	//创建团
	CreateClique();

	//排序组成员的索引
	SortMembersOfIndexGroup();

	//备份原始组
	std::map<int, std::vector<int> > originalclique(indexgroup_);

	//添加成员
	AddFriendsIntoGroup();

	//保证一定的内聚度，删除组成员
	DeleteMembersFromGroup(HUMAN, originalclique);

	//备份组
	std::map<int, std::vector<int> > originalgroup(indexgroup_);

	//组合并
	UniteSimilarGroup();

	//删除多余成员
	DeleteMembersFromGroup(AUTO, originalgroup);

	//产生真实用户id
	RefineIndexGroup();

	//生成分组结果
	CreateRealGroup();
}

void AutoGroupWithFree::CreateGroupTags() {
}

void AutoGroupWithFree::CreateStats() {
	grouped_stats_.userid_ = userid_;
	grouped_stats_.friendsnum_ = friendgraphptr_->friendsnum_;
	grouped_stats_.group_num_ = (int)indexgroup_.size();
	
	std::set<int> groupedindex;
	for (std::map<int, std::vector<int> >::iterator iter = indexgroup_.begin();
			iter != indexgroup_.end(); ++iter) {
		for (std::vector<int>::iterator it = iter->second.begin();
				it != iter->second.end(); ++it) {
			groupedindex.insert(*it);
		}
	}
	grouped_stats_.groupednum_ = (int)groupedindex.size();

	grouped_stats_.groupedrate_ = (float)groupedindex.size() / friendgraphptr_->friendsnum_;

	grouped_stats_.ungroupednum_ = friendgraphptr_->friendsnum_ - (int)groupedindex.size();
}

void AutoGroupWithFree::Display() {
	std::cout << std::endl;
	std::cout << "[AutoGroupWithFree] Display" << std::endl;
	for (std::map<int, std::vector<int> >::iterator iter = group_members_.begin();
			iter != group_members_.end(); ++iter) {
		std::cout << iter->first << ":" << std::endl;
		for (std::vector<int>::iterator it = iter->second.begin();
				it != iter->second.end(); ++it) {
			xce::adapter::username::UserNameInfoPtr nameptr = xce::adapter::username::UserNameAdapter::instance().getUserName(*it);
			std::cout << nameptr->name() << " ";
		}
		std::cout << std::endl;
		std::cout << std::endl;
	}
}

void AutoGroupWithFree::CreateClique() {
	//一个人在一个组
	int groupindex = 0;
	for (int i = 0; i < friendgraphptr_->friendsnum_; ++i) {
		if (i == 0 && indexgroup_.empty()) {
			indexgroup_[(++groupindex)].push_back(i);
			continue;
		}

		std::map<int, std::vector<int> >::iterator iter;
		for (iter = indexgroup_.begin(); iter != indexgroup_.end(); ++iter) {
			bool insert = true;
			for (std::vector<int>::iterator it = iter->second.begin();
					it != iter->second.end(); ++it) {
				if (0 == friendgraphptr_->graph_[i][*it]) {
					insert = false;
					break;
				}	
			}

			if (insert) {
				iter->second.push_back(i);
				break;	
			}
		}

		if (iter == indexgroup_.end()) {
			indexgroup_[++groupindex].push_back(i);
		}
	}

	//一个人在多个组
	for (int i = 0; i < friendgraphptr_->friendsnum_; ++i) {
		std::map<int, std::vector<int> >::iterator iter; for (iter = indexgroup_.begin(); iter != indexgroup_.end(); ++iter) {
			bool insert = true;
			for (std::vector<int>::iterator it = iter->second.begin();
					it != iter->second.end(); ++it) {
				if (0 == friendgraphptr_->graph_[i][*it]) {
					insert = false;	
					break;
				}
			}

			if (insert) {
				iter->second.push_back(i);
				break;
			}
		}
	}

	//保留多于两人的组
	std::map<int, std::vector<int> > newgroup;
	int newaccount = 0;
	size_t LEFTNUM = 2;
	for (std::map<int, std::vector<int> >::iterator iter = indexgroup_.begin();
			iter != indexgroup_.end(); ++iter) {
		if (iter->second.size() > LEFTNUM) {
			newgroup.insert(make_pair<int, std::vector<int> >(++newaccount, iter->second));
		}
	}

	indexgroup_.swap(newgroup);
}

void AutoGroupWithFree::SortMembersOfIndexGroup() {
	for (std::map<int, std::vector<int> >::iterator iter = indexgroup_.begin();
			iter != indexgroup_.end(); ++iter) {
		std::sort(iter->second.begin(), iter->second.end());	
	}
}

void AutoGroupWithFree::AddFriendsIntoGroup() {
	std::map<int, std::vector<int> > addmembers;
	for (std::map<int, std::vector<int> >::iterator iter = indexgroup_.begin();
			iter != indexgroup_.end(); ++iter) {
		addmembers.insert(make_pair<int, std::vector<int> >(iter->first, std::vector<int>()));
	}

	for (int i = 0; i < friendgraphptr_->friendsnum_; ++i) {
		for (std::map<int, std::vector<int> >::iterator iter = indexgroup_.begin();
				iter != indexgroup_.end(); ++iter) {
			float relationaccount = 0;
			float relationtotal = 0;

			std::vector<int>::iterator fit = lower_bound(iter->second.begin(), iter->second.end(), i);
			if (fit == iter->second.end() || *fit != i) {
				for (std::vector<int>::iterator it = iter->second.begin();
						it != iter->second.end(); ++it) {
					if (1 == friendgraphptr_->graph_[i][*it]) {
						++relationaccount;
					}
				}

				relationtotal = iter->second.size();
				float relationrate = relationaccount/relationtotal;
			
				//认识该组成员的比例>THRESHOLD1时添加进组
				if (relationrate >= THRESHOLD1) {
					std::map<int, std::vector<int> >::iterator addmapiter = addmembers.find(iter->first);
					if (addmapiter != addmembers.end()) {
						addmapiter->second.push_back(i);
					}
				}
			}
		}
	}

	//最后在进行一次添加成员
	for (std::map<int, std::vector<int> >::iterator iter = addmembers.begin();
			iter != addmembers.end(); ++iter) {
		if (iter->second.empty()) {
			continue;
		}
		
		std::map<int, std::vector<int> >::iterator fit = indexgroup_.find(iter->first);
		if (fit != indexgroup_.end()) {
			fit->second.insert(fit->second.end(), iter->second.begin(), iter->second.end());
		}
	}
}

void AutoGroupWithFree::DeleteMembersFromGroup(Strategy strategy, const std::map<int, std::vector<int> >& backupmap) {
	for (std::map<int, std::vector<int> >::iterator iter = indexgroup_.begin();
			iter != indexgroup_.end(); ++iter) {
		float relationtotal = iter->second.size() * (iter->second.size() - 1);
		float relationaccount = 0;

		for (std::vector<int>::iterator it = iter->second.begin();
				it != iter->second.end(); ++it) {
			for (std::vector<int>::iterator jt = it + 1;
					jt != iter->second.end(); ++jt) {
				if (1 == friendgraphptr_->graph_[*it][*jt]) {
					relationaccount += 2;
				}
			}
		}

		float relationrate = relationaccount / relationtotal;

		float threshold = 0;
		//策略选择，是进行严格分组还是自适应分组
		if (HUMAN == strategy) {
			threshold = THRESHOLD2;
		} else {
			size_t members_size = iter->second.size();	
			if (members_size < 4.0) {
				continue;
			}
			threshold = 0.6 * exp(-0.0038 * members_size);
		}

		if (relationrate < threshold) {
			std::vector<int>::iterator min_node;
			bool change = false;
			int min_relation = (int)iter->second.size();

			std::map<int, std::vector<int> >::const_iterator fit = backupmap.find(iter->first);
			if (fit == backupmap.end()) {
				continue;
			}

			for (std::vector<int>::iterator iit = iter->second.begin();
					iit != iter->second.end(); ++iit) {
				std::vector<int>::const_iterator existfit = lower_bound(fit->second.begin(), fit->second.end(), *iit);
				if (existfit != fit->second.end() && *existfit == *iit) {
					continue;
				}

				int real_relation = 0;
				for (std::vector<int>::iterator jjt = iter->second.begin();
						jjt != iter->second.end(); ++jjt) {
					if (1 == friendgraphptr_->graph_[*iit][*jjt]) {
						++real_relation;
					}
				}
				//过滤出关系最少的节点
				if (real_relation < min_relation) {
					min_relation = real_relation;
					min_node = iit;
					change = true;
				}
			}

			if (change) {
				iter->second.erase(min_node);
				--iter;
			}
		}
	}
}

void AutoGroupWithFree::UniteSimilarGroup() {
	int group_size = (int)indexgroup_.size();
	//创建组成员和每组对应的graph
	int** friends_group_graph = new int*[friendgraphptr_->friendsnum_];
	for (int i = 0; i < friendgraphptr_->friendsnum_; ++i) {
		friends_group_graph[i] = new int[group_size + 1];
	}

	for (int i = 0; i < friendgraphptr_->friendsnum_; ++i) {
		for (int j = 0; j < group_size + 1; ++j) {
			friends_group_graph[i][j] = 0;
		}
	}

	for (std::map<int, std::vector<int> >::iterator iter = indexgroup_.begin();
			iter != indexgroup_.end(); ++iter) {
		for (std::vector<int>::iterator it = iter->second.begin();
				it != iter->second.end(); ++it) {
			friends_group_graph[*it][iter->first] = 1;
		}
	}
	//删除队列
	int* disablegroup = new int[group_size + 1];
	for (int i = 0; i < group_size + 1; ++i) {
		disablegroup[i] = 0;
	}

	//组与组之间相同成员数
	int** same_element_array = new int *[group_size + 1];
	for (int i = 0; i < group_size + 1; ++i) {
		same_element_array[i] = new int[group_size + 1];
	}

	for (int i = 0; i < group_size + 1; ++i) {
		for (int j = 0; j < group_size + 1; ++j) {
			same_element_array[i][j] = 0;
		}
	}

	bool first = true;
//找到重合度最大的两个组，然后合并
	while (true) {
		float max_element_rate = 0;
		int max_same_group_x = 0;
		int max_same_group_y = 0;

		for (std::map<int, std::vector<int> >::iterator iter = indexgroup_.begin();
				iter != indexgroup_.end(); ++iter) {
			if (1 == disablegroup[iter->first]) {
				continue;
			}

			std::map<int, std::vector<int> >::iterator it = iter;
			++it;
			for (; it != indexgroup_.end(); ++it) {
				if (1 == disablegroup[it->first]) {
					continue;
				}
				float same_element_account = 0;
				float total_element_account = 0;

				if (first) {
					for (std::vector<int>::iterator vit = it->second.begin();
							vit != it->second.end(); ++vit) {
						if (1 == friends_group_graph[*vit][iter->first]) {
							++same_element_account;
						}
					}

					same_element_array[iter->first][it->first] = (int)same_element_account;
				} else {
					same_element_account = same_element_array[iter->first][it->first];
				}

				total_element_account = min(iter->second.size(), it->second.size());
				float same_element_rate = same_element_account / total_element_account;

				if (same_element_rate > max_element_rate) {
					max_element_rate = same_element_rate;
					max_same_group_x = iter->first;
					max_same_group_y = it->first;
				}
			}
		}

		if (max_element_rate >= THRESHOLD3) {
			first = false;
			std::map<int, std::vector<int> >::iterator xfit = indexgroup_.find(max_same_group_x);
			std::map<int, std::vector<int> >::iterator yfit = indexgroup_.find(max_same_group_y);
			if (xfit == indexgroup_.end() || yfit == indexgroup_.end()) {
				break;
			}

			for (std::vector<int>::iterator it = yfit->second.begin();
					it != yfit->second.end(); ++it) {
				if (0 == friends_group_graph[*it][max_same_group_x]) {
					xfit->second.push_back(*it);
					friends_group_graph[*it][max_same_group_x] = 1;
				}
			}
			
			indexgroup_.erase(max_same_group_y);
			disablegroup[max_same_group_y] = 1;

			for (std::map<int, std::vector<int> >::iterator iter = indexgroup_.begin();
					iter != indexgroup_.end(); ++iter) {
				if (disablegroup[iter->first] || iter->first == max_same_group_x) {
					continue;
				}

				int same_element_account = 0;
				for (std::vector<int>::iterator it = xfit->second.begin();
						it != xfit->second.end(); ++it) {
					if (1 == friends_group_graph[*it][iter->first]) {
						++same_element_account;
					}
				}
				same_element_array[min(iter->first, max_same_group_x)][max(iter->first, max_same_group_x)] = same_element_account;
			}
		} else {
			break;
		}
	}

	for (int i = 0; i < friendgraphptr_->friendsnum_; ++i) {
		delete[] friends_group_graph[i];
	}
	delete[] friends_group_graph;
	friends_group_graph = NULL;

	delete[] disablegroup;
	disablegroup = NULL;

	for (int i = 0; i < group_size + 1; ++i) {
		delete[] same_element_array[i];
	}
	delete[] same_element_array;
	same_element_array = NULL;
}

void AutoGroupWithFree::RefineIndexGroup() {
	int newindex = 0;
	std::map<int, std::vector<int> > newindexgroup;
	for (std::map<int, std::vector<int> >::iterator iter = indexgroup_.begin();
			iter != indexgroup_.end(); ++iter) {
		if ((int)iter->second.size() > MEMBERS_THRESHOLD) {
			newindexgroup.insert(make_pair<int, std::vector<int> >(++newindex, iter->second));
		}
	}
	indexgroup_.swap(newindexgroup);
}

void AutoGroupWithFree::CreateRealGroup() {
	for (std::map<int, std::vector<int> >::iterator iter = indexgroup_.begin();
			iter != indexgroup_.end(); ++iter) {
		for (std::vector<int>::iterator it = iter->second.begin();
				it != iter->second.end(); ++it) {
			group_members_[iter->first].push_back(friendlist_.at(*it));
		}
	}
}

GroupStats AutoGroupWithFree::GetGroupStats() {
	return grouped_stats_;
}

void AutoGroupWithFree::GetGroupResult(Items& result) {

}

}
}
