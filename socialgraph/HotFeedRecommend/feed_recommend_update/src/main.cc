#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>

#include "recommend_interest_model.h"
#include "ugc_model.h"
#include "MyTask.h"

using namespace std;
using namespace MyUtil;
using namespace xce::task;
using namespace xce;
using namespace com::xiaonei::xce;

RawUgcManager* g_ugc_manager = NULL; 

void Init();
void PrintTime();        
void End();
char* time2str(const time_t ltime, char* str);
void CreateFeedRecommendData(const std::string& src);

int main(int argc, char* argv[]) {
	PrintTime();
	Init();

	//--------------------------------------------------------------------
	time_t phrase_start = 0, phrase_end = 0;

  phrase_start = time(NULL);

	string file = "/data/xce/Project/hotfeedrecommend/add_user_interest/UserInterestData";
	CreateFeedRecommendData(file);

  phrase_end = time(NULL);
	//--------------------------------------------------------------------

	End();
}

void Init() {
	ConnectionPoolManager::instance().initialize();

	time_t phrase_start = 0, phrase_end = 0;

  phrase_start = time(NULL);

	string ugc_model_file = "/data/xce/Project/hotfeedrecommend/ugc_interest_tags/ugc_list_interest";
	g_ugc_manager = new RawUgcManager();
	g_ugc_manager->LoadDataFromFile(ugc_model_file);
//	g_ugc_manager->PrintInfo();

  phrase_end = time(NULL);

	fprintf(stderr, "[Init] load ugc_model_file, using %.1fm\n", (phrase_end - phrase_start)/60.0); 
}

void End() {
	delete g_ugc_manager;
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

void CreateFeedRecommendData(const std::string& src) {
	TaskManager::instance().config(TASK_LEVEL_SET_CACHE, ThreadPoolConfig(0, 100));
	int task_size = 0;
	long wait = 0;

	FILE* fp = fopen(src.c_str(), "rb");
	char* buff = new char[5<<20];

	RecommendInterestModelHead head;
	while (true) {
		if (fread(&head, sizeof(RecommendInterestModelHead), 1, fp) != 1) {
			break;
		}

		if (fread(buff, head.len, 1, fp) != 1) {
			break;
		}

		RecommendInterestModel item(head.uid);
		item.FromBuffer(buff, head.len);

		TaskManager::instance().execute(new LoadHotFeedRecommendTask(item));

		while ((task_size = TaskManager::instance().size(TASK_LEVEL_SET_CACHE)) > 10000) {
			wait += 2;
			sleep(2);
			cout << "wait(" << wait << ") task_size(" << task_size << ")" << endl;
		}
	}

	while (TaskManager::instance().size(TASK_LEVEL_SET_CACHE) > 0) {
		sleep(2);
	}

	sleep(3);
	
	fclose(fp);
	delete buff;
}
