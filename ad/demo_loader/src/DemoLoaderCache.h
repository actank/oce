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
#include "Util.h"

namespace xce{
namespace ad{
using namespace com::xiaonei::xce;
using namespace std;
using namespace MyUtil;
//const char* db_host = "10.3.16.119";
//const char* db_name = "test";
//const char* db_host = "10.3.17.173";

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


const double base_ecpm_state1 = 0.009;
const double base_ecpm_state2 = 0.019;
const double base_ecom_state3 = 0.03;

const int cpc_type = 0;
const int cpm_type = 1;
const int cpa_type = 2;

struct CpmAd{
  Ice::Long adgroup_id_;
  Ice::Long adzone_id_;
  Ice::Long contract_start_time_;
  Ice::Long start_time_;
  int duration_;
  int past_days_;
  Ice::Long pv_;
  Ice::Long cost_;
  Ice::Long past_pv_;
  Ice::Long past_click_;
  Ice::Long past_cost_;
  Ice::Long today_pv_;
  Ice::Long today_click_;
  Ice::Long today_cost_;
  Ice::Long cur_hour_pv_;
  Ice::Long cur_hour_click_;
  Ice::Long cur_hour_cost_;
  int price_;
  int type_;
};

class DemoLoaderCache :public AdDemoQualityManager, public MyUtil::Singleton<DemoLoaderCache> {
public:
  virtual AdQualityMap GetDemoPool(const ::Ice::Current& = ::Ice::Current());
  virtual AdQualityMap GetAppPool(const ::Ice::Current& = ::Ice::Current());
  virtual FreshAdQualityMap GetFreshAdMap(const ::Ice::Current& = ::Ice::Current());
  virtual FreshAdQualityMap GetDisplayLessAds(const ::Ice::Current& = ::Ice::Current());
  
  virtual AdQualityScoreMap GetAdsQuality(const ::Ice::Current& = ::Ice::Current());  //by
  
  virtual AdPosMap GetAverageEcpm(const ::Ice::Current& = ::Ice::Current());
  virtual AdSortPosMap GetAppSortPos(const ::Ice::Current& = ::Ice::Current());
  virtual CpmAdgroupMap GetCpmAdGroupMap(const ::Ice::Current& = ::Ice::Current());
  virtual LimitedAdGroupMap GetLimitedAdGroupMap(const ::Ice::Current& = ::Ice::Current());
  void LoadPool();
  void LoadAppPool();
  void LoadDisplayLessAd();
  void LoadAverageEcpm();
  void LoadAppSortPos();
  void LoadCpmPool();
  void LoadCpmMemberPool(CpmAdgroupMap &cpm_member_groups);
  void LoadLimitedAdPool();
  void LoadSystemParameter();
  string GetParameterByName(string para_name);
private:
  static const int TIME_RANGE = 48;
  void LoadByTime(const string & begin_time, const string & end_time);
  void LoadUpdatedAd(set<Ice::Long>& update_ads);
  void InsertAmplifyGrouptoPool(AdQualityMap& res_pool);
  void InsertUpdateGrouptoPool(AdQualityMap& res_pool);
  void ExpandCtrByPos(map<string,map<Ice::Long,map<int,pair<Ice::Long, Ice::Long> > > >& temp_pool,
      AdQualityMap& res_pool,int type);
  void ExpandCtrByZone(map<string,map<Ice::Long,map<int,pair<Ice::Long, Ice::Long> > > >& temp_pool,
      AdQualityMap& res_pool,int type);
  void ExpandEcpmByZone(map<Ice::Long,map<Ice::Long,map<int,pair<Ice::Long, Ice::Long> > > >& temp_pool,
      map<Ice::Long, double>& res_pool);
  void ExpandCtrByPosAndZone(map<string,map<Ice::Long,map<int,pair<Ice::Long, Ice::Long> > > >& temp_pool,
      AdQualityMap& res_pool,int type);
	void ExpandCtrByPosAndZoneForAmplify(map<string,map<Ice::Long,map<int,pair<Ice::Long, Ice::Long> > > >& temp_pool,
      AdQualityMap& res_pool,map<Ice::Long,Ice::Long>& amplify_gids_price);
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

    int GetPvThreshold(int type) {
    int pv_th = 100000;
    switch(type) {
      case no_gender:
        pv_th = AdDemoConfig::instance().GetNoGenderPvTh();
        break;
      case no_stage:
        pv_th = AdDemoConfig::instance().GetNoStagePvTh();
        break;
      case spid_type:
        pv_th = AdDemoConfig::instance().GetSpidPvTh();
        break;
      case all_type:
        pv_th = AdDemoConfig::instance().GetAllPvTh();
        break;
      case two_feature:
        pv_th = AdDemoConfig::instance().GetTwoFeaturePvTh();
        break;
      case app_two_feature:
        pv_th = AdDemoConfig::instance().GetAppTwoFeaturePvTh();
        break;      
      case app_no_gender:
        pv_th = AdDemoConfig::instance().GetAppNoGenderPvTh();
        break;
      case app_no_stage:
        pv_th = AdDemoConfig::instance().GetAppNoStagePvTh();
        break;
      case app_spid_type:
        pv_th = AdDemoConfig::instance().GetAppSpidPvTh();
        break;
      case app_all_type:
        pv_th = AdDemoConfig::instance().GetAppAllPvTh();
        break;
      default:
        MCE_WARN("GetPvThreshold,no type:" << type);
        pv_th = 100000;
    }
    return pv_th;
  }

