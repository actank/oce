/*
 * AdRecommendation.h
 *
 *  Created on: 2010-11-21
 *      Author: sunzz
 */

#ifndef ADRECOMMENDATION_H_
#define ADRECOMMENDATION_H_
#include <bitset>
#include <map>
#include <vector>
#include <IceUtil/Mutex.h>
#include <IceUtil/Shared.h>
#include <IceUtil/Handle.h>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <iostream>
#include "Singleton.h"
#include"AdRecommendItemI.h"
#include "AdReConfig.h"
#include <boost/lexical_cast.hpp>
using namespace std;
using namespace boost::multi_index;
using namespace boost;
using namespace MyUtil;
using namespace xce::ad;

const double probability = 0.16;//已点击过的广告降低到原来的百分比
const size_t RecommendNum= 30;
const int GidType63 = 0;
const int GidTypeBrand = 1;


//const size_t ItemSize = 1500;
//const size_t kBitSetSize = ItemSize * 2;//广告数*占的位数



class ItemUsers : public IceUtil::Shared {
private:
public:
  int cluster;
  int gidtype;  //type ==GidType63 or GidTypeBrand
  set<Ice::Long>gids;
  set<int> merge_users;

};
typedef IceUtil::Handle<ItemUsers> ItemUsersPtr;


class  UpdateTask: public MyUtil::Task {
public:
  UpdateTask(ItemUsersPtr u):upt(u){
  }
  virtual void handle() {
//    MCE_DEBUG("UpdateGidUidTask:start: gid:" << upt->gid <<",gid type:" << upt->gidtype);
//    UpdateGidUid(upt->gidtype);
  }
  void UpdateGidUid(int gidtype);
  ItemUsersPtr upt;
};

class ItemUsersPool : public Singleton<ItemUsersPool> {
private:

  typedef map<int, ItemUsersPtr> ItemUsersEvictor;
  ItemUsersEvictor gidmap_;
  map<Ice::Long, int> gid_cluster_map;
  map<int, map<int, double> > knncache_;

  IceUtil::Mutex mutex_;


  /////////////////////////////////////////////////////////
  static const int KNNCOUNT = 60; // k:最近邻居个数
  static const size_t RecommendNum= 16;
  static const size_t LeastCommonNum= 5;
  static const double SimThreshold = 0.1;
  static const double ReThreshold = 0.0;
  static const double OccupyPercent = 0.1;
  static const double BrandPercent = 0;
  int GetKNNCount() {
    int value = KNNCOUNT;
    string s = RecommendConfig::instance().GetValue("KNNCount");
    try {
      if (!s.empty()) {
        value = lexical_cast<int> (s);
      }
    } catch (std::exception & e) {
      MCE_WARN("AdRecommend::GetKNNCount --> sting cast error :-" << s <<"-"<< e.what());
    }

//    MCE_DEBUG("AdRecommend::--> GetKNNCount = " << value);
    return value;
  }

  int GetLeastCommonNum() {
    int value = LeastCommonNum;
    string s = RecommendConfig::instance().GetValue("LeastCommonNum");
    try {
      if (!s.empty()) {
        value = lexical_cast<int> (s);
      }
    } catch (std::exception & e) {
      MCE_WARN("AdRecommend::GetLeastCommonNum --> sting cast error :-" << s <<"-"<< e.what());
    }
    return value;
  }

  int GetMaxReCount() {
    int value = RecommendNum;
    string s = RecommendConfig::instance().GetValue("MaxReCount");
    try {
      if (!s.empty()) {
        value = lexical_cast<int> (s);
      }
    } catch (std::exception & e) {
      MCE_WARN("AdRecommend::GetMaxReCount --> sting cast error :-" << s <<"-"<< e.what());
    }
 //   MCE_DEBUG("AdRecommend::--> GetMaxReCount = " << value);
    return value;
  }
  double GetSimThreshold() {
    double value = SimThreshold;
    string s = RecommendConfig::instance().GetValue("SimThreshold");
    try {
      if (!s.empty()) {
        value = lexical_cast<double> (s);
      }
    } catch (std::exception & e) {
      MCE_WARN("AdRecommend::GetSimThreshold --> sting cast error :-" << s <<"-"<< e.what());
    }
  //  MCE_DEBUG("AdRecommend::--> GetSimThreshold = " << value);
    return value;
  }
  double GetReThreshold() {
    double value = ReThreshold;
    string s = RecommendConfig::instance().GetValue("ReThreshold");
    try {
      if (!s.empty()) {
        value = lexical_cast<double> (s);
      }
    } catch (std::exception & e) {
      MCE_WARN("AdRecommend::GetReThreshold --> sting cast error :-" << s <<"-"<< e.what());
    }
   // MCE_DEBUG("AdRecommend::--> GetReThreshold = " << value);
    return value;
  }
  double GetOccupyPercent() {
    double value = OccupyPercent;
    string s = RecommendConfig::instance().GetValue("OccupyPercent");
    try {
      if (!s.empty()) {
        value = lexical_cast<double> (s);
      }
    } catch (std::exception & e) {
      MCE_WARN("AdRecommend::GetOccupyPercent --> sting cast error :-" << s <<"-"<< e.what());
    }
   // MCE_DEBUG("AdRecommend::--> GetReThreshold = " << value);
    return value;
  }
  double GetBrandPercent() {
    double value = BrandPercent;
    string s = RecommendConfig::instance().GetValue("BrandPercent");
    try {
      if (!s.empty()) {
        value = lexical_cast<double> (s);
      }
    } catch (std::exception & e) {
      MCE_WARN("AdRecommend::GetBrandPercent --> sting cast error :-" << s <<"-"<< e.what());
    }
   // MCE_DEBUG("AdRecommend::--> GetReThreshold = " << value);
    return value;
  }

