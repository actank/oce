#include "socialgraph/friendcluster/compute/computethread.h"
#include <fstream>
#include <set>
#include <algorithm>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "base/logging.h"
#include "base/ptime.h"

#include "OceCxxAdapter/src/BuddyByIdReplicaAdapter.h"

#include "socialgraph/socialgraphproto/friendcluster.pb.h"
#include "socialgraph/socialgraphproto/item.pb.h"
#include "socialgraph/socialgraphutil/mcclient.h"
#include "socialgraph/socialgraphutil/algostuff.h"

namespace xce {
namespace socialgraph {

struct CPerson {
  CPerson(int i, int w) :
    id_(i), weight_(w) {
  }
  int id_;
  int weight_;
};

struct CFriendCluster {
  CFriendCluster(const std::string& n, int w, int index) :
    name_(n), weight_(w), index_(index) {
  }
  std::string name_;
  int weight_;
  int index_;
};



int CompareCPerson(const CPerson& cp1, const CPerson& cp2) {
  return cp1.weight_ > cp2.weight_;
}

int CompareCFriendCluster(const CFriendCluster& cf1, const CFriendCluster& cf2) {
  return cf1.weight_ > cf2.weight_;
}


ClusterNameIndex::ClusterNameIndex(std::string filePath) : nameIndexFilePath_(filePath) {
  PTIME(pt, "ClusterNameIndex::ClusterNameIndex()", true, 0);
  std::ifstream in(nameIndexFilePath_.c_str());
  if (!in) {
		LOG(LERROR) << " cannot open " << "file:" << nameIndexFilePath_;
    exit(1);
	}
  std::string line;
  std::getline(in, line);
  while (!line.empty()) {
    std::vector<std::string> units;
    boost::split(units, line, boost::is_any_of("\t"));
		if (units.size() == 2) {
			std::string clusterName = units[0];
      try {
				int clusterId = boost::lexical_cast<int>(units[1]);
        clusterNameIndexMap_.insert(std::make_pair(clusterName, clusterId));
			} catch (...) {	}
		}
  	std::getline(in, line);
	}

  LOG(INFO) << "clusterNameIndexMap_.size():" << clusterNameIndexMap_.size();
}

bool ClusterNameIndex::GetClusterNameIndex(const std::string& name, int& v) {
	std::hash_map<std::string, int>::const_iterator pos = clusterNameIndexMap_.find(name);
  if (pos != clusterNameIndexMap_.end()) {
		v = pos->second;
    return true;
	}	
  return false;
}

//-----------------------------------------------------------------------------------
ClusterComputeMessage::ClusterComputeMessage(const std::string line, boost::shared_ptr<ClusterNameIndex>& ptr) : line_(line) {
  clusterNameIndexPtr_ = ptr;
}

ClusterComputeMessage::~ClusterComputeMessage() {

}

void ClusterComputeMessage::Run() {
	PTIME(pt, "ClusterComputeMessage::Run()", true, 0);
  if (!line_.empty())
  	SaveOneCluster(line_);
}


void ClusterComputeMessage::SaveOneCluster(const std::string& line) {
	std::vector<std::string> units;
  boost::split(units, line, boost::is_any_of("\t"));

  if (units.size() == 3) {
    std::string clusterName = units[0];
    
    //取一下clusterid
    int clusterId = 0;
    if (!clusterNameIndexPtr_->GetClusterNameIndex(clusterName, clusterId)) {
			LOG(LERROR) << "clusterName:" << clusterName << " cannot fetch clusterId";
      return;
		}

    int clusterWeight = 0;
    try {
      clusterWeight = (int) boost::lexical_cast<double>(units[1]);
    } catch (...) {
			LOG(LERROR) << "cast error clusterName:" << clusterName;
    }
		std::string personsInfo = units[2];
    std::vector<std::string> personUnits;
    std::vector<CPerson> cPersons;
    boost::split(personUnits, personsInfo, boost::is_any_of(";"));
    for (size_t i = 0; i < personUnits.size(); ++i) {
      std::string& personInfo = personUnits[i];
      if (!personInfo.empty()) {
        std::vector<std::string> infos;
        boost::split(infos, personInfo, boost::is_any_of(","));
        if (infos.size() == 2) {
          int id = 0;
          int weight = 0;
          try {
            id = boost::lexical_cast<int>(infos[0]);
            weight = boost::lexical_cast<int>(infos[1]);
            CPerson p(id, weight);
            cPersons.push_back(p);
          } catch (...) {
            continue;
          }
        }
      }
    }
    if (!cPersons.empty()) {
      std::sort(cPersons.begin(), cPersons.end(), CompareCPerson);
    }
    //开始持久化
    xce::socialgraph::FriendCluster friendCluster;
    friendCluster.set_weight(clusterWeight);
    std::vector<CPerson>::const_iterator iter = cPersons.begin();
    for (; iter != cPersons.end(); ++iter) {
      friendCluster.add_useridseq(iter->id_);
    }

    //保存
    const std::string mckey = xce::Comb("FFCU_", clusterId);
    int size = friendCluster.ByteSize();
    unsigned char* chs = new unsigned char[size];
    friendCluster.SerializeToArray(chs, size);
    std::vector<unsigned char> v;
    v.reserve(size);
    v.assign(chs, chs + size);
    xce::MemcachedClient::GetInstance().SetByDistCache(mckey, v);
    LOG(INFO) << "save clusterId:" << clusterId << " clusterName:" << clusterName << "  mckey:" << mckey;
		delete []chs;
	
  }
}

//-----------------------------------------------

Uid2ClusterComputeMessage::Uid2ClusterComputeMessage(const std::string& line, boost::shared_ptr<ClusterNameIndex>& ptr) : line_(line) {
  clusterNameIndexPtr_ = ptr;
	Init();
}

Uid2ClusterComputeMessage::~Uid2ClusterComputeMessage() {

}

void Uid2ClusterComputeMessage::Init() {

}

void Uid2ClusterComputeMessage::Run() {
	SaveOneUserCluster(line_);
}

void Uid2ClusterComputeMessage::SaveOneUserCluster(const std::string& line) {
	std::vector<std::string> units;
  boost::split(units, line, boost::is_any_of("\t"));

	if (units.size() == 2) {
		int uid = 0;
    try {
			uid = boost::lexical_cast<int>(units[0]);
		} catch (...) {
			return;
		}	
		std::vector<CFriendCluster> cFriendClusters;
		std::vector<std::string> clusterUnits;
    boost::split(clusterUnits, units[1], boost::is_any_of(";"));
		for (size_t i = 0; i < clusterUnits.size(); ++i) {
			std::string& clusterUnit = clusterUnits[i];
 			if (!clusterUnit.empty()) {
				std::vector<std::string> infos;
        boost::split(infos, clusterUnit, boost::is_any_of("|"));
				if (infos.size() == 2) {
					std::string& clusterName = infos[0];
          int clusterId = 0;
       		if (!clusterNameIndexPtr_->GetClusterNameIndex(clusterName, clusterId)) {
						LOG(LERROR) << "clusterName:" << clusterName << " cannot fetch clusterId";
            continue;
					}   
					int weight = 0;
					try {
						weight = (int) boost::lexical_cast<double>(infos[1]);
					} catch (...) {
						continue;
					}				
					CFriendCluster cf(clusterName, weight, clusterId);
          cFriendClusters.push_back(cf);
				}
			}     
		}

		if (!cFriendClusters.empty()) 
				std::sort(cFriendClusters.begin(), cFriendClusters.end(), CompareCFriendCluster);

		xce::socialgraph::Person person;

		std::vector<CFriendCluster>::const_iterator iter = cFriendClusters.begin();
		for (; iter != cFriendClusters.end(); ++iter) {
      person.add_clusterindexseq(iter->index_);
		}

		char chs[person.ByteSize()];
    bool flag = person.SerializeToArray(chs, person.ByteSize());
		std::vector<unsigned char> data;
    data.reserve(person.ByteSize());
    data.assign(chs, chs + person.ByteSize());
    if (flag) {
			const static std::string kBizName = "FFUC";
      std::ostringstream oss;
      oss << kBizName << "_" << uid;
      const std::string mckey = oss.str();
      LOG(INFO) << "mckey:" << mckey << "  data.size():" << data.size();
			xce::MemcachedClient::GetInstance().SetByDistCache(mckey, data);
    }
		else {
			LOG(LERROR) << "SerializeToString failed! uid:" << uid;
		}
	}
}

//====================================================================================


void FriendClusterRecommendMessage::Run() {
  // 1.取出用户hid所在的好友簇的全部用户
  // 2.将1的数据过滤掉用户hid的好友,即为可向用户hid推荐的人,结果为推荐列表
  // 3.算出推荐列表中每个人与用户hid的共同好友数, 保存数据到cache
  using namespace xce::buddy::adapter;
  std::ostringstream oss;
  oss << "FriendClusterRecommendedMessage::Run() id_:" << hid_;
	PTIME(pt, oss.str(), true, 0);

  std::vector<int> clusterUserSeq;
  if (!GetFriendClusterUser(hid_, clusterUserSeq)) {
		return;
  }

  //取用户hid的好友列表
  std::vector<int> hidFriends;
  try {
    hidFriends = BuddyByIdCacheAdapter::instance().getFriendListAsc(hid_, 2000);
  } catch (...) { 
    LOG(LERROR) << "BuddyByIdCacheAdapter::instance().getFriendListAsc() exception hid_:" << hid_;
  }
  
  if (hidFriends.empty()) {
    return;
  }
  // 从clusterUserSeq中过滤掉用户hid的好友
  std::vector<int> recommendSeq;
	for (size_t i = 0; i < clusterUserSeq.size(); ++i) {
    int p = clusterUserSeq[i];
    if (hidFriends.end() == std::find(hidFriends.begin(), hidFriends.end(), p) && p != hid_) {
			recommendSeq.push_back(p);
    }
  }

  if (recommendSeq.empty()) {
    return;
  }

  //计算共同好友
  xce::socialgraph::Items items;
  for (size_t i = 0; i < recommendSeq.size(); ++i) {
    int recommend_id = recommendSeq[i];
    std::vector<int> recommend_id_friend_seq;
    try {
      recommend_id_friend_seq = BuddyByIdCacheAdapter::instance().getFriendListAsc(recommend_id, 2000);
    } catch (...) {
      LOG(LERROR) << "BuddyByIdReplicaAdapter::instance().getFriendListAsc() exception... recommend_id:" << recommend_id;
    }
    std::vector<int> intersection;
 		xce::Intersection(hidFriends, recommend_id_friend_seq, intersection);     
    LOG(INFO) << "hid_:" << hid_ << " recommend_id:" << recommend_id 
              << " intersection.size():" << intersection.size() << " recommendSeq.size():" << recommendSeq.size();

    int common_count = intersection.size();
    
    xce::socialgraph::Item* item = items.add_items();
    item->set_id(recommend_id);
    item->set_field(common_count);
    int field_loop = std::min(common_count, 10);
    for(int i = 0; i < field_loop; ++i) {
 			item->add_fields(intersection[i]);     
    }
  } 
  
 	//保存到cache
  const std::string mckey = xce::Comb("FFUCR_", hid_);
  LOG(INFO) << "mckey:" << mckey << "\titems.ByteSize():" << items.ByteSize();
  int byte_size = items.ByteSize();
  char ch[byte_size];
  items.SerializeToArray(ch, byte_size);
  std::vector<unsigned char> data;
  data.reserve(byte_size);
  data.assign(ch, ch + byte_size);
  xce::MemcachedClient::GetInstance().SetByDistCache(mckey, data);
}
 
bool FriendClusterRecommendMessage::GetFriendClusterUser(int userId, std::vector<int>& userSeq) {
  std::ostringstream oss;
  oss << "FFUC_" << userId;
  std::vector<unsigned char> data;
  xce::MemcachedClient::GetInstance().GetByDistCache(oss.str(), data);
  xce::socialgraph::Person person;
  person.ParseFromArray(data.data(), data.size());
  std::vector<int> clusterIndexSeq;
  for (int i = 0; i < person.clusterindexseq_size(); ++i) {
    clusterIndexSeq.push_back(person.clusterindexseq(i));
  }

  if (clusterIndexSeq.empty()) {
    return false;
  }
  
  std::set<int> userSet;
  for (size_t i = 0; i < clusterIndexSeq.size(); ++i) {
    std::string s = xce::Comb("FFCU_", clusterIndexSeq[i]);
    std::vector<unsigned char> d;
    xce::MemcachedClient::GetInstance().GetByDistCache(s, d);
    FriendCluster fc;
    fc.ParseFromArray(d.data(), d.size());
    for (int i = 0; i < fc.useridseq_size(); ++i) {
      int uid = fc.useridseq(i);
      if (userSet.find(uid) == userSet.end()) {
        userSet.insert(uid); 
        userSeq.push_back(uid);
      }
    }
 
  }  
  //LOG(INFO) << "userSet.size():" << userSet.size() << "  userSeq.size():" << userSeq.size();
  if (!userSet.empty()) {
    return true;
  }
  return false;
}

}}

