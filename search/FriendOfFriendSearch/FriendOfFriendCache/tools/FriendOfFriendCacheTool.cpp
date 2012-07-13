#include <sys/time.h>
#include "OceCxxAdapter/src/FriendOfFriendCacheAdapter.h"
#include <fstream>
#include <map>
#include <ext/hash_map>

using namespace __gnu_cxx;
using std::endl;
using std::cout;
//using namespace xce::searchcache;
using MyUtil::Int2IntSeqMap;
using namespace com::renren::search2::friendoffriend;

const unsigned int kRequestNum = 100;
const unsigned int kMaxIndirectFriendsNum = 100000;

void Usage(char *cmd) {
  cout << cmd << " <show_indirect_friends_of_special_user user_id | test_map_traversal uid id_2_name_file | update_all_related_cache user_id>" << endl;
}

/// @brief GetIndirectFriend 取指定用户的多维好友信息
/// 
/// @param user_id 
/// 
/// @return 
FriendInfoSeq GetIndirectFriend(int user_id) {
  struct timeval begin, end;
  long cost;
  gettimeofday(&begin, NULL);
  FriendInfoSeq info_seq = FriendOfFriendCacheAdapter::instance().get(user_id);
  gettimeofday(&end, NULL);
  cost = (end.tv_sec*1000000 + end.tv_usec) - (begin.tv_sec*1000000 + begin.tv_usec);

  MCE_INFO("result.size=" << info_seq.size() << " time cost = " << cost << " us.");
  cout << "result.size=" << info_seq.size() << " time cost = " << cost << " us, for more details see ./FriendOfFriendCacheTool.log" << endl;
  for (FriendInfoSeq::const_iterator iter=info_seq.begin(); iter!=info_seq.end(); ++iter) {
    MCE_INFO("\tid=" << iter->id <<", type=" << iter->type << ", common_friends=" << iter->commonFriendsNum);
  }
  return info_seq;
}

bool UpdateAllRelatedCache(int user_id) {
  return FriendOfFriendCacheAdapter::instance().UpdateAllRelatedCache(user_id);
}

bool TestMapTraversal(int user_id, string id_2_name_file) {

    FriendInfoSeq info_seq = FriendOfFriendCacheAdapter::instance().get(user_id);

    ifstream input_file(id_2_name_file.c_str());

    struct timeval start, end;
    long cost;
    gettimeofday(&start, NULL);

    hash_map<int, string> all_id_2_name_map(200000000);
    string line;
    int idx = 0;
    while(getline(input_file, line)) {
      int id;
      string name;
      stringstream ss(line);
      ss >>  id;
      ss >> name;
      //MCE_INFO("id=" << id << ", name=" << name);
      all_id_2_name_map.insert(make_pair(id, name));
      if (0 == ++idx % 100000) {
        MCE_INFO("load " << idx);
      }
    }

    MCE_INFO("finished build all map, all_id_2_name_map.size()=" << all_id_2_name_map.size());
    gettimeofday(&end, NULL);

    cost = ((end.tv_sec*1000000 + end.tv_usec) - (start.tv_sec*1000000 + start.tv_usec)) / 1000;
    MCE_INFO("build map time cost=" << cost << " ms");

    gettimeofday(&start, NULL);
    map<int, string> id_2_name;
    for (FriendInfoSeq::iterator iter=info_seq.begin(); iter!=info_seq.end(); ++iter) {
      //MCE_INFO("iter->id=" << iter->id);
      hash_map<int, string>::iterator all_iter = all_id_2_name_map.find(iter->id);
      if (all_id_2_name_map.end() != all_iter) {
        id_2_name.insert(make_pair(iter->id, all_iter->second));
        //MCE_INFO("iter->name=" << all_iter->second);
      } else {
        MCE_INFO("cannot find name for " << iter->id);
      }
    }
    gettimeofday(&end, NULL);

    cost = ((end.tv_sec*1000000 + end.tv_usec) - (start.tv_sec*1000000 + start.tv_usec)) / 1000;
    MCE_INFO("find " << info_seq.size() << " indirect friends time cost=" << cost << " ms");

  return true;
}

int main(int argc, char** argv) {
  MyUtil::init_logger("Mce", "./FriendOfFriendCacheTool.log", "DEBUG");

  if (1 == argc || 0 == strcmp(argv[1], "--help")) {
    Usage(argv[0]);
    exit(0);
  }

  if (0 == strcmp("show_indirect_friends_of_special_user", argv[1])) {

    stringstream ss;
    ss << argv[2];
    int user_id;
    ss >> user_id;

    GetIndirectFriend(user_id);
  } else if(0 == strcmp("test_map_traversal", argv[1])) {

    stringstream ss;
    ss << argv[2];
    int user_id;
    ss >> user_id;

    ss.clear();
    ss.str();
    ss << argv[3];
    string file;
    ss >> file;

    TestMapTraversal(user_id, file);
  } else if (0 == strcmp("update_all_related_cache", argv[1])) {
    stringstream ss;
    ss << argv[2];
    int user_id;
    ss >> user_id;

    UpdateAllRelatedCache(user_id);
  } 
 // else if (0 == strcmp("traversal_all_user_indirect_friends", argv[1])) {
 //   int start_pos = 0;
 //   while (true) {
 //     //先取一批用户id
 //     IntSeq user_ids = TTManagerAdapter::instance().TraversalKey(start_pos, kRequestNum);
 //     for (IntSeq::const_iterator iter=user_ids.begin(); iter!=user_ids.end(); ++iter) {
 //       GetIndirectFriend(*iter);
 //     }
 //     int max_id = user_ids[user_ids.size() - 1 ];
 //     start_pos = max_id + 1;
 //     sleep(2);
 //   }
 // }
  return true;
}
