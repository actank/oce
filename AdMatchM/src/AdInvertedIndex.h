/*
 * AdInvertedIndex.h
 *
 *  Created on: Jul 29, 2010
 *      Author: antonio
 */

#ifndef ADINVERTEDINDEX_H_
#define ADINVERTEDINDEX_H_

#include "AdUtil.h"
#include "LogWrapper.h"
#include <map>
#include <cmath>
#include <IceUtil/Mutex.h>
#include <IceUtil/Shared.h>
#include <IceUtil/Handle.h>
#include <IceUtil/RWRecMutex.h>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <bits/stl_multimap.h>

namespace xce {
namespace ad {
using namespace MyUtil;
using namespace std;
using namespace xce::ad;
using namespace boost;

class Creative {
public:
  Creative() :
    id(0), click(0), pv(0), update_time(0) {
  }
  ;
  Ice::Long id;
  int click;
  int pv;

  time_t update_time;
};

class CreativeSet: public IceUtil::Shared {
public:

  map<Ice::Long, Creative> set_;

};

typedef IceUtil::Handle<CreativeSet> CreativeSetPtr;

class AdGroup: public IceUtil::Shared {
public:
  AdGroup() {
    creative_ = new CreativeSet;
  }
  Ice::Long member_id_;
  Ice::Long campaign_id_;
  Ice::Long group_id_;
  Ice::Long bid_id_;
  Ice::Long adzone_id_;

  int max_price_;
  //  bool run_out_;
  //  bool valid_;
  vector<int> keys_;
  set<Ice::Long> schools_;
  set<Ice::Long> areas_;
  CreativeSetPtr creative_;
  bool use_average_click_;
  //  time_t price_time_;
  //  time_t bid_time_;

};
typedef IceUtil::Handle<AdGroup> AdGroupPtr;

typedef vector<AdGroupPtr> AdGroupSeq;


class AdSet;
typedef IceUtil::Handle<AdSet> AdSetPtr;

class AdSet: public IceUtil::Shared {
public:
  void add(const AdGroupPtr & group) {
    SubGroupPtr g = new SubGroup();
    if (group->use_average_click_) { //系统启动后load的
      //      MCE_DEBUG(__FILE__ << ":" << __LINE__);
      pair<int, int> click_pv = GetAverageClick();//
      if (click_pv.second == 0) {//说明这个AdSet中还没有subgroup
        g->click = 1;
        g->pv = 10000;
        //        MCE_DEBUG("AdSet::add --> set_ is empty, set click and pv to default values,click = " << g->click << ",pv = " <<g->pv);
      } else {
        g->click = click_pv.first;
        g->pv = click_pv.second;
        //        MCE_DEBUG("AdSet::add --> use average click and pv, click = " << click_pv.first << ",pv = " <<click_pv.second);
      }
    } else {
      g->click = 1;
      g->pv = 10000;
    }
    g->group = group;
    g->key = key_;
    //    g->current_price = group->max_price_;
    {
      IceUtil::Mutex::Lock lock(mutex_);
      //      set_.insert(pair<Ice::Long, SubGroupPtr> (group->group_id_, g));
      set_[group->group_id_] = g;
    }
  }
  //  AdSetPtr clone() {
  //    AdSetPtr p = new AdSet();
  //    p->set_ = set_;
  //    return p;
  //  }