  void InitFromCategory(map<Ice::Long, int> &gidmap);
  void InitFromDB(ItemUsersEvictor &container, map<Ice::Long, int> &clustermap,int type);
  void UniFormResult(map<Ice::Long, double> & result, double max, double min);
  void CalculatePropKnn();
//  void Merge(map<Ice::Long,double>& result, vector<Ice::Long>& clickad, double max_re);
  void MergeResult(map<Ice::Long, double> & to, map<Ice::Long, double> & from, double percent);
  int GetGidType(Ice::Long groupid);
  double GetItemPropDistance(const ItemUsersPtr& item_ptrx, const ItemUsersPtr& item_ptry);
  vector<int> LookUpGidCluster(vector<Ice::Long>click);
  set<Ice::Long> FindBestGidInCluster(int userid,int cluster);
  //////////////////////////////////////////////////////////////////////////

public:
  //ItemUsersPool() {
   //TaskManager::instance().schedule(new UpdateKNNTimer());
 // }
  //   ItemUsersPtr GetItemUsersPtr(int userid);
//  vector<Ice::Long> GetItemDistance(int userid);//为了测试
  vector<int> GetGidUser(Ice::Long groupid);//为了测试
  //void UpdateClick(int userid, Ice::Long adid);
  //double GetItemDistance(int userid1, int userid2);
  double GetItemDistance(const ItemUsersPtr& userptr1, const ItemUsersPtr& userptr2, int num);
  void Click(int userid, Ice::Long groupid);
  multimap<double, ItemUsers> GetKNNeighbour(int userid1);
//  map<Ice::Long,double> GetKNNItemProp(Ice::Long groupid);
  map<Ice::Long, double> GetRecommendation(int userid);
  map<Ice::Long,double> GetAverageOpinion();
  bool NeedLoad(int userid);
  void Kick();
  bool Load(int userid);
  set<Ice::Long> LoadDB(int userid);
  bool IsGidExist(int);
  void UpdateClick(int, Ice::Long, bool isbrand);
  void Init();

//  int GetIndex(UserItemEvictorSet & container,int userid, Ice::Long groupid);
  vector<int> LoadGidUsersDB(Ice::Long groupid, int userid);
 // map<Ice::Long,double> GetKNNItem(Ice::Long groupid);
  map<int,double>GetKNNItem(int cluster);
  void UpdateKNNItem(); 

  class UpdateKNNTimer : virtual public MyUtil::Timer {
    public:
      UpdateKNNTimer() :
        Timer(0, -1) {
      }
      virtual void handle() {
        MCE_INFO("ItemUsersPool:: UpdateKNNTimer handle--> begin");
        ItemUsersPool::instance().UpdateKNNItem();
        MCE_INFO("ItemUsersPool:: UpdateKNNTimer handle--> end");
      //  TaskManager::instance().schedule(new UpdateKNNTimer());

        this->delay(10 * 1000); //10秒后重新计算
        TaskManager::instance().schedule(this);
      }
  };

  class UpdateKNNPropTimer : virtual public MyUtil::Timer {
    public:
    UpdateKNNPropTimer() :
        Timer(1 * 10 * 1000, -4) {
      }
      virtual void handle() {
        MCE_INFO("ItemUsersPool:: UpdateKNNPropTimer handle--> begin");
        ItemUsersPool::instance().CalculatePropKnn();
        MCE_INFO("ItemUsersPool:: UpdateKNNPropTimer handle--> end");
      //  TaskManager::instance().schedule(new UpdateKNNTimer());

        this->delay(30 * 60 * 1000); //10秒后重新计算
        TaskManager::instance().schedule(this);
      }
  };
};

