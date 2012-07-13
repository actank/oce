#include "socialgraph/socialgraphutil/calculator/component/sortbyfriendrankutil.h"

namespace xce {
namespace socialgraph {

void SortByFriendRankUtil::SortTargetByFriendRank(std::map<int, std::vector<int> >& target) {
	time_t phrase_start = 0, phrase_end = 0;
	phrase_start = time(NULL);

	//批量计算
	MyUtil::TaskManager::instance().config(TASK_LEVEL_SORTBY_FRIENDRANK, MyUtil::ThreadPoolConfig(0, 100));
	int task_size = 0;
	long wait = 0;
	long count = 0;

	for (std::map<int, std::vector<int> >::iterator iter = target.begin();
			iter != target.end(); ++iter) {
		if (++count % 1000 == 0) {
			fprintf(stderr, "SortByFriendRankUtil %ld / %ld\r", count, target.size());
		}
		MyUtil::TaskManager::instance().execute(new SortByFriendRankTask(iter->first, iter->second));
		while ((task_size = MyUtil::TaskManager::instance().size(TASK_LEVEL_SORTBY_FRIENDRANK)) > 10000) {
			wait += 2;
			sleep(2);
			fprintf(stderr, "TASK_LEVEL_SORTBY_FRIENDRANK: sleep %ld second. left %d task.\n", wait, task_size);
		}
	}

	fprintf(stderr, "\nSortTargetByFriendRank completed.\n");
	//等待所有线程运行结束
	while (MyUtil::TaskManager::instance().size(TASK_LEVEL_SORTBY_FRIENDRANK) > 0) {
		fprintf(stderr, "final left TASK_LEVEL_SORTBY_FRIENDRANK size = %d.\n", task_size);
		sleep(2);
	}
	sleep(60);
	phrase_end = time(NULL);
	fprintf(stderr, "[SortByFriendRankUtil] size of target = %ld, using %.1fm\n", target.size(), (phrase_end - phrase_start)/60.0);
}

//-----------------------------------------------------------------------------------

void SortByFriendRankTask::handle() {
	using namespace xce::socialgraph;
	if (_idList.empty()) {
		return;
	}

	std::map<int, int> friendRank;
	for (std::vector<int>::iterator iter = _idList.begin();
			iter != _idList.end(); ++iter) {
		friendRank.insert(std::make_pair<int, int>(*iter, 0));
	}

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

	std::vector< std::pair<int, int> > sortVec;
	for (std::map<int, int>::iterator iter = friendRank.begin();
			iter != friendRank.end(); ++iter) {
		sortVec.push_back(std::make_pair<int, int>(iter->first, iter->second));
	}
	std::sort(sortVec.begin(), sortVec.end(), RankComp());
	_idList.clear();

	for (std::vector< std::pair<int, int> >::iterator iter = sortVec.begin();      
			iter != sortVec.end(); ++iter) {
		_idList.push_back(iter->first);
	}
}

}
}
