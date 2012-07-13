#ifndef _XCE_SOCIALGRAPH_FRIENDCLUSTERCACHE_H__
#define _XCE_SOCIALGRAPH_FRIENDCLUSTERCACHE_H__

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include <boost/thread/mutex.hpp>

#include "base/hashmap.h"
#include "ServiceI.h"
#include "FriendClusterCache.h"

namespace xce {
namespace socialgraph {


struct CPerson {
  int id_;
  int weight_;
};

int CompareCPerson(const CPerson& cp1, const CPerson& cp2) {
  return cp1.weight_ > cp2.weight_;
}

//好友簇原始数据cache
//分别保存两个map:
//No.1:簇名 对 簇成员
//No.2:成员 对 簇名
class FriendClusterCacheI :  virtual public FriendClusterCache, virtual public MyUtil::Singleton<FriendClusterCacheI> {
public:
  virtual FriendClusterData GetFriendClusterData(const ::MyUtil::IntList&, const ::Ice::Current& = ::Ice::Current());
  virtual MyUtil::StrSeq GetC(int, const ::Ice::Current& = ::Ice::Current());
  virtual MyUtil::IntSeq GetU(const string&, const ::Ice::Current& = ::Ice::Current());
  virtual ~FriendClusterCacheI() {}
private:
  FriendClusterCacheI();
  friend class MyUtil::Singleton<FriendClusterCacheI>;
  void Init();
  void SaveCluster(const std::string&);
  void SaveUidCluster(const std::string&);
private:
  std::hash_map<std::string, std::vector<int> > cname_to_id_map_;
  std::hash_map<int, std::vector<std::string> > id_to_cname_map_;
  boost::mutex mutex_; 
private:
  bool start_;
};


}}
#endif


