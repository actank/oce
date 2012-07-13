#include "SortByFriendRankUtil.h"

using namespace xce::socialgraph;
using namespace sortbyrank;
using namespace MyUtil;
using namespace std;

void SortByFriendRankUtil::SortTargetByFriendRank(std::map<int, std::vector<int> >& target) {
	time_t phrase_start = 0, phrase_end = 0;
	phrase_start = time(NULL);

	TaskManager::instance().config(TASK_LEVEL_SORTBY_FRIENDRANK, ThreadPoolConfig(0, 100));
	int task_size = 0;
	long wait = 0;
	long count = 0;

	for (map<int, vector<int> >::iterator iter = target.begin();
			iter != target.end(); ++iter) {
		if (++count % 1000 == 0) {
			fprintf(stderr, "SortByFriendRankUtil %ld / %ld\r", count, target.size());
		}

		TaskManager::instance().execute(new SortByFriendRankTask(iter->first, iter->second));

		while ((task_size = TaskManager::instance().size(TASK_LEVEL_SORTBY_FRIENDRANK)) > 10000) {
			wait += 2;
			sleep(2);
			fprintf(stderr, "TASK_LEVEL_SORTBY_FRIENDRANK: sleep %ld second. left %d task.\n", wait, task_size);
		}
	}

	fprintf(stderr, "\nSortTargetByFriendRank completed.\n");

	while ((task_size = TaskManager::instance().size(TASK_LEVEL_SORTBY_FRIENDRANK)) > 10000) {
		fprintf(stderr, "final left TASK_LEVEL_SORTBY_FRIENDRANK size = %d.\n", task_size);
		sleep(2);
	}
	//sleep(10);
	phrase_end = time(NULL);
	fprintf(stderr, "[SortByFriendRankUtil] size of target = %ld, using %.1fm\n", target.size(), (phrase_end - phrase_start)/60.0);
}

//-----------------------------------------------------------------------------------

void SortByFriendRankTask::handle() {
	if (_idList.empty()) {
		return;
	}

	map<int, int> friendRank;
	for (vector<int>::iterator iter = _idList.begin();
			iter != _idList.end(); ++iter) {
		friendRank.insert(make_pair<int, int>(*iter, 1));
	}

	//for (map<int, int>::iterator iter = friendRank.begin();
	//		iter != friendRank.end(); ++iter) {
	//	cout << iter->first << " : " << iter->second << endl;
	//}
	//cout << endl;

	RankSeq vecRank;
	try {
		vecRank = FriendRankCacheAdapter::instance().GetRank(_userId);
	} catch (Ice::Exception& e) {
		cout << "[SortByFriendRankTask] FriendRankCacheAdapter Ice Exception " << e.what();
	} catch (...) {
		cout << "[SortByFriendRankTask0] FriendRankCacheAdapter Unknown Exception" << endl;
	}


	for (RankSeq::const_iterator iter = vecRank.begin();
			iter != vecRank.end(); ++iter) {
		if (friendRank.find(iter->userid) != friendRank.end() && iter->weight > 0) {
			friendRank[iter->userid] = iter->weight;
		}
	}

//	for (map<int, int>::iterator iter = friendRank.begin();
//			iter != friendRank.end(); ++iter) {
//		cout << iter->first << " : " << iter->second << endl;
//	}
//	cout << endl;

	vector< pair<int, int> > sortVec;
	for (map<int, int>::iterator iter = friendRank.begin();
			iter != friendRank.end(); ++iter) {
		sortVec.push_back(make_pair<int, int>(iter->first, iter->second));
	}

	sort(sortVec.begin(), sortVec.end(), RankComp());
	_idList.clear();
	for (vector< pair<int, int> >::iterator iter = sortVec.begin();
			iter != sortVec.end(); ++iter) {
		_idList.push_back(iter->first);
	}
}
