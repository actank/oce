#include <fstream>
#include <sstream>

//#include "tools/SortByFriendRankUtil.h"
#include "SortByFriendRankUtil.h"
#include "ReadFriendListUtil.h"

#include "MyTasks.h"

using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace sortbyrank;
using namespace tasks;
using namespace readfriend;
//using namespace xce::socialgraph;

SortByFriendRankUtil* g_sortbyrank_ptr = NULL;
ReadFriendListUtil* g_readfriend_ptr = NULL;

void Init();
void PrintTime();
void End();
char* time2str(const time_t ltime, char* str);
void CreateZeroCountList(vector<int>& negtiverList, const string& fileName);
void CreateSingleRelationMap(map<int, vector<int> >& singleContactedMap, map<int, vector<int> >& singleContactingMap, const vector<int>& negtiverList, const string& fileName);
bool IsNegtiver(int userId, const vector<int>& negtiverList);
void MergeContactList(map<int, vector<int> >& singleContactedMap, 
		map<int, vector<int> >& singleContactingMap, 
		map<int, vector<int> >& singleContactingRecommendMap);
void SortContactListByFriendRank(map<int, vector<int> >& singleContactedMap, map<int, vector<int> >& singleContactingRecommendMap);

void CreatePUKBuddyApplyMap(map<int, set<int> >& buddyApplyMap, const string& fileName);

void CreatePUKFeedAndSetCache(const map<int, vector<int> >& singleContactedMap,
		const map<int, vector<int> >& singleContactingRecommendMap,
		const map<int, set<int> >& buddyApplyMap);
void CreatingOnePUKFeedTask(const map<int, vector<int> >& resourceMap,
		const map<int, set<int> >& buddyApplyMap);

void PrintMap(const map<int, vector<int> >& resource); 

int main(int argc, char* argv[]) {
	Init();
	PrintTime();

	//-------------------------------------------------------

	time_t phrase_start = 0, phrase_end = 0;
	
	phrase_start = time(NULL);

	fprintf(stderr, "\ncreate ZeroCountList ...\n");
	string fname = "/data/xce/Project/pukfeed/data/dualrelation_count";
	vector<int> negtiverList;	
	CreateZeroCountList(negtiverList, fname);       //创建双向交互为0的用户id

	phrase_end = time(NULL);
	fprintf(stderr, "[CreateZeroCountList] size of negtiverList = %ld, using %.1fm\n", negtiverList.size(), (phrase_end - phrase_start)/60.0);

	//-------------------------------------------------------

	phrase_start = time(NULL);

	string fname_raw_relationcount = "/data/xce/Project/pukfeed/data/action_count";
	map<int, vector<int> > singleContactedMap;               // 双向交互中被单向的用户id为key
	map<int, vector<int> > singleContactingMap;              //双向交互中主动单向他人的用户id为key

	CreateSingleRelationMap(singleContactedMap, singleContactingMap, negtiverList, fname_raw_relationcount);	//创建那些双向关系为0的用户，哪些人单向了他们
	phrase_end = time(NULL);
	fprintf(stderr, "[CreateSingleRelationMap] size of singleContactedMap= %ld, size of singleContactingMap= %ld, using %.1fm\n", 
				singleContactedMap.size(), singleContactingMap.size(), (phrase_end - phrase_start)/60.0);

	//-------------------------------------------------------

	phrase_start = time(NULL);
	map<int, vector<int> > singleContactingRecommendMap;

	MergeContactList(singleContactedMap, singleContactingMap, singleContactingRecommendMap);  //合并两个map
	phrase_end = time(NULL);
	fprintf(stderr, "[MergeContactList] size of singleContactedMap= %ld, size of singleContactingMap= %ld, size of singleContactingRecommendMap= %ld, using %.1fm\n", 
		singleContactedMap.size(), singleContactingMap.size(), singleContactingRecommendMap.size(), (phrase_end - phrase_start)/60.0);

	//-------------------------------------------------------

	phrase_start = time(NULL);
	//singleContactedMap[238111132].push_back(1331);
	//singleContactedMap[238111132].push_back(202911262);
	//singleContactedMap[238111132].push_back(239737004);
	//singleContactedMap[238111132].push_back(190136739);

	//PrintMap(singleContactedMap);

	SortContactListByFriendRank(singleContactedMap, singleContactingRecommendMap); 		//将推荐id按亲密度排序

	//PrintMap(singleContactedMap);

	phrase_end = time(NULL);
	fprintf(stderr, "[SortContactListByFriendRank] size of singleContactingMap= %ld, size of singleContactingRecommendMap= %ld, using %.1fm\n", 
		singleContactingMap.size(), singleContactingRecommendMap.size(), (phrase_end - phrase_start)/60.0);

	//------------------------------------------------------- 
	phrase_start = time(NULL);

	map<int, set<int> > buddyApplyMap;
	string fname_buddyapply = "/data/xce/Project/pukfeed/data/puk_addfriend.recently";
	CreatePUKBuddyApplyMap(buddyApplyMap, fname_buddyapply);

	phrase_end = time(NULL);
	fprintf(stderr, "[CreateSingleRelationMap] size of buddyApplyMap=%ld, using %.1fm\n",
		buddyApplyMap.size(), (phrase_end - phrase_start)/60.0);

	//-------------------------------------------------------
	phrase_start = time(NULL);

	//singleContactedMap[238111132].push_back(222677625);
	//singleContactedMap[238111132].push_back(233940269);
	//singleContactedMap[1331].push_back(233940269);
	//singleContactedMap[222677625].push_back(233940269);
	//buddyApplyMap[233940269].insert(246246210);
	//buddyApplyMap[233940269].insert(240122709);
	//buddyApplyMap[233940269].insert(200450830);
	//buddyApplyMap[233940269].insert(1331);
	//buddyApplyMap[233940269].insert(238111132);
	//buddyApplyMap[222677625].insert(123);
	//buddyApplyMap[222677625].insert(1234);

	CreatePUKFeedAndSetCache(singleContactedMap, singleContactingRecommendMap, buddyApplyMap);

	phrase_end = time(NULL);
	fprintf(stderr, "[CreatePUKFeedAndSetCache] size of singleContactedMap=%ld, size of singleContactingRecommendMap=%ld, size of buddyApplyMap=%ld, using %.1fm\n", 
		singleContactedMap.size(), singleContactingRecommendMap.size(), 
		buddyApplyMap.size(), (phrase_end - phrase_start)/60.0);

	//-------------------------------------------------------
	//remove(fname.c_str());        //remove dualrelation_count file
	//remove(fname_raw_relationcount.c_str());        //remove action_count file

	//-------------------------------------------------------
	End();
}

