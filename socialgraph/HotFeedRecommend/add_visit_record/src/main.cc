#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sys/stat.h>
#include <dirent.h>

#include "recommend_interest_model.h"
#include "UGCVisitRecord.h"
#include "UserInterestTag.h"

using namespace std;
using namespace MyUtil;
using namespace xce;

UGCVisitRecord* g_ugc_record = NULL;
UserInterestTag* g_user_tags = NULL;

void Init();
void End();
void PrintTime();
char* time2str(const time_t ltime, char* str);

void CreateAddVisitRecordData(const std::string& src, const std::string& des);

int main(int argc, char* argv[]) {
	PrintTime();

	Init();

	//g_ugc_record->PrintMap();
	//g_user_tags->PrintMap();

	//-------------------------------------------------------------------------	
	time_t phrase_start = 0, phrase_end = 0;
	phrase_start = time(NULL);	

	string source = "/data/xce/Project/hotfeedrecommend/create_recommend_list/RecommendListData";
	string destination = "/data/xce/Project/hotfeedrecommend/add_visit_record/VisitRecordData";
	
	CreateAddVisitRecordData(source, destination);

	phrase_end = time(NULL);

	fprintf(stderr, "[AddInterestAndBlockListTask] using %.1fm\n", (phrase_end - phrase_start)/60.0);

	//-------------------------------------------------------------------------	

	End();
}

void Init() {
  time_t phrase_start = 0, phrase_end = 0;                                                                                                                   
  phrase_start = time(NULL);                                                                                                                               
  g_ugc_record = new UGCVisitRecord();
  string fileName = "/data/xce/Project/hotfeedrecommend/data/ugc_join_result";
	g_ugc_record->LoadData(fileName);                                                                                                                        
  g_user_tags = new UserInterestTag();        //加入兴趣度
  string user_tags_file = "/data/xce/Project/hotfeedrecommend/data/user_tag";
	//g_user_tags->LoadData(user_tags_file);                                                                                                                   
  phrase_end = time(NULL);
  fprintf(stderr, "[Init] load ugc_join_result, user_tag, using %.1fm\n", (phrase_end - phrase_start)/60.0);
}

void End() {
	delete g_ugc_record;
	delete g_user_tags;
}

void PrintTime() {
	time_t now = time(NULL);
	char date_str[20];
	time2str(now, date_str);
	fprintf(stderr, "+-----------------------+\n");
	fprintf(stderr, "|%s	|\n", date_str);
	fprintf(stderr, "+-----------------------+\n");
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

void CreateAddVisitRecordData(const std::string& src, const std::string& des) {
	FILE* fp_src = fopen(src.c_str(), "rb");
	FILE* fp_des = fopen(des.c_str(), "wb");

	char* buff = new char[5<<20];
	RecommendInterestModelHead head;

	while (true) {
		if (fread(&head, sizeof(RecommendInterestModelHead), 1, fp_src) != 1) {
			break;
		}

		if (fread(buff, head.len, 1, fp_src) != 1) {
			break;
		}

		RecommendInterestModel item(head.uid);
		item.FromBuffer(buff, head.len);
		//item.PrintInfo();

		int uid = item.GetUserId();

		set<long> visit_ugcs = g_ugc_record->GetVisitRecord(uid);       //添加兴趣和浏览记录
		//set<string> user_interests = g_user_tags->GetUserInterest(uid);

		if (!visit_ugcs.empty()) {
			item.SetBlockUgcList(visit_ugcs);
		}

		//if (!user_interests.empty()) {
		//	item.SetInterestList(user_interests);
		//}

		//cout << "after SetBlockUgcList" << endl;
		//item.PrintInfo();

		head.uid = uid;
		head.len = item.ToBuffer(buff);
		
		fwrite(&head, sizeof(RecommendInterestModelHead), 1, fp_des);
		fwrite(buff, head.len, 1, fp_des);
	}

	fclose(fp_src);
	fclose(fp_des);
	delete buff;
}
