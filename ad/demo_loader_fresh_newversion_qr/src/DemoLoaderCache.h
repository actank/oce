/*
 * DemoLoader.h
 *
 *  Created on:May 10, 2011
 *      Author: kingsfield
 */

#ifndef DEMOLOADERCACHE_H_
#define DEMOLOADERCACHE_H_

#include <map>
#include <set>
#include <vector>
#include <string>
#include <iostream>
#include <cstring>
#include <sstream>
#include <Ice/Ice.h>
#include "Date.h"
#include "Singleton.h"
#include "FeedMemcProxy/client/user_profile_client.h"
//#include "AdStruct.h"
#include "AdDemoQuality.h"
#include "IceLogger.h"
#include "ServiceI.h"
#include "Date.h"
#include "client/linux/handler/exception_handler.h"
#include "AdConfig.h"

namespace xce{
namespace ad{
using namespace com::xiaonei::xce;
using namespace std;
using namespace MyUtil;
//const char* db_host = "10.3.16.119";
//const char* db_name = "test";
//const char* db_host = "10.3.17.173";
const char* db_host = "10.3.23.68";
const char* db_name = "ad_strategy";
const char* db_jebe_host = "10.3.16.159";
const char* db_jebe_name = "jebe_audit";
const int no_gender = 1;
const int no_stage = 2;
const int spid_type = 3;
const int all_type = 4;
const int two_feature = 5;

const int app_no_gender = 11;
const int app_no_stage = 12;
const int app_spid_type = 13;
const int app_all_type = 14;
const int app_two_feature = 15;

const string pos_721 = "1000000000721";
const string pos_722 = "1000000000722";
const string pos_723 = "1000000000723";
const string pos_701 = "1000000000701";
const string pos_702 = "1000000000702";
const string pos_703 = "1000000000703";
const string pos_631 = "1000000000631";
const string pos_632 = "1000000000632";
const string pos_633 = "1000000000633";
const double base_ecpm_state1 = 0.009;
const double base_ecpm_state2 = 0.019;
const double base_ecom_state3 = 0.03;

class FreshAdGroup: public IceUtil::Shared {
  private:
    Ice::Long group_id_;
    Ice::Long sponsor_id_;
    double price_;
    map<string,pair<Ice::Long,Ice::Long> > zone_pos_pv_click_;
    map<int,double> predict_state_ctr_;
//    map<string, double> base_ctr;
    int current_state_;
    double sponsor_ctr_;
  public:
    FreshAdGroup(Ice::Long group_id, Ice::Long sponsor_id, double price) : group_id_(group_id), sponsor_id_(sponsor_id),price_(price),current_state_(-1),sponsor_ctr_(0.0) {
      zone_pos_pv_click_[pos_721] = make_pair(0,0);
      zone_pos_pv_click_[pos_722] = make_pair(0,0);
      zone_pos_pv_click_[pos_723] = make_pair(0,0);
      zone_pos_pv_click_[pos_701] = make_pair(0,0);
      zone_pos_pv_click_[pos_702] = make_pair(0,0);
      zone_pos_pv_click_[pos_703] = make_pair(0,0);
      zone_pos_pv_click_[pos_631] = make_pair(0,0);
      zone_pos_pv_click_[pos_632] = make_pair(0,0);
      zone_pos_pv_click_[pos_633] = make_pair(0,0);
    }
    void SetData(string key,Ice::Long impr, Ice::Long click) {
      zone_pos_pv_click_[key] = make_pair(impr,click);
    }
    Ice::Long sponsor_id() {
      return sponsor_id_;
    }
    void set_sponsor_ctr(double ctr) {
      sponsor_ctr_ = ctr;
    }
    map<int,double> get_predict_ctr_map() {
      return predict_state_ctr_;
    }
    void reset_data();
    void calculate_state();

