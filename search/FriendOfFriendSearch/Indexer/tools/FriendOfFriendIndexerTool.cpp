#include "OceCxxAdapter/src/FriendOfFriendIndexerAdapter.h"
#include <stdio.h>
#include <iostream>
#include "search/FriendOfFriendSearch/library/include/map.h"
#include "search/FriendOfFriendSearch/library/include/mapdb.h"
#include "search/FriendOfFriendSearch/library/include/userdata.h"

using std::endl;
using std::cout;
using namespace com::renren::search2::friendoffriend;

const unsigned int kRequestNum = 100;
const unsigned int kMaxIndirectFriendsNum = 100000;

static const char *server_strings = "localhost:2012";

void Usage(char *cmd) {
  cout << cmd << " < build_index_async user_id level |" << endl;
  cout <<        "   rebuild_index_async user_id level |" << endl;
  cout <<        "   rebuild_index_sync user_id level |" << endl;
  cout <<        "   get_index_and_search_query user_id query >" << endl;
}


static void ShowResult(const FriendOfFriendMap::Result &result)
{
  const UserData *items = result.GetItems();
  for (size_t i = 0; i < result.GetCount(); ++i) {
    printf("id: %u, type: %u, mutual %u\n", items[i].GetID(), items[i].GetType(), items[i].GetMutual());
  }
}

bool BuildIndex(const int user_id, const Ice::Short level) {
  //struct timeval begin, end;
  //long cost;
  //gettimeofday(&begin, NULL);
  FriendOfFriendIndexerAdapter::instance().BuildIndex(user_id, level);
  //gettimeofday(&end, NULL);
  //cost = (end.tv_sec*1000000 + end.tv_usec) - (begin.tv_sec*1000000 + begin.tv_usec);

  //MCE_INFO("result.size=" << info_seq.size() << " time cost = " << cost << " us.");
  //cout << "result.size=" << info_seq.size() << " time cost = " << cost << " us, for more details see ./FriendOfFriendIndexerTool.log" << endl;
  //for (FriendInfoSeq::const_iterator iter=info_seq.begin(); iter!=info_seq.end(); ++iter) {
  //  MCE_INFO("\tid=" << iter->id <<", type=" << iter->type << ", common_friends=" << iter->commonFriendsNum);
  //}
  //return info_seq;
  return true;
}



bool RebuildIndexAsync(const int user_id_a, const Ice::Short level) {
  FriendOfFriendIndexerAdapter::instance().RebuildIndexAsync(user_id_a, level);
  return true;
}

bool RebuildIndexSync(const int user_id_a, const Ice::Short level) {
  FriendOfFriendIndexerAdapter::instance().RebuildIndex(user_id_a, level);
  return true;
}


bool GetIndexAndSearchName(const int user_id, const string& query) {

  FriendOfFriendIndexerAdapter::instance().BuildIndex(user_id, HighLevel);
  
  FriendOfFriendMapDB db;
  FriendOfFriendMap map;

  if (!db.SetServers(server_strings)) {
    cout << "SetServers() failed." << endl;
    return false;
  }

  if (!db.GetMap(user_id, map)) {
    cout << "GetMap() failed." << endl;
    return false;
  }


  FriendOfFriendMap::Result map_result;
  if (!map.Find(query.c_str(), map_result)) {
    cout << "Find " << query << " failed." << endl;
    return false;
  }
  cout << "Found " << query << endl;
  ShowResult(map_result);
  return true;
}

int main(int argc, char** argv) {
  MyUtil::init_logger("Mce", "./FriendOfFriendIndexerTool.log", "DEBUG");

  if (1 == argc || 0 == strcmp(argv[1], "--help")) {
    Usage(argv[0]);
    exit(0);
  }

  if (0 == strcmp("build_index_async", argv[1])) {
    stringstream ss;
    ss << argv[2];
    int user_id;
    ss >> user_id;

    ss.clear();
    ss.str();

    Ice::Short level;
    ss << argv[3];
    ss >> level;
    BuildIndex(user_id, level);
    cout << "sleep 5 sec ..." << endl;
    sleep(5);
  } else if (0 == strcmp("rebuild_index_async", argv[1])) {
    stringstream ss;
    ss << argv[2];
    int user_id;
    Ice::Short level;
    ss >> user_id;
    ss.clear();
    ss.str();
    ss << argv[3];
    ss >> level;
    RebuildIndexAsync(user_id, level);
    cout << "sleep 5 sec ..." << endl;
    sleep(5);
  } else if (0 == strcmp("rebuild_index_sync", argv[1])) {
    stringstream ss;
    ss << argv[2];
    int user_id;
    Ice::Short level;
    ss >> user_id;
    ss.clear();
    ss.str();
    ss << argv[3];
    ss >> level;
    RebuildIndexSync(user_id, level);
    cout << "sleep 5 sec ..." << endl;
    sleep(5);
  } else if (0 == strcmp("get_index_and_search_query", argv[1])) {
    stringstream ss;
    ss << argv[2];
    int user_id;
    ss >> user_id;

    ss.clear();
    ss.str();
    string query;
    ss << argv[3];
    ss >> query;
    GetIndexAndSearchName(user_id, query);
  }

  return true;
}