  int GetClickThreshold(int type) {
    int click_th = 20;
    switch(type) {
      case no_gender:
        click_th = AdDemoConfig::instance().GetNoGenderClickTh();
        break;
      case no_stage:
        click_th = AdDemoConfig::instance().GetNoStageClickTh();
        break;
      case spid_type:
        click_th = AdDemoConfig::instance().GetSpidClickTh();
        break;
      case all_type:
        click_th = AdDemoConfig::instance().GetAllClickTh();
        break;
      case two_feature:
        click_th = AdDemoConfig::instance().GetTwoFeatureClickTh();
        break;
      case app_two_feature:
        click_th = AdDemoConfig::instance().GetAppTwoFeatureClickTh();
        break;      
      case app_no_gender:
        click_th = AdDemoConfig::instance().GetAppNoGenderClickTh();
        break;
      case app_no_stage:
        click_th = AdDemoConfig::instance().GetAppNoStageClickTh();
        break;
      case app_spid_type:
        click_th = AdDemoConfig::instance().GetAppSpidClickTh();
        break;
      case app_all_type:
        click_th = AdDemoConfig::instance().GetAppAllClickTh();
        break;
      default:
        MCE_WARN("GetClickThreshold, no type:" << type);
        click_th = 20;
    }
    return click_th;
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

  AdQualityMap demo_pool;
  AdQualityMap app_pool;
  AdSortPosMap app_sort_pos;
  CpmAdgroupMap cpm_groups;
  LimitedAdGroupMap limited_groups;

  FreshAdQualityMap display_less_gids_;
  map<Ice::Long,pair<Ice::Long,double> > ecpm_table;
  map<string, double> average_ecpm_talbe_;
  map<string, string> system_para_;

  //Ice::Long hour_impr;
  static const size_t DAYCOUNT = 10;
  static const size_t HOURCOUNT = 24;
  static const size_t TIMEINTERVAL = 1 * 60 * 60;
  static const int NO_USE = -1;
  IceUtil::RWRecMutex mutex_;
};

class DemoLoaderCacheTimer : public MyUtil::Timer {
public:
  DemoLoaderCacheTimer(int time = 5 * 60 * 1000) :
    Timer(time) { //30閸掑棝鎸撻柌宥嗘煀鐠侊紕鐣绘稉锟筋偧
  }
  virtual void handle() {
    MCE_INFO("DemoLoaderCacheTimer:begin");
    DemoLoaderCache::instance().LoadPool();
    MCE_INFO("DemoLoaderCacheTimer:stop");
    MyUtil::TaskManager::instance().schedule(new DemoLoaderCacheTimer(3 * 60 * 1000));
  }
};

class CpmLoaderCacheTimer : public MyUtil::Timer {
public:
  CpmLoaderCacheTimer(int time = 2 * 60 * 1000) :
    Timer(time,-5) { //30閸掑棝鎸撻柌宥嗘煀鐠侊紕鐣绘稉锟筋偧
  }
  virtual void handle() {
    MCE_INFO("CpmLoaderCacheTimer:begin");
    DemoLoaderCache::instance().LoadCpmPool();
    MCE_INFO("CpmLoaderCacheTimer:stop");
    MyUtil::TaskManager::instance().schedule(new CpmLoaderCacheTimer(2 * 60 * 1000));
  }
};
class AppLoaderCacheTimer : public MyUtil::Timer {
public:
  AppLoaderCacheTimer(int time = 5 * 60 * 1000) :
    Timer(time) { //30閸掑棝鎸撻柌宥嗘煀鐠侊紕鐣绘稉锟筋偧
  }
  virtual void handle() {
    MCE_INFO("AppLoaderCacheTimer:begin");
    DemoLoaderCache::instance().LoadAppPool();
    MCE_INFO("AppLoaderCacheTimer:stop");
    MyUtil::TaskManager::instance().schedule(new AppLoaderCacheTimer(4 * 60 * 1000));
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

class DemoLoaderSortPosCacheTimer : public MyUtil::Timer {
public:
  DemoLoaderSortPosCacheTimer(int time =0 ) :Timer(time){
  }
  virtual void handle() {
  MCE_INFO("DemoLoaderFreshAdCacheTimer handle :begin");
  MyUtil::TaskManager::instance().schedule(new DemoLoaderSortPosCacheTimer(60 * 60 * 1000)); //每1小时读一次
  DemoLoaderCache::instance().LoadAppSortPos();
  }

};

class DemoLoaderSystemParaTimer : public MyUtil::Timer {
public:
  DemoLoaderSystemParaTimer(int time =0 ) :Timer(time){
  }
  virtual void handle() {
  MCE_INFO("DemoLoaderSystemParaTimer handle :begin");
  MyUtil::TaskManager::instance().schedule(new DemoLoaderSystemParaTimer(3 * 60 * 1000)); //每3分钟读一次
  DemoLoaderCache::instance().LoadSystemParameter();
  }

};

class DemoLoaderLimitedAdTimer : public MyUtil::Timer {
public:
  DemoLoaderLimitedAdTimer(int time =0 ) :Timer(time){
  }
  virtual void handle() {
  MCE_INFO("DemoLoaderLimitedAdTimer handle :begin");
  MyUtil::TaskManager::instance().schedule(new DemoLoaderLimitedAdTimer(60 * 60 * 1000)); //每1小时读一次
  DemoLoaderCache::instance().LoadLimitedAdPool();
  }

};

}
}

#endif /* DEMOLOADERCACHE_H_ */
