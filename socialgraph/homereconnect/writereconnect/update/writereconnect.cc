#include "socialgraph/homereconnect/writereconnect/update/writereconnect.h"

#include "socialgraph/socialgraphutil/calculator/component/readfriendlistfromdisk.h"
#include "socialgraph/socialgraphutil/calculator/component/readfriendlistutil.h"

#include "socialgraph/homereconnect/writereconnect/update/sortbyfriendrankutil.h"
#include "socialgraph/homereconnect/writereconnect/update/preloadlogindata.h"
#include "socialgraph/homereconnect/writereconnect/update/mytask.h"

namespace xce {
namespace socialgraph {

using namespace std;
using namespace xce::logindata;
using namespace readfriend;
using namespace com::xiaonei::xce;
using namespace MyUtil;
using namespace xce::task;
using namespace sortbyrank;
using namespace xce;

void WriteReconnect::Detail() {
	SGTIME(t, "WR");
//--------------LoadComponent------------------------
	Component* component = new ReadFriendListFromDisk("/data/xce/Project/buddyrelationdb");
	SetComponent(0, component);
	LoadComponent(0);
	SGTIME_CHECK(t, "LoadComponent");

//--------------Init----------------------------
	Init();
	ostringstream oss;

//--------------GetNegtiveList-------------------
  vector<int> negtive_list; 
  PreloadLoginData worker;  
  worker.LoadFromDB();   
  worker.GetNegtiveList(negtive_list); 
	
	oss << "GetNegtiveList negtive_list size : " << negtive_list.size();
	SGCTIME_CHECK(t, oss.str());

//--------------GetNegtiveFriendsList-------------------
	map<int, vector<int> > friend_list_map;
	ReadFriendListUtil read_util;
	read_util.GetFriendList(negtive_list, friend_list_map);
	sleep(5);

	oss.str("");
	oss << "GetNegtiveFriendsList friend_list_map size : " << friend_list_map.size();
	SGCTIME_CHECK(t, oss.str());

//--------------CreateWriteList-------------------
	map<int, vector<int> > write_reconnect_map;
	CreateWriteReconnectMapByTask(friend_list_map);
	CreateWriteReconnectTask::g_writereconnect_ptr->GetWriteReconnectMap(write_reconnect_map);
	friend_list_map.clear();
	negtive_list.clear();

	oss.str("");
	oss << "CreateWriteList write_reconnect_map size : " << write_reconnect_map.size();
	SGCTIME_CHECK(t, oss.str());

//--------------SortByFriendRank-------------------
	SortByFriendRankUtil friend_rank_util;
	friend_rank_util.SortTargetByFriendRank(write_reconnect_map);
	SGCTIME_CHECK(t, "SortTargetByFriendRank");

//--------------SetDistCache-------------------
	SetDistCache(write_reconnect_map);	
	SGCTIME_CHECK(t, "SetDistCache");
}

void WriteReconnect::Init() {
  ConnectionPoolManager::instance().initialize();
  CreateWriteReconnectTask::g_writereconnect_ptr = new WriteReconnectItem();
}

void WriteReconnect::CreateWriteReconnectMapByTask(const std::map<int, std::vector<int> >& friend_list_map) {
  TaskManager::instance().config(TASK_LEVEL_CREATE_WRITE, ThreadPoolConfig(0, 100));
  int task_size = 0;
  long wait = 0;
  long count = 0;

  for (map<int, vector<int> >::const_iterator iter = friend_list_map.begin();
      iter != friend_list_map.end(); ++iter) {
    if (++count % 1000 == 0) {
      fprintf(stderr, "CreateWriteReconnectMapByTask %ld / %ld \r", count, friend_list_map.size());
    }
    TaskManager::instance().execute(new CreateWriteReconnectTask(iter->first, iter->second));
    while ((task_size = TaskManager::instance().size(TASK_LEVEL_CREATE_WRITE)) > 10000) {
      wait += 2;
      sleep(2);
    }
  }
  fprintf(stderr, "\n CreateWriteReconnectTask completed.\n");
  while (TaskManager::instance().size(TASK_LEVEL_CREATE_WRITE) > 0) {
    sleep(2);
  }
	sleep(2);
}

void WriteReconnect::SetDistCache(const std::map<int, std::vector<int> >& write_reconnect_map) {
  TaskManager::instance().config(TASK_LEVEL_SET_CACHE, ThreadPoolConfig(0, 100));
  int task_size = 0;
  long wait = 0;
  long count = 0;

  for (map<int, vector<int> >::const_iterator iter = write_reconnect_map.begin();
      iter != write_reconnect_map.end(); ++iter) {
    if (++count % 1000 == 0) {
      fprintf(stderr, "SetDistCacheByWriteReconnectTask %ld / %ld \r", count, write_reconnect_map.size());
    }
    TaskManager::instance().execute(new SetDistCacheByWriteReconnectTask(iter->first, iter->second));
    while ((task_size = TaskManager::instance().size(TASK_LEVEL_SET_CACHE)) > 10000) {
      wait += 2;
      sleep(2);
    }
  } 

  fprintf(stderr, "\n SetDistCacheByWriteReconnectTask set cache completed.\n");
  while (TaskManager::instance().size(TASK_LEVEL_SET_CACHE) > 0) {
    sleep(2);
  }
}

}
}
