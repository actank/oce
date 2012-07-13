#include <algorithm>

#include "gtest/gtest.h"
#include "boost/foreach.hpp"

#include "base/logging.h"

#include "OceSlice/slice/FriendClusterCache.h"
#include "OceCxxAdapter/src/FriendClusterCacheAdapter.h"
#include "OceCxxAdapter/src/BuddyByIdReplicaAdapter.h"

#include "socialgraph/socialgraphutil/mc_adapter.h"
#include "socialgraph/socialgraphutil/algostuff.h"
#include "socialgraph/socialgraphproto/item.pb.h"


namespace xce {
namespace socialgraph {

/*
TEST(FriendClusterCacheAdapter, Get) {
  MyUtil::IntList uid_list;
  uid_list.push_back(301814863);
  FriendClusterData data;
  try {
    data = FriendClusterCacheAdapter::instance().Get(uid_list);
  } catch (...) {
    LOG(LERROR) << "FriendClusterCacheAdapter::instance().Get() exception...";
  }
  EXPECT_NE(0, data.data1.size());
  Int2StrListMap::const_iterator it1 = data.data1.begin();
  for (; it1 != data.data1.end(); ++it1) {
    LOG(INFO) << it1->first;
  }

  EXPECT_NE(0, data.data2.size());
  Str2IntListMap::const_iterator it2 = data.data2.begin();
  int count = 0;
  for (; it2 != data.data2.end(); ++it2) {
    LOG(INFO) << "==:" << it2->first;
    BOOST_FOREACH(int i, it2->second) {
      LOG(INFO) << "  " << i << "\t" << ++count;
    }
  }
}

TEST(BuddyByIdCacheAdapter, Get) {
  using namespace xce::buddy::adapter;
  MyUtil::IntSeq id_seq;
  id_seq.push_back(27640369);
  id_seq.push_back(286162347);
  MyUtil::Int2IntSeqMap map;
  try {
    map = BuddyByIdCacheAdapter::instance().getFriendLists(id_seq);
  } catch (...) {
    LOG(LERROR) << "BuddyByIdCacheAdapter::instance().getFriendLists() exception ... ";
  }

  MyUtil::Int2IntSeqMap::const_iterator it = map.begin();
  for (; it != map.end(); ++it) {
    LOG(INFO) << "id:" << it->first << "\tfriends.size:" << it->second.size();
  }
}
*/

TEST(FriendClusterRecommend, Compute) {
  using namespace xce::buddy::adapter;
  using namespace xce::socialgraph;

  int test_id = 266235977;
  MyUtil::IntList uid_list;
  uid_list.push_back(test_id);
  FriendClusterData data;
  try {
    data = FriendClusterCacheAdapter::instance().Get(uid_list);
    LOG(INFO) << "data.data1.size():" << data.data1.size() << "\tdata.data2.size():" << data.data2.size();
  } catch (...) {
    LOG(LERROR) << "FriendClusterCacheAdapter::instance().Get() exception...";
  }
  Int2StrListMap::const_iterator it1 = data.data1.find(test_id);
  for (; it1 != data.data1.end(); ++it1) {
    LOG(INFO) << it1->first;
  }
  
  MyUtil::IntSeq cluster_id_seq;
  Str2IntListMap::const_iterator it2 = data.data2.begin();
  for (; it2 != data.data2.end(); ++it2) {
    LOG(INFO) << "it2->second.size():" << it2->second.size();
    BOOST_FOREACH(int i, it2->second) {
      cluster_id_seq.push_back(i);
    }
  }
  LOG(INFO) << "cluster_id_seq.size():" << cluster_id_seq.size();
  MyUtil::IntSeq friend_seq;
  try {
    friend_seq = BuddyByIdCacheAdapter::instance().getFriendListAsc(test_id, 2000);
    LOG(INFO) << "test_id:" << test_id << "\tfriend_seq.size():" << friend_seq.size();
  } catch (...) {
    LOG(LERROR) << "BuddyByIdCacheAdapter::instance().getFriendLists() exception ... ";
  }

  MyUtil::IntSeq recommend_id_seq;
  BOOST_FOREACH(int cluster_id, cluster_id_seq) {
    LOG(INFO) << "cluster_id:" << cluster_id;   
    if (std::find(friend_seq.begin(), friend_seq.end(), cluster_id) == friend_seq.end()) {
      if (cluster_id != test_id) {
        recommend_id_seq.push_back(cluster_id);
      }
    }
  }
  BOOST_FOREACH(int r_id, recommend_id_seq) {
    LOG(INFO) << "r_id:" << r_id;
  }
  LOG(INFO) << "recommend_id_seq.size():" << recommend_id_seq.size();


  MyUtil::Int2IntSeqMap map;
  try {
    map = BuddyByIdCacheAdapter::instance().getFriendLists(recommend_id_seq);
  } catch (...) {
    LOG(LERROR) << "BuddyByIdCacheAdapter::instance().getFriendLists() exception ... ";
  }
  
  ASSERT_NE(0, recommend_id_seq.size());
  Items items;
  BOOST_FOREACH(int r_id, recommend_id_seq) {
    MyUtil::IntSeq common_friend_seq;
    MyUtil::Int2IntSeqMap::const_iterator it = map.find(r_id);
    if (it != map.end()) {
      LOG(INFO) << "friend_seq.size():" << friend_seq.size() << "\tit->second.size():" << it->second.size();
      xce::Intersection(friend_seq, it->second, common_friend_seq);
    }
    LOG(INFO) << "test_id:" << test_id << "\tr_id:" << r_id << "\tcommon_friend_seq.size():" << common_friend_seq.size();

    Item* item = items.add_items();
    item->set_id(r_id);
    item->set_field(common_friend_seq.size());
    for (int i = 0; i < common_friend_seq.size() && i < 10; ++i) {
      item->add_fields(common_friend_seq[i]);
    }
  }
  // 保存
  int byte_size = items.ByteSize();
  unsigned char* chs = new unsigned char[byte_size];
  items.SerializeToArray(chs, byte_size);
  std::vector<unsigned char> v;
  v.reserve(byte_size);
  v.assign(chs, chs + byte_size);
  delete []chs;
//  MemcachedAdapter::GetInstance().Set(xce::Comb2("FFUCR", test_id), v);
}

TEST(FFUCR, GET) {
  using namespace xce::buddy::adapter;
  using namespace xce::socialgraph;
  int test_id = 266235977;
  MyUtil::IntSeq friend_seq;
  try {
    friend_seq = BuddyByIdCacheAdapter::instance().getFriendListAsc(test_id, 2000);
    LOG(INFO) << "test_id:" << test_id << "\tfriend_seq.size():" << friend_seq.size();
  } catch (...) {
    LOG(LERROR) << "BuddyByIdCacheAdapter::instance().getFriendLists() exception ... ";
  }
  std::vector<unsigned char> v;
  Items items;
  MemcachedAdapter::GetInstance().Get(xce::Comb2("FFUCR", test_id), v);
  items.ParseFromArray(v.data(), v.size());
  std::vector<int> ffucr_seq;
  for (int i = 0; i < items.items_size(); ++i) {
    int id = items.items(i).id();
    ffucr_seq.push_back(id);
    bool is_friend = false;
    if (std::find(friend_seq.begin(), friend_seq.end(), id) != friend_seq.end()) {
      is_friend = true;
    }
    LOG(INFO) << "test_id:" << test_id << "item id:" << id << "\tis_friend:" << std::boolalpha << is_friend;
  }
}

}}



