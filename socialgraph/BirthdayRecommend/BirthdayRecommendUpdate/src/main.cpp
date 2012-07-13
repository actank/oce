#include <iostream>
#include <map>
#include <vector>
#include <sstream>

//#include "DBHelper.h"
#include "MyTasks.h"

using namespace std;
using namespace xce::dbhelper;
using namespace MyUtil;
//using namespace xce::task;

DBHelper* dbhelper = NULL;

void PrintTime();
char* Time2Str(const time_t ltime, char* str);
time_t Begin();
time_t Endup();

void CreateBirthdayRecommendList(vector<int>& birthdayRecommendList);
void ReadFriendLists(const vector<int>& birthdayRecommendList, map<int, vector<int> >& userFriends);
void CreateRecommendResult(const map<int, vector<int> >& userFriends, map<int, vector<int> >& birthdayRecommendMap);
void FliterRecommendBlock(map<int, vector<int> >& birthdayRecommendMap);
void SetCacheSortByRank(map<int, vector<int> >& birthdayRecommendMap);

int main(int argc, char* argv[]) {
	PrintTime();

	time_t start = Begin();
	time_t phase_start = 0, phase_end = 0;
	phase_start = time(NULL);

	//--------------------------
	vector<int> birthdayRecommendList;
	CreateBirthdayRecommendList(birthdayRecommendList);   //获取今天过生日用户的列表
	
	phase_end = time(NULL);	
	fprintf(stderr, "size of birthdayRecommendList = %ld, using %.1fm\n",
			birthdayRecommendList.size(), (phase_end - phase_start)/60.0);	

	//--------------------------
	phase_start = time(NULL);
	
	map<int, vector<int> > userFriends;
	ReadFriendLists(birthdayRecommendList, userFriends);   //获取过生日用户的好友列表

//	for (map<int, vector<int> >::iterator iter = userFriends.begin();
//			iter != userFriends.end(); ++iter) {
//		cout << iter->first << " : " << iter->second.size() << endl;
//	}
//	const vector<int>& mm = userFriends[2171];
//	for (vector<int>::const_iterator iter = mm.begin();
//			iter != mm.end(); ++iter) {
//		cout << *iter << "  ";
//	}
//	cout << endl;
	
	phase_end = time(NULL);	
	fprintf(stderr, "size of userFriends = %ld, using %.1fm\n",
			userFriends.size(), (phase_end - phase_start)/60.0);

	//--------------------------
	phase_start = time(NULL);
	
	map<int, vector<int> > birthdayRecommendMap;
	CreateRecommendResult(userFriends, birthdayRecommendMap);   //创建用户好友生日推荐列表

//	birthdayRecommendMap[1894].push_back(666);
//	birthdayRecommendMap[1894].push_back(888);

//	for (map<int, vector<int> >::iterator iter = birthdayRecommendMap.begin();
//			iter != birthdayRecommendMap.end(); ++iter) {
//		cout << "first : " << iter->first << endl;
//		for (vector<int>::iterator it = iter->second.begin();
//				it != iter->second.end(); ++it) {
//			cout << *it << "  ";
//		}
//		cout << endl;
//		cout << endl;
//	}

	phase_end = time(NULL);	
	fprintf(stderr, "size of birthdayRecommendMap = %ld, using %.1fm\n",
			birthdayRecommendMap.size(), (phase_end - phase_start)/60.0);

	//--------------------------
	phase_start = time(NULL);

	FliterRecommendBlock(birthdayRecommendMap);               //过滤Recommend_Block列表
		
	phase_end = time(NULL);	
	fprintf(stderr, "finish fliter block table birthdayRecommendMap size = %ld, using %.1fm\n",
			birthdayRecommendMap.size(), (phase_end - phase_start)/60.0);
	
	//for (map<int, vector<int> >::iterator iter = birthdayRecommendMap.begin();
	//		iter != birthdayRecommendMap.end(); ++iter) {
	//	cout << "first : " << iter->first << endl;
	//	for (vector<int>::iterator it = iter->second.begin();
	//			it != iter->second.end(); ++it) {
	//		cout << *it << "  ";
	//	}
	//	cout << endl;
	//	cout << endl;
	//}
	//--------------------------
	phase_start = time(NULL);

	SetCacheSortByRank(birthdayRecommendMap);                  //按好友亲密度排序并且向cache中添加数据

	phase_end = time(NULL);	
	fprintf(stderr, "SetCacheSortByRank birthdayRecommendMap size = %ld, using %.1fm\n",
			birthdayRecommendMap.size(), (phase_end - phase_start)/60.0);

	time_t end = Endup();

	printf("time used %ld second\n", end - start);
	return 0;
}

void PrintTime() {
	time_t now = time(NULL);
	char date_str[20];
	Time2Str(now, date_str);
	fprintf(stderr, "+-----------------------+\n");
	fprintf(stderr, "| %s	|\n", date_str);
	fprintf(stderr, "+-----------------------+\n");
}

char* Time2Str(const time_t ltime, char* str) {
	if (str == NULL) {
		fprintf(stderr, "WARN Time2Str str is NULL");
		return NULL;
	}

	struct tm *Tm = localtime(&ltime);
	sprintf(str, "%04d-%02d-%02d %02d:%02d:%02d", 
			Tm->tm_year + 1900,
			Tm->tm_mon + 1,
			Tm->tm_mday,
			Tm->tm_hour,
			Tm->tm_min,
			Tm->tm_sec);
	return str;
}

