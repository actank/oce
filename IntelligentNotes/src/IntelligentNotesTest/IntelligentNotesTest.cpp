#include <IceUtil/IceUtil.h>
#include <IntelligentNotes.h>
#include <IntelligentNotesAdapter.h>
#include <XceCacheAdapter.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#define MAX 20
using std::cout;
using std::endl;
using namespace com::xiaonei::services::IntelligentNotes;
const int TASK_LEVEL_UPDATE_DBCACHE = 9;
class ExecuteTask: virtual public MyUtil::Task  {
    public:
       ExecuteTask();
       virtual void handle();
private:
 static int i;
};


ExecuteTask::ExecuteTask() :
   MyUtil::Task(TASK_LEVEL_UPDATE_DBCACHE) {
} 
int ExecuteTask::i = 0;

void ExecuteTask::handle()  {
	++i;
std::cout << " thead seq is " << i << std::endl;
   std::string query[]={"qinghua","qh","清hua","qing华","bei大","北大","dong大","bd","zhongkeda","dd",
                        "hagongda","wuda","zd","huazhongda","nk","td","xjd","fudan","shangjiao","by"};
   std::vector<int> results;
   while(1)  {
      for(unsigned int i=0;i<MAX;i++)  {
         results = IntelligentNotesAdapter::instance().ProcessSequence(query[i]);
         std::vector<string> words = IntelligentNotesAdapter::instance().GetResults();
     }
   }
  sleep(200000);
}

//TaskManager::instance().config(xce::searchcache::TASK_LEVEL_EXECUTE_BASIC, ThreadPoolConfig(1, 32));

int main(int argc, char ** argv)
{
   
   //std::string query;
   //std::vector<int> results;
   //std::vector<string> words = IntelligentNotesAdapter::instance().GetResults();
   //TaskManager::instance().config(ThreadPoolConfig(64, 128)); 
   TaskManager::instance().config(TASK_LEVEL_UPDATE_DBCACHE, ThreadPoolConfig(64, 128)); 
   for (int i=0; i<50;i++)
     TaskManager::instance().execute(new ExecuteTask());
   sleep(240);
   /*while(1)  {
      std::cin>>query;
      if(query=="exit")
         break;
      results = IntelligentNotesAdapter::instance().ProcessSequence(query);
      std::vector<string> words = IntelligentNotesAdapter::instance().GetResults();
      if(results.size()==1)  {
          if(results.at(0)==-1)  {  std::cerr<<"找不到结果！"<<endl;  continue;  }
      }
      for(size_t i=0; i<results.size() ;i++)
         std::cerr<<words.at(results.at(i))<<" ";
      std::cerr<<endl;      
   }*/
   return 0;
}
