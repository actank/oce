
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "base/logging.h"
#include "base/ptime.h"
#include "base/threadpool.h"
#include "base/logrotate.h"

#include "OceSlice/slice/FriendClusterCache.h"
#include "OceCxxAdapter/src/FriendClusterCacheAdapter.h"
#include "OceCxxAdapter/src/BuddyByIdReplicaAdapter.h"
#include "OceCxxAdapter/src/BuddyFriendsAdapter.h"

#include "OceCxxAdapter/src/BuddyCoreAdapter.h"

#include "socialgraph/socialgraphutil/mc_adapter.h"
#include "socialgraph/socialgraphutil/algostuff.h"
#include "socialgraph/socialgraphproto/item.pb.h"
#include "socialgraph/socialgraphutil/dbutil.h"

namespace xce {
namespace socialgraph {

class FFUCRMessage : public xce::Message {
public:
  FFUCRMessage(int uid): uid_(uid) {

  }

  virtual ~FFUCRMessage() {

  }
  void Run() {
    //1,取好友簇数据
    //2,取uid好友列表
    //3,计算推荐人。过滤掉好友，过滤掉黑名单,过滤掉sg_block
    //4,组合Items数据结构
    //5,保存数据到cache
    std::ostringstream oss;
    oss << "FFUCRMessage.Run(" << uid_ << ")";
    PTIME(pt, oss.str(), true, 0);

    MyUtil::IntList uid_list;
    uid_list.push_back(uid_);
    FriendClusterData data;
    try {
      data = FriendClusterCacheAdapter::instance().Get(uid_list);
    } catch (...) {
      LOG(LERROR) << "FriendClusterCacheAdapter::instance().Get() exception..." << "uid_:" << uid_;
      return;
    }
    MyUtil::IntSeq cluster_id_seq;
    Str2IntListMap::const_iterator it2 = data.data2.begin();
    for (; it2 != data.data2.end(); ++it2) {
      BOOST_FOREACH(int i, it2->second) {
        cluster_id_seq.push_back(i);
      }
    }
    MyUtil::IntSeq friend_seq;
    try {
      friend_seq = xce::buddy::adapter::BuddyByIdCacheAdapter::instance().getFriendListAsc(uid_, 2000);
    } catch (...) {
      LOG(LERROR) << "BuddyByIdCacheAdapter::instance().getFriendLists() exception ... " << "uid_:" << uid_;
      return;
    } 

    MyUtil::IntSeq recommend_id_seq;
    BOOST_FOREACH(int cluster_id, cluster_id_seq) {
      if (std::find(friend_seq.begin(), friend_seq.end(), cluster_id) == friend_seq.end()) {
        if (cluster_id != uid_) {
          recommend_id_seq.push_back(cluster_id);
        }
      }
    }
    if (recommend_id_seq.empty()) {
      LOG(INFO) << "recommend_id_seq is empty!" << "\tuid_:" << uid_;
      return;
    }
    std::set<int> filter;
    std::vector<int> block_seq;
    try {
      block_seq = com::xiaonei::service::buddy::BuddyCoreAdapter::instance().getBlockSeq(uid_, 0, -1);
    } catch (...) {
      LOG(LERROR) << "BuddyCoreAdapter.getBlockSeq ... exception uid_:" << uid_;
    }
    filter.insert(recommend_id_seq.begin(), recommend_id_seq.end());
    BOOST_FOREACH(int block, block_seq) {
      filter.erase(block);
    }
    recommend_id_seq.clear();
    recommend_id_seq.insert(recommend_id_seq.end(), filter.begin(), filter.end());

    MyUtil::Int2IntSeqMap map;
    try {
      map = xce::buddy::adapter::BuddyByIdCacheAdapter::instance().getFriendLists(recommend_id_seq);
    } catch (...) {
      LOG(LERROR) << "BuddyByIdCacheAdapter::instance().getFriendLists() exception ... " << "uid_:" << uid_;
      return;
    }

    Items items;
    BOOST_FOREACH(int r_id, recommend_id_seq) {
      MyUtil::IntSeq common_friend_seq;
      MyUtil::Int2IntSeqMap::const_iterator it = map.find(r_id);
      if (it != map.end()) {
        xce::Intersection(friend_seq, it->second, common_friend_seq);
      }
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
    MemcachedAdapter::GetInstance().Set(xce::Comb2("FFUCR", uid_), v);
  }
private:
   int uid_;
};

}}


namespace xce {
namespace socialgraph {

struct FriendClusterUnit_S {
  int user_id_;
  int u_weight_;
};

struct FriendCluster_S {
  std::string c_name_;
  double c_weight_;
  std::vector<FriendClusterUnit_S> units_;
};

void LoadClusterData(std::ifstream* in, std::hash_map<std::string, FriendCluster_S>* cluster_data_map) {
  PTIME(pt, "LoadClusterData", true, 0);
  std::string line; //1-1070-1997-0	1.5	305148731,3;312498668,1;743864307,1;744248339,1;
  std::getline(*in, line);
  do {
    std::vector<std::string> splits;
    boost::split(splits, line, boost::is_any_of("\t"));
    if (splits.size() == 3) {
      FriendCluster_S fc;
      fc.c_name_ = splits[0];
      try {
        fc.c_weight_ = boost::lexical_cast<double>(splits[1]);
      } catch(...) {  
        fc.c_weight_ = 0;
      }
      std::vector<std::string> sub_splits;
      boost::split(sub_splits, splits[2], boost::is_any_of(";"));
      BOOST_FOREACH(const std::string& s, sub_splits) {
        std::vector<std::string> seq;
        boost::split(seq, s, boost::is_any_of(","));
        if (seq.size() == 2) {
          FriendClusterUnit_S fcu;
          try {
            fcu.user_id_ = boost::lexical_cast<int>(seq[0]);
            fcu.u_weight_ = boost::lexical_cast<int>(seq[1]);
          } catch (...) {
            continue;
          }
          fc.units_.push_back(fcu);
        }
      }
      cluster_data_map->insert(std::make_pair(fc.c_name_, fc));
    } 
    line.clear();
    std::getline(*in, line);
  } while(!line.empty());

  LOG(INFO) << "cluster_data_map->size():" << cluster_data_map->size();
}

void LoadUid2ClusterData(std::ifstream* in, std::hash_map<int, std::vector<std::string> >* uid2clustername_map) {
  PTIME(pt, "LoadUid2ClusterData", true, 0);
  std::string line; //100577418	99-1044-2005-6|11;99-1044-2005-3|11;99-1044-2005-9|11;99-1044-2005-8|11;
  std::getline(*in, line);
  do {
    std::vector<std::string> splits;
    boost::split(splits, line, boost::is_any_of("\t"));
    if (splits.size() == 2) {
      int uid = -1;
      std::vector<std::string> cname_seq;
      try {
        uid = boost::lexical_cast<int>(splits[0]);
        std::vector<std::string> sub_splits;
        boost::split(sub_splits, splits[1], boost::is_any_of(";"));
        BOOST_FOREACH(const std::string& s, sub_splits) {
          std::vector<std::string> seq;
          boost::split(seq, s, boost::is_any_of("|"));
          if (seq.size() == 2) {
            cname_seq.push_back(seq[0]);
          }
        }
      } catch (...) {

      }
      uid2clustername_map->insert(std::make_pair(uid, cname_seq));
    }
    line.clear();
    std::getline(*in, line);
  } while(!line.empty());
  LOG(INFO) << "uid2clustername_map->size():" << uid2clustername_map->size();
}


class ComputeMessage : public xce::Message {
 public:
  ComputeMessage(int uid, 
                 std::hash_map<std::string, FriendCluster_S>* cd_map, 
                 std::hash_map<int, std::vector<std::string> >* uc_map) : user_id_(uid), cluster_data_map_(cd_map), uid2clustername_map_(uc_map) {

  }
 
