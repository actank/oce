#include "socialgraph/recommendbyinfo/cpp/recommendbyinfo.h"

#include <fstream>
#include <sstream>
#include <algorithm>

#include "socialgraph/socialgraphutil/algostuff.h"
#include "socialgraph/socialgraphutil/clogging.h"
#include "socialgraph/recommendbyinfo/cpp/threadcontroller.h"
#include "socialgraph/recommendbyinfo/cpp/dbhelper.h"
#include "util/cpp/MD5Util.h"
#include "DbCxxPool/src/QueryRunner.h"
#include "OceCxxAdapter/src/PhotoStateFilterAdapter.h"
#include "OceCxxAdapter/src/StatusCacheAdapter.h"

namespace xce {
namespace socialgraph {

Md2List RecommendByInfo::univ_info_map = Md2List();
Md2List RecommendByInfo::highschool_info_map = Md2List();
Md2List RecommendByInfo::juniorschool_info_map = Md2List();
Md2List RecommendByInfo::elementary_info_map = Md2List();
Int2IntMap RecommendByInfo::view_count_map = Int2IntMap();

//-----------------------------------------------------------------

void RecommendByInfo::Detail() {
	using namespace com::xiaonei::xce;
	ConnectionPoolManager::instance().initialize();		//初始化db线程池

	CLOG(INFO) << "Begin to Recommend by info load_file_:" << load_file_;
	//初始化静态成员
	Initialize();
	CLOG(INFO) << "view_count_map.size : " << RecommendByInfo::view_count_map.size();
	CLOG(INFO) << "univ_info_map.size : " << RecommendByInfo::univ_info_map.size();
	CLOG(INFO) << "highschool_info_map.size : " << RecommendByInfo::highschool_info_map.size();
	CLOG(INFO) << "juniorschool_info_map.size : " << RecommendByInfo::juniorschool_info_map.size();
	CLOG(INFO) << "elementary_info_map.size : " << RecommendByInfo::elementary_info_map.size();

	//获取计算的id列表
	std::vector<int> userids;
	GetLoginIds(load_file_, userids);
	CLOG(INFO) << "userids size:" << userids.size();
	//for (std::vector<int>::iterator iter = userids.begin();
	//		iter != userids.end(); ++iter) {
	//	CLOG(INFO) << "userid:" << *iter;
	//}

	//异步加载用户数据
	MyUtil::TaskManager::instance().config(LEVEL_UPDATE_RECOMMEND, MyUtil::ThreadPoolConfig(0, 5));

	size_t task_size = 0;
	int task_count = 0;
	for (std::vector<int>::iterator iter = userids.begin();
			iter != userids.end(); ++iter) {
		if (++task_count % 1000 == 0) {
			CLOG(DEBUG) << "RecommendByInfo task_count:" << task_count;
		}	
		MyUtil::TaskManager::instance().execute(new UpdateRecommendDataTask(*iter));

		while ((task_size = MyUtil::TaskManager::instance().size(LEVEL_UPDATE_RECOMMEND)) > 5000) {
			sleep(2);
		}
	}
	sleep(5);
	while (MyUtil::TaskManager::instance().size(LEVEL_UPDATE_RECOMMEND) > 0) {
		sleep(2);
	}
}

void RecommendByInfo::Initialize() {
	std::vector<IceUtil::ThreadControl> thread_ctrl;
	
	////加载用户viewcount信息 ---暂时不加入viewcount排序策略
	//IceUtil::Handle<ViewCountThread>	viewcount_thread_ptr = new ViewCountThread();
	//thread_ctrl.push_back(viewcount_thread_ptr->start());

	//加载大学信息
	IceUtil::Handle<LoadUnivThread>	univ_thread_ptr = new LoadUnivThread();
	thread_ctrl.push_back(univ_thread_ptr->start());

	//加载高中信息
	IceUtil::Handle<LoadHighSchoolThread> highschool_thread_ptr = new LoadHighSchoolThread();
	thread_ctrl.push_back(highschool_thread_ptr->start());

	//加载初中信息
	IceUtil::Handle<LoadJuniorSchoolThread> juniorschool_thread_ptr = new LoadJuniorSchoolThread();
	thread_ctrl.push_back(juniorschool_thread_ptr->start());

	//加载小学信息
	IceUtil::Handle<LoadElementarySchoolThread> elementaryschool_thread_ptr = new LoadElementarySchoolThread();
	thread_ctrl.push_back(elementaryschool_thread_ptr->start());
	
	for (std::vector<IceUtil::ThreadControl>::iterator iter = thread_ctrl.begin();
			iter != thread_ctrl.end(); ++iter) {
		iter->join();
	}
}

void RecommendByInfo::GetLoginIds(const std::string& file, std::vector<int>& userids) {
  CLOG(INFO) << "file:" << file << "  userids size:" << userids.size();
  std::fstream file_stream;
  file_stream.open(file.c_str(), std::ios::in);
  assert(file_stream);    //判断文件是否存在，不存在就不做

  std::ifstream infile(file.c_str(), std::ifstream::in);
  std::string line = "";
  while (getline(infile, line)) {
    int id = 0;
    std::istringstream stream(line);
    stream >> id;

    if (0 != id) {
      userids.push_back(id);
    }
  }
}

//-----------------------------------------------------------------

void UpdateRecommendDataTask::handle() {
	if (IfCalculate(userid_)) {
		std::vector<int> friends;
		GetBlockList(userid_, friends);

		CLOG(INFO) << "UpdateRecommendDataTask calculate userid_:" << userid_ << " friends_size:" << friends.size();

		InsertUnivData(friends);
		InsertHighschoolData(friends);
		InsertJuniorschoolData(friends);
		InsertElementaryschoolData(friends);
	} else {
		CLOG(INFO) << "UpdateRecommendDataTask delay userid_:" << userid_;
	}
}

void UpdateRecommendDataTask::InsertUnivData(const std::vector<int>& friends) {
	InfoList univ_info;
	DbHelper::GetUnivInfo(userid_, univ_info);		//Get Univ info from db
	//for (InfoList::iterator iter = univ_info.begin();
  //    iter != univ_info.end(); ++iter) {
  //  std::ostringstream oss;
  //  for (std::vector<std::string>::iterator it = iter->begin();
  //      it != iter->end(); ++it) {
  //    oss << *it << " ";
  //  }
  //  CLOG(INFO) << oss.str();
  //}
	DbHelper::DeleteRecommendByInfoDb(userid_, UNIV_TYPE);		//Delete recommend item of UNIV_TYPE

	int index = 0;
	for (InfoList::iterator iter = univ_info.begin();
			iter != univ_info.end(); ++iter) {
		long key = MyUtil::MD5Util::stringSeqToMD5(*iter);
		Md2List::iterator fit = RecommendByInfo::univ_info_map.find(key);
		if (fit != RecommendByInfo::univ_info_map.end()) {
			std::vector<int> univ_friends = fit->second;
			ResizeRecommends(univ_friends);		//限制个数

			std::set<int> recommend_set;
			ConvertToSet(univ_friends, recommend_set);

			FilterByFriends(friends, recommend_set);
			std::vector<int> recommend_list;
			ConvertToVector(recommend_set, recommend_list);

			FilterByPhoto(recommend_list);		//过滤头像
			FilterByStatus(recommend_list);		//过滤用户的状态，过滤封禁用户
			//xce::PRINT_ELEMENTS(recommend_list);
			FilterByUserAge(recommend_list);		//考虑大学有研究生和本科的情况，按用户年龄过滤推荐列表
			//xce::PRINT_ELEMENTS(recommend_list);
			std::string school_name = iter->at(0);		//get school name
			//CLOG(INFO) << "school_name : " << school_name;
			if (!recommend_list.empty()) {
				DbHelper::InsertRecommendByInfoDbByStage(userid_, UNIV_TYPE, index++, school_name, recommend_list);		//可能空字段会推出推荐好友
			}
			//std::vector<RecommendItem> recommends;
			//DbHelper::GetRecommendFromInfoDbByStage(userid_, UNIV_TYPE, recommends);
			//CLOG(INFO) << "recommends size:" << recommends.size();
			//for (std::vector<RecommendItem>::iterator iter = recommends.begin();
			//		iter != recommends.end(); ++iter) {
			//	CLOG(INFO) << "name:" << iter->school_name_;
			//	xce::PRINT_ELEMENTS(iter->recommend_list_);
			//}
		}
	}
}

void UpdateRecommendDataTask::InsertHighschoolData(const std::vector<int>& friends) {
	InfoList highschool_info;
	DbHelper::GetHighschoolInfo(userid_, highschool_info);		//Get High school info
	DbHelper::DeleteRecommendByInfoDb(userid_, HIGHSCHOOL_TYPE);		//Delete recommend item of HIGHSCHOOL_TYPE

	int index = 0;
	for (InfoList::iterator iter = highschool_info.begin();
			iter != highschool_info.end(); ++iter) {
		long key = MyUtil::MD5Util::stringSeqToMD5(*iter);
		Md2List::iterator fit = RecommendByInfo::highschool_info_map.find(key);
		if (fit != RecommendByInfo::highschool_info_map.end()) {
			std::vector<int> highschool_friends = fit->second;
			ResizeRecommends(highschool_friends);		//限制个数

			std::set<int> recommend_set;
			ConvertToSet(highschool_friends, recommend_set);

			FilterByFriends(friends, recommend_set);
			std::vector<int> recommend_list;
			ConvertToVector(recommend_set, recommend_list);

			FilterByPhoto(recommend_list);		//过滤头像
			FilterByStatus(recommend_list);		//过滤用户的状态，过滤封禁用户

			std::string school_name = iter->at(0);		//school_name
			if (!recommend_list.empty()) {
				DbHelper::InsertRecommendByInfoDbByStage(userid_, HIGHSCHOOL_TYPE, index++, school_name, recommend_list);
			}
		}
	}
}

void UpdateRecommendDataTask::InsertJuniorschoolData(const std::vector<int>& friends) {
	InfoList juniorschool_info;
	DbHelper::GetJuniorschoolInfo(userid_, juniorschool_info);
	DbHelper::DeleteRecommendByInfoDb(userid_, JUNIORSCHOOL_TYPE);		//Delete recommend item of JUNIORSCHOOL_TYPE 

	int index = 0;
	for (InfoList::iterator iter = juniorschool_info.begin();
			iter != juniorschool_info.end(); ++iter) {
		long key = MyUtil::MD5Util::stringSeqToMD5(*iter);
		Md2List::iterator fit = RecommendByInfo::juniorschool_info_map.find(key);
		if (fit != RecommendByInfo::juniorschool_info_map.end()) {
			std::vector<int> juniorschool_friends = fit->second;
			ResizeRecommends(juniorschool_friends);		//限制个数

			std::set<int> recommend_set;
			ConvertToSet(juniorschool_friends, recommend_set);

			FilterByFriends(friends, recommend_set);
			std::vector<int> recommend_list;
			ConvertToVector(recommend_set, recommend_list);

			FilterByPhoto(recommend_list);		//过滤头像
			FilterByStatus(recommend_list);		//过滤用户的状态，过滤封禁用户

			std::string school_name = iter->at(0); 		//Get school name
			if (!recommend_list.empty()) {
				DbHelper::InsertRecommendByInfoDbByStage(userid_, JUNIORSCHOOL_TYPE, index++, school_name, recommend_list);
			}
		}
	}
}

void UpdateRecommendDataTask::InsertElementaryschoolData(const std::vector<int>& friends) {
	InfoList elementaryschool_info;
	DbHelper::GetElementaryschoolInfo(userid_, elementaryschool_info);
	DbHelper::DeleteRecommendByInfoDb(userid_, ELEMENTARYSCHOOL_TYPE);		//Delete recommend item of ELEMENTARYSCHOOL_TYPE 

	int index = 0;
	for (InfoList::iterator iter = elementaryschool_info.begin();
			iter != elementaryschool_info.end(); ++iter) {
		long key = MyUtil::MD5Util::stringSeqToMD5(*iter);
		Md2List::iterator fit = RecommendByInfo::elementary_info_map.find(key);
		if (fit != RecommendByInfo::elementary_info_map.end()) {
			std::vector<int> elementaryschool_friends = fit->second;
			ResizeRecommends(elementaryschool_friends);		//限制个数

			std::set<int> recommend_set;
			ConvertToSet(elementaryschool_friends, recommend_set);

			FilterByFriends(friends, recommend_set);
			std::vector<int> recommend_list;
			ConvertToVector(recommend_set, recommend_list);

			FilterByPhoto(recommend_list);		//过滤头像
			FilterByStatus(recommend_list);		//过滤用户的状态，过滤封禁用户
	
			std::string school_name = iter->at(0);		//Get school name
			if (!recommend_list.empty()) {
				DbHelper::InsertRecommendByInfoDbByStage(userid_, ELEMENTARYSCHOOL_TYPE, index++, school_name, recommend_list);
			}
		}
	}
}

void UpdateRecommendDataTask::ConvertToSet(const std::vector<int>& source, std::set<int>& dest) {
	for (std::vector<int>::const_iterator iter = source.begin();
			iter != source.end(); ++iter) {
		dest.insert(*iter);
	}
}

void UpdateRecommendDataTask::ConvertToVector(const std::set<int>& source, std::vector<int>& dest) {
	for (std::set<int>::const_iterator iter = source.begin();
			iter != source.end(); ++iter) {
		dest.push_back(*iter);
	}
}

void UpdateRecommendDataTask::FilterByFriends(const std::vector<int>& friends, std::set<int>& dest) {
	for (std::vector<int>::const_iterator iter = friends.begin();
			iter != friends.end(); ++iter) {
		std::set<int>::iterator fit = dest.find(*iter);
		if (fit != dest.end()) {
			dest.erase(*iter);
		}
	}
}

void UpdateRecommendDataTask::FilterByPhoto(std::vector<int>& recommends) {
	using namespace xce::photostatefilter::adapter;
	
	try {
		std::map<int, bool> state_map = PhotoStateFilterAdapter::instance().getPhotoStateBatch(recommends);
		std::vector<int> tmp_recommends;
		for (std::map<int, bool>::iterator iter = state_map.begin();
				iter != state_map.end(); ++iter) {
			if (true == iter->second) {
				tmp_recommends.push_back(iter->first);
			}	
		}
		recommends.swap(tmp_recommends);
	} catch (Ice::Exception& e) {
		CLOG(WARN) << "UpdateRecommendDataTask::FilterByPhoto " << e.what();
	} catch (...) {
		CLOG(WARN) << "UpdateRecommendDataTask::FilterByPhoto unknown exception";
	}
}

void UpdateRecommendDataTask::FilterByStatus(std::vector<int>& recommends) {
	using namespace xce::statuscache;
	try {
		std::map<int, bool> state_map = StatusCacheAdapter::instance().getStatusBatch(recommends);
		std::vector<int> tmp_recommends;
		for (std::map<int, bool>::iterator iter = state_map.begin();
				iter != state_map.end(); ++iter) {
			if (false == iter->second) {
				tmp_recommends.push_back(iter->first);
			}	
		}
		recommends.swap(tmp_recommends);
	} catch (Ice::Exception& e) {
		CLOG(WARN) << "UpdateRecommendDataTask::FilterByStatus " << e.what();
	} catch (...) {
		CLOG(WARN) << "UpdateRecommendDataTask::FilterByStatus unknown exception";
	}
}

void UpdateRecommendDataTask::FilterByUserAge(std::vector<int>& recommends) {
	std::vector<int> total_ids(recommends);
	total_ids.push_back(userid_);

	std::map<int, int> id2age;
	DbHelper::GetBatchAgeInfoDb(total_ids, id2age);

	int user_birthyear = 0;
	std::map<int, int>::iterator fit = id2age.find(userid_);
	if (fit == id2age.end()) {
		return;
	} else {
		user_birthyear = fit->second;
	}
	
	std::vector<int> tmp_recommends;
	for (std::vector<int>::iterator iter = recommends.begin();
			iter != recommends.end(); ++iter) {
		std::map<int, int>::iterator it = id2age.find(*iter);
		if (it != id2age.end()) {
			if (abs(user_birthyear - it->second) < AGE_INTERVAL) {
				tmp_recommends.push_back(*iter);
			}
		}
	}

	tmp_recommends.swap(recommends);
}

void UpdateRecommendDataTask::ResizeRecommends(std::vector<int>& recommends) {
	if ((int)recommends.size() <= RECOMMEND_DB_SIZE) {
		return;
	}
	
	std::random_shuffle(recommends.begin(), recommends.end());
	recommends.resize(RECOMMEND_DB_SIZE);
}

bool UpdateRecommendDataTask::IfCalculate(int userid) {
	long current_time = time(NULL);
	long create_time = DbHelper::GetCreateDataTimeFromNewInfoDb(userid);
	if (current_time - create_time < REBUILD_INTERVAL) {
		return false;
	}

	return true;
}

void UpdateRecommendDataTask::GetBlockList(int userid, std::vector<int>& block_list) {
	DbHelper::ReadFriendList(userid_, block_list);	//add friends list

	std::vector<int> apply_list;
	DbHelper::ReadApplyList(userid_, apply_list);		
	block_list.insert(block_list.end(), apply_list.begin(), apply_list.end());		//add apply list

	block_list.push_back(userid_);		//过滤自己
}
	
void UpdateRecommendDataTask::PrintSet(const std::set<int>& idset) const {
	CLOG(DEBUG) << "UpdateRecommendDataTask::PrintSet idset.size:" << idset.size();
	std::ostringstream oss;
	for (std::set<int>::const_iterator iter = idset.begin();
			iter != idset.end(); ++iter) {
		oss << *iter << " ";
	}
	CLOG(DEBUG) << oss.str();
}

void UpdateRecommendDataTask::PrintVec(const std::vector<int>& idvec) const {
	CLOG(DEBUG) << "UpdateRecommendDataTask::PrintVec idvec.size:" << idvec.size();
	std::ostringstream oss;
	for (std::vector<int>::const_iterator iter = idvec.begin();
			iter != idvec.end(); ++iter) {
		oss << *iter << " ";
	}
	CLOG(DEBUG) << oss.str();
}

}
}