    void predict_fresh_ctr();
    void PrintHelp();
};

typedef IceUtil::Handle<FreshAdGroup> FreshAdGroupPtr;

class DemoLoaderCache :public AdDemoQualityManager, public MyUtil::Singleton<DemoLoaderCache> {
public:
  virtual AdQualityMap GetDemoPool(const ::Ice::Current& = ::Ice::Current());
  virtual AdQualityMap GetAppPool(const ::Ice::Current& = ::Ice::Current());
  virtual FreshAdQualityMap GetFreshAdMap(const ::Ice::Current& = ::Ice::Current());
  virtual FreshAdQualityMap GetDisplayLessAds(const ::Ice::Current& = ::Ice::Current());
  virtual AdPosMap GetAverageEcpm(const ::Ice::Current& = ::Ice::Current());
  void LoadPool();
  void LoadFreshData();
  void LoadDisplayLessAd();
  void LoadAverageEcpm();
private:
  static const int TIME_RANGE = 48;
  void LoadByTime(const string & begin_time, const string & end_time);
  void LoadFreshAd(string begin_time, map<Ice::Long, FreshAdGroupPtr> &FreshAdMap,
      set<Ice::Long> & fresh_gids, set<Ice::Long> & fresh_sponsors);
  void LoadAdGroupData(string begin_time, map<Ice::Long, FreshAdGroupPtr> &FreshAdMap,
      set<Ice::Long> & fresh_gids);
  map<Ice::Long, double> GetFreshAdSponsorData(string begin_time, set<Ice::Long> & fresh_gids);
  void SetFreshAdSponsorData(map<Ice::Long, FreshAdGroupPtr> & FreshAdMap,
      map<Ice::Long, double>& fresh_sponsors_ctr);
  void CalculateState(map<Ice::Long, FreshAdGroupPtr>& FreshAdMap);
  void PredictFreshAdCtr(map<Ice::Long, FreshAdGroupPtr>& FreshAdMap);
  FreshAdQualityMap AssignFreshAdImpr(map<Ice::Long,FreshAdGroupPtr> & FreshAdMap);
  void TranstoAssignStruct(map<Ice::Long,FreshAdGroupPtr> & FreshAdMap,
      map<int, vector<pair<Ice::Long,double> > >& state_gids_ctr);
  void UniFormImprRatio(FreshAdQualityMap& fresh_ad_pool);
  FreshAdQualityMap AssignImprRatio(map<int, vector<pair<Ice::Long,double> > >&state_gids_ctr,
      map<string, Ice::Long> &hour_impr);
  void LoadHourImpr(map<string,Ice::Long> & hour_impr);
  void LoadUpdatedAd(set<Ice::Long>& update_ads);
  void ResetUpdatedAdGroupData(map<Ice::Long, FreshAdGroupPtr> &FreshAdMap, set<Ice::Long>& update_ads);
  void ReloadUpdatedAdGroupData(map<Ice::Long, FreshAdGroupPtr> &FreshAdMap, set<Ice::Long>& update_ads);

  void LoadUpdateAdInfo(map<Ice::Long, FreshAdGroupPtr> &FreshAdMap, set<Ice::Long> & updated_gids,
        set<Ice::Long> & updated_sponsors);

  void InsertAmplifyGrouptoPool(AdQualityMap& res_pool);
  void InsertUpdateGrouptoPool(AdQualityMap& res_pool);

  void ExpandCtrByPos(map<string,map<Ice::Long,map<int,pair<Ice::Long, Ice::Long> > > >& temp_pool,
      AdQualityMap& res_pool,int type);
  void ExpandCtrByZone(map<string,map<Ice::Long,map<int,pair<Ice::Long, Ice::Long> > > >& temp_pool,
      AdQualityMap& res_pool,int type);
  void ExpandCtrByPosAndZone(map<string,map<Ice::Long,map<int,pair<Ice::Long, Ice::Long> > > >& temp_pool,
      AdQualityMap& res_pool,int type);

  string GetKey(Ice::Long zone_id, Ice::Long sp_id ,Ice::Long group_id,int pos ,int gender,int stage){
    sp_id = sp_id * 10 + pos;
    group_id = group_id * 10 + pos;
    ostringstream sin;
    sin << zone_id <<"_"<< sp_id <<"_"<< group_id <<"_" <<gender<<"_"<<stage;
    return sin.str();
  };
  string GetKey(Ice::Long sp_id ,Ice::Long group_id,int gender,int stage){
    ostringstream sin;
    sin << sp_id <<"_"<< group_id <<"_" <<gender<<"_"<<stage;
    return sin.str();
  };
  