class User : public IceUtil::Shared {
public:
  int userid;
  map<Ice::Long, time_t> recent_clicked_;
};
typedef IceUtil::Handle<User> UserPtr;

class RecentClickCache : public Singleton<RecentClickCache> {

private:
  typedef boost::multi_index_container<UserPtr, indexed_by<sequenced<> ,
      ordered_unique<BOOST_MULTI_INDEX_MEMBER(User, int, userid)> > >
  UserEvictor;
  UserEvictor evictor_;
  typedef UserEvictor::nth_index<0>::type SeqIndex;
  typedef UserEvictor::nth_index<1>::type UidIndex;
  static const int JUST_CLICK_DURATION = 2 * 60 * 60;
  static const size_t kUserSize = 80000;
  IceUtil::Mutex mutex_;

public:
  UserPtr Locate(int userid);
  UserPtr Load(int userid, int type);
  void InitFromDB(UserEvictor &evictor, int type);
  void Init();
  bool Available(int uid, Ice::Long group_id) {

    UserPtr u = Locate(uid);
    const map<Ice::Long, time_t>& groups = u->recent_clicked_;
    map<Ice::Long, time_t>::const_iterator iit = groups.find(group_id);
   if (iit != groups.end()) {
     time_t duration = time(NULL) - iit->second;
     MCE_DEBUG("RecentClickCache::Available --> uid = " << uid <<",gid:" << group_id << ",in the cache, time ecapled:" << duration);
     return duration > JUST_CLICK_DURATION;
   }
   return true;

 /*   IceUtil::RWRecMutex::RLock lock(mutex_);
    map<int, map<Ice::Long, time_t> >::const_iterator it = recent_clicked_.find(uid);
    if (it != recent_clicked_.end()) {
      const map<Ice::Long, time_t>& groups = it->second;
      map<Ice::Long, time_t>::const_iterator iit = groups.find(group_id);
      if (iit != groups.end()) {
       // MCE_DEBUG("RecentClickCache::Available --> uid = " << uid <<",gid:" << group_id << ",in the cache");
        time_t duration = time(NULL) - iit->second;
        MCE_DEBUG("RecentClickCache::Available --> uid = " << uid <<",gid:" << group_id << ",in the cache, time ecapled:" << duration);
        return duration > JUST_CLICK_DURATION;
      }
      return true;
    }
    return true;*/
  }
  void Click(int uid, Ice::Long group_id) {
//    IceUtil::RWRecMutex::WLock lock(mutex_);
//    recent_clicked_[uid][group_id] = time(NULL);
//    MCE_DEBUG("RecentClickCache::Click-->uid:"<<uid <<",gid:"<<group_id << "RecentClickCache size:" << recent_clicked_.size());
    UserPtr u = Locate(uid);
    {
      UserPtr tmp = new User();
      tmp -> userid = u->userid;
      tmp->recent_clicked_ = u->recent_clicked_;
      tmp->recent_clicked_[group_id] = time(NULL);

      IceUtil::Mutex::Lock lock(mutex_);
      UidIndex & index = evictor_.get<1> ();
      UidIndex::iterator uit = index.find(uid);
      index.replace(uit, tmp);
      SeqIndex::iterator sit = evictor_.project<0> (uit);//更新之后放到前面
      evictor_.relocate(evictor_.get<0> ().begin(), sit);
    }
  }

  vector<Ice::Long> GetClickedGid(int userid) {
    vector<Ice::Long> result;
    UserPtr u = Locate(userid);
    {
      const map<Ice::Long, time_t>& groups = u->recent_clicked_;
      map<Ice::Long, time_t>::const_iterator iit = groups.begin();
      for(; iit != groups.end(); ++iit) {
        result.push_back(iit->first);
      }
    }

/*    vector<Ice::Long> result;
    IceUtil::RWRecMutex::RLock lock(mutex_);
    map<int, map<Ice::Long, time_t> >::const_iterator it = recent_clicked_.find(userid);
    if (it != recent_clicked_.end()) {
      const map<Ice::Long, time_t>& groups = it->second;

      map<Ice::Long, time_t>::const_iterator iit = groups.begin();
      for(; iit != groups.end(); ++iit) {
        result.push_back(iit->first);
      }

    }*/
    MCE_INFO("RecentClickCache::GetClickedGid,userid:" << userid << ",clicked gids size:" << result.size());
    return result;
  }
};




#endif /* ADRECOMMENDATION_H_ */
