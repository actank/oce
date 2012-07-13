#include "MyTask.h"

using namespace xce::task;
using namespace item;
using namespace xce::socialgraph;
using namespace xce::dbhelper;

//-----------------------------------------------------------------------------------------------------------

void LoadHotFeedRecommendTask::handle() {
	HotFeedSeq feed_result_list;
	int userid = user_model_.GetUserId();
	vector<int> recommend_list = user_model_.GetRecommedList();

	for (vector<int>::iterator iter = recommend_list.begin();
			iter != recommend_list.end(); ++iter) {
		if ((int)feed_result_list.size() >= 5) {
			break;
		}

		UGCItem ugc_unit = GetMostWeigheUGC(userid, *iter);

		if (ugc_unit.getUgcId() == 0 || ugc_unit.getUGCType() == 0) {
			continue;
		}

		HotFeed feed;
		feed.userId = *iter;
		feed.feedId = ugc_unit.getUgcId();
		feed.feedType = ugc_unit.getUGCType();

		feed_result_list.push_back(feed);
	}

	cout << "create " << userid << " : " << feed_result_list.size() << endl; 

	try {
		HotFeedRecommendCacheAdapter::instance().Set(userid, feed_result_list);
	} catch (Ice::Exception& e) {
		cout << "[LoadHotFeedRecommendTask] HotFeedRecommendCacheAdapter Ice Exception " << e.what() << endl;
	} catch (std::exception& e) {
		cout << "[LoadHotFeedRecommendTask] HotFeedRecommendCacheAdapter std Exception " << e.what() << endl;
	} catch (...) {
		cout << "[LoadHotFeedRecommendTask] HotFeedRecommendCacheAdapter unknown Exception " << endl;
	}	
}

item::UGCItem LoadHotFeedRecommendTask::GetMostWeigheUGC(int hostid, int friendid) {
	UGCItem result(0, 0, 0, 0);	

	vector<UGCItem> sort_list;
	vector<long> block_ugc_list;

	HotFeedUpdateDBHelper::GetUserBlockUGC(hostid, friendid, block_ugc_list);

	sort(block_ugc_list.begin(), block_ugc_list.end());

	vector<RawUgcModel> ugc_list = g_ugc_manager->GetUgcList(friendid);

	for (vector<RawUgcModel>::iterator iter = ugc_list.begin();
			iter != ugc_list.end(); ++iter) {
		vector<long>::iterator fit = lower_bound(block_ugc_list.begin(), block_ugc_list.end(), iter->ugcid_);
		if (fit != block_ugc_list.end() && *fit == iter->ugcid_) {    //过滤block列表
			continue;
		}

		if (user_model_.IsVisit(iter->ugcid_)) {             //不能是用户以前看过的ugc
			continue;
		}	

		UGCItem item(iter->ugcid_, iter->comment_count_, iter->view_count_, iter->type_);		

		if ("NULL" != iter->interest_) {     //加入兴趣度衡量
			if (user_model_.IsInterested(iter->interest_)) {
				item.setInterest(1000);
			}
		}

		sort_list.push_back(item);
	}

	sort(sort_list.begin(), sort_list.end(), UgcComp());

	if (sort_list.empty()) {
		return UGCItem(0, 0, 0, 0);
	}

	return sort_list.front();
}