  map<Ice::Long, SubGroupPtr>& GetSubGroupMap() {
    IceUtil::Mutex::Lock lock(mutex_);
    return set_;
  }
  bool Click(Ice::Long gid) {
    IceUtil::Mutex::Lock lock(mutex_);
    map<Ice::Long, SubGroupPtr>::iterator mit = set_.find(gid);
    if (mit != set_.end()) {
      //TODO 测试时，不修改点击数
      ++(mit->second->click);
      MCE_DEBUG("AdSet::Click --> click is = " << mit->second->click << ",pv = " << mit->second->pv );
      return true;
    } else {
      MCE_DEBUG("AdSet::Click --> can not find group in this set, group = " << gid );
      return false;
    }
  }
  void Del(Ice::Long gid) {
    IceUtil::Mutex::Lock lock(mutex_);
    set_.erase(gid);
  }
  pair<int, int> GetAverageClick() {
    IceUtil::Mutex::Lock lock(mutex_);

    if (set_.empty()) {
      return make_pair(0, 0);
    }
    int click = 0, pv = 0;
    //    MCE_DEBUG("AdSet::GetAverageClick --> set_ size = " << set_.size());

    map<Ice::Long, SubGroupPtr>::iterator mit = set_.begin();
    for (; mit != set_.end(); ++mit) {
      click += mit->second->click;
      pv += mit->second->pv;
    }
    click /= set_.size();
    pv /= set_.size();
    return make_pair(click, pv);
  }
  void PrintInfo() {
    IceUtil::Mutex::Lock lock(mutex_);
    ostringstream ios;
    ios << "(";
    bool is_first = true;
    for (map<Ice::Long, SubGroupPtr>::iterator mit = set_.begin(); mit
        != set_.end(); ++mit) {
      if (is_first) {
        ios << mit->first;
        is_first = false;
      } else {
        ios << "," << mit->first;
      }
    }
    ios << ")";
    MCE_DEBUG("AdSet::PrintInfo --> group ids = " << ios.str());
  }
  int Size() {
    IceUtil::Mutex::Lock lock(mutex_);
    return set_.size();
  }
  ;

  AdSetPtr operator &(const AdSet& p2) {
    map<Ice::Long, SubGroupPtr>::const_iterator it1, it2;
    MCE_DEBUG( __FILE__ << ":" << __LINE__ );

    AdSetPtr res = new AdSet();
    IceUtil::Mutex::Lock lock1(mutex_);
    IceUtil::Mutex::Lock lock2(p2.mutex_);

    it1 = set_.begin();
    it2 = p2.set_.begin();
    while (it1 != set_.end() && it2 != p2.set_.end()) {

      if (it1->first < it2->first) {
        ++it1;
      } else if (it1->first > it2->first) {
        ++it2;
      } else {
        res->set_[it1->first] = it1->second;
        ++it1;
        ++it2;
      }
    }
    return res;
  }

  AdSetPtr operator +(const AdSet& p2) {

    map<Ice::Long, SubGroupPtr>::const_iterator it1, it2;

    AdSetPtr res = new AdSet();
    {
      IceUtil::Mutex::Lock lock(mutex_);
      it1 = set_.begin();
      while (it1 != set_.end()) {
        res->set_[it1->first] = it1->second;
        ++it1;
      }
    }
    {

      IceUtil::Mutex::Lock lock(p2.mutex_);
      it2 = p2.set_.begin();
      while (it2 != p2.set_.end()) {
        res->set_[it2->first] = it2->second;
        ++it2;
      }
    }
    //    MCE_DEBUG("AdSet operator + --> res size = " << res->set_.size() << __FILE__ << ":" << __LINE__);
    return res;
  }

  //测试用，设置点击率
  bool SetClickInfo(Ice::Long gid, int click, int pv) {
    IceUtil::Mutex::Lock lock(mutex_);
    map<Ice::Long, SubGroupPtr>::iterator mit = set_.find(gid);
    if (mit == set_.end()) {
      MCE_WARN("AdSet::SetClickInfo --> Can not find this subgroup ,gid = " << gid);
      return false;
    }
    MCE_INFO("AdSet::SetClickInfo --> gid = " << gid <<
        ",old click:pv = " << mit->second->click << " : " << mit->second->pv << ", "
        "new click:pv = " << click << " : " << pv );
    mit->second->click = click;
    mit->second->pv = pv;
    return true;
  }

