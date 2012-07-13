#include "socialgraph/autogroup/algorithm/method/impl/autogroupwithlabel.h"
#include <set>

#include <boost/foreach.hpp>

#include "base/logging.h"

namespace xce {
namespace socialgraph {
namespace autogroupwithlabel {

void AutoGroupWithLabel::CreateGroup() {
	//std::cout << "[AutoGroupWithLabel] Grouping userid(" << userid_ << ")" << std::endl;
	//PTIME(pt, "PTIME CreateGroup", true, 0);

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


void AutoGroupWithLabel::CreateStats() {
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

void AutoGroupWithLabel::Display() {
	std::cout << std::endl;
	std::cout << "[AutoGroupWithLabel] Display" << std::endl;
	for (std::map<int, std::vector<int> >::iterator iter = group_members_.begin();
			iter != group_members_.end(); ++iter) {
		std::map<int, GroupLabel>::iterator label_fit = group_label_.find(iter->first);

		std::cout << iter->first << ":"; 
		if (label_fit != group_label_.end()) {
			for (std::vector<std::string>::iterator it = label_fit->second.label_list.begin();
					it != label_fit->second.label_list.end(); ++it) {
				std::cout << *it << " ";
			}
		}
		std::cout << std::endl;
		for (std::vector<int>::iterator it = iter->second.begin();
				it != iter->second.end(); ++it) {
			xce::adapter::username::UserNameInfoPtr nameptr = xce::adapter::username::UserNameAdapter::instance().getUserName(*it);
			std::cout << nameptr->name() << " ";
		}
		std::cout << std::endl;
		std::cout << std::endl;
	}
}

void AutoGroupWithLabel::CreateClique() {
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

void AutoGroupWithLabel::SortMembersOfIndexGroup() {
	for (std::map<int, std::vector<int> >::iterator iter = indexgroup_.begin();
			iter != indexgroup_.end(); ++iter) {
		std::sort(iter->second.begin(), iter->second.end());	
	}
}

void AutoGroupWithLabel::AddFriendsIntoGroup() {
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

void AutoGroupWithLabel::DeleteMembersFromGroup(Strategy strategy, const std::map<int, std::vector<int> >& backupmap) {
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

void AutoGroupWithLabel::UniteSimilarGroup() {
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

void AutoGroupWithLabel::RefineIndexGroup() {
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

void AutoGroupWithLabel::CreateRealGroup() {
	for (std::map<int, std::vector<int> >::iterator iter = indexgroup_.begin();
			iter != indexgroup_.end(); ++iter) {
		for (std::vector<int>::iterator it = iter->second.begin();
				it != iter->second.end(); ++it) {
			group_members_[iter->first].push_back(friendlist_.at(*it));
		}
	}
}

GroupStats AutoGroupWithLabel::GetGroupStats() {
	return grouped_stats_;
}

void AutoGroupWithLabel::GetGroupResult(Items& result) {
	for (std::map<int, GroupLabel>::iterator iter = group_label_.begin();
			iter != group_label_.end(); ++iter) {
		Item* itemptr = result.add_items();
		itemptr->set_id(iter->first);
		
		for (std::vector<std::string>::iterator it = iter->second.label_list.begin();
				it != iter->second.label_list.end(); ++it) {
			itemptr->add_messages(*it);
		}
		
		std::map<int, std::vector<int> >::iterator mfit = group_members_.find(iter->first);
		if (mfit != group_members_.end()) {
			for (std::vector<int>::iterator it = mfit->second.begin();
					it != mfit->second.end(); ++it) {
				itemptr->add_fields(*it);
			}
		}
	}

	if (0 < result.items_size()) {
		result.set_timestamp(time(NULL));
	}
}

void AutoGroupWithLabel::CreateGroupTags() {
	//LOG(INFO) << "[AutoGroupWithLabel] CreateGroupTags";
	//PTIME(pt, "PTIME AutoGroupWithLabel CreateGroupTags", true, 0);
	//float tags_rate = 0.34;
	float tags_rate = 0.2;		//当该组的最适合tags rate大于该值时，认为该tags可取
	
	GetFriendsNetwork();		//获取好友的资料信息
	
	//PTIME_CHECK(pt, "PTIME AutoGroupWithLabel CreateGroupTags GetFriendsNetwork check");

	std::vector<GroupLabel> label_list;
	for (std::map<int, std::vector<int> >::iterator iter = group_members_.begin();
			iter != group_members_.end(); ++iter) {
		label_list.push_back(GetGroupLabel(UniversityType, iter->second));
		label_list.push_back(GetGroupLabel(HighSchoolType, iter->second));
		label_list.push_back(GetGroupLabel(JuniorSchoolType, iter->second));
		label_list.push_back(GetGroupLabel(ElementarySchoolType, iter->second));
		label_list.push_back(GetGroupLabel(CollegeType, iter->second));
	
		std::sort(label_list.begin(), label_list.end(), GroupLabelComp());
		//LOG(INFO) << "label_list size : " << label_list.size();
		if (!label_list.empty() && label_list.at(0).rate > tags_rate) {
			group_label_[iter->first] = label_list.at(0);
			//LOG(INFO) << label_list.at(0).num << ":" << label_list.at(0).rate << ":" << label_list.at(0).type;
			//for (std::vector<std::string>::iterator it = label_list.at(0).label_list.begin();
			//		it != label_list.at(0).label_list.end(); ++it) {
			//	LOG(INFO) << *it;
			//}
		} else {
			group_label_[iter->first] = GroupLabel();
		}
		label_list.clear();
	}
	//PTIME_CHECK(pt, "PTIME AutoGroupWithLabel CreateGroupTags create group_label check");

	MergeSameLabel();
}

void AutoGroupWithLabel::GetFriendsNetwork() {
	//LOG(INFO) << "[AutoGroupWithLabel] GetFriendsNetwork friendlist_ size:" << friendlist_.size();
	//PTIME(pt, "PTIME GetFriendsNetwork", true, 0);
	using namespace com::xiaonei::xce;
	ConnectionPoolManager::instance().initialize();		//for main test

	std::vector<std::string> terms_list;

	terms_list.push_back("userid"); terms_list.push_back("university_name"); 
	terms_list.push_back("department"); terms_list.push_back("enroll_year");
	GetFriendsInfo(terms_list, "university_info", UniversityType);	//获取大学资料
	//PTIME_CHECK(pt, "PTIME GetFriendsNetwork get university");

	terms_list.clear();
	terms_list.push_back("userid"); terms_list.push_back("high_school_name"); terms_list.push_back("enroll_year");
	GetFriendsInfo(terms_list, "high_school_info", HighSchoolType);		//获取高中资料
	//PTIME_CHECK(pt, "PTIME GetFriendsNetwork get highschool");

	terms_list.clear();
	terms_list.push_back("userid"); terms_list.push_back("junior_high_school_name"); terms_list.push_back("junior_high_school_year");
	GetFriendsInfo(terms_list, "junior_high_school_info", JuniorSchoolType); 	//获取初中资料
	//PTIME_CHECK(pt, "PTIME GetFriendsNetwork get junior_highschool");

	terms_list.clear();
	terms_list.push_back("userid"); terms_list.push_back("elementary_school_name"); terms_list.push_back("elementary_school_year");
	GetFriendsInfo(terms_list, "elementary_school_info", ElementarySchoolType); 	//获取小学资料
	//PTIME_CHECK(pt, "PTIME GetFriendsNetwork get elementaryschool");

	terms_list.clear();
	terms_list.push_back("userid"); terms_list.push_back("college_name"); terms_list.push_back("enroll_year");
	GetFriendsInfo(terms_list, "college_info", CollegeType);		//获取中专资料
	//PTIME_CHECK(pt, "PTIME GetFriendsNetwork get college");
	//ShowUserNetwork();
	//LOG(INFO) << "user_network_map_ size : " << user_network_map_.size();
}

void AutoGroupWithLabel::ShowUserNetwork() {
	for (std::map<int, UserNetWork>::iterator iter = user_network_map_.begin();
			iter != user_network_map_.end(); ++iter) {
		for (UnivInfoList::iterator it = iter->second.univ_list_.begin();
				it != iter->second.univ_list_.end(); ++it) {
			LOG(INFO) << iter->first << ":" << it->name << ":" << it->academy << ":" << it->enroll_year;
		}	
		for (HighSchoolList::iterator it = iter->second.highschool_list_.begin();
				it != iter->second.highschool_list_.end(); ++it) {
			LOG(INFO) << iter->first << ":" << it->name << ":" << it->enroll_year;
		}
		for (JuniorSchoolList::iterator it = iter->second.juniorschool_list_.begin();
				it != iter->second.juniorschool_list_.end(); ++it) {
			LOG(INFO)	<< iter->first << ":" << it->name << ":" << it->enroll_year;
		}
		for (ElementarySchoolList::iterator it = iter->second.elementaryschool_list_.begin();
				it != iter->second.elementaryschool_list_.end(); ++it) {
			LOG(INFO) << iter->first << ":" << it->name << ":" << it->enroll_year;
		}
		for (CollegeList::iterator it = iter->second.college_list_.begin();
				it != iter->second.college_list_.end(); ++it) {
			LOG(INFO) << iter->first << ":" << it->name << ":" << it->enroll_year;
		}
	}
}

void AutoGroupWithLabel::GetFriendsInfo(const std::vector<std::string> terms, const std::string table, LabelType type) {
	using namespace com::xiaonei::xce;

	if (terms.empty()) {
		return;
	} 
	std::string sql;
	sql = "SELECT ";
	BOOST_FOREACH(const std::string& term, terms) {		//拼接要获取的数据项
		sql += term + ", ";
	}
	
	if (boost::ends_with(sql, ", ")) {		//过滤末尾的", "
		boost::erase_last(sql, ", ");
	} 
	sql += " FROM " + table + " WHERE userid in (";
	BOOST_FOREACH(int fid, friendlist_) {				//拼接要获取的好友id的数据
		sql += boost::lexical_cast<std::string>(fid) + ", ";
		//sql += boost::lexical_cast<std::string>(1331) + ", "; //for test
		//break;			//for test
	}

	if (boost::ends_with(sql, ", ")) {
		boost::erase_last(sql, ", ");
	} else if (boost::ends_with(sql, "(")) {
		return;
	}
	sql += ")";
	//LOG(INFO) << sql;

	Statement exe_sql;
	exe_sql << sql;

	try {
		if (JuniorSchoolType == type) {
			//初中信息的enroll_year字段为junior_high_school_year
			QueryRunner("search2_userbasic", CDbRServer).query(exe_sql, InfoHandler(type, user_network_map_, "junior_high_school_year"));
		} else if (ElementarySchoolType == type) {
			//小学信息的enroll_year字段为elementary_school_year
			QueryRunner("search2_userbasic", CDbRServer).query(exe_sql, InfoHandler(type, user_network_map_, "elementary_school_year"));
		} else {
			//使用默认的enroll_year选项
			QueryRunner("search2_userbasic", CDbRServer).query(exe_sql, InfoHandler(type, user_network_map_));
		}
	} catch (Ice::Exception& e) {
		LOG(INFO) << "[AutoGroupWithLabel] GetFriendsInfo Ice Exception : " << e.what();
	} catch (...) {
		LOG(INFO) << "[AutoGroupWithLabel] GetFriendsInfo Unknown Exception";
	}
}

GroupLabel AutoGroupWithLabel::GetGroupLabel(LabelType type, const std::vector<int>& friendlist) {
	//LOG(INFO) << "[AutoGroupWithLabel] GetGroupLabel friendlist size:" << friendlist.size();
	UserNetWorkSeq networkseq;
	for (std::vector<int>::const_iterator iter = friendlist.begin();
			iter != friendlist.end(); ++iter) {
		std::map<int, UserNetWork>::iterator fit = user_network_map_.find(*iter);
		if (fit != user_network_map_.end()) {
			networkseq.push_back(fit->second);
		}
	}
	int friends_num = (int)friendlist.size();

	GroupLabel result;
	switch (type) {
		case UniversityType:
			{
				UnivInfoList univ_list;
				BOOST_FOREACH(const UserNetWork& network, networkseq) {
					univ_list.insert(univ_list.end(), network.univ_list_.begin(), network.univ_list_.end());
				}
				std::pair<University, int> info = FetchBestItem(univ_list);
				result.label_list.push_back(info.first.name);
				result.label_list.push_back(info.first.academy);
				result.label_list.push_back(boost::lexical_cast<std::string>(info.first.enroll_year));
				result.num = info.second;
				result.rate = (float)info.second / friends_num;
				result.type = UniversityType;
				break;
			}
		case HighSchoolType:
			{
				HighSchoolList highschool_list;
				BOOST_FOREACH(const UserNetWork& network, networkseq) {
					highschool_list.insert(highschool_list.end(), network.highschool_list_.begin(), network.highschool_list_.end());
				}
				std::pair<HighSchool, int> info = FetchBestItem(highschool_list);
				result.label_list.push_back(info.first.name);
				result.label_list.push_back(boost::lexical_cast<std::string>(info.first.enroll_year));
				result.num = info.second;
				result.rate = (float)info.second / friends_num;
				result.type = HighSchoolType;
				break;
			}
		case JuniorSchoolType:
			{
				JuniorSchoolList juniorschool_list;
				BOOST_FOREACH(const UserNetWork& network, networkseq) {
					juniorschool_list.insert(juniorschool_list.end(), network.juniorschool_list_.begin(), network.juniorschool_list_.end());
				}
				std::pair<JuniorSchool, int> info = FetchBestItem(juniorschool_list);
				result.label_list.push_back(info.first.name);
				result.label_list.push_back(boost::lexical_cast<std::string>(info.first.enroll_year));
				result.num = info.second;
				result.rate = (float)info.second / friends_num;
				result.type = JuniorSchoolType;
				break;
			}
		case ElementarySchoolType:
			{
				ElementarySchoolList elementaryschool_list;
				BOOST_FOREACH(const UserNetWork& network, networkseq) {
					elementaryschool_list.insert(elementaryschool_list.end(), network.elementaryschool_list_.begin(), 
																																			network.elementaryschool_list_.end());	
				}	
				std::pair<ElementarySchool, int> info = FetchBestItem(elementaryschool_list);
				result.label_list.push_back(info.first.name);
				result.label_list.push_back(boost::lexical_cast<std::string>(info.first.enroll_year));
				result.num = info.second;
				result.rate = (float)info.second / friends_num;
				result.type = ElementarySchoolType;
				break;
			}
		case CollegeType:
			{
				CollegeList college_list;
				BOOST_FOREACH(const UserNetWork& network, networkseq) {
					college_list.insert(college_list.end(), network.college_list_.begin(), network.college_list_.end());
				}
				std::pair<College, int> info = FetchBestItem(college_list);
				result.label_list.push_back(info.first.name);
				result.label_list.push_back(boost::lexical_cast<std::string>(info.first.enroll_year));
				result.num = info.second;
				result.rate = (float)info.second / friends_num;
				result.type = CollegeType;
				break;
			}
	}
	return result;
}

template <typename T>
std::pair<T, int> AutoGroupWithLabel::FetchBestItem(const std::vector<T>& item_list) {
	std::pair<T, int> result = std::make_pair<T, int>(T(), 0);

	typename std::vector< T >::const_iterator fpos;		//第一个指针
	typename std::vector< T >::const_iterator spos;		//第二个指针

	for (fpos = item_list.begin(); fpos != item_list.end(); ++fpos) {
		std::pair<T, int> item_pair = std::make_pair<T, int>(*fpos, 0);
		for (spos = fpos + 1; spos != item_list.end(); ++spos) {
			if (*fpos == *spos) {
				++item_pair.second;
			}
		}
		if (item_pair.second > result.second) {
			result = item_pair;
		}
	}
	return result;
}

void AutoGroupWithLabel::MergeSameLabel() {
	//PTIME(pt, "PTIME AutoGroupWithLabel MergeSameLabel", true, 0);
	std::map<int, std::vector<int> > new_group_members;
	std::map<int, GroupLabel> new_group_label;
	bool check = true;

	while (check) {
		if (group_label_.empty()) {		//group_label为0即为跳出条件
			break;
		}

		int first_key = group_label_.begin()->first;		//每次都取map中的第一个值
		GroupLabel first_label = group_label_.begin()->second;
		std::vector<int> merge_keys;				//用来存放要合并的组号
		merge_keys.push_back(first_key);		

		for (std::map<int, GroupLabel>::iterator iter = group_label_.begin();
				iter != group_label_.end(); ++iter) {
			if (first_key == iter->first) {			//第一个已经放入vector中
				continue;
			}
		
			if (first_label.label_list == iter->second.label_list &&		//组名相同
					!first_label.label_list.empty()) {			//组名为空的不合并
				merge_keys.push_back(iter->first);
			}
		}
		std::set<int> new_members;		//新的组成员
		BOOST_FOREACH(int k, merge_keys) {			
			std::map<int, GroupLabel>::iterator gfit = group_label_.find(k);
			if (gfit != group_label_.end()) {
				group_label_.erase(gfit);			//因为要合并，所以删除group_label中对应的组
			}
			std::map<int, std::vector<int> >::iterator mfit = group_members_.find(k);
			if (mfit != group_members_.end()) {
				new_members.insert(mfit->second.begin(), mfit->second.end());
			}
		}
		for (std::set<int>::iterator iter = new_members.begin();
				iter != new_members.end(); ++iter) {
			new_group_members[first_key].push_back(*iter);		//插入新组中
		}
		new_group_label[first_key] = first_label;		//插入新组名
	}

	group_members_.swap(new_group_members);
	group_label_.swap(new_group_label);
}

//------------------------------------------------------------------------------------------

bool InfoHandler::handle(mysqlpp::Row& row) const {
	int enroll_year = (int)row[enroll_term_.c_str()];
	int userid = (int)row[userid_term_.c_str()];

	switch (type_) {
		case UniversityType:
			{
				std::string univ(row["university_name"]);
				std::string academy(row["department"]);
				boost::trim(univ);
				boost::trim(academy);
				if ("" == univ || "" == academy) break;
				//LOG(INFO) << "UniversityType userid:" << userid << " enroll_year:" 
				//					<< enroll_year << " univ:" << univ << " academy:" << academy;

				University item;
				item.name = univ;
				item.academy = academy;
				item.enroll_year = enroll_year;

				user_network_map_[userid].univ_list_.push_back(item);
				break;
			}
		case HighSchoolType:
			{
				std::string highschool(row["high_school_name"]);
				boost::trim(highschool);
				if ("" == highschool) break;
				//LOG(INFO) << "HighSchoolType userid:" << userid << " enroll_year:"
				//					<< enroll_year << " highschool:" << highschool;

				HighSchool item;
				item.name = highschool;
				item.enroll_year = enroll_year;
				
				user_network_map_[userid].highschool_list_.push_back(item);
				break;
			}

		case JuniorSchoolType:
			{
				std::string juniorschool(row["junior_high_school_name"]);
				boost::trim(juniorschool);
				if ("" == juniorschool) break;
				//LOG(INFO) << "JuniorSchoolType userid:" << userid << " enroll_year:"
				//					<< enroll_year << " juniroschool:" << juniorschool;

				JuniorSchool item;
				item.name = juniorschool;
				item.enroll_year = enroll_year;

				user_network_map_[userid].juniorschool_list_.push_back(item);
				break;
			}

		case ElementarySchoolType:
			{
				std::string elementaryschool(row["elementary_school_name"]);
				boost::trim(elementaryschool);
				if ("" == elementaryschool) break;
				//LOG(INFO) << "ElementarySchoolType userid:" << userid << " enroll_year:"
				//					<< enroll_year << " elementaryschool:" << elementaryschool;

				ElementarySchool item;
				item.name = elementaryschool;
				item.enroll_year = enroll_year;

				user_network_map_[userid].elementaryschool_list_.push_back(item);
				break;
			}

		case CollegeType:
			{
				std::string college(row["college_name"]);
				boost::trim(college);
				if ("" == college) break;
				//LOG(INFO) << "CollegeType userid:" << userid << " enroll_year:"
				//					<< enroll_year << " college:" << college;

				College item;
				item.name = college;
				item.enroll_year = enroll_year;

				user_network_map_[userid].college_list_.push_back(item);
				break;
			}
		default:
			break;
	}
	return true;
}

}
}
}
