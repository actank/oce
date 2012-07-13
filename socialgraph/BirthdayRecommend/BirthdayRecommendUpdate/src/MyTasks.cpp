#include "MyTasks.h"
using namespace std;
//using namespace xce::task;
using namespace xce::socialgraph;

void ReadFriendTask::handle() {
	dbhelper->ReadFriendList(_userId, _friends);
}

//--------------------------------------------------------------------------------------------

void FliterBlockTask::handle() {
	dbhelper->FliterRecommendBlock(_userId, _birthdayRecommendList);
}

//--------------------------------------------------------------------------------------------

void SetCacheAndSortTask::handle() {
	//if (_userId % 10 != 2) {      //加载尾号为2的用户
	//	return;
	//}	

	if (_birthdayRecommendList.empty()) {
		return;
	}

	if ((int)_birthdayRecommendList.size() > 1) {
		map<int, int> friendRank;
		for (vector<int>::iterator iter = _birthdayRecommendList.begin();
				iter != _birthdayRecommendList.end(); ++iter) {
			friendRank.insert(make_pair<int, int>(*iter, 1));
		}

		//if (_userId == 1894) {
		//	friendRank[666] = 56;
		//	friendRank[888] = 100;
		//	friendRank[2171] = 9;
		//}

		RankSeq vecRank;
		try {
			vecRank = FriendRankCacheAdapter::instance().GetRank(_userId);
		} catch (Ice::Exception& e) {
			cout << "[SetCacheAndSortTask] FriendRankCacheAdapter Ice Exception " << e.what() << endl;
		} catch (std::exception& e) {
			cout << "[SetCacheAndSortTask] FriendRankCacheAdapter std Exception " << e.what() << endl;
		} catch (...) {
			cout << "[SetCacheAndSortTask] FriendRankCacheAdapter unknown Exception " << endl;
		}

		for (RankSeq::const_iterator iter = vecRank.begin();
				iter != vecRank.end(); ++iter) {
			if (friendRank.find(iter->userid) != friendRank.end() && iter->weight > 0) {
				friendRank[iter->userid] = iter->weight;
			}
		}
	
		vector< pair<int, int> > recommendResult; 
		for (map<int, int>::iterator iter = friendRank.begin();
				iter != friendRank.end(); ++iter) {
			recommendResult.push_back(make_pair<int, int>(iter->first, iter->second));	
		}

		sort(recommendResult.begin(), recommendResult.end(), RankComp());

		//for (vector< pair<int, int> >::iterator iter = recommendResult.begin();
		//		iter != recommendResult.end(); ++iter) {
		//	cout << iter->first << " : " << iter->second << endl;
		//}
		//cout << endl;

		_birthdayRecommendList.clear();	
		for (vector< pair<int, int> >::iterator iter = recommendResult.begin();
				iter != recommendResult.end(); ++iter) {
			_birthdayRecommendList.push_back(iter->first);
		}

		//for (vector<int>::iterator iter = _birthdayRecommendList.begin();
		//		iter != _birthdayRecommendList.end(); ++iter) {
		//	cout << *iter << "  ";
		//}
		//cout << endl;
	}

	try {
		BirthdayRecommendCacheAdapter::instance().Set(_userId, _birthdayRecommendList);
	} catch (Ice::Exception& e) {
		cout << "[SetCacheAndSortTask] BirthdayRecommendCacheAdapter Ice Exception " << e.what() << endl;
	} catch (std::exception& e) {
		cout << "[SetCacheAndSortTask] BirthdayRecommendCacheAdapter std Exception " << e.what() << endl;
	} catch (...) {
		cout << "[SetCacheAndSortTask] BirthdayRecommendCacheAdapter unknown Exception" << endl;
	}
}