  string GetKey(string zone_id, Ice::Long sp_id ,Ice::Long group_id,int pos ,int gender,int stage){
    sp_id = sp_id * 10 + pos;
    group_id = group_id * 10 + pos;
    ostringstream sin;
    sin << zone_id <<"_"<< sp_id <<"_"<< group_id <<"_" <<gender<<"_"<<stage;
    return sin.str();
  };

  string GetKey(Ice::Long zone_id, int pos) {
    Ice::Long key = zone_id * 10 + pos;
    ostringstream sin;
    sin << key;
    return sin.str();
  }
 
  bool IsCredible(int pv, int click, int type) {
    int pv_th = 100000;
    int click_th = 20;
    switch(type) {
      case no_gender:
        pv_th = AdDemoConfig::instance().GetNoGenderPvTh();
        click_th = AdDemoConfig::instance().GetNoGenderClickTh();
       // MCE_DEBUG("IsCredbile,type:" << type << "no_gender,pv:" << pv_th  <<",click:" << click_th);
        break;
      case no_stage:
        pv_th = AdDemoConfig::instance().GetNoStagePvTh();
        click_th = AdDemoConfig::instance().GetNoStageClickTh();
       // MCE_DEBUG("IsCredbile,type:" << type << "no_stage,pv:" << pv_th  <<",click:" << click_th);
        break;
      case spid_type:
        pv_th = AdDemoConfig::instance().GetSpidPvTh();
        click_th = AdDemoConfig::instance().GetSpidClickTh();
       // MCE_DEBUG("IsCredbile,type:" << type << "spidtype,pv:" << pv_th  <<",click:" << click_th);
        break;
      case all_type:
        pv_th = AdDemoConfig::instance().GetAllPvTh();
        click_th = AdDemoConfig::instance().GetAllClickTh();
       // MCE_DEBUG("IsCredbile,type:" << type << "alltype,pv:" << pv_th  <<",click:" << click_th);
        break;
      case two_feature:
        pv_th = AdDemoConfig::instance().GetTwoFeaturePvTh();
        click_th = AdDemoConfig::instance().GetTwoFeatureClickTh();
       // MCE_DEBUG("IsCredbile,type:" << type << "two_feature,pv:" << pv_th  <<",click:" << click_th);
        break;
      case app_two_feature:
        pv_th = AdDemoConfig::instance().GetAppTwoFeaturePvTh();
        click_th = AdDemoConfig::instance().GetAppTwoFeatureClickTh();
       // MCE_DEBUG("IsCredbile,type:" << type << "two_feature,pv:" << pv_th  <<",click:" << click_th);
        break;      
      case app_no_gender:
        pv_th = AdDemoConfig::instance().GetAppNoGenderPvTh();
        click_th = AdDemoConfig::instance().GetAppNoGenderClickTh();
       // MCE_DEBUG("IsCredbile,type:" << type << "no_gender,pv:" << pv_th  <<",click:" << click_th);
        break;
      case app_no_stage:
        pv_th = AdDemoConfig::instance().GetAppNoStagePvTh();
        click_th = AdDemoConfig::instance().GetAppNoStageClickTh();
       // MCE_DEBUG("IsCredbile,type:" << type << "no_stage,pv:" << pv_th  <<",click:" << click_th);
        break;
      case app_spid_type:
        pv_th = AdDemoConfig::instance().GetAppSpidPvTh();
        click_th = AdDemoConfig::instance().GetAppSpidClickTh();
       // MCE_DEBUG("IsCredbile,type:" << type << "spidtype,pv:" << pv_th  <<",click:" << click_th);
        break;
      case app_all_type:
        pv_th = AdDemoConfig::instance().GetAppAllPvTh();
        click_th = AdDemoConfig::instance().GetAppAllClickTh();
       // MCE_DEBUG("IsCredbile,type:" << type << "alltype,pv:" << pv_th  <<",click:" << click_th);
        break;
      default:
        MCE_WARN("IsCredblie,no type:" << type);
        pv_th = 100000;
        click_th = 20;
    }
    return (pv > pv_th) || (click > click_th);
  }