  Ice::Long key_;
private:
  IceUtil::Mutex mutex_;
  map<Ice::Long, SubGroupPtr> set_;

};

template<class INDX>
class DimIndex {
public:
  DimIndex() :
    ready_(false) {
  }
  ;
  void SetReady(bool state) {
    ready_ = state;
  }
  void Build(const vector<INDX> & index_seq, const AdGroupPtr & group) {
    IceUtil::RWRecMutex::RLock lock(index_mutex_);
    for (size_t i = 0; i < index_seq.size(); ++i) {
      typename map<INDX, AdSetPtr>::iterator it = index_.find(index_seq.at(i));
      if (it != index_.end()) {
        AdSetPtr p = it->second;
        p->key_ = index_seq.at(i);
        //        MCE_DEBUG("Build A--> p->key_ = " << p->key_);
        p->add(group);////
      } else {
        AdSetPtr p = new AdSet();
        p->key_ = index_seq.at(i);
        //        MCE_DEBUG("Build B--> p->key_ = " << p->key_);
        p->add(group);
        index_[index_seq.at(i)] = p;
      }
    }
  }

  void Add(INDX index, const AdGroupPtr& group) {
    IceUtil::RWRecMutex::RLock lock(index_mutex_);
    //    for (size_t i = 0; i < index_seq.size(); ++i) {
    typename map<INDX, AdSetPtr>::iterator it;

    it = index_.find(index);
    if (it != index_.end()) {
      it->second->add(group);
      return;
    }

    AdSetPtr p = new AdSet();
    p->key_ = (long) index;
    p->add(group);
    index_[index] = p;
    // }
  }

  void Add(const set<INDX>& index_set, const AdGroupPtr& group) {

    for (typename set<INDX>::const_iterator it = index_set.begin(); it
        != index_set.end(); ++it) {
      Add(*it, group);
    }
  }
  void Add(const vector<INDX>& index_set, const AdGroupPtr& group) {

    for (typename vector<INDX>::const_iterator it = index_set.begin(); it
        != index_set.end(); ++it) {
      Add(*it, group);
    }
  }

  AdSetPtr Get(INDX index) {
    //    MCE_DEBUG("DimIndex::Get --> index = " << index);
    if (!ready_) {
      return 0;
    }
    IceUtil::RWRecMutex::RLock lock(index_mutex_);
    typename map<INDX, AdSetPtr>::iterator it = index_.find(index);
    if (it != index_.end()) {
      //MCE_DEBUG( __FILE__ << ":" << __LINE__);
      return it->second;
    } else {
      MCE_DEBUG( __FILE__ << ":" << __LINE__ << ",key = " << index);
      return 0;
    }
  }

  bool Click(INDX index, Ice::Long gid) {
    MCE_DEBUG(__FILE__ << ":" << __LINE__);
    if (!ready_) {
      MCE_DEBUG("DimIndex::Click --> it's not ready,ready_ = " << ready_);
      return false;
    }
    IceUtil::RWRecMutex::RLock lock(index_mutex_);
    typename map<INDX, AdSetPtr>::iterator it = index_.find(index);
    if (it != index_.end()) {
      return it->second->Click(gid);
    } else {
      MCE_DEBUG("DimIndex::Click --> can not find key = " << index);
      return false;
    }
  }

  void PrintInfo() {
    for (typename map<INDX, AdSetPtr>::iterator mit = index_.begin(); mit
        != index_.end(); ++mit) {
      MCE_DEBUG("DimIndex::PrintInfo --> key = " << mit->first << ",value size = " << mit->second->Size());
      //      mit->second->PrintInfo();
    }
  }

  size_t Size() {
    IceUtil::RWRecMutex::RLock lock(index_mutex_);
    return index_.size();
  }
private:
  bool ready_;
  map<INDX, AdSetPtr> index_;
  IceUtil::RWRecMutex index_mutex_;
};

typedef DimIndex<int> MultiDimIndex;
typedef DimIndex<Ice::Long> SchoolIndex;
typedef DimIndex<Ice::Long> AreaIndex;

}
}

//namespace std {
//template<> struct less<xce::ad::AdGroupPtr> : public binary_function<
//    xce::ad::AdGroupPtr, xce::ad::AdGroupPtr, bool> {
//  bool operator()(const xce::ad::AdGroupPtr& g1, const xce::ad::AdGroupPtr& g2) const {
//    return g1->group_id_ < g2->group_id_;
//  }
//};
//}
//;
#endif /* ADINVERTEDINDEX_H_ */