void Init() {
	ConnectionPoolManager::instance().initialize();
	g_sortbyrank_ptr = new SortByFriendRankUtil();
	g_readfriend_ptr = new ReadFriendListUtil();
}

void PrintTime() {
	time_t now = time(NULL);
	char date_str[20];
	time2str(now, date_str);
	fprintf(stderr, "+---------------------------+\n");
	fprintf(stderr, "| %s       |\n", date_str);
	fprintf(stderr, "+---------------------------+\n");
}

char* time2str(const time_t ltime, char* str) {
	if (str == NULL) {
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

void CreateZeroCountList(vector<int>& negtiverList, const string& fileName) {
	ifstream infile(fileName.c_str(), ifstream::in);
	string line = "";
	int lineNumber = 0;
	
	while (getline(infile, line)) {
		if (++lineNumber % 5000 == 0) {
			fprintf(stderr, "CreateZeroCountList reading line %d\r", lineNumber);		
		}

		if (line.empty()) {
			continue;
		}
		
		int val0 = 0;
		int val1 = 0;

		istringstream stream(line);
		stream >> val0 >> val1;
		//cout << val0 << " : " << val1 << endl;

		if (val1 == 0) {
			negtiverList.push_back(val0);
		}
	}
	//cout << "CreateZeroCountList negtiverList size = " << negtiverList.size() << endl;

	sort(negtiverList.begin(), negtiverList.end());       //对双向交互为0的id进行排序
}

void CreateSingleRelationMap(map<int, vector<int> >& singleContactedMap, map<int, vector<int> >& singleContactingMap, 
		const vector<int>& negtiverList, const string& fileName) {
	ifstream infile(fileName.c_str(), ifstream::in);
	string line = "";
	int lineNumber = 0;
	while (getline(infile, line)) {
		if (++lineNumber % 5000 == 0) {
			fprintf(stderr, "CreateSingleRelationMap reading line %d\r", lineNumber);		
		}
		
		if (line.empty()) {
			continue;
		}
		int val0 = 0;
		int val1 = 0;
		int val2 = 0;
		int val3 = 0;
		int val4 = 0;
		int val5 = 0;
		int val6 = 0;
		int val7 = 0;

		istringstream stream(line);
		stream >> val0 >> val1 >> val2 >> val3 >> val4 >> val5 >> val6 >> val7;
//		cout << val0 << "  " << val1 << "  " << val2 << "  " << val3 << "  " << val4 << "  " << val5 << "  " << val6 << "  " << val7 << endl;

		if (IsNegtiver(val0, negtiverList)) {
			if (val3 > 0 || val5 > 0) {
				singleContactedMap[val0].push_back(val1);            //val0被val1单向
			}

			if (val2 > 0 || val4 > 0) {
				singleContactingMap[val0].push_back(val1);           //val0单向过val1
			}
		}

		if (IsNegtiver(val1, negtiverList)) {
			if (val2 > 0 || val4 > 0) {
				singleContactedMap[val1].push_back(val0);             //val1被val0单向
			}

			if (val3 > 0 || val5 > 0) {
				singleContactingMap[val1].push_back(val0);            //val1单向过val0
			}
		}
	}
}

bool IsNegtiver(int userId, const vector<int>& negtiverList) {
	vector<int>::const_iterator fIt = lower_bound(negtiverList.begin(), negtiverList.end(), userId);
	if (fIt != negtiverList.end() && *fIt == userId) {
		return true;
	}
	return false;
}

void MergeContactList(map<int, vector<int> >& singleContactedMap, 
		map<int, vector<int> >& singleContactingMap, 
		map<int, vector<int> >& singleContactingRecommendMap) {
	for (map<int, vector<int> >::iterator iter = singleContactingMap.begin();
			iter != singleContactingMap.end(); ++iter) {
		for (vector<int>::iterator it = iter->second.begin();
				it != iter->second.end(); ++it) {
			singleContactingRecommendMap[*it].push_back(iter->first);
		}
	}
	
	long count = 0;
	int kk = 0;
	for (map<int, vector<int> >::iterator iter = singleContactedMap.begin();
			iter != singleContactedMap.end(); ++iter) {
		if (++count % 1000 == 0) {
			fprintf(stderr, "MergeContactList %ld / %ld\r", count, singleContactedMap.size());
		}

		map<int, vector<int> >::iterator fIt = singleContactingRecommendMap.find(iter->first);

		if (fIt != singleContactingRecommendMap.end()) {
			++kk;
			
			for (vector<int>::iterator it = fIt->second.begin();
					it != fIt->second.end(); ++it) {
				if (find(iter->second.begin(), iter->second.end(), *it) == iter->second.end()) {
					iter->second.push_back(*it);	
				}
			}
			singleContactingRecommendMap.erase(fIt);
		}
	}
	fprintf(stderr, "kk = %d\n", kk);
}

void SortContactListByFriendRank(map<int, vector<int> >& singleContactedMap, map<int, vector<int> >& singleContactingRecommendMap) {
	g_sortbyrank_ptr->SortTargetByFriendRank(singleContactedMap);
	g_sortbyrank_ptr->SortTargetByFriendRank(singleContactingRecommendMap);	
}

void CreatePUKBuddyApplyMap(map<int, set<int> >& buddyApplyMap, const string& fileName) {
	ifstream infile(fileName.c_str(), ifstream::in);
	string line = "";
	int lineNumber = 0;
	
	while (getline(infile, line)) {
		if (++lineNumber % 5000 == 0) {
			fprintf(stderr, "CreatePUKBuddyApplyMap reading line %d\r", lineNumber);		
		}

		if (line.empty()) {
			continue;
		}
		
		int val0 = 0;
		int val1 = 0;

		istringstream stream(line);
		stream >> val0 >> val1;
		//cout << val0 << " : " << val1 << endl;
		buddyApplyMap[val0].insert(val1);
	}
	cout << "CreatePUKBuddyApplyMap buddyApplyMap size = " << buddyApplyMap.size() << endl;
}

void CreatePUKFeedAndSetCache(const map<int, vector<int> >& singleContactedMap,
		const map<int, vector<int> >& singleContactingRecommendMap,
		const map<int, set<int> >& buddyApplyMap) {
	CreatingOnePUKFeedTask(singleContactedMap, buddyApplyMap);
	CreatingOnePUKFeedTask(singleContactingRecommendMap, buddyApplyMap);
}

void CreatingOnePUKFeedTask(const map<int, vector<int> >& resourceMap,
		const map<int, set<int> >& buddyApplyMap) {
	TaskManager::instance().config(TASK_LEVEL_CREATE_FEED, ThreadPoolConfig(0, 100));
	int task_size = 0;
	long wait = 0;
	long count = 0;
	
	for (map<int, vector<int> >::const_iterator iter = resourceMap.begin();
			iter != resourceMap.end(); ++iter) {
		//if (iter->first % 10 != 2) {          //尾号控制
		//	continue;
		//}

		if (++count % 1000 == 0) {
			fprintf(stderr, "CreatingOnePUKFeedTask %ld / %ld \r", count, resourceMap.size());
		}

		TaskManager::instance().execute(new CreateFeedAndSetCacheTask(iter->first, iter->second, buddyApplyMap));

		while ((task_size = TaskManager::instance().size(TASK_LEVEL_CREATE_FEED)) > 10000) {
			wait += 2;
			sleep(2);
		}
	}

	//sleep(5);
	fprintf(stderr, "\n CreatingOnePUKFeedTask set cache completed.\n");

	while (TaskManager::instance().size(TASK_LEVEL_CREATE_FEED) > 0) {
		sleep(2);
	}
}

void PrintMap(const map<int, vector<int> >& resource) {
	for (map<int, vector<int> >::const_iterator iter = resource.begin();
			iter != resource.end(); ++iter) {
		cout << iter->first << " : " << endl;
		for (vector<int>::const_iterator it = iter->second.begin();
				it != iter->second.end(); ++it) {
			cout << *it << " ";
		}
		cout << endl;
	}
}

void End() {
	delete g_sortbyrank_ptr;
	delete g_readfriend_ptr;
}
