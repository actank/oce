#include "friendclustercache.h"
#include <vector>
#include <list>
#include <algorithm>
#include <fstream>

#include <boost/thread/locks.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "util/cpp/TimeCost.h"

void MyUtil::initialize() {
  using namespace MyUtil;
  using namespace com::xiaonei::xce;
  using namespace xce::serverstate;
  using namespace xce::socialgraph;
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&FriendClusterCacheI::instance(), service.createIdentity("FFUCA", ""));

  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("FriendClusterCache.Mod");
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FriendClusterCache.Interval", 5);

  static const std::string kControllerName = "ControllerFriendClusterCache";
  // register to Controller Service
  ServerStateSubscriber::instance().initialize(kControllerName,  &FriendClusterCacheI::instance(), mod, interval);
  ServerStateSubscriber::instance().setStatus(1);
}

namespace xce {
namespace socialgraph {

static const std::string kClusterDataFileLocation = "/data/xce/Project/friendcluster/data/cluster_data";
static const std::string kUid2ClusterDataFileLocation = "/data/xce/Project/friendcluster/data/uid2cluster_data";

FriendClusterCacheI::FriendClusterCacheI() : start_(false) {
  
}

void FriendClusterCacheI::Init() {
  MyUtil::TimeCost tc = MyUtil::TimeCost::create("FriendClusterCacheI::FriendClusterCacheI()", MyUtil::TimeCost::LOG_LEVEL_INFO);
  std::ifstream cluster_data_in(kClusterDataFileLocation.c_str());
  if (!cluster_data_in) 
    MCE_WARN("can not load file:" << kClusterDataFileLocation);

  std::ifstream uid2cluster_data_in(kUid2ClusterDataFileLocation.c_str());
  if (!uid2cluster_data_in)
    MCE_WARN("can not load file:" << kUid2ClusterDataFileLocation);   


  int line_count = 0;
  //处理cluster_data_file
  std::string line;
  std::getline(cluster_data_in, line);
  while (!line.empty()) {
    ++line_count;
    SaveCluster(line);
    line.clear();
    std::getline(cluster_data_in, line);
  }
  MCE_INFO(kClusterDataFileLocation << " line_count:" << line_count << " cname_to_id_map_.size():" << cname_to_id_map_.size());
  cluster_data_in.close();
  tc.step("ClusterData Init Done", true);

  line_count = 0;
  line.clear();
  //处理uid2cluster_data_file
  std::getline(uid2cluster_data_in, line);
  while (!line.empty()) {
    ++line_count;
    SaveUidCluster(line);
    line.clear();
    std::getline(uid2cluster_data_in, line);
  }
  MCE_INFO(kUid2ClusterDataFileLocation << " line_count:" << line_count << " id_to_cname_map_.size():" << id_to_cname_map_.size());
  uid2cluster_data_in.close();
  tc.step("Uid2ClusterData Init Done", true);

  start_ = true;
}

void FriendClusterCacheI::SaveCluster(const std::string& line) {
  //1-1-2009-0	3.000000	315004597,1;316368102,5;
  std::vector<std::string> units;
  boost::split(units, line, boost::is_any_of("\t"));
  if (units.size() != 3) {
    MCE_INFO("error format line:" << line);
    return;
  }

  std::string cname = units[0];
  std::vector<int> cid_seq;
  std::vector<CPerson> cperson_seq;
  
  std::vector<std::string> person_units;
  boost::split(person_units, units[2], boost::is_any_of(";"));
  BOOST_FOREACH(std::string& s, person_units) {
    std::vector<std::string> info_units;
    boost::split(info_units, s, boost::is_any_of(","));
    if (info_units.size() == 2) {
      CPerson cp;
      try {
        cp.id_ = boost::lexical_cast<int>(info_units[0]);
        cp.weight_ = boost::lexical_cast<int>(info_units[1]);
      } catch(...) {
        continue;
      }
      cperson_seq.push_back(cp);
    }
  }
  std::sort(cperson_seq.begin(), cperson_seq.end(), CompareCPerson);
  BOOST_FOREACH(CPerson& cp, cperson_seq) {
    cid_seq.push_back(cp.id_);
  }
  //保存地本地
  cname_to_id_map_.insert(std::make_pair(cname, cid_seq));
}

void FriendClusterCacheI::SaveUidCluster(const std::string& line) {
  //100001395    9-9-2004-50|13.67;9-9-2004-32|6.83;9-9-2004-33|6.83;
  std::vector<std::string> units;
  boost::split(units, line, boost::is_any_of("\t"));
  if (units.size() != 2) {
    MCE_INFO("error format line:" << line);
    return;
  }

  int user_id;
  std::vector<std::string> cname_seq;
  try {
    user_id = boost::lexical_cast<int>(units[0]);
  } catch (...) {
    MCE_WARN("boost::lexical_cat exception" << "\t" << "line:" << line);
    return;
  }

  std::vector<std::string> cname_units;
  boost::split(cname_units, units[1], boost::is_any_of(";"));
  BOOST_FOREACH(const std::string& s, cname_units) {
    std::vector<std::string> info_units;
    boost::split(info_units, s, boost::is_any_of("|"));
    if (info_units.size() == 2) {
      cname_seq.push_back(info_units[0]);
    }
  }  

  id_to_cname_map_.insert(std::make_pair(user_id, cname_seq));
}
 

FriendClusterData FriendClusterCacheI::GetFriendClusterData(const MyUtil::IntList& idList, const Ice::Current& current) {
	FriendClusterData friend_cluster_data;
  boost::lock_guard<boost::mutex> guard(mutex_);
  if (!start_) {
    MCE_INFO("FriendClusterCacheI Service is Init...");
    Init();
    return friend_cluster_data;
  }
  MCE_INFO("FriendClusterCacheI idList.size():" << idList.size());
  BOOST_FOREACH(int id, idList) {
    std::hash_map<int, std::vector<std::string> >::const_iterator iter1 = id_to_cname_map_.find(id);
    if (iter1 != id_to_cname_map_.end()) {
      friend_cluster_data.data1.insert(std::make_pair(iter1->first, iter1->second));
    }
  }

  std::set<std::string> cname_set; 
  Int2StrListMap::const_iterator it = friend_cluster_data.data1.begin();
  for (; it != friend_cluster_data.data1.end(); ++it) {
    cname_set.insert(it->second.begin(), it->second.end());
  }

  BOOST_FOREACH(const std::string& c, cname_set) {
    std::hash_map<std::string, std::vector<int> >::const_iterator iter2 = cname_to_id_map_.find(c);
    if (iter2 != cname_to_id_map_.end()) {
      friend_cluster_data.data2.insert(std::make_pair(iter2->first, iter2->second));
    }
  }

  MCE_INFO("FriendClusterCacheI::GetFriendClusterData data1.size():" << friend_cluster_data.data1.size() << " data2.size():" << friend_cluster_data.data2.size());
  return friend_cluster_data;
}


MyUtil::StrSeq FriendClusterCacheI::GetC(int id, const ::Ice::Current& current) {
  boost::lock_guard<boost::mutex> guard(mutex_);
  MyUtil::StrSeq str_seq;
  std::hash_map<int, std::vector<std::string> >::const_iterator it = id_to_cname_map_.find(id); 
  if (it != id_to_cname_map_.end()) {
    str_seq = it->second;
    MCE_INFO("GetC" << "\t" << "id:" << id << "\t" << "str_seq.size():" << str_seq.size());
  }
  else {
    MCE_INFO("GetC" << "\t" << "id:" << id << " is not in any cluster!");
  }
  return str_seq;
}
MyUtil::IntSeq FriendClusterCacheI::GetU(const string& cname, const ::Ice::Current& current) {
  boost::lock_guard<boost::mutex> guard(mutex_);
  MyUtil::IntSeq id_seq;
  std::hash_map<std::string, std::vector<int> >::const_iterator it = cname_to_id_map_.find(cname);
  if (it != cname_to_id_map_.end()) {
    id_seq = it->second;
    MCE_INFO("GetU" << "\t" << "cname:" << cname << "\t" << "id_seq.size():" << id_seq.size());
  }
  else {
    MCE_INFO("GetU" << "\t" << "no cluster name is " << cname);
  }
  return id_seq;
}

}} // end xce::socialgraph