  virtual ~ComputeMessage() { }

  void Run() {
    //1,取好友簇数据
    //2,取uid好友列表
    //3,计算推荐人。过滤掉好友，过滤掉黑名单,过滤掉sg_block
    //4,组合Items数据结构
    //5,保存数据到cache
    std::ostringstream otem;
    otem << "ComputeMessage::Run() user_id_" << user_id_;
    PTIME(pt, otem.str(), true, 0);

    std::hash_map<int, std::vector<std::string> >::iterator it1 = uid2clustername_map_->find(user_id_);
    std::vector<std::string> c_name_seq;
    if (it1 != uid2clustername_map_->end()) {
      c_name_seq = it1->second;
    }

    if (c_name_seq.empty()) {
      LOG(INFO) << "user_id:" << user_id_ << " cluster_name_seq's size is 0";
      return;
    }
    std::set<int> cluster_user_set;
    BOOST_FOREACH(const std::string& c_name, c_name_seq) {
      std::hash_map<std::string, FriendCluster_S>::iterator it2 = cluster_data_map_->find(c_name);
      if (it2 != cluster_data_map_->end()) {
        BOOST_FOREACH(const FriendClusterUnit_S& fcu, (it2->second).units_) {
          cluster_user_set.insert(fcu.user_id_);
        }
      }
    }

    if (cluster_user_set.empty()) {
      LOG(INFO) << "user_id:" << user_id_ << " cluster_user_set is 0";
      return;
    }
    
    std::set<int> block_set;
    MyUtil::IntSeq friend_seq;
    try {
      friend_seq = xce::buddy::adapter::BuddyByIdCacheAdapter::instance().getFriendListAsc(user_id_, 2000);
      block_set.insert(friend_seq.begin(), friend_seq.end());
    } catch (...) {
      LOG(LERROR) << "BuddyByIdCacheAdapter::instance().getFriendListAsc() exception ... " << "user_id_:" << user_id_;
      return;
    }

    try {
      std::vector<int> block_seq;
      block_seq = com::xiaonei::service::buddy::BuddyCoreAdapter::instance().getBlockSeq(user_id_, 0, -1);
      block_set.insert(block_seq.begin(), block_seq.end());
    } catch (...) {
      LOG(LERROR) << "BuddyCoreAdapter.getBlockSeq ... exception user_id_:" << user_id_;
      return;
    }
    
    std::vector<int> sg_block_seq = DbUtil::GetInstance().GetSgBlockListByBiz(user_id_, "FFUCR");
    if (!sg_block_seq.empty()) {
      block_set.insert(sg_block_seq.begin(), sg_block_seq.end());
    }

    //做cluster_user_set中去掉block_set的人
    std::vector<int> rcd_seq;
    BOOST_FOREACH(int c_id, cluster_user_set) {
      std::set<int>::iterator it3 = block_set.find(c_id);
      if (it3 == block_set.end()) {
        rcd_seq.push_back(c_id);
      }
    }

    if (rcd_seq.empty()) {
      LOG(INFO) << "user_id_:" << user_id_ << " rcd_seq is 0";
      return;
    }

    //组装Items数据
    Items items;
    BOOST_FOREACH(int rcd, rcd_seq) {
      int common_friend_size = -1;
      std::vector<int> common_friend_list;
      try {
        MyUtil::IntSeq cf = xce::buddy::adapter::BuddyFriendsCacheAdapter::instance().getCommonFriends(user_id_, rcd);
        common_friend_size = cf.size();
        if (common_friend_size > 10) {
          for (size_t i = 0; i < 10; ++i) {
            common_friend_list.push_back(cf[i]);
          }
        }
        else {
          common_friend_list = cf;
        }
      } catch(...) {
        continue;
      }
      Item* item = items.add_items();
      item->set_id(rcd);
      item->set_field(common_friend_size);
      BOOST_FOREACH(int cid, common_friend_list) {
        item->add_fields(cid);
      }
    }   
    //存储Cache   
    int byte_size = items.ByteSize();   
    std::string s;
    items.SerializeToString(&s);
    std::vector<unsigned char> bin_data;
    bin_data.reserve(byte_size);
    bin_data.assign(s.data(), s.data() + byte_size);
    const static std::string kBiz = "FFUCR";
    BusinessCacheAdapter::GetInstance().Set(kBiz, user_id_, bin_data);
    LOG(INFO) << "RCD INFO user_id_" << user_id_ << " rcd_seq.size():" << rcd_seq.size();
  }

