/*
 * FreshAdTesting.h
 *
 *  Created on: 2011-6-22
 *      Author: sunzz
 */

#ifndef FRESHADTESTING_H_
#define FRESHADTESTING_H_

#include "AdQualityUtil.h"
#include "AdDemoQuality.h"
static const int duration_day = 10;
static const double average_ctr = 0.0002;
static const int pv_limit_count = 100000;

namespace xce{
namespace ad{
using namespace com::xiaonei::xce;
using namespace std;
using namespace MyUtil;
class FreshAdGroup: public IceUtil::Shared {
  private:
    Ice::Long group_id_;
    Ice::Long sponsor_id_;
    double price_;
    map<Ice::Long, map<string,pair<Ice::Long,Ice::Long> > > time_zone_pos_pv_click_; //key -->time
    map<string,pair<Ice::Long,Ice::Long> > zone_pos_pv_click_;
    bool is_test_over_;
    double sponsor_ctr_;
    Ice::Long create_time_;
    Ice::Long audit_time_;
    double weight_;

    map<string,double> click_discount_ratio_table;
  public:
    FreshAdGroup(Ice::Long group_id, Ice::Long sponsor_id, double price) : group_id_(group_id),
    sponsor_id_(sponsor_id),price_(price),is_test_over_(false),sponsor_ctr_(0.0),create_time_(0)
    ,audit_time_(0),weight_(0.000000001) {
      init();
    }
    void init();
    Ice::Long sponsor_id() {
      return sponsor_id_;
    }
    void set_create_time(Ice::Long time) {
      create_time_ = time;
    }
    void set_audit_time(Ice::Long time) {
      audit_time_ = time;
    }
    void add_time_pv_click(Ice::Long time_key, string zone_key,Ice::Long impr, Ice::Long click) {
      map<string, pair<Ice::Long,Ice::Long> >::iterator it = time_zone_pos_pv_click_[time_key].find(zone_key);
      if(it != time_zone_pos_pv_click_[time_key].end()) {
        it->second.first += impr;
        it->second.second += click;
      }else {
        time_zone_pos_pv_click_[time_key][zone_key] = make_pair(impr, click);
      }
    }
    void add_pv_click(string key, Ice::Long impr, Ice::Long click) {
      map<string,pair<Ice::Long,Ice::Long> >::iterator it = zone_pos_pv_click_.find(key);
      if(it != zone_pos_pv_click_.end()) {
        it->second.first += impr;
        it->second.second += click;
      } else {
        zone_pos_pv_click_[key] = make_pair(impr, click);
      }
    }

    void set_sponsor_ctr(double ctr) {
      sponsor_ctr_ = ctr;
    }
    bool is_test_over() {
      return is_test_over_;
    }
    double weight() {
      return weight_;
    }
   void calculate_display_count();
   void calculate_weight();
   void print_display_count();

};
typedef IceUtil::Handle<FreshAdGroup> FreshAdGroupPtr;
class FreshAdCache : public MyUtil::Singleton<FreshAdCache> {
private:
  FreshAdQualityMap fresh_ad_pool_;
  map<Ice::Long, FreshAdGroupPtr> adgroup_map_;
  map<Ice::Long, double> sponsor_map_;
  set<Ice::Long> sponsor_set_;
  IceUtil::RWRecMutex mutex_;

  void LoadAuditedOnlineFreshAd();
  void LoadFreshAdData();
  void LoadAdSponsorCtr();
  void CalculateDisplayCount();
  void CalculateWeight();
  void PrintDisplayCount();
  void AssignFreshAdImprRatio(FreshAdQualityMap & fresh_pool);
  void Clear() { //清理中间结果
    sponsor_map_.clear();
    sponsor_set_.clear();
    adgroup_map_.clear();
  }
  void Init();
public:
  void LoadFreshAdPool();
  void LoadFreshAdMap(FreshAdQualityMap& fresh_ad_pool);
};

class FreshAdCacheTimer : public MyUtil::Timer {
public:
  FreshAdCacheTimer(int time =0 ) :Timer(time){
  }
  virtual void handle() {
    MCE_INFO("FreshAdCacheTimer handle :begin");
    FreshAdCache::instance().LoadFreshAdPool();
    MCE_INFO("FreshAdCacheTimer handle :stop");
    MyUtil::TaskManager::instance().schedule(new FreshAdCacheTimer(5 * 60 * 1000));
  }

};
}
}
#endif /* FRESHADTESTING_H_ */