time_t Begin() {
	dbhelper = new DBHelper();
	return time(NULL);
}

time_t Endup() {
	cout << "\ncleaning jobs ... OK" << endl;
	delete dbhelper;

	return time(NULL);
}

void CreateBirthdayRecommendList(vector<int>& birthdayRecommendList) {
	time_t ltime = time(NULL);
	struct tm *Tm = localtime(&ltime);

	int to_month = Tm->tm_mon + 1;
	int to_day = Tm->tm_mday;

	time_t rtime = ltime + 24 * 3600;
	struct tm *Tn = localtime(&rtime);

	int ne_month = Tn->tm_mon + 1;
	int ne_day = Tn->tm_mday;

	fprintf(stderr, "%d-%d\n", to_month, to_day);
	//fprintf(stderr, "%d-%d\n", ne_month, ne_day);

	dbhelper->GetBirthdayRecommendList(to_month, to_day, ne_month, ne_day, birthdayRecommendList);
}

void ReadFriendLists(const vector<int>& birthdayRecommendList, map<int, vector<int> >& userFriends) {
	TaskManager::instance().config(TASK_LEVEL_READ_FRIENDS, ThreadPoolConfig(0, 100));

	int task_size = 0;
	long wait = 0;
	long count = 0;
	
	for (vector<int>::const_iterator iter = birthdayRecommendList.begin();
			iter != birthdayRecommendList.end(); ++iter) {
		if (++count % 1000 == 0) {
			fprintf(stderr, "%ld / %ld\r", count, birthdayRecommendList.size());
		}

		pair<map<int, vector<int> >::iterator, bool> result =
			userFriends.insert(make_pair<int, vector<int> >(*iter, vector<int>()));

		TaskManager::instance().execute(new ReadFriendTask(*iter, result.first->second));

		while ((task_size = TaskManager::instance().size(TASK_LEVEL_READ_FRIENDS)) > 10000) {
			wait += 2;
			sleep(2);
			fprintf(stderr, "read friend list sleep %ld second. left %d task.\r", wait, task_size);
		}
	}
	fprintf(stderr, "\nread friendlist completed.\n");

	while (TaskManager::instance().size(TASK_LEVEL_READ_FRIENDS) > 0) {
		sleep(2);
	}
}

void CreateRecommendResult(const map<int, vector<int> >& userFriends, map<int, vector<int> >& birthdayRecommendMap) {
	for (map<int, vector<int> >::const_iterator iter = userFriends.begin();
			iter != userFriends.end(); ++iter) {
		for (vector<int>::const_iterator it = iter->second.begin();
				it != iter->second.end(); ++it) {
			birthdayRecommendMap[*it].push_back(iter->first);
		}
	}
}

void FliterRecommendBlock(map<int, vector<int> >& birthdayRecommendMap) {
	TaskManager::instance().config(TASK_LEVEL_FLITER_BLOCK, ThreadPoolConfig(0, 100));

	int task_size = 0;
	long wait = 0;
	long count = 0;

	for (map<int, vector<int> >::iterator iter = birthdayRecommendMap.begin();
			iter != birthdayRecommendMap.end(); ++iter) {
		if (++count % 1000 == 0) {
			fprintf(stderr, "%ld / %ld\r", count, birthdayRecommendMap.size());
		}

		TaskManager::instance().execute(new FliterBlockTask(iter->first, iter->second));

		while ((task_size = TaskManager::instance().size(TASK_LEVEL_FLITER_BLOCK)) > 10000) {
			wait += 2;
			sleep(2);
			//fprintf(stderr, "fliter recommend block table sleep %ld second. left %d task.\r", wait, task_size);
		}
	}	

	fprintf(stderr, "\nfliter recommend block completed.\n");

	while (TaskManager::instance().size(TASK_LEVEL_FLITER_BLOCK) > 0) {
		sleep(2);
	}
}

void SetCacheSortByRank(map<int, vector<int> >& birthdayRecommendMap) {
	TaskManager::instance().config(TASK_LEVEL_SET_CACHE, ThreadPoolConfig(0, 100));

	int task_size = 0;
	long wait = 0;
	long count = 0;

	for (map<int, vector<int> >::iterator iter = birthdayRecommendMap.begin();
			iter != birthdayRecommendMap.end(); ++iter) {
		if (++count % 1000 == 0) {
			fprintf(stderr, "%ld / %ld\r", count, birthdayRecommendMap.size());
		}

		TaskManager::instance().execute(new SetCacheAndSortTask(iter->first, iter->second));

		while ((task_size = TaskManager::instance().size(TASK_LEVEL_SET_CACHE)) > 10000) {
			wait += 2;
			sleep(2);
			//fprintf(stderr, "set cache and sort sleep %ld second. left %d task.\r", wait, task_size);
		}
	}	

	fprintf(stderr, "\nfliter set cache completed.\n");

	while (TaskManager::instance().size(TASK_LEVEL_SET_CACHE) > 0) {
		sleep(2);
	}
}
