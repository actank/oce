#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <dirent.h>

#include "recommend_interest_model.h"
#include "ReadFriendListUtil.h"
#include "ReverseMapUtil.h"
#include "SortByFriendRankUtil.h"

using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace readfriend;
using namespace reversemap;
using namespace sortbyrank;
using namespace xce;

void Init();
void PrintTime();
void End();
char* time2str(const time_t ltime, char* str);
void CreateOwnerList(vector<int>& ownerList, const string& fileName);
void WriteToFile(const std::string& des_file, std::map<int, std::vector<int> >& recommend_map);
void PrintMap(const map<int, vector<int> >& resource); 

int main(int argc, char* argv[]) {
	PrintTime();

	Init();

	//-------------------------------------------------------
	time_t phrase_start = 0, phrase_end = 0;
	
	phrase_start = time(NULL);

	fprintf(stderr, "\ncreate owner_list ...\n");
	string fname = "/data/xce/Project/hotfeedrecommend/data/owner_list";
	vector<int> ownerList;	
	CreateOwnerList(ownerList, fname); 			//创建15天内发过ugc的用户列表

	phrase_end = time(NULL);
	fprintf(stderr, "[CreateOwnerList] size of ownerList = %ld, using %.1fm\n", ownerList.size(), (phrase_end - phrase_start)/60.0);

	//-------------------------------------------------------
	phrase_start = time(NULL);

	map< int, vector<int> > friendListMap;

	ReadFriendListUtil obj;
	obj.GetFriendList(ownerList, friendListMap);   //获取好友列表

	phrase_end = time(NULL);
	fprintf(stderr, "[ReadFriendListUtil] size of ownerList = %ld, size of friendListMap = %ld, using %.1fm\n", 
				friendListMap.size(), friendListMap.size(), (phrase_end - phrase_start)/60.0);

	sleep(5);

	//-------------------------------------------------------
	phrase_start = time(NULL);
	map< int, vector<int> > recommendMap;

	ReverseMapUtil::ReverseMap(friendListMap, recommendMap);  		//生成推荐列表

	phrase_end = time(NULL);
	fprintf(stderr, "[ReverseMapUtil] size of friendListMap= %ld, size of recommendMap= %ld, using %.1fm\n", 
		friendListMap.size(), recommendMap.size(), (phrase_end - phrase_start)/60.0);

	//-------------------------------------------------------

	phrase_start = time(NULL);
	
	SortByFriendRankUtil friendrankObj;
	friendrankObj.SortTargetByFriendRank(recommendMap);            //推荐列表亲密度排序
	sleep(120);

	phrase_end = time(NULL);
	fprintf(stderr, "[SortByFriendRankUtil] size of recommendMap = %ld, using %.1fm\n", 
		recommendMap.size(), (phrase_end - phrase_start)/60.0);

	//-------------------------------------------------------

	phrase_start = time(NULL);

	string dest_file = "/data/xce/Project/hotfeedrecommend/create_recommend_list/RecommendListData";
	WriteToFile(dest_file, recommendMap);

	phrase_end = time(NULL);
	fprintf(stderr, "write file, using %.1fm\n", (phrase_end - phrase_start)/60.0);

	//-------------------------------------------------------
	//vector<RecommendInterestModel> recommend_interest_list;
	//ReadFromFile(dest_file, recommend_interest_list);
	////remove(fname.c_str());        //remove dualrelation_count file
	////remove(fname_raw_relationcount.c_str());        //remove action_count file

	End();
}

void Init() {
}

void End() {
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

void CreateOwnerList(vector<int>& ownerList, const string& fileName) {
	ifstream infile(fileName.c_str(), ifstream::in);
	string line = "";
	int lineNumber = 0;
	set<int> tmpSet;

	while (getline(infile, line)) {
		if (++lineNumber % 5000 == 0) {
			fprintf(stderr, "CreateOwnerList reading line %d\r", lineNumber);		
		}

		if (line.empty()) {
			continue;
		}
		
		int ownerId = 0;
		long sourceId = 0;
		int flag = 0;

		istringstream stream(line);
		stream >> ownerId >> sourceId >> flag;
//		cout << ownerId << "	" << sourceId << "	" << flag << endl;

		tmpSet.insert(ownerId);

//		if (tmpSet.size() >= 10) {
//			break;	
//		}
	}
	cout << "tmpSet size = " << tmpSet.size() << endl;

	for (set<int>::iterator iter = tmpSet.begin();
			iter != tmpSet.end(); ++iter) {
		ownerList.push_back(*iter);
	}

	cout << "[CreateOwnerList] ownerList size : " << ownerList.size() << endl;

//	sort(ownerList.begin(), ownerList.end());       //对双向交互为0的id进行排序
	vector<int>(ownerList).swap(ownerList);
}

void PrintMap(const map<int, vector<int> >& resource) {
	for (map<int, vector<int> >::const_iterator iter = resource.begin();
			iter != resource.end(); ++iter) {
		cout << iter->first << " : " << endl;
		ostringstream oss;
		for (vector<int>::const_iterator it = iter->second.begin();
				it != iter->second.end(); ++it) {
			oss << *it << " ";
		}
		cout << oss.str() << endl;	
		cout << endl;
	}
}


void WriteToFile(const std::string& des_file, std::map<int, std::vector<int> >& recommend_map) {
	FILE* fp = fopen(des_file.c_str(), "wb");
	char* buff = new char[5<<20];

	for (map<int, vector<int> >::iterator iter = recommend_map.begin();
			iter != recommend_map.end(); ++iter) {
		RecommendInterestModel item(iter->first);
		item.SetRecommendList(iter->second);
		int len = item.ToBuffer(buff);
		//item.PrintInfo();

		RecommendInterestModelHead head;
		head.uid = iter->first;
		head.len = len;

		fwrite(&head, sizeof(RecommendInterestModelHead), 1, fp);
		fwrite(buff, head.len, 1, fp);
	}

	fclose(fp);
	delete buff;
}