 private:
  int user_id_;
  std::hash_map<std::string, FriendCluster_S>* cluster_data_map_;
  std::hash_map<int, std::vector<std::string> >* uid2clustername_map_;
};

}}


int main(int argc, char **argv) {
  using namespace xce::socialgraph;
  PTIME(pt, "main", true, 0);
	LOG(INFO) << "argc:" << argc;
  if (argc != 3) {
    LOG(INFO) << "Usage: " << argv[0] << " cluster_data uid2cluster_data";
    exit(0);
  }
  // load cluster_data file
  std::ifstream cluster_data_in(argv[1]);
  if (!cluster_data_in) {
    LOG(LERROR) << "cannot open cluster_data file:" << argv[1];
    exit(1);
  }
  
  std::hash_map<std::string, FriendCluster_S> cluster_data_map;
  LoadClusterData(&cluster_data_in, &cluster_data_map);

  //std::hash_map<std::string, FriendCluster_S>::iterator it = cluster_data_map.begin();
  //for (; it != cluster_data_map.end(); ++it) {
  //  LOG(INFO) << it->second.c_name_ << "\t" << it->second.c_weight_;
  //  BOOST_FOREACH(const FriendClusterUnit_S& fcu, it->second.units_) {
  //    LOG(INFO) << "\t" << fcu.user_id_ << "\t" << fcu.u_weight_;
  //  }
  //}

  std::ifstream uid2clustername_in(argv[2]);
  if (!uid2clustername_in) {
    LOG(LERROR) << "cannot open uid2clusterdata file: " << argv[2];
    exit(1);
  }

  std::hash_map<int, std::vector<std::string> > uid2clustername_map;
  LoadUid2ClusterData(&uid2clustername_in, &uid2clustername_map);

  //std::hash_map<int, std::vector<std::string> >::iterator iter = uid2clustername_map.begin();
  //for (; iter != uid2clustername_map.end(); ++iter) {
  //  LOG(INFO) << "uid:" << iter->first;
  //  BOOST_FOREACH(const std::string& s, iter->second) {
  //    LOG(INFO) << "\t" << s;
  //  }
  //}

	xce::ThreadPool thread_pool(20, 60);
  std::hash_map<int, std::vector<std::string> >::iterator iter = uid2clustername_map.begin();
  for (; iter != uid2clustername_map.end(); ++iter) {
    xce::Message* msg = new xce::socialgraph::ComputeMessage(iter->first, &cluster_data_map, &uid2clustername_map);
    thread_pool.Post(msg);
  }
   
  sleep(3);
  thread_pool.Stop();
  thread_pool.Join();
  return 0;
}
  /*
  PTIME(pt, "main", true, 0);
	LOG(INFO) << "argc:" << argc;
	if (argc < 2) {
		LOG(INFO) << "uasge: ./ffucr_compute uid.txt";
		exit(-1);
	}

  std::ifstream in(argv[1]);
  if (!in) {
    LOG(LERROR) << "cannot open file:" << argv[1];
    exit(1);
  }

  std::string line;
  std::list<int> _uidseq;
  do {
    line.clear();
    std::getline(in, line);
    try {
      int uid = boost::lexical_cast<int>(line);
      _uidseq.push_back(uid);
    } catch (...) {
      continue;
    }
  } while(!line.empty());
  LOG(INFO) << "_uidseq.size():" << _uidseq.size();
  
	xce::ThreadPool threadPool(20, 60);
  BOOST_FOREACH(int id, _uidseq) {
    xce::Message* message = new FFUCRMessage(id);
    threadPool.Post(message);

  }
  sleep(3);
  threadPool.Stop();
  threadPool.Join();
  return 0;
  */