  bool IsFreshCredible(int pv, int click, Ice::Long zone_id, int pos_id, double ecpm,double local_ecpm_thr) {
    int click_th = AdDemoConfig::instance().GetFreshClickThr();
    if (local_ecpm_thr <= 0.0){
       local_ecpm_thr = 100000.0;
       MCE_DEBUG("Error local_ecpm is 0 at " << zone_id << " " << pos_id);
    }
    return (ecpm > local_ecpm_thr) && (click >= click_th);
  }

  /*
  bool IsFreshCredible(int pv, int click, Ice::Long zone_id, int pos_id, double ecpm,double local_ecpm_thr) {
    int click_th = AdDemoConfig::instance().GetFreshClickThr();
    if (local_ecpm_thr <= 0.0){
       local_ecpm_thr = 100000.0;
       MCE_DEBUG("Error local_ecpm is 0 at " << zone_id << " " << pos_id);
    }
    double ratio = ecpm / local_ecpm_thr;
    if(ratio < 1.25) {//濡傛灉涓嶆槸鏄捐憲楂橈紝鎻愰珮闃堝�
      click_th = click_th * 2;
    }
   
    return (ecpm > local_ecpm_thr) && (click >= click_th);
  }*/


  AdQualityMap demo_pool;
  AdQualityMap app_pool;
  FreshAdQualityMap fresh_ad_pool_;
  FreshAdQualityMap display_less_gids_;
  map<Ice::Long,pair<Ice::Long,double> > ecpm_table;
  map<string, double> average_ecpm_talbe_;

  //Ice::Long hour_impr;
  static const size_t DAYCOUNT = 10;
  static const size_t HOURCOUNT = 24;
  static const size_t TIMEINTERVAL = 1 * 60 * 60;
  static const int NO_USE = -1;
  IceUtil::RWRecMutex mutex_;
};

class DemoLoaderCacheTimer : public MyUtil::Timer {
public:
  DemoLoaderCacheTimer(int time = 10 * 60 * 1000) :
    Timer(time) { //30閸掑棝鎸撻柌宥嗘煀鐠侊紕鐣绘稉锟筋偧
  }
  virtual void handle() {
    MCE_INFO("DemoLoaderCacheTimer:begin");
    DemoLoaderCache::instance().LoadPool();
    MCE_INFO("DemoLoaderCacheTimer:stop");
    MyUtil::TaskManager::instance().schedule(new DemoLoaderCacheTimer(3 * 60 * 1000));
  }
};

class DemoLoaderFreshAdCacheTimer : public MyUtil::Timer {
public:
  DemoLoaderFreshAdCacheTimer(int time =0 ) :Timer(time){
  }
  virtual void handle() {
    MCE_INFO("DemoLoaderFreshAdCacheTimer handle :begin");
    MyUtil::TaskManager::instance().schedule(new DemoLoaderFreshAdCacheTimer(5 * 60 * 1000));
    DemoLoaderCache::instance().LoadFreshData();

  }

};
class DemoLoaderDisplayLessAdTimer : public MyUtil::Timer {
public:
  DemoLoaderDisplayLessAdTimer(int time =0 ) :Timer(time){
  }
  virtual void handle() {
    MCE_INFO("DemoLoaderDisplayLessAdTimer handle :begin");
    DemoLoaderCache::instance().LoadDisplayLessAd();
//    DemoLoaderCache::instance().LoadAverageEcpm();
    MyUtil::TaskManager::instance().schedule(new DemoLoaderDisplayLessAdTimer(6 * 60 * 1000));

  }

};

class DemoLoaderAverageEcpmTimer : public MyUtil::Timer {
public:
  DemoLoaderAverageEcpmTimer(int time =0 ) :Timer(time){
  }
  virtual void handle() {
    MCE_INFO("DemoLoaderAverageEcpmTimer handle :begin");
    DemoLoaderCache::instance().LoadAverageEcpm();
    MyUtil::TaskManager::instance().schedule(new DemoLoaderAverageEcpmTimer(30 * 60 * 1000));
  }

};



}
}

#endif /* DEMOLOADERCACHE_H_ */
