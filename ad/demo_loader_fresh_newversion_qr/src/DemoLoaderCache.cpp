/*
 * DemoLoaderCache.cpp
 *
 *  Created on: May 10, 2011
 *      Author: kingsfield
 */

#include "DemoLoaderCache.h"
#include <connection.h>
#include <query.h>
#include <string>
#include <algorithm>
#include "AdConfig.h"
#include "QueryRunner.h"
#include "Util.h"
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/regex.hpp>
using namespace boost;

bool MyCallback(const char *dump_path, const char *minidump_id, void *context, bool succeeded) {
  printf("got crash %s %s\n", dump_path, minidump_id);
  return true;
}

void ::MyUtil::initialize() {
  static google_breakpad::ExceptionHandler eh(".", 0, MyCallback, 0, true);
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&xce::ad::DemoLoaderCache::instance(), service.createIdentity("M",""));

  MyUtil::TaskManager::instance().schedule(new xce::ad::AdDemoConfig::ConfigLoadTimer(0));
  xce::ad::AdDemoConfig::instance().LoadConfig();
  //xce::ad::DemoLoaderCache::instance().LoadPool();
  MyUtil::TaskManager::instance().schedule(new xce::ad::DemoLoaderFreshAdCacheTimer(0));
  MyUtil::TaskManager::instance().schedule(new xce::ad::DemoLoaderAverageEcpmTimer(0));
  MyUtil::TaskManager::instance().schedule(new xce::ad::DemoLoaderDisplayLessAdTimer(0));
  //TaskManager::instance().scheduleRepeated(new xce::ad::DemoLoaderCacheTimer());
  xce::ad::DemoLoaderCache::instance().LoadPool();
  TaskManager::instance().schedule(new xce::ad::DemoLoaderCacheTimer());

}

namespace xce{
namespace ad{
using namespace mysqlpp;

string GetFreshKey(Ice::Long zone_id, int pos) {
  Ice::Long key = zone_id * 10 + pos;
  ostringstream sin;
  sin << key;
  return sin.str();
}
Ice::Long GetTime(int year, int mon, int day, int hour) {
  return (((year * 100 + mon) * 100 + day) * 100 + hour);
}
vector<string> GetZonePosByState(int state) {
  vector<string> result;
  if(state == 1) {
    result.push_back(pos_723);
    result.push_back(pos_702);
    result.push_back(pos_703);
  } else if(state == 2) {
    result.push_back(pos_701);
    result.push_back(pos_722);
    result.push_back(pos_633);
  } else if(state == 3) {
    result.push_back(pos_721);
    result.push_back(pos_632);
    result.push_back(pos_631);

  }
  return result;
}
int GetStateByZonePos(string key) {
  if(key == pos_723 || key == pos_702 || key == pos_703) {
    return 1;
  } else if(key == pos_701 || key == pos_722 || key == pos_633) {
    return 2;
  } else if(key == pos_721 || key == pos_632 || key == pos_631) {
    return 3;
  }
  MCE_WARN("GetStateByZonePos,invalid key:" << key);
  return 1;
}
void FreshAdGroup::calculate_state() {
  int c_state = 0;
  int impr_th = AdDemoConfig::instance().GetStatePvTh();
  int click_th = AdDemoConfig::instance().GetStateClickPvTh();
  MCE_DEBUG("FreshAdGroup::calculate_state,pv_th:" << impr_th <<",click_th:" << click_th );
  for(int i = 1; i <= 3; ++ i ) {
    vector<string> zone_pos =GetZonePosByState(i);
    Ice::Long impr = 0;
    Ice::Long click = 0;
    for(vector<string>::iterator it = zone_pos.begin(); it != zone_pos.end(); ++it) {
      impr += zone_pos_pv_click_[*it].first;
      click += zone_pos_pv_click_[*it].second;
    }
    if(impr < impr_th && click < click_th) {
      break;
    }
    c_state = i;
  }
  current_state_ = c_state;

}
void FreshAdGroup::reset_data(){
  for(map<string,pair<Ice::Long,Ice::Long> >::iterator itz = zone_pos_pv_click_.begin(); itz != zone_pos_pv_click_.end(); ++itz) {
    itz->second.first = 0;
    itz->second.second = 0;
  }
}
void FreshAdGroup::PrintHelp() {
  ostringstream os;
  for(map<int,double>::iterator it = predict_state_ctr_.begin(); it != predict_state_ctr_.end(); ++it) {
    os <<" " <<it->first <<" " <<it->second;
  }
  ostringstream oo;
  for(map<string, pair<Ice::Long,Ice::Long> >::iterator itz = zone_pos_pv_click_.begin();
      itz != zone_pos_pv_click_.end(); ++itz) {
    oo <<" " <<itz->first.substr(10,3) <<" " << itz->second.first << " " <<itz->second.second;
  }
  MCE_INFO("PrintHelp gid:" <<group_id_ << ",sponsor_id:" << sponsor_id_ <<",sponsor ctr:" <<sponsor_ctr_
      <<",price:" <<price_ <<",current_state:" <<current_state_ <<" predict ecpm:" <<os.str()
      << " orign data:" << oo.str());
}
void FreshAdGroup::predict_fresh_ctr() {
  if(current_state_ >= 3) { //说明是删除状态，所有状态都已经测试过了,不参与投放
    MCE_INFO("FreshAdGroup::predict_fresh_ctr:test over gid:" << group_id_ <<",state:" << current_state_);
    return;
  }
  if(current_state_ == 0) { //说明当前状态处于全新，或者在第一级别的广告投放量不满足10w,需要在第一级别广告投放测试
    vector<string> zone_pos = GetZonePosByState(1);
    Ice::Long impr = 0;
    Ice::Long click = 0;
    for(int i = 1; i <= 3; i++) {
      vector<string> zone_pos = GetZonePosByState(i);
      for(vector<string>::iterator itz = zone_pos.begin(); itz != zone_pos.end(); ++itz){
        impr += zone_pos_pv_click_[*itz].first;
        click += zone_pos_pv_click_[*itz].second;
      }
    }

    double ctr = 0.0;
    if(impr > 0.0) {
      ctr = (double)click / (double) impr;
    }
    double ecpm = ctr * price_;
    double base_ecpm = base_ecpm_state1;
    if(impr > 2000){ //已经有展示
      if(ecpm > 0.0) { //有点击
        predict_state_ctr_[1] = ecpm;
      } else { //无点击
        predict_state_ctr_[1] = base_ecpm * 0.9;
      }
    }else if(sponsor_ctr_ > 0.0){
      predict_state_ctr_[1] = sponsor_ctr_ * price_;
    } else {
      predict_state_ctr_[1] = base_ecpm;
    }
    return;
  }
  if(current_state_ == 1) { //说明第一级别的广告已经满足测试阈值，考虑是否在第二级别广告或者第三级别广告投放测试
    vector<string> zone_pos = GetZonePosByState(current_state_);
    Ice::Long impr = 0;
    Ice::Long click = 0;
    for(vector<string>::iterator itz = zone_pos.begin(); itz != zone_pos.end(); ++itz){
      impr += zone_pos_pv_click_[*itz].first;
      click += zone_pos_pv_click_[*itz].second;
    }
    double ctr = 0.0;
    if(impr > 0.0) {
      ctr = (double)click / (double) impr;
    }
    double ecpm = ctr * price_;
    double base_ecpm = base_ecpm_state1;
    double alpha = ecpm / base_ecpm;
    if(alpha <= 1.0) { //测试点击率不通过，不再进行投放
      MCE_INFO("FreshAdGroup::predict_fresh_ctr:test over,low quality gid:" << group_id_ <<",state:" << current_state_<<",ecpm:" << ecpm);
      return;
    } else if(alpha >= 4) { //显著高，需要在第二级别和第三级别的广告上投放测试
      predict_state_ctr_[2] = ecpm;
      predict_state_ctr_[3] = ecpm;

    } else if(alpha >1.0 && alpha <4.0){ //测试通过，需要在第二级别上投放进一步测试
      predict_state_ctr_[2] = ecpm;
    }
    return;
  }
  if(current_state_ == 2) { //说明第二级别的广告已经满足测试阈值，考虑是否在第三级别广告投放测试
    vector<string> zone_pos = GetZonePosByState(current_state_);
    Ice::Long impr = 0;
    Ice::Long click = 0;
    for(vector<string>::iterator itz = zone_pos.begin(); itz != zone_pos.end(); ++itz){
      impr += zone_pos_pv_click_[*itz].first;
      click += zone_pos_pv_click_[*itz].second;
    }
    double ctr = 0.0;
    if(impr > 0.0) {
      ctr = (double)click / (double) impr;
    }
    double ecpm = ctr * price_;
    double base_ecpm = base_ecpm_state2;
    double alpha = ecpm / base_ecpm;
    if(alpha <= 1.0) { //测试点击率不通过，不再进行投放
      MCE_INFO("FreshAdGroup::predict_fresh_ctr:test over,low quality gid:" << group_id_ <<",state:" << current_state_ <<",ecpm:" <<ecpm);
      return;
    } else {//测试通过，需要在第三级别上投放进一步测试
      predict_state_ctr_[3] = ecpm;
    }
    return;
  }
}

AdQualityMap DemoLoaderCache::GetDemoPool(const ::Ice::Current& current){
  MCE_INFO("DemoLoaderCache::GetDemoPool--> call from " << current.con->toString());
  AdQualityMap result;
  IceUtil::RWRecMutex::RLock lock(mutex_);  
  result = demo_pool;
  MCE_INFO("DemoLoaderCache::GetDemoPool,pool size:"<<result.size());
  return result;
}

AdQualityMap DemoLoaderCache::GetAppPool(const ::Ice::Current& current){
  MCE_INFO("DemoLoaderCache::GetAppPool--> call from " << current.con->toString());
  AdQualityMap result;
  IceUtil::RWRecMutex::RLock lock(mutex_);  
  result = app_pool;
  MCE_INFO("DemoLoaderCache::GetAppPool,pool size:"<<result.size());
  return result;
}


FreshAdQualityMap DemoLoaderCache::GetFreshAdMap(const ::Ice::Current& current){
  MCE_INFO("DemoLoaderCache::GetFreshAdMap--> call from " << current.con->toString());
  FreshAdQualityMap result;
  IceUtil::RWRecMutex::RLock lock(mutex_);
  result = fresh_ad_pool_;
  MCE_INFO("DemoLoaderCache::GetFreshAd,pool size:"<<result.size()); 
  return result;
}

FreshAdQualityMap DemoLoaderCache::GetDisplayLessAds(const ::Ice::Current& current){
  MCE_INFO("DemoLoaderCache::GetDisplayLessAds--> call from " << current.con->toString());
  FreshAdQualityMap result;
  IceUtil::RWRecMutex::RLock lock(mutex_);
  result = display_less_gids_;
  MCE_INFO("DemoLoaderCache::GetDisplayLessAds,gids size:"<<result.size());
  return result;
}

AdPosMap DemoLoaderCache::GetAverageEcpm(const ::Ice::Current& current){
  MCE_INFO("DemoLoaderCache::GetAverageEcpm--> call from " << current.con->toString());
  AdPosMap result;
  IceUtil::RWRecMutex::RLock lock(mutex_);
  result = average_ecpm_talbe_;
  MCE_INFO("DemoLoaderCache::GetAverageEcpm,AdPosMap size:"<<result.size());
  return result;
}


string TransformForSql(set<Ice::Long>& gids) {
  ostringstream result;
  if (gids.empty()) {
    return result.str();
  }
  for(set<Ice::Long>::iterator itg = gids.begin(); itg!= gids.end(); ++itg) {
    if(itg == gids.begin()) {
      result << " ( " << *itg;
    }else {
      result <<" , " << *itg;
    }
  }
  result << " ) ";
  return result.str();
}
void DemoLoaderCache::LoadAverageEcpm(){
  MCE_INFO("DemoLoaderCache::LoadAverageEcpm begin");
  map<string, double> average_ecpm_talbe;
  Ice::Long t = time(NULL);
  ostringstream sin;
  MyUtil::Date ecpm_begin = MyUtil::Date::seconds(t - 24 * 3600);  //读前一天当前1小时开始时间
  MyUtil::Date ecpm_end = MyUtil::Date::seconds(t - 23 * 3600);
  string ecpm_begin_time = ecpm_begin.str("%Y-%m-%d %H:%M:%S");
  string ecpm_end_time = ecpm_end.str("%Y-%m-%d %H:%M:%S");
  //Connection conn("test","10.3.16.119","ad","rrad");
  stringstream ctr_sql;
  //MyUtil::Date now = MyUtil::Date::seconds(time(NULL));
  ctr_sql << "select zone_id,pos_id,sum(impr) as sum_impr,sum(cost) as sum_cost "
      "from average_ecpm where time >= '" << ecpm_begin_time << ":00:00' and time <='" <<
      ecpm_end_time  << ":00:00' group by zone_id,pos_id";
  MCE_INFO("DemoLoaderCache::LoadAverageEcpm try to excute " << ctr_sql.str());
  string ctr_s= ctr_sql.str();
  try {
    const char* db_host = AdDemoConfig::instance().db_host().c_str();
    const char* db_name = AdDemoConfig::instance().db_name().c_str();
    Connection conn(db_name,db_host,"ad","rrad");

    Query ctr_q(&conn,true,ctr_s.c_str());
    mysqlpp::StoreQueryResult ctr_res = ctr_q.store();
    if (!ctr_res || ctr_res.num_rows() <= 0) {
      MCE_WARN("DemoLoaderCache::LoadAverageEcpm Error " << ctr_sql.str() << ".  select  fail!");
    }
    MCE_INFO("DemoLoaderCache::LoadAverageEcpm   res size : " << ctr_res.num_rows());
    for (size_t i = 0; i < ctr_res.num_rows(); ++i) {
      mysqlpp::Row row = ctr_res.at(i);
      Ice::Long zone_id = (Ice::Long) row["zone_id"];
      int pos_id = (int) row["pos_id"];
      Ice::Long sum_impr = (Ice::Long) row["sum_impr"];
      double sum_cost = (Ice::Long) row["sum_cost"];
      string key = GetKey(zone_id, pos_id);
      double ecpm = 10.0;
      if(sum_impr > 0) {
        ecpm = (double)sum_cost / (double)sum_impr;
      }
      average_ecpm_talbe[key] = ecpm;
    }
  }catch (std::exception& e) {
    MCE_WARN("DemoLoaderCache::LoadAverageEcpm error: " << e.what());
  } catch (...) {
    MCE_WARN("DemoLoaderCache::LoadAverageEcpm error!");
  }

  IceUtil::RWRecMutex::WLock lock(mutex_);
  average_ecpm_talbe_.swap(average_ecpm_talbe);
  MCE_INFO("DemoLoaderCache::LoadAverageEcpm stop,average_ecpm_table size:" << average_ecpm_talbe_.size());
}
void DemoLoaderCache::LoadDisplayLessAd(){
  MCE_INFO("DemoLoaderCache::LoadDisplayLessAd begin");
  MyUtil::Date day = MyUtil::Date::seconds(time(NULL) - 10 * 24 * 3600);  //10天的数据
  string begin_time = day.str("%Y-%m-%d %H");
  Ice::Long begin_pos = GetTime(day.year(),day.month(),day.day(),day.hour());
  map<string, double> average_ctr;

  FreshAdQualityMap result;
  mysqlpp::StoreQueryResult res;
  try {
    const char* db_host = AdDemoConfig::instance().db_host().c_str();
    const char* db_name = AdDemoConfig::instance().db_name().c_str();
    Connection conn(db_name,db_host,"ad","rrad");
    ///////////////////////////获得前一天当前小时平均点击率//////////////////////////

    Ice::Long t = time(NULL);
    ostringstream sin;
    MyUtil::Date ctr_begin = MyUtil::Date::seconds(t - 24 * 3600);  //读前一天当前小时
    MyUtil::Date ctr_end = MyUtil::Date::seconds(t - 23 * 3600);
    //string ctr_begin_time = ctr_begin.str("%Y-%m-%d %H:%M:%S");
    //string ctr_end_time = ctr_end.str("%Y-%m-%d %H:%M:%S");
    Ice::Long bgein_pos = GetTime(ctr_begin.year(),ctr_begin.month(),ctr_begin.day(), ctr_begin.hour());
    Ice::Long end_pos = GetTime(ctr_end.year(),ctr_end.month(),ctr_end.day(), ctr_end.hour());
    //Connection conn("test","10.3.16.119","ad","rrad");
    stringstream ctr_sql;
    //MyUtil::Date now = MyUtil::Date::seconds(time(NULL));
    ctr_sql << "select adzone_id,abs_pos,sum(pv_count) as sum_impr,sum(click_count) as sum_click from "
        " rt_HZPMGGS where time >= " << begin_pos << " and time <= "  << end_pos  << " group by adzone_id,abs_pos";
    MCE_INFO("DemoLoaderCache::LoadDisplayLessAd try to excute " << ctr_sql.str());
    string ctr_s= ctr_sql.str();
    Query ctr_q(&conn,true,ctr_s.c_str());
    mysqlpp::StoreQueryResult ctr_res = ctr_q.store();
    if (!ctr_res || ctr_res.num_rows() <= 0) {
      MCE_WARN("DemoLoaderCache::LoadDisplayLessAd Error " << ctr_sql.str() << ".  select  fail!");
    }
    MCE_INFO("DemoLoaderCache::LoadDisplayLessAd  average_ctr res size : " << ctr_res.num_rows());
    for (size_t i = 0; i < ctr_res.num_rows(); ++i) {
      mysqlpp::Row row = ctr_res.at(i);
      Ice::Long zone_id = (Ice::Long) row["adzone_id"];
      int pos_id = (int) row["abs_pos"];
      Ice::Long sum_impr = (Ice::Long) row["sum_impr"];
      Ice::Long sum_click = (Ice::Long) row["sum_click"];
      string key = GetKey(zone_id, pos_id);
      double ctr = 0.000001;
      if(sum_impr > 0) {
        ctr = (double)sum_click / (double) sum_impr;
      }
      average_ctr[key] = ctr;
    }
    ///////////////////////////加载展示量比较少的广告/////////////////////////
    stringstream sql;
    sql << "select adzone_id, abs_pos, adgroup_id as group_id, sum(pv_count) as sum_impr , sum(click_count) as sum_click  from "
        " rt_HZPMGGS  where  time >= " << begin_pos
        << " group by adzone_id, abs_pos, adgroup_id";
    MCE_INFO("DemoLoaderCache::LoadDisplayLessAd try to excute " << sql.str());
    string s= sql.str();
    Query q(&conn,true,s.c_str());
    //mysqlpp::StoreQueryResult res = q.store();
    res = q.store();
    if (!res || res.num_rows() <= 0) {
      MCE_INFO("DemoLoaderCache::LoadDisplayLessAd has no display less gids ");
    }

    MCE_INFO("DemoLoaderCache::LoadDisplayLessAd. display gids Size=: " << res.num_rows());
    if (res && res.num_rows() > 0) {
      for (size_t i = 0; i < res.num_rows(); ++i) {
        mysqlpp::Row row = res.at(i);
        Ice::Long zone_id = (Ice::Long) row["adzone_id"];
        int pos_id = (Ice::Long) row["abs_pos"];
        Ice::Long group_id = (Ice::Long) row["group_id"];
        Ice::Long impr = (Ice::Long) row["sum_impr"];
        Ice::Long click = (Ice::Long) row["sum_click"];

        if(impr < 100000) {//只找到那些展示比较少的广告
          string key = GetKey(zone_id, pos_id);
          double a_ctr = 0.000001;
          map<string,double>::iterator ita = average_ctr.find(key);
          if(ita != average_ctr.end()) {
            a_ctr = ita->second;
          }

          double ctr = a_ctr;
          if(impr > 0 && click > 0) {
            ctr = double (click) /double(impr);
          }
          if(impr >10000 && 0 == click ) { //展示没有点击，降低出现机会
            ctr *= 0.8;
          }
          FreshAdQualityMap::iterator itr = result.find(key);
          if(itr != result.end()) {
            itr->second.insert(make_pair(group_id,ctr));
          } else {
            map<Ice::Long, double> gid_ctr;
            gid_ctr[group_id] = ctr;
            //result.insert(make_pair<key,gid_ctr>);
            result[key] = gid_ctr;
          }
        }
      }
    }
  }catch (std::exception& e) {
    MCE_WARN("DemoLoaderCache::LoadDisplayLessAd error: " << e.what());
  } catch (...) {
    MCE_WARN("DemoLoaderCache::LoadDisplayLessAd error!");
  }
  IceUtil::RWRecMutex::WLock lock(mutex_);
  display_less_gids_.swap(result);
  MCE_INFO("DemoLoaderCache::LoadDisplayLessAd stop,display_less_gids size:" << display_less_gids_.size());
}
void DemoLoaderCache::LoadFreshData() {
  MCE_INFO("DemoLoaderCache::LoadFreshData begin");
  map<Ice::Long, FreshAdGroupPtr> FreshAdMap;
  set<Ice::Long> fresh_gids;
  set<Ice::Long> fresh_sponsors;
  map<Ice::Long, double> fresh_sponsors_ctr;
  set<Ice::Long> update_ads;
  FreshAdQualityMap fresh_ad_pool;
  MyUtil::Date the_day_before_yesterday = MyUtil::Date::seconds(time(NULL) - 10 * 24 * 3600);  //10天的数据Fixed 2-->10 天
  string begin_time = the_day_before_yesterday.str("%Y-%m-%d %H");
  string begin_time_pos = boost::lexical_cast<string> (GetTime(the_day_before_yesterday.year(),the_day_before_yesterday.month(),
      the_day_before_yesterday.day(),the_day_before_yesterday.hour()));
  //查找新广告，加载进结构
  LoadFreshAd(begin_time, FreshAdMap, fresh_gids, fresh_sponsors);

  //加载新广告的历史投放记录
  LoadAdGroupData(begin_time_pos, FreshAdMap, fresh_gids);

  //加载新广告的历史sponsor点击率，并设置到相应的位置上
  fresh_sponsors_ctr = GetFreshAdSponsorData(begin_time_pos, fresh_sponsors);
  SetFreshAdSponsorData(FreshAdMap, fresh_sponsors_ctr);

  //加载最近更新过的广告

  LoadUpdatedAd(update_ads);
  MCE_INFO("DemoLoaderCache::LoadFreshData-->Load updated gids count:" << update_ads.size());
  if(!update_ads.empty()) {
    set<Ice::Long> updated_sponsors;
    map<Ice::Long, double> updated_sponsors_ctr;
    ResetUpdatedAdGroupData(FreshAdMap, update_ads);
    MCE_INFO("DemoLoaderCache::LoadFreshData-->befor insert FreshAdMap,size:" << FreshAdMap.size());
    LoadUpdateAdInfo(FreshAdMap, update_ads, updated_sponsors);
    ReloadUpdatedAdGroupData(FreshAdMap, update_ads);
    updated_sponsors_ctr = GetFreshAdSponsorData(begin_time_pos, updated_sponsors);
    SetFreshAdSponsorData(FreshAdMap, updated_sponsors_ctr);
    MCE_INFO("DemoLoaderCache::LoadFreshData-->after insert FreshAdMap,size:" << FreshAdMap.size());
  }

  //计算新广告的投放状态
  CalculateState(FreshAdMap);

  //根据新广告的投放状态，预测新广告在不同位置上的点击率
  PredictFreshAdCtr(FreshAdMap);

  //根据预测点击率 分配新广告的投放量
  fresh_ad_pool = AssignFreshAdImpr(FreshAdMap);

  IceUtil::RWRecMutex::WLock lock(mutex_);
  fresh_ad_pool_.swap(fresh_ad_pool);
  MCE_INFO("DemoLoaderCache::LoadFreshData stop");
}

void DemoLoaderCache::LoadUpdateAdInfo(map<Ice::Long, FreshAdGroupPtr> &FreshAdMap,
    set<Ice::Long> & updated_gids, set<Ice::Long> & updated_sponsors) {
  string update_list = TransformForSql(updated_gids);
  Statement sql;
  stringstream sql_string;
  sql_string << "select adgroup_id as group_id,  member_id as sponsor_id, max_price as price from adgroup where adgroup_id in " << update_list;
  sql << sql_string.str();
  MCE_INFO("try to execute:" << sql_string.str());
  try {
    mysqlpp::StoreQueryResult res = QueryRunner("ade_db", CDbRServer, "adgroup").store(sql);
    if (!res || res.num_rows() < 0){
      MCE_WARN("DemoLoaderCache LoadFreshAd error");
      return;
    }
    else if (res.num_rows() == 0) {
      MCE_INFO("DemoLoaderCache::LoadUpdateAdInfo Load update.NO updated ADS");
      return;
    }
    MCE_INFO("[DemoLoaderCache]::DemoLoaderCache::LoadUpdateAdInfo. updated Size=: " << res.num_rows());
    //stringstream fresh_ad_list;
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      Ice::Long group_id = (Ice::Long) row["group_id"];
      Ice::Long sponsor_id = (Ice::Long) row["sponsor_id"];
      double price = (Ice::Long) row["price"];
      FreshAdGroupPtr f_ptr = new FreshAdGroup(group_id,sponsor_id,price);
      FreshAdMap[group_id] = f_ptr;
      updated_sponsors.insert(sponsor_id);
    }
  }catch (std::exception& e) {
    MCE_WARN("LoadByTime --> load ad_quality error: " << e.what());
  } catch (...) {
    MCE_WARN("LoadByTime --> load ad_quality error!");
  }
}

void DemoLoaderCache::LoadFreshAd(string begin_time, map<Ice::Long, FreshAdGroupPtr> &FreshAdMap, set<Ice::Long> & fresh_gids, set<Ice::Long> & fresh_sponsors) {
  Statement sql;
  stringstream sql_string;
  sql_string << "select adgroup_id as group_id,  member_id as sponsor_id, max_price as price from adgroup where create_time >='" << begin_time << "' AND  member_status = 1 AND am_status = 1 AND audit_status = 1 AND delete_flag= 1 AND display_type = 4 AND daily_sys_status = 1 AND total_sys_status = 1";
  sql << sql_string.str();
  MCE_INFO("try to execute:" << sql_string.str());
  try {
    const string DB_INSTANCE = AdDemoConfig::instance().GetFreshDbName();
    mysqlpp::StoreQueryResult res = QueryRunner("ade_db", CDbRServer, "adgroup").store(sql);
    if (!res || res.num_rows() < 0){
      MCE_WARN("DemoLoaderCache LoadFreshAd error");
      return;
    }
    else if (res.num_rows() == 0) {
      MCE_INFO("DemoLoaderCache LoadFreshAd.NO NEW ADS");
      return;
    }
    MCE_INFO("[DemoLoaderCache]::LoadFreshAd. Fresh Size=: " << res.num_rows());  
    //stringstream fresh_ad_list;    
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      Ice::Long group_id = (Ice::Long) row["group_id"];
      Ice::Long sponsor_id = (Ice::Long) row["sponsor_id"];
      double price = (Ice::Long) row["price"];
      FreshAdGroupPtr f_ptr = new FreshAdGroup(group_id,sponsor_id,price);
      FreshAdMap[group_id] = f_ptr;
      fresh_gids.insert(group_id);
      fresh_sponsors.insert(sponsor_id);
    }
  }catch (std::exception& e) {
    MCE_WARN("LoadByTime --> load ad_quality error: " << e.what());
  } catch (...) {
    MCE_WARN("LoadByTime --> load ad_quality error!");
  }
}
void DemoLoaderCache::LoadAdGroupData(string begin_time, map<Ice::Long, FreshAdGroupPtr> &FreshAdMap, set<Ice::Long> & fresh_gids) {
  mysqlpp::StoreQueryResult res;
  if(fresh_gids.empty()) {
    MCE_INFO("DemoLoaderCache::LoadAdGroupData,not get freshgids,size:" << fresh_gids.size());
    return;
  }
  string fresh_list = TransformForSql(fresh_gids);
  try {
    const char* db_host = AdDemoConfig::instance().db_host().c_str();
    const char* db_name = AdDemoConfig::instance().db_name().c_str();
    Connection conn(db_name,db_host,"ad","rrad");
    stringstream sql;
    sql.str("");
    sql << "select adzone_id, adgroup_id as group_id, abs_pos , sum(pv_count) as sum_impr , sum(click_count) as sum_click  from "
        " rt_HZPMGGS  where  time >= " << begin_time << " AND  adgroup_id  in " << fresh_list
        << " group by adzone_id, abs_pos, adgroup_id";
    MCE_INFO("try to excute " << sql.str());
    string s= sql.str();
    Query q(&conn,true,s.c_str());
    //mysqlpp::StoreQueryResult res = q.store();
    res = q.store();
    if (!res || res.num_rows() <= 0) {
      MCE_INFO("DemoLoaderCache::LoadAdGroupData all gids is truely fresh ");
    }
  }catch (std::exception& e) {
    MCE_WARN("LoadByTime --> load ad_quality error: " << e.what());
  } catch (...) {
    MCE_WARN("LoadByTime --> load ad_quality error!");
  }

  MCE_INFO("[DemoLoaderCache]::LoadFreshAd. Fresh Size=: " << res.num_rows());  
  if (res && res.num_rows() > 0) {
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      Ice::Long zone_id = (Ice::Long) row["adzone_id"];
      Ice::Long group_id = (Ice::Long) row["group_id"];
      int pos_id = (int) row["abs_pos"];
      Ice::Long impr = (Ice::Long) row["sum_impr"];
      Ice::Long click = (Ice::Long) row["sum_click"];
      map<Ice::Long, FreshAdGroupPtr>::iterator it = FreshAdMap.find(group_id);
      if(it != FreshAdMap.end()) {
        string fresh_key = GetFreshKey(zone_id, pos_id);
        it->second->SetData(fresh_key, impr, click);
      }
    }
  }
}
void DemoLoaderCache::ResetUpdatedAdGroupData(map<Ice::Long, FreshAdGroupPtr> &FreshAdMap, set<Ice::Long>& update_ads){
  MCE_INFO("DemoLoaderCache::ResetUpdatedAdGroupData,begin");
  for(set<Ice::Long> ::iterator itu = update_ads.begin(); itu != update_ads.end(); ++itu){
    map<Ice::Long, FreshAdGroupPtr> ::iterator itf = FreshAdMap.find(*itu);
    if(itf != FreshAdMap.end()) {
      itf->second->reset_data();
    }
  }
}
void DemoLoaderCache::LoadUpdatedAd(set<Ice::Long>& update_ads){ //只加载修改过的广告，不加载新建广告
  set<Ice::Long> fresh_gids;
  MyUtil::Date begin_day = MyUtil::Date::seconds(time(NULL) - 2 * 24 * 3600);  //2天的数据
  string begin_time = begin_day.str("%Y-%m-%d %H");
  /////////////////////////////////////Load fresh gids////////////////////////////////////
  Statement sql;
  stringstream sql_string;//加载两天之内的新广告
  sql_string << "select adgroup_id as group_id from adgroup where create_time >='"
      << begin_time << "' AND  member_status = 1 AND am_status = 1 AND audit_status = 1 "
          "AND delete_flag= 1 AND display_type = 4 AND daily_sys_status = 1 AND total_sys_status = 1";
  sql << sql_string.str();
  MCE_INFO("DemoLoaderCache::LoadUpdatedAd try to execute:" << sql_string.str());
  try {
    mysqlpp::StoreQueryResult res = QueryRunner("ade_db", CDbRServer, "adgroup").store(sql);
    if (!res || res.num_rows() < 0){
      MCE_WARN("DemoLoaderCache::LoadUpdatedAd error");
      return;
    }
    else if (res.num_rows() == 0) {
      MCE_INFO("DemoLoaderCache::LoadUpdatedAd.NO NEW ADS");
      return;
    }
    MCE_INFO("DemoLoaderCache::LoadUpdatedAd. Fresh Size=: " << res.num_rows());
    //stringstream fresh_ad_list;
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      Ice::Long group_id = (Ice::Long) row["group_id"];
      fresh_gids.insert(group_id);
    }
  }catch (std::exception& e) {
    MCE_WARN("DemoLoaderCache::LoadUpdatedAd error: " << e.what());
  } catch (...) {
    MCE_WARN("DemoLoaderCache::LoadUpdatedAd error!");
  }
  ///////////////////////////////////////////////////////////////////////////////////////
  MCE_INFO("DemoLoaderCache::LoadUpdatedAd, begin:updte ads size:" << update_ads.size() << ",fresh size:" << fresh_gids.size());
  try {
    Connection conn(db_jebe_name,db_jebe_host,"ad","adjb###");
    stringstream sql;
    sql << "select adgroup_id as group_id from audit_his where audit_time >='" <<
            begin_time << "' AND  audit_status  = 1";
    MCE_INFO("DemoLoaderCache::LoadUpdatedAd try to excute " << sql.str());
    string s= sql.str();
    Query q(&conn,true,s.c_str());
    mysqlpp::StoreQueryResult res = q.store();
    if (!res) {
      MCE_WARN("DemoLoaderCache Load Error " << sql.str() << ".  select  fail!");
      return;
    }
    MCE_INFO("DemoLoaderCache::LoadUpdatedAd: updated gids Size=: " << res.num_rows());
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      Ice::Long group_id = (Ice::Long) row["group_id"];
      set<Ice::Long>::iterator itf = fresh_gids.find(group_id);
      if(itf == fresh_gids.end()) {
        update_ads.insert(group_id);
      }
    }
    MCE_INFO("DemoLoaderCache::LoadUpdatedAd -->Load Update gids, size:" << update_ads.size());
  }catch (std::exception& e) {
    MCE_WARN("DemoLoaderCache::LoadUpdatedAd error: " << e.what());
  } catch (...) {
    MCE_WARN("DemoLoaderCache::LoadUpdatedAd error!");
  }
  MCE_INFO("DemoLoaderCache::LoadUpdatedAd, end: updte ads size:" << update_ads.size() << ",fresh size:" << fresh_gids.size());
}

void DemoLoaderCache::ReloadUpdatedAdGroupData(map<Ice::Long, FreshAdGroupPtr> &FreshAdMap, set<Ice::Long>& update_ads){
  MCE_INFO("DemoLoaderCache::ReloadUpdatedAdGroupData, begin")
  MyUtil::Date begin_day = MyUtil::Date::seconds(time(NULL) - 1 * 24 * 3600);  //2天的数据
  //string begin_time = begin_day.str("%Y-%m-%d %H");
  string begin_time = boost::lexical_cast<string> (GetTime(begin_day.year(),begin_day.month(),begin_day.day(),begin_day.hour()));
  LoadAdGroupData(begin_time, FreshAdMap, update_ads);
}
map<Ice::Long, double>  DemoLoaderCache::GetFreshAdSponsorData(string begin_time, set<Ice::Long> & fresh_gids) {
  map<Ice::Long, double> result;
  mysqlpp::StoreQueryResult res;
  if(fresh_gids.empty()) {
    MCE_INFO("DemoLoaderCache::LoadFreshAdSponsorData,not get freshgids,size:" << fresh_gids.size());
    return result;
  }
  string fresh_list = TransformForSql(fresh_gids);
  try {
    const char* db_host = AdDemoConfig::instance().db_host().c_str();
    const char* db_name = AdDemoConfig::instance().db_name().c_str();
    Connection conn(db_name,db_host,"ad","rrad");
    stringstream sql;
    sql.str("");
    sql << "select member_id , sum(pv_count) as sum_impr , sum(click_count) as sum_click  from  rt_HZPMGGS  where  time >= " << begin_time
        << " AND  member_id  in " << fresh_list
        << " and ((adzone_id = 100000000072 and abs_pos = 3) or ( adzone_id = 100000000070 and abs_pos = 2) or ( adzone_id = 100000000070 and abs_pos = 3 )) group by member_id";
    MCE_INFO("try to excute " << sql.str());
    string s= sql.str();
    Query q(&conn,true,s.c_str());
    //mysqlpp::StoreQueryResult res = q.store();
    res = q.store();
    if (!res || res.num_rows() <= 0) {
      MCE_INFO("DemoLoaderCache::LoadFreshAdSponsorData all gids is truely fresh ");
    }
  }catch (std::exception& e) {
    MCE_WARN("LoadByTime --> load ad_quality error: " << e.what());
  } catch (...) {
    MCE_WARN("LoadByTime --> load ad_quality error!");
  }

  MCE_INFO("[DemoLoaderCache]::LoadFreshAd. Fresh Size=: " << res.num_rows());  
  if (res && res.num_rows() > 0) {
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      Ice::Long sponsor_id = (Ice::Long) row["member_id"];
      Ice::Long impr = (Ice::Long) row["sum_impr"];
      Ice::Long click = (Ice::Long) row["sum_click"];
      if(impr > 0 && click > 0) {
        result[sponsor_id] = (double) click * 1.0 / (double) impr;
      }
    }
  }
}
void DemoLoaderCache::SetFreshAdSponsorData(map<Ice::Long, FreshAdGroupPtr> & FreshAdMap, map<Ice::Long, double>& fresh_sponsors_ctr) {
  for(map<Ice::Long, FreshAdGroupPtr>::iterator it = FreshAdMap.begin(); it != FreshAdMap.end(); ++it) {
    map<Ice::Long, double>::iterator itf = fresh_sponsors_ctr.find(it->second->sponsor_id());
    if(itf != fresh_sponsors_ctr.end()) {
      if(itf->second > 0) {
        it->second->set_sponsor_ctr(itf->second);
      }
    }
  }
}
void DemoLoaderCache::CalculateState(map<Ice::Long, FreshAdGroupPtr>& FreshAdMap) {
  for(map<Ice::Long, FreshAdGroupPtr>::iterator itf = FreshAdMap.begin(); itf != FreshAdMap.end(); ++itf) {
    itf->second->calculate_state();
  }
}

void DemoLoaderCache::PredictFreshAdCtr(map<Ice::Long, FreshAdGroupPtr>& FreshAdMap) {
  for(map<Ice::Long, FreshAdGroupPtr>::iterator itf = FreshAdMap.begin(); itf != FreshAdMap.end(); ++itf) {
    itf->second->predict_fresh_ctr();
    itf->second->PrintHelp();
  }
}
FreshAdQualityMap DemoLoaderCache::AssignFreshAdImpr(map<Ice::Long,FreshAdGroupPtr> & FreshAdMap) {
  map<string,Ice::Long> hour_impr;
  map<int,vector<pair<Ice::Long, double> > >state_gids_ctr;
  FreshAdQualityMap fresh_ad_pool;
  //  LoadHourImpr(hour_impr);
  TranstoAssignStruct(FreshAdMap, state_gids_ctr);
  fresh_ad_pool = AssignImprRatio(state_gids_ctr, hour_impr);
  UniFormImprRatio(fresh_ad_pool);
  return fresh_ad_pool;
}
void DemoLoaderCache::TranstoAssignStruct(map<Ice::Long,FreshAdGroupPtr> & FreshAdMap, map<int, vector<pair<Ice::Long,double> > >& state_gids_ctr) {

  for(map<Ice::Long, FreshAdGroupPtr>::iterator it = FreshAdMap.begin(); it != FreshAdMap.end(); ++it) {
    map<int,double> ctr_map = it->second->get_predict_ctr_map();
    for(map<int, double>::iterator itc = ctr_map.begin(); itc != ctr_map.end(); ++itc) {
      map<int, vector<pair<Ice::Long,double> > >::iterator itz = state_gids_ctr.find(itc->first);
      if(itz != state_gids_ctr.end() ) {
        itz->second.push_back(make_pair(it->first, itc->second));
      } else {
        vector<pair<Ice::Long, double> > vec;
        vec.push_back(make_pair(it->first,itc->second));
        state_gids_ctr[itc->first] = vec;
      }
    }
  }
}
void DemoLoaderCache::UniFormImprRatio(FreshAdQualityMap& fresh_ad_pool){ //归一化到分配比例
  for(FreshAdQualityMap::iterator it = fresh_ad_pool.begin(); it != fresh_ad_pool.end(); ++it) {
    int state = GetStateByZonePos(it->first);
    double test_ratio = AdDemoConfig::instance().GetStateTestImprRatio(state);
    MCE_INFO("DemoLoaderCache::UniFormImprRatio,state:" << state <<",test_ratio:" << test_ratio);
    map<Ice::Long, double> &ctr_map = it->second;
    double total = 0.0;
    for(map<Ice::Long,double>::iterator itc = ctr_map.begin(); itc != ctr_map.end(); ++itc) {
      total += itc->second;
    }
    double alpha = test_ratio / total;
    for(map<Ice::Long,double>::iterator itc = ctr_map.begin(); itc != ctr_map.end(); ++itc) {
      itc->second *= alpha;
    }
  }
}
bool SortRule(const pair<Ice::Long, double>& f1, const pair<Ice::Long, double>& f2) {
  return f1.second > f2.second;
}
/*
FreshAdQualityMap DemoLoaderCache::AssignImprRatio(map<int, vector<pair<Ice::Long,double> > >&state_gids_ctr, map<string, Ice::Long> &hour_impr) {
  FreshAdQualityMap fresh_ad_pool;

  Ice::Long test_base_impr = AdDemoConfig::instance().GetBaseTestImpr();
  Ice::Long test_real_impr = AdDemoConfig::instance().GetRealTestImpr();
  double test_impr_ratio = AdDemoConfig::instance().GetTestImprRatio(); 
  MCE_INFO("config test_base_impr = " << test_base_impr << " test_real_impr = " << test_real_impr << " test_impr_ratio" << test_impr_ratio);

  for(map<int, vector<pair<Ice::Long,double> > >::iterator its = state_gids_ctr.begin(); its != state_gids_ctr.end(); ++its) {
    //在每个状态上给测试广告组分配流量

    vector<string> zone_pos =GetZonePosByState(its->first); //获得当前状态的广告位置

    vector<pair<Ice::Long, double> > test_gids = its->second; //获得要测试的广告列表
    if(test_gids.size() == 0) { //该状态没有要测试的广告列表，则直接返回
      continue;
    }
    sort(test_gids.begin(),test_gids.end(), SortRule); //对要测试的广告排序，按照点击率
    Ice::Long sum_impr = 0;
    bool exit_flag = false; //如果要要测试的广告组没有了，直接退出
    int insert_index = 0; //已经分配到的最大广告位置
    for(vector<string>::iterator itz = zone_pos.begin(); itz != zone_pos.end(); ++itz) {//遍历广告位置，安排测试
      map<string,Ice::Long>::iterator ith = hour_impr.find(*itz);
      if( ith == hour_impr.end()) {
        continue;
      }
      Ice::Long impr_num = ith->second;//获得当前位置的流量
     // sum_impr += impr;
      double impr = impr_num * 0.05;

      int count = (int)((double) impr / (double) test_base_impr + 0.5); //计算当前位置可以放几个广告,四舍五入

      for(int i = 0; i < count; i++) {
        if(insert_index > (test_gids.size() - 1)){ //如果插入位置已经超过了测试列表的最大位置，直接退出,否则将当前要测试广告加入广告位置
          exit_flag = true;
          break;
        }
        Ice::Long gid = test_gids[insert_index].first;//获取测试广告及点击率，并插入测试位置中
        double ecpm = test_gids[insert_index].second;
        map<string,map<Ice::Long,double> >::iterator itf = fresh_ad_pool.find(*itz);
        if(itf != fresh_ad_pool.end()) {
          itf->second.insert(make_pair(gid,ecpm));
        }else {
          map<Ice::Long,double> c_map;
          c_map.insert(make_pair(gid,ecpm));
          fresh_ad_pool[*itz] = c_map;
        }
        insert_index ++;
      }
      if(exit_flag) { //如果已经超过测试广告数目，直接退出本状态的分配
        break;
      }
    }
  }
  return fresh_ad_pool;
}*/

/*
FreshAdQualityMap DemoLoaderCache::AssignImprRatio(map<int, vector<pair<Ice::Long,double> > >&state_gids_ctr, map<string, Ice::Long> &hour_impr) {
  FreshAdQualityMap fresh_ad_pool;

  Ice::Long test_base_impr = AdDemoConfig::instance().GetBaseTestImpr();
  Ice::Long test_real_impr = AdDemoConfig::instance().GetRealTestImpr();
  double test_impr_ratio = AdDemoConfig::instance().GetTestImprRatio();
  MCE_INFO("config test_base_impr = " << test_base_impr << " test_real_impr = " << test_real_impr << " test_impr_ratio" << test_impr_ratio);

  for(map<int, vector<pair<Ice::Long,double> > >::iterator its = state_gids_ctr.begin(); its != state_gids_ctr.end(); ++its) {
    //在每个状态上给测试广告组分配流量

    vector<string> zone_pos =GetZonePosByState(its->first); //获得当前状态的广告位置

    vector<pair<Ice::Long, double> > test_gids = its->second; //获得要测试的广告列表
    if(test_gids.size() == 0) { //该状态没有要测试的广告列表，则直接返回
      continue;
    }
    int alpha = 1;
    double excellent_ecpm_th = 0.2;
    sort(test_gids.begin(),test_gids.end(), SortRule); //对要测试的广告排序，按照点击率

    Ice::Long sum_impr = 0;
    bool exit_flag = false; //如果要要测试的广告组没有了，直接退出
    int insert_index = 0; //已经分配到的最大广告位置
    double test_ratio = AdDemoConfig::instance().GetStateTestImprRatio(its->first);
    MCE_DEBUG("DemoLoaderCache::AssignImprRatio,state:" << its->first <<",test_ratio:" << test_ratio);
    for(vector<string>::iterator itz = zone_pos.begin(); itz != zone_pos.end(); ++itz) {//遍历广告位置，安排测试
      map<string,Ice::Long>::iterator ith = hour_impr.find(*itz);
      if( ith == hour_impr.end()) {
        continue;
      }
      Ice::Long impr_num = ith->second;//获得当前位置的流量
     // sum_impr += impr;
      double impr = impr_num * test_ratio;

      int count = (int)((double) impr / (double) test_base_impr + 0.5) + 3; //计算当前位置可以放几个广告,四舍五入,多加入几个广告为了能够减少定向过滤的影响
      MCE_INFO("DemoLoaderCache::AssignImprRatio zonepos:"<< *itz <<",assign new_ad count:" << count);
      int i = 0;
      for(vector<pair<Ice::Long, double> >::iterator itt = test_gids.begin(); itt != test_gids.end(); ++itt) {
        if(i >= count) {
          break;
        }
        Ice::Long gid = (*itt).first;
        double ecpm = (*itt).second;
        map<string,map<Ice::Long,double> >::iterator itf = fresh_ad_pool.find(*itz);
        if(itf != fresh_ad_pool.end()) {
          itf->second.insert(make_pair(gid,ecpm));
        }else {
          map<Ice::Long,double> c_map;
          c_map.insert(make_pair(gid,ecpm));
          fresh_ad_pool[*itz] = c_map;
        }
        i++;
      }
    }
  }
  return fresh_ad_pool;
}*/

FreshAdQualityMap DemoLoaderCache::AssignImprRatio(map<int, vector<pair<Ice::Long,double> > >&state_gids_ctr, map<string, Ice::Long> &hour_impr) {
  FreshAdQualityMap fresh_ad_pool;

  for(map<int, vector<pair<Ice::Long,double> > >::iterator its = state_gids_ctr.begin(); its != state_gids_ctr.end(); ++its) {
    //在每个状态上给测试广告组分配流量

    vector<string> zone_pos =GetZonePosByState(its->first); //获得当前状态的广告位置

    vector<pair<Ice::Long, double> > test_gids = its->second; //获得要测试的广告列表
    if(test_gids.size() == 0) { //该状态没有要测试的广告列表，则直接返回
      continue;
    }
    for(vector<string>::iterator itz = zone_pos.begin(); itz != zone_pos.end(); ++itz) {//遍历广告位置，安排测试

      for(vector<pair<Ice::Long, double> >::iterator itt = test_gids.begin(); itt != test_gids.end(); ++itt) {
        Ice::Long gid = (*itt).first;
        double ecpm = (*itt).second;
        ecpm = ecpm * ecpm * ecpm;
        map<string,map<Ice::Long,double> >::iterator itf = fresh_ad_pool.find(*itz);
        if(itf != fresh_ad_pool.end()) {
          itf->second.insert(make_pair(gid,ecpm));
        }else {
          map<Ice::Long,double> c_map;
          c_map.insert(make_pair(gid,ecpm));
          fresh_ad_pool[*itz] = c_map;
        }
      }
    }
  }
  return fresh_ad_pool;
}
void DemoLoaderCache::LoadHourImpr(map<string,Ice::Long> & hour_impr) {
  MCE_INFO("DemoLoaderCache::LoadHourImpr begin:");
  Ice::Long t = time(NULL);
  MyUtil::Date yesterday_begin = MyUtil::Date::seconds(t - 1 * 24 * 3600);  //读1天前本小时开始时间
  MyUtil::Date yesterday_end = MyUtil::Date::seconds(t - 1 * 23 * 3600);  //读1天前本小时结束时间
  MCE_INFO("time is " << t);

  string yesterday_begin_time = yesterday_begin.str("%Y-%m-%d %H");
  string yesterday_end_time = yesterday_end.str("%Y-%m-%d %H");
  Ice::Long yesterday_begin_time_pos = GetTime(yesterday_begin.year(),yesterday_begin.month(),yesterday_begin.day(),yesterday_begin.hour());
  Ice::Long yesterday_end_time_pos = GetTime(yesterday_end.year(),yesterday_end.month(),yesterday_end.day(),yesterday_end.hour());

  try {
    const char* db_host = AdDemoConfig::instance().db_host().c_str();
    const char* db_name = AdDemoConfig::instance().db_name().c_str();
    Connection conn(db_name,db_host,"ad","rrad");
    stringstream impr_sql;
    impr_sql << "select adzone_id,abs_pos,sum(pv_count) as impr_sum from rt_HZPMGGS where time >= " << yesterday_begin_time << " and time <= " << yesterday_end_time  << " group by adzone_id,abs_pos";
    MCE_INFO("try to excute " << impr_sql.str());
    string impr_s= impr_sql.str();
    Query impr_q(&conn,true,impr_s.c_str());
    mysqlpp::StoreQueryResult impr_res = impr_q.store();
    if (!impr_res || impr_res.num_rows() <= 0) {
      MCE_WARN("DemoLoaderCache Load Error " << impr_sql.str() << ".  select  fail!");
    }
    MCE_INFO("[DemoLoaderCache]::Load   res size : " << impr_res.num_rows());
    for (size_t i = 0; i < impr_res.num_rows(); ++i) {
      mysqlpp::Row row = impr_res.at(i);
      Ice::Long zone_id = (Ice::Long) row["adzone_id"];
      Ice::Long local_impr = (Ice::Long) row["impr_sum"];
      Ice::Long pos_id = (int) row["abs_pos"];
      string key = GetFreshKey(zone_id, pos_id);
      hour_impr[key] = local_impr;
      MCE_INFO("[DemoLoaderCache]::hour impr is " << local_impr << " at "  << yesterday_begin_time << " zone = " << zone_id << " pos = " << pos_id);
    }

  }catch (std::exception& e) {
    MCE_WARN("LoadByTime --> load ad_quality error: " << e.what());
  } catch (...) {
    MCE_WARN("LoadByTime --> load ad_quality error!");
  }
  MCE_INFO("DemoLoaderCache::LoadHourImpr end,hour_impr size:" << hour_impr.size());
}
/////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

void DemoLoaderCache::InsertAmplifyGrouptoPool(AdQualityMap& res_pool){
  MCE_INFO("DemoLoaderCache::InsertAmplifyGrouptoPool,begin");
  const char* db_host = AdDemoConfig::instance().db_host().c_str();
  const char* db_name = AdDemoConfig::instance().db_name().c_str();
  Connection conn(db_name,db_host,"ad","rrad");
  MCE_INFO("DemoLoaderCache::LoadPool,db host:" << db_host << ",db_name:" << db_name);
  stringstream sql;
  /***************Load amplify Ads****************************/
  set<Ice::Long> amplify_gids;
  map<Ice::Long,Ice::Long> amplify_gids_price;
  map<Ice::Long,pair<Ice::Long,double> > ecpm_table_tmp;
  {
    Statement fresh_qr_sql;
    stringstream fresh_qr_sql_string;
    fresh_qr_sql_string << "select adgroup_id as group_id, max_price as price from adgroup where "
        " member_status = 1 AND am_status = 1 AND audit_status = 1 AND delete_flag= 1 AND display_type = 4 "
        " AND daily_sys_status = 1 AND total_sys_status = 1";
    fresh_qr_sql << fresh_qr_sql_string.str();

    MCE_INFO("DemoLoaderCache::InsertAmplifyGrouptoPool execute:" << fresh_qr_sql_string.str());
    mysqlpp::StoreQueryResult qr_res = QueryRunner("ade_db", CDbRServer, "adgroup").store(fresh_qr_sql);
    MCE_INFO("DemoLoaderCache::InsertAmplifyGrouptoPool::Load amplify Ads. Size=: " << qr_res.num_rows());
    //stringstream fresh_ad_list;
    for (size_t i = 0; i < qr_res.num_rows(); ++i) {
      mysqlpp::Row row = qr_res.at(i);
      Ice::Long group_id = (Ice::Long) row["group_id"];
      Ice::Long price = (Ice::Long) row["price"];
      amplify_gids.insert(group_id);
      amplify_gids_price[group_id] = price;
    }

    Ice::Long t = time(NULL);
    ostringstream sin;
    MyUtil::Date ecpm_begin = MyUtil::Date::seconds(t - 2 * 3600);  //读2小时前开始时间
    MyUtil::Date ecpm_end = MyUtil::Date::seconds(t);
    string ecpm_begin_time = ecpm_begin.str("%Y-%m-%d %H:%M:%S");
    string ecpm_end_time = ecpm_end.str("%Y-%m-%d %H:%M:%S");
    //Connection conn("test","10.3.16.119","ad","rrad");
    stringstream ctr_sql;
    ctr_sql.str("");
    //MyUtil::Date now = MyUtil::Date::seconds(time(NULL));
    ctr_sql << "select zone_id,pos_id,sum(impr) as sum_impr,sum(cost) as sum_cost from average_ecpm where time >= '" << ecpm_begin_time << ":00:00' and time <='" << ecpm_end_time  << ":00:00' group by zone_id,pos_id";
    MCE_INFO("DemoLoaderCache::InsertAmplifyGrouptoPool try to excute " << ctr_sql.str());
    string ctr_s= ctr_sql.str();
    Query ctr_q(&conn,true,ctr_s.c_str());
    mysqlpp::StoreQueryResult ctr_res = ctr_q.store();
    if (!ctr_res || ctr_res.num_rows() <= 0) {
      MCE_WARN("DemoLoaderCache::InsertAmplifyGrouptoPool Error " << ctr_sql.str() << ".  select  fail!");
    }
    MCE_INFO("DemoLoaderCache::InsertAmplifyGrouptoPool   res size : " << ctr_res.num_rows());
    for (size_t i = 0; i < ctr_res.num_rows(); ++i) {

      mysqlpp::Row row = ctr_res.at(i);
      Ice::Long zone_id = (Ice::Long) row["zone_id"];
      int pos_id = (int) row["pos_id"];
      Ice::Long local_impr = (Ice::Long) row["sum_impr"];
      double local_cost = (Ice::Long) row["sum_cost"];
      Ice::Long tmp_key = zone_id * 10 + pos_id;
      if (ecpm_table_tmp.find(tmp_key) != ecpm_table_tmp.end()){
        pair<Ice::Long,double> tmp_value = ecpm_table_tmp[tmp_key];
        tmp_value.first += local_impr;
        tmp_value.second += local_cost;
        ecpm_table_tmp[tmp_key] = tmp_value;
      }else{
        pair<Ice::Long,double> tmp_value;
        tmp_value.first = local_impr;
        tmp_value.second = local_cost;
        ecpm_table_tmp[tmp_key] = tmp_value;
      }
    }
    {
      IceUtil::RWRecMutex::WLock lock(mutex_);
      if(ecpm_table_tmp.size() > 0) {
        ecpm_table.swap(ecpm_table_tmp);
      }
    }
    MCE_INFO("Load new ecpm_table:size:" << ecpm_table.size() <<" begin time:" <<ecpm_begin_time <<" end time:" <<ecpm_end_time);
  }

  {
    Ice::Long t = time(NULL);
    MyUtil::Date fresh_begin = MyUtil::Date::seconds(t - 4 * 3600);  //读4小时开始时间
    MyUtil::Date fresh_end = MyUtil::Date::seconds(t);
    MCE_INFO("time is " << t);
    ostringstream sin;
    //string fresh_begin_time = fresh_begin.str("%Y-%m-%d %H:%M:%S");
    //string fresh_end_time = fresh_end.str("%Y-%m-%d %H:%M:%S");
    Ice::Long fresh_begin_time = GetTime(fresh_begin.year(),fresh_begin.month(),fresh_begin.day(),fresh_begin.hour());
    Ice::Long fresh_end_time = GetTime(fresh_end.year(),fresh_end.month(),fresh_end.day(),fresh_end.hour());


    sql.str("");
    sql << "select adzone_id,member_id, adgroup_id as group_id, abs_pos, sum(pv_count) as sum_impr, sum(click_count) as sum_click from "
        " rt_HZPMGGS  where time >= " << fresh_begin_time  << " and time <= "
        << fresh_end_time  << " group by adzone_id, adgroup_id ,abs_pos ";
    MCE_INFO("DemoLoaderCache::InsertAmplifyGrouptoPool try to excute " << sql.str());
    string s= sql.str();
    Query q(&conn,true,s.c_str());
    mysqlpp::StoreQueryResult res = q.store();
    if (!res || res.num_rows() <= 0) {
      MCE_WARN("DemoLoaderCache::InsertAmplifyGrouptoPool Load Error " << sql.str() << ".  select  fail!");
    }
    MCE_INFO("DemoLoaderCache::InsertAmplifyGrouptoPool LoadAmplifyGroup res size : " << res.num_rows());
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      Ice::Long zone_id = (Ice::Long) row["adzone_id"];
      Ice::Long group_id = (Ice::Long) row["group_id"];
      int pos_id = (int) row["abs_pos"];
      Ice::Long sponsor_id = (Ice::Long) row["member_id"];
      Ice::Int gender = NO_USE;
      Ice::Int stage = NO_USE;
      Ice::Long impr = (Ice::Long) row["sum_impr"];
      Ice::Long click = (Ice::Long) row["sum_click"];
      string key = GetKey(zone_id,sponsor_id, group_id,pos_id,gender,stage);
      if (amplify_gids.find(group_id) != amplify_gids.end()){
        double ctr = 0.0;
        if (impr > 0){
          ctr = click * 1.0 / impr;
        }
        double ecpm = amplify_gids_price[group_id] * ctr;
        map<Ice::Long, pair<Ice::Long,double> >::iterator it =  ecpm_table.find(zone_id * 10 + pos_id);
        double local_ecpm_thr = 0.0;
        if (it!= ecpm_table.end() && (*it).second.first > 0){
          local_ecpm_thr = (*it).second.second / (*it).second.first;
        }
        local_ecpm_thr = local_ecpm_thr * AdDemoConfig::instance().GetFreshEcpmRatio();
        if(res_pool.find(key) == res_pool.end() && IsFreshCredible(impr, click, zone_id, pos_id ,ecpm,local_ecpm_thr)) {
          res_pool[key] = (double) click / (double) impr;
          MCE_INFO("DemoLoaderCache::InsertAmplifyGrouptoPool Load fresh ad with low thr,gid=" << group_id << " impr=" << impr << " click=" << click << " ecpm=" << ecpm << " ecpm thr=" << local_ecpm_thr << " ecpm_ratio="<< AdDemoConfig::instance().GetFreshEcpmRatio() << " at zone=" << zone_id << " pos=" << pos_id);
        }else{
          if (res_pool.find(key) != res_pool.end() && IsFreshCredible(impr, click, zone_id, pos_id ,ecpm,local_ecpm_thr)){
            MCE_INFO("DemoLoaderCache::InsertAmplifyGrouptoPool The ad has got enough impr. Load no fresh ad with low thr,gid=" << group_id << " impr=" << impr << " click=" << click << " ecpm=" << ecpm << " ecpm thr=" << local_ecpm_thr << " ecpm_ratio="<< AdDemoConfig::instance().GetFreshEcpmRatio() << " at zone=" << zone_id << " pos=" << pos_id);
          }else{
            MCE_INFO("DemoLoaderCache::InsertAmplifyGrouptoPool Load no fresh ad with low thr,gid=" << group_id << " impr=" << impr << " click=" << click << " ecpm=" << ecpm << " ecpm thr=" << local_ecpm_thr << " ecpm_ratio="<< AdDemoConfig::instance().GetFreshEcpmRatio() << " at zone=" << zone_id << " pos=" << pos_id);
          }
        }
      }
    }
  }
}

void DemoLoaderCache::InsertUpdateGrouptoPool(AdQualityMap& res_pool){
  MCE_INFO("DemoLoaderCache::InsertUpdateGrouptoPool,begin");
  const char* db_host = AdDemoConfig::instance().db_host().c_str();
  const char* db_name = AdDemoConfig::instance().db_name().c_str();
  Connection conn(db_name,db_host,"ad","rrad");
  MCE_INFO("DemoLoaderCache::LoadPool,db host:" << db_host << ",db_name:" << db_name);
  stringstream sql;
  int update_count = 0;

  /*******************************load updated ad***************************************************************/
  /***************Load updated Ads****************************/
  { ////加载最新更新的广告
    set<Ice::Long> update_ads;
    map<Ice::Long,Ice::Long> update_ad_price;
    DemoLoaderCache::instance().LoadUpdatedAd(update_ads);
    MCE_INFO(" DemoLoaderCache::InsertUpdateGrouptoPool-->load updated gids:" << update_ads.size());
    if(!update_ads.empty()){
      string update_ads_list = TransformForSql(update_ads);
      //加载相应广告价格
      //////////////////////////////////////////////////////////////////////////////////////////
      {
        Statement update_gid_price_sql;
        stringstream fresh_qr_sql_string;
        fresh_qr_sql_string << "select adgroup_id as group_id, max_price as price from adgroup where adgroup_id in " << update_ads_list;
        update_gid_price_sql << fresh_qr_sql_string.str();
        MCE_INFO("try to Load updated Ads gid:" << fresh_qr_sql_string.str());
        mysqlpp::StoreQueryResult qr_res = QueryRunner("ade_db", CDbRServer, "adgroup").store(update_gid_price_sql);
        MCE_INFO(" DemoLoaderCache::InsertUpdateGrouptoPool:Load updated Ads Size=: " << qr_res.num_rows());
        //stringstream fresh_ad_list;
        for (size_t i = 0; i < qr_res.num_rows(); ++i) {
          mysqlpp::Row row = qr_res.at(i);
          Ice::Long group_id = (Ice::Long) row["group_id"];
          Ice::Long price = (Ice::Long) row["price"];
          update_ad_price[group_id] = price;
        }
      }
      /*****************************insert updated Ads into pool***************************/
      {
        Ice::Long t = time(NULL);
        MyUtil::Date fresh_begin = MyUtil::Date::seconds(t - 10 * 3600);  //读10小时开始时间
        MyUtil::Date fresh_end = MyUtil::Date::seconds(t);
        MCE_INFO("time is " << t);
        ostringstream sin;
        //string fresh_begin_time = fresh_begin.str("%Y-%m-%d %H:%M:%S");
        Ice::Long fresh_begin_time = GetTime(fresh_begin.year(),fresh_begin.month(),fresh_begin.day(),fresh_begin.hour());
        string fresh_end_time = fresh_end.str("%Y-%m-%d %H:%M:%S");

        sql.str("");
        sql << "select adzone_id,member_id, adgroup_id as group_id, abs_pos, sum(pv_count) as sum_impr, sum(click_count) as sum_click from "
    << " rt_HZPMGGS  where time >= " << fresh_begin_time <<"  and adgroup_id in " << update_ads_list
            << " group by adzone_id, adgroup_id ,abs_pos ";
        MCE_INFO("try to excute " << sql.str());
        string s= sql.str();
        Query q(&conn,true,s.c_str());
        mysqlpp::StoreQueryResult res = q.store();
        if (!res || res.num_rows() <= 0) {
          MCE_WARN(" DemoLoaderCache::InsertUpdateGrouptoPool Load Error " << sql.str() << ".  select  fail!");
        }
        MCE_INFO(" DemoLoaderCache::InsertUpdateGrouptoPool Load   res size : " << res.num_rows());

        for (size_t i = 0; i < res.num_rows(); ++i) {
          mysqlpp::Row row = res.at(i);
          Ice::Long zone_id = (Ice::Long) row["adzone_id"];
          Ice::Long group_id = (Ice::Long) row["group_id"];
          int pos_id = (int) row["abs_pos"];
          Ice::Long sponsor_id = (Ice::Long) row["member_id"];
          Ice::Int gender = NO_USE;
          Ice::Int stage = NO_USE;
          Ice::Long impr = (Ice::Long) row["sum_impr"];
          Ice::Long click = (Ice::Long) row["sum_click"];
          string key = GetKey(zone_id,sponsor_id, group_id,pos_id,gender,stage);
          if (update_ads.find(group_id) != update_ads.end()){
            double ctr = 0.0;
            if (impr > 0){
              ctr = click * 1.0 / impr;
            }
            double ecpm = update_ad_price[group_id] * ctr;
            map<Ice::Long, pair<Ice::Long,double> >::iterator it =  ecpm_table.find(zone_id * 10 + pos_id);
            double local_ecpm_thr = 0.0;
            if (it!= ecpm_table.end() && (*it).second.first > 0){
              local_ecpm_thr = (*it).second.second / (*it).second.first;
            }
            local_ecpm_thr = local_ecpm_thr * AdDemoConfig::instance().GetFreshEcpmRatio();
            if(IsFreshCredible(impr, click, zone_id, pos_id ,ecpm,local_ecpm_thr)) {
              res_pool[key] = (double) click / (double) impr;
              MCE_INFO(" DemoLoaderCache::InsertUpdateGrouptoPool::Load update ad with low thr,gid=" << group_id << " impr=" <<
              impr << " click=" << click << " ecpm=" << ecpm << " ecpm thr=" << local_ecpm_thr <<
              " ecpm_ratio="<< AdDemoConfig::instance().GetFreshEcpmRatio() << " at zone=" <<
              zone_id << " pos=" << pos_id);
              update_count++;

            }else{
             MCE_INFO(" DemoLoaderCache::InsertUpdateGrouptoPool::Load no update ad with low thr,gid=" << group_id << " impr=" <<
             impr << " click=" << click << " ecpm=" << ecpm << " ecpm thr=" << local_ecpm_thr << " ecpm_ratio="<<
             AdDemoConfig::instance().GetFreshEcpmRatio() << " at zone=" << zone_id << " pos=" << pos_id);
            }
          }
        }
      }
    }
  }
  MCE_INFO("[DemoLoaderCache]:: demo_pool size is " << res_pool.size() <<",insert updated num:" << update_count);
}
void DemoLoaderCache::ExpandCtrByPos(map<string,map<Ice::Long,map<int,pair<Ice::Long, Ice::Long> > > >& temp_pool,AdQualityMap& res_pool,int type){
  MCE_INFO("DemoLoaderCache::ExpandCtrByPos,begin:temp size:" << temp_pool.size() <<",res_pool size:" << res_pool.size());

  map<string,double> click_discount_ratio_table;
  click_discount_ratio_table[pos_721] = 1;
  click_discount_ratio_table[pos_722] = 1.647276818;
  click_discount_ratio_table[pos_723] = 2.881352008;
  click_discount_ratio_table[pos_701] = 1;
  click_discount_ratio_table[pos_702] = 2.047550947;
  click_discount_ratio_table[pos_703] = 2.613874943;
  click_discount_ratio_table[pos_631] = 1;
  click_discount_ratio_table[pos_632] = 1.358939264;
  click_discount_ratio_table[pos_633] = 1.711976435;
  for(map<string,map<Ice::Long,map<int,pair<Ice::Long, Ice::Long> > > >::iterator it = temp_pool.begin();
      it!= temp_pool.end();++it) {
    vector < string > tmp;
    boost::algorithm::split_regex(tmp, it->first, regex("_"));

    Ice::Long spid = boost::lexical_cast<Ice::Long>(tmp[0]);
    Ice::Long gid = boost::lexical_cast<Ice::Long>(tmp[1]);
    int gender = boost::lexical_cast<int>(tmp[2]);
    int stage = boost::lexical_cast<int>(tmp[3]);


    map<Ice::Long,map<int,pair<Ice::Long, Ice::Long> > >& zone_map = it->second;
    for(map<Ice::Long,map<int,pair<Ice::Long, Ice::Long> > >::iterator itz = zone_map.begin(); itz != zone_map.end(); ++itz) {
      map<int,pair<Ice::Long, Ice::Long> >& pos_map = itz->second;
      Ice::Long sum_impr = 0;
      double sum_click = 0;
      for(map<int,pair<Ice::Long, Ice::Long> >::iterator itp = pos_map.begin(); itp != pos_map.end(); ++itp) {
        string key = GetFreshKey(itz->first,itp->first);
        map<string,double>::iterator itc = click_discount_ratio_table.find(key);
        double ratio = 1.0;
        if(itc != click_discount_ratio_table.end()) {
          ratio = itc->second;
        }else{
          continue;
        }
        sum_impr += itp->second.first;
        sum_click += itp->second.second * ratio ;
      }

      for(map<int,pair<Ice::Long, Ice::Long> >::iterator itp = pos_map.begin(); itp != pos_map.end(); ++itp) {
        Ice::Long impr = itp->second.first;
        Ice::Long click = itp->second.second;

        string c_key = GetFreshKey(itz->first,itp->first);
        map<string,double>::iterator itc = click_discount_ratio_table.find(c_key);
        double ratio = 1.0;
        if(itc != click_discount_ratio_table.end()) {
          ratio = itc->second;
        }

        string key = GetKey(itz->first,spid,gid,itp->first,gender,stage);

        if(IsCredible(impr, click, type)) {
          res_pool[key] = (double) click / (double) impr;
        } else if(IsCredible(sum_impr, sum_click, type)){
          res_pool[key] = (double) sum_click / (double) (sum_impr * ratio);
        }
      }
    }
  }
  MCE_INFO("DemoLoaderCache::ExpandCtrByPos,stop:temp size:" << temp_pool.size() <<",res_pool size:" << res_pool.size());
}
void DemoLoaderCache::ExpandCtrByZone(map<string,map<Ice::Long,map<int,pair<Ice::Long, Ice::Long> > > >& temp_pool,AdQualityMap& res_pool,int type){
  MCE_INFO("DemoLoaderCache::ExpandCtrByZone,begin:temp size:" << temp_pool.size() <<",res_pool size:" << res_pool.size());

  map<string,double> click_discount_ratio_table;
  click_discount_ratio_table[pos_721] = 2.075563365;
  click_discount_ratio_table[pos_722] = 3.419027415;
  click_discount_ratio_table[pos_723] = 5.980428574;
  click_discount_ratio_table[pos_701] = 2.702867005;
  click_discount_ratio_table[pos_702] = 5.534257896;
  click_discount_ratio_table[pos_703] = 7.06495615;
  click_discount_ratio_table[pos_631] = 1;
  click_discount_ratio_table[pos_632] = 1.358939264;
  click_discount_ratio_table[pos_633] = 1.711976347;
  for(map<string,map<Ice::Long,map<int,pair<Ice::Long, Ice::Long> > > >::iterator it = temp_pool.begin();
      it!= temp_pool.end();++it) {
    vector < string > tmp;
    boost::algorithm::split_regex(tmp, it->first, regex("_"));

    Ice::Long spid = boost::lexical_cast<Ice::Long>(tmp[0]);
    Ice::Long gid = boost::lexical_cast<Ice::Long>(tmp[1]);
    int gender = boost::lexical_cast<int>(tmp[2]);
    int stage = boost::lexical_cast<int>(tmp[3]);
    Ice::Long sum_impr = 0;
    double sum_click = 0;

    map<Ice::Long,map<int,pair<Ice::Long, Ice::Long> > >& zone_map = it->second;
    for(map<Ice::Long,map<int,pair<Ice::Long, Ice::Long> > >::iterator itz = zone_map.begin(); itz != zone_map.end(); ++itz) {
      map<int,pair<Ice::Long, Ice::Long> >& pos_map = itz->second;

      for(map<int,pair<Ice::Long, Ice::Long> >::iterator itp = pos_map.begin(); itp != pos_map.end(); ++itp) {
        string key = GetFreshKey(itz->first,itp->first);
        map<string,double>::iterator itc = click_discount_ratio_table.find(key);
        double ratio = 1.0;
        if(itc != click_discount_ratio_table.end()) {
          ratio = itc->second;
        }else{
          continue;
        }
        sum_impr += itp->second.first;
        sum_click += itp->second.second * ratio ;
      }

    }
    for(map<Ice::Long,map<int,pair<Ice::Long, Ice::Long> > >::iterator itz = zone_map.begin(); itz != zone_map.end(); ++itz) {
      map<int,pair<Ice::Long, Ice::Long> >& pos_map = itz->second;

      for(map<int,pair<Ice::Long, Ice::Long> >::iterator itp = pos_map.begin(); itp != pos_map.end(); ++itp) {
        Ice::Long impr = itp->second.first;
        Ice::Long click = itp->second.second;

        string c_key = GetFreshKey(itz->first,itp->first);
        map<string,double>::iterator itc = click_discount_ratio_table.find(c_key);
        double ratio = 1.0;
        if(itc != click_discount_ratio_table.end()) {
          ratio = itc->second;
        }

        string key = GetKey(itz->first,spid,gid,itp->first,gender,stage);
        if(IsCredible(impr, click, type)) {
          res_pool[key] = (double) click / (double) impr;
        } else if(IsCredible(sum_impr, sum_click, type)){
          res_pool[key] = (double) sum_click / (double) (sum_impr * ratio);
        }
      }
    }

  }
  MCE_INFO("DemoLoaderCache::ExpandCtrByZone,stop:temp size:" << temp_pool.size() <<",res_pool size:" << res_pool.size());
}
void DemoLoaderCache::ExpandCtrByPosAndZone(map<string,map<Ice::Long,map<int,pair<Ice::Long, Ice::Long> > > >& temp_pool,AdQualityMap& res_pool,int type){
  MCE_INFO("DemoLoaderCache::ExpandCtrByPosAndZone,begin:temp size:" << temp_pool.size() <<",res_pool size:" << res_pool.size());

  map<string,double> click_discount_ratio_table_by_pos;
  click_discount_ratio_table_by_pos[pos_721] = 1;
  click_discount_ratio_table_by_pos[pos_722] = 1.647276818;
  click_discount_ratio_table_by_pos[pos_723] = 2.881352008;
  click_discount_ratio_table_by_pos[pos_701] = 1;
  click_discount_ratio_table_by_pos[pos_702] = 2.047550947;
  click_discount_ratio_table_by_pos[pos_703] = 2.613874943;
  click_discount_ratio_table_by_pos[pos_631] = 1;
  click_discount_ratio_table_by_pos[pos_632] = 1.358939264;
  click_discount_ratio_table_by_pos[pos_633] = 1.711976435;
 
  map<string,double> click_discount_ratio_table_by_zone;
  click_discount_ratio_table_by_zone[pos_721] = 2.075563365;
  click_discount_ratio_table_by_zone[pos_722] = 3.419027415;
  click_discount_ratio_table_by_zone[pos_723] = 5.980428574;
  click_discount_ratio_table_by_zone[pos_701] = 2.702867005;
  click_discount_ratio_table_by_zone[pos_702] = 5.534257896;
  click_discount_ratio_table_by_zone[pos_703] = 7.06495615;
  click_discount_ratio_table_by_zone[pos_631] = 1;
  click_discount_ratio_table_by_zone[pos_632] = 1.358939264;
  click_discount_ratio_table_by_zone[pos_633] = 1.711976347;

  for(map<string,map<Ice::Long,map<int,pair<Ice::Long, Ice::Long> > > >::iterator it = temp_pool.begin();
      it!= temp_pool.end();++it) {
    vector < string > tmp;
    boost::algorithm::split_regex(tmp, it->first, regex("_"));

    Ice::Long spid = boost::lexical_cast<Ice::Long>(tmp[0]);
    Ice::Long gid = boost::lexical_cast<Ice::Long>(tmp[1]);
    int gender = boost::lexical_cast<int>(tmp[2]);
    int stage = boost::lexical_cast<int>(tmp[3]);

    map<Ice::Long,pair<Ice::Long, double> > gid_map;
    Ice::Long sum_impr = 0;
    double sum_click = 0;

    map<Ice::Long,map<int,pair<Ice::Long, Ice::Long> > >& zone_map = it->second;
    for(map<Ice::Long,map<int,pair<Ice::Long, Ice::Long> > >::iterator itz = zone_map.begin(); itz != zone_map.end(); ++itz) {
      map<int,pair<Ice::Long, Ice::Long> >& pos_map = itz->second;
      Ice::Long sum_impr_by_pos = 0;
      double sum_click_by_pos = 0;
      for(map<int,pair<Ice::Long, Ice::Long> >::iterator itp = pos_map.begin(); itp != pos_map.end(); ++itp) {
        string key = GetFreshKey(itz->first,itp->first);
        map<string,double>::iterator itc = click_discount_ratio_table_by_pos.find(key);
        double ratio = 1.0;
        if(itc != click_discount_ratio_table_by_pos.end()) {
          ratio = itc->second;
        }else{
          continue;
        }
        sum_impr_by_pos += itp->second.first;
        sum_impr += itp->second.first;
        sum_click_by_pos += itp->second.second * ratio ;
        itc = click_discount_ratio_table_by_zone.find(key);
        if(itc != click_discount_ratio_table_by_zone.end()) {
          ratio = itc->second;
        }else{
          continue;
        }
        sum_click += itp->second.second * ratio ;
      }
      gid_map[itz->first] = make_pair(sum_impr_by_pos,sum_click_by_pos);
    }
    
    for(map<Ice::Long,map<int,pair<Ice::Long, Ice::Long> > >::iterator itz = zone_map.begin(); itz != zone_map.end(); ++itz) {
      map<int,pair<Ice::Long, Ice::Long> >& pos_map = itz->second;

      for(map<int,pair<Ice::Long, Ice::Long> >::iterator itp = pos_map.begin(); itp != pos_map.end(); ++itp) {
        Ice::Long impr = itp->second.first;
        Ice::Long click = itp->second.second;

        string key = GetKey(itz->first,spid,gid,itp->first,gender,stage);
        if(IsCredible(impr, click, type)) {
          res_pool[key] = (double) click / (double) impr;
        } else{
          Ice::Long sum_impr_by_pos = 0;
          double sum_click_by_pos = 0;
          map<Ice::Long,pair<Ice::Long, double> >::iterator itg = gid_map.find(itz->first);
          if(itg != gid_map.end()){
            sum_impr_by_pos = itg->second.first;
            sum_click_by_pos = itg->second.second;
          }
          
          string c_key = GetFreshKey(itz->first,itp->first);
          if(IsCredible(sum_impr_by_pos, sum_click_by_pos, type)){
            map<string,double>::iterator itc = click_discount_ratio_table_by_pos.find(c_key);
            double ratio = 1.0;
            if(itc != click_discount_ratio_table_by_pos.end()) {
              ratio = itc->second;
            }
            res_pool[key] = (double) sum_click_by_pos / (double) (sum_impr_by_pos * ratio);
          }else if(IsCredible(sum_impr, sum_click, type)){
            map<string,double>::iterator itc = click_discount_ratio_table_by_zone.find(c_key);
            double ratio = 1.0;
            if(itc != click_discount_ratio_table_by_zone.end()) {
              ratio = itc->second;
            }
            res_pool[key] = (double) sum_click / (double) (sum_impr * ratio);
            
          }
        }
      }
    }
  }
  MCE_INFO("DemoLoaderCache::ExpandCtrByPosAndZone,stop:temp size:" << temp_pool.size() <<",res_pool size:" << res_pool.size());
}
void DemoLoaderCache::LoadPool() {
  AdQualityMap res_pool;
  AdQualityMap local_app_pool;

  MCE_INFO("[DemoLoaderCache]::try to load.LoadPool begin");
  MyUtil::Date begin = MyUtil::Date::seconds(time(NULL));
  string begin_time = begin.str("%Y-%m-%d %H:%M:%S");
  Ice::Long begin_time_pos = GetTime(begin.year(),begin.month(),begin.day(),begin.hour());
  MyUtil::Date end = MyUtil::Date::seconds(time(NULL) - DAYCOUNT * HOURCOUNT * 3600);
  Ice::Long end_time_pos = GetTime(end.year(),end.month(),end.day(),end.hour());
  MyUtil::Date state_2_end = MyUtil::Date::seconds(time(NULL) - 2 * HOURCOUNT * 3600);
  string end_time = end.str("%Y-%m-%d %H:%M:%S");
  string state_2_end_time = state_2_end.str("%Y-%m-%d %H:%M:%S");
  Ice::Long state_2_end_time_pos = GetTime(state_2_end.year(),state_2_end.month(),state_2_end.day(),state_2_end.hour());


  try {
    const char* db_host = AdDemoConfig::instance().db_host().c_str();
    const char* db_name = AdDemoConfig::instance().db_name().c_str();
    //MCE_INFO("DemoLoaderCache::db_host config: " << db_host << ", again : " << AdDemoConfig::instance().db_host());
    Connection conn(db_name,db_host,"ad","rrad");
    MCE_INFO("DemoLoaderCache::LoadPool,db host:" << AdDemoConfig::instance().db_host()<< ",db_name:" << db_name);
    stringstream sql;
    {/****************no_gender*****************************/
      sql.str("");
      sql
        << "select adzone_id, member_id, adgroup_id as group_id, abs_pos, stage, sum(pv_count) as sum_impr, sum(click_count) as sum_click from "
        " rt_HZPMGGS where time <= " << begin_time_pos << " and time >= "
        << end_time_pos << " "
        << " group by adzone_id,adgroup_id,abs_pos,stage ";
      //sql << "select adzone_id,member_id, adgroup_id as group_id , abs_pos, stage, sum(pv_count) as sum_impr, sum(click_count) as sum_click from rt_copy  where time <= " << begin_time_pos  << " and time >= " << end_time_pos << " " <<" group by adzone_id,adgroup_id,abs_pos,stage ";
      //sql << "select adzone_id,member_id, adgroup_id as group_id , abs_pos, stage, sum(pv_count) as sum_impr, sum(click_count) as sum_click from rt_HZPMGGS_copy  where time <= " << begin_time_pos  << " and time >= " << end_time_pos << " " <<" group by adzone_id,adgroup_id,abs_pos,stage ";
      MCE_INFO("try to excute " << sql.str());
      string s= sql.str();
      Query q(&conn,true,s.c_str());
      mysqlpp::StoreQueryResult res = q.store();
      if (!res || res.num_rows() <= 0) {
        MCE_WARN("DemoLoaderCache Load Error " << sql.str() << ".  select  fail!");
      }
      MCE_INFO("[DemoLoaderCache]::Load   res size : " << res.num_rows());
      for (size_t i = 0; i < res.num_rows(); ++i) {
        mysqlpp::Row row = res.at(i);
        Ice::Long zone_id = (Ice::Long) row["adzone_id"];
        Ice::Long group_id = (Ice::Long) row["group_id"];
        int pos_id = (int) row["abs_pos"];
        Ice::Long sponsor_id = (Ice::Long) row["member_id"];
        Ice::Int gender = NO_USE;
        Ice::Int stage = (Ice::Int) row["stage"];
        Ice::Long impr = (Ice::Long) row["sum_impr"];
        Ice::Long click = (Ice::Long) row["sum_click"];
        string key = GetKey(zone_id,sponsor_id, group_id,pos_id,gender,stage);
        if(IsCredible(impr, click, no_gender)) {
          res_pool[key] = (double) click / (double) impr;
        }
      }
    }
    /*******************no_stage*************************************/  
    {
      sql.str("");
      sql << "select adzone_id,member_id, adgroup_id as group_id, abs_pos, gender, sum(pv_count) as sum_impr, sum(click_count) as sum_click from rt_HZPMGGS  where time <= " << begin_time_pos  << " and time >=" << end_time_pos << " " <<" group by adzone_id, adgroup_id, abs_pos, gender ";
      MCE_INFO("try to excute " << sql.str());
      string s= sql.str();
      Query q(&conn,true,s.c_str());
      mysqlpp::StoreQueryResult res = q.store();
      if (!res || res.num_rows() <= 0) {
        MCE_WARN("DemoLoaderCache Load Error " << sql.str() << ".  select  fail!");
      }
      MCE_INFO("[DemoLoaderCache]::Load   res size : " << res.num_rows());
      for (size_t i = 0; i < res.num_rows(); ++i) {
        mysqlpp::Row row = res.at(i);
        Ice::Long zone_id = (Ice::Long) row["adzone_id"];
        Ice::Long group_id = (Ice::Long) row["group_id"];
        int pos_id = (int) row["abs_pos"];
        Ice::Long sponsor_id = (Ice::Long) row["member_id"];
        Ice::Int gender = (int) row["gender"];
        Ice::Int stage = NO_USE;
        Ice::Long impr = (Ice::Long) row["sum_impr"];
        Ice::Long click = (Ice::Long) row["sum_click"];
        string key = GetKey(zone_id,sponsor_id, group_id,pos_id,gender,stage);
        if(IsCredible(impr, click, no_stage)) {
          res_pool[key] = (double) click / (double) impr;
        }
      }
    }
    /*****************************spid_type***********************************************/
    {
      sql.str("");
      sql << "select adzone_id,member_id, abs_pos, gender,stage, sum(pv_count) as sum_impr, sum(click_count) as sum_click from rt_HZPMGGS  where time <= " << begin_time_pos  << " and time >=" << end_time_pos << "" <<" group by adzone_id, member_id, abs_pos, stage,gender ";
      MCE_INFO("try to excute " << sql.str());
      string s= sql.str();
      Query q(&conn,true,s.c_str());
      mysqlpp::StoreQueryResult res = q.store();
      if (!res || res.num_rows() <= 0) {
        MCE_WARN("DemoLoaderCache Load Error " << sql.str() << ".  select  fail!");
      }
      MCE_INFO("[DemoLoaderCache]::Load   res size : " << res.num_rows());
      for (size_t i = 0; i < res.num_rows(); ++i) {
        mysqlpp::Row row = res.at(i);
        Ice::Long zone_id = (Ice::Long) row["adzone_id"];
        Ice::Long group_id = NO_USE;
        int pos_id = (int) row["abs_pos"];
        Ice::Long sponsor_id = (Ice::Long) row["member_id"];
        Ice::Int gender = (int) row["gender"];
        Ice::Int stage = (Ice::Int) row["stage"];
        Ice::Long impr = (Ice::Long) row["sum_impr"];
        Ice::Long click = (Ice::Long) row["sum_click"];
        string key = GetKey(zone_id,sponsor_id, group_id,pos_id,gender,stage);
        if(IsCredible(impr, click, spid_type)) {
          res_pool[key] = (double) click / (double) impr;
        }
      }
    }
    /*************************************two feature**********************************/
    {
      map<string,map<Ice::Long,map<int,pair<Ice::Long, Ice::Long> > > > temp_map;
      sql.str("");
      sql << "select adzone_id,member_id, adgroup_id as group_id, abs_pos, gender, stage, sum(pv_count) as sum_impr, sum(click_count) as sum_click from rt_HZPMGGS  where time <= " << begin_time_pos  << " and time >=" << end_time_pos << "" <<" group by adzone_id , adgroup_id , abs_pos , gender,stage ";
      MCE_INFO("try to excute " << sql.str());
      string s= sql.str();
      Query q(&conn,true,s.c_str());
      mysqlpp::StoreQueryResult res = q.store();
      if (!res || res.num_rows() <= 0) {
        MCE_WARN("DemoLoaderCache Load Error " << sql.str() << ".  select  fail!");
      }
      MCE_INFO("[DemoLoaderCache]::Load   res size : " << res.num_rows());
      for (size_t i = 0; i < res.num_rows(); ++i) {
        mysqlpp::Row row = res.at(i);
        Ice::Long zone_id = (Ice::Long) row["adzone_id"];
        Ice::Long group_id = (Ice::Long) row["group_id"];
        int pos_id = (int) row["abs_pos"];
        Ice::Long sponsor_id = (Ice::Long) row["member_id"];
        Ice::Int gender = (int) row["gender"];
        Ice::Int stage = (Ice::Int) row["stage"];
        Ice::Long impr = (Ice::Long) row["sum_impr"];
        Ice::Long click = (Ice::Long) row["sum_click"];
        string key = GetKey(sponsor_id,group_id,gender,stage);
        temp_map[key][zone_id][pos_id] = make_pair(impr,click);

        //string key = GetKey(zone_id,sponsor_id, group_id,pos_id,gender,stage);
        //if(IsCredible(impr, click, two_feature)) {
        //  res_pool[key] = (double) click / (double) impr;
        //}
      }
      int ctr_discount_type = AdDemoConfig::instance().ctr_discount_type();
      if(1==ctr_discount_type) {
        ExpandCtrByPos(temp_map,res_pool,two_feature);
      }else if(2 == ctr_discount_type) {
        ExpandCtrByZone(temp_map,res_pool,two_feature);
      }else if(3 == ctr_discount_type) {
        ExpandCtrByPosAndZone(temp_map,res_pool,two_feature);
      }else {
        ExpandCtrByPos(temp_map,res_pool,two_feature);
      }


    }
    /******************************all**********************************/
    {
      sql.str("");
      sql << "select adzone_id,member_id, adgroup_id as group_id, abs_pos, sum(pv_count) as sum_impr, sum(click_count) as sum_click from rt_HZPMGGS  where time <= " << begin_time_pos  << " and time >=" << state_2_end_time_pos << "" << " group by adzone_id, adgroup_id ,abs_pos ";
      MCE_INFO("try to excute " << sql.str());
      string s= sql.str();
      Query q(&conn,true,s.c_str());
      mysqlpp::StoreQueryResult res = q.store();
      if (!res || res.num_rows() <= 0) {
        MCE_WARN("DemoLoaderCache Load Error " << sql.str() << ".  select  fail!");
      }
      MCE_INFO("[DemoLoaderCache]::Load   res size : " << res.num_rows());
      for (size_t i = 0; i < res.num_rows(); ++i) {
        mysqlpp::Row row = res.at(i);
        Ice::Long zone_id = (Ice::Long) row["adzone_id"];
        Ice::Long group_id = (Ice::Long) row["group_id"];
        int pos_id = (int) row["abs_pos"];
        Ice::Long sponsor_id = (Ice::Long) row["member_id"];
        Ice::Int gender = NO_USE;
        Ice::Int stage = NO_USE;
        Ice::Long impr = (Ice::Long) row["sum_impr"];
        Ice::Long click = (Ice::Long) row["sum_click"];
        string key = GetKey(zone_id,sponsor_id, group_id,pos_id,gender,stage);
        if(IsCredible(impr, click, all_type)) {
          res_pool[key] = (double) click / (double) impr;
        }
      }
    }

    InsertAmplifyGrouptoPool(res_pool);
    InsertUpdateGrouptoPool(res_pool);
    /*************************Load App Data**************************/
    /*************************************app two feature**********************************/
   // Connection app_conn("test","10.3.16.119","ad","rrad");
    {
    sql.str("");
    sql << "select app_name , sponsor_id, group_id, pos_id, gender, stage, sum(impr) as sum_impr, sum(click) as sum_click from app_demographic_ctr where time <= '" << begin_time  << "' and time >='" << end_time << "'" <<" group by app_name , group_id , pos_id , gender,stage ";
    MCE_INFO("try to excute " << sql.str());
    string s= sql.str();
   // Query q(&app_conn,true,s.c_str());
    Query q(&conn,true,s.c_str());
    mysqlpp::StoreQueryResult res = q.store();
    if (!res || res.num_rows() <= 0) {
      MCE_WARN("DemoLoaderCache Load Error " << sql.str() << ".  select  fail!");
    }
    MCE_INFO("[DemoLoaderCache]::Load   res size : " << res.num_rows());
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      string zone_name = (string) row["app_name"];
      Ice::Long group_id = (Ice::Long) row["group_id"];
      int pos_id = (int) row["pos_id"];
      Ice::Long sponsor_id = (Ice::Long) row["sponsor_id"];
      Ice::Int gender = (int) row["gender"];
      Ice::Int stage = (Ice::Int) row["stage"];
      Ice::Long impr = (Ice::Long) row["sum_impr"];
      Ice::Long click = (Ice::Long) row["sum_click"];
      string key = GetKey(zone_name,sponsor_id, group_id,pos_id,gender,stage);
      if(IsCredible(impr, click, app_two_feature)) {
        local_app_pool[key] = (double) click / (double) impr;
      }
    }
    }    
    /*****************************app spid_type***********************************************/
    {
    sql.str("");
    sql << "select app_name,sponsor_id, pos_id, gender,stage, sum(impr) as sum_impr, sum(click) as sum_click from app_demographic_ctr where time <= '" << begin_time  << "' and time >='" << end_time << "'" <<" group by app_name, sponsor_id, pos_id, stage,gender ";
    MCE_INFO("try to excute " << sql.str());
    string s= sql.str();
    Query q(&conn,true,s.c_str());
    mysqlpp::StoreQueryResult res = q.store();
    if (!res || res.num_rows() <= 0) {
      MCE_WARN("DemoLoaderCache Load Error " << sql.str() << ".  select  fail!");
    }
    MCE_INFO("[DemoLoaderCache]::Load   res size : " << res.num_rows());
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      string app_name = (string) row["app_name"];
      Ice::Long group_id = NO_USE;
      int pos_id = (int) row["pos_id"];
      Ice::Long sponsor_id = (Ice::Long) row["sponsor_id"];
      Ice::Int gender = (int) row["gender"]; 
      Ice::Int stage = (Ice::Int) row["stage"];
      Ice::Long impr = (Ice::Long) row["sum_impr"];
      Ice::Long click = (Ice::Long) row["sum_click"];
      string key = GetKey(app_name,sponsor_id, group_id,pos_id,gender,stage);
      if(IsCredible(impr, click, app_spid_type)) {
        local_app_pool[key] = (double) click / (double) impr;
      }
    }
    }    
    /****************app no_gender*****************************/
    {
    sql.str("");
    sql << "select app_name,sponsor_id, group_id, pos_id, stage, sum(impr) as sum_impr, sum(click) as sum_click from app_demographic_ctr where time <= '" << begin_time  << "' and time >='" << end_time << "'" <<" group by app_name,group_id,pos_id,stage ";
    MCE_INFO("try to excute " << sql.str());
    string s= sql.str();
    Query q(&conn,true,s.c_str());
    mysqlpp::StoreQueryResult res = q.store();
    if (!res || res.num_rows() <= 0) {
      MCE_WARN("DemoLoaderCache Load Error " << sql.str() << ".  select  fail!");
    }
    MCE_INFO("[DemoLoaderCache]::Load   res size : " << res.num_rows());
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      string app_name = (string) row["app_name"];
      Ice::Long group_id = (Ice::Long) row["group_id"];
      int pos_id = (int) row["pos_id"];
      Ice::Long sponsor_id = (Ice::Long) row["sponsor_id"];
      Ice::Int gender = NO_USE;
      Ice::Int stage = (Ice::Int) row["stage"];
      Ice::Long impr = (Ice::Long) row["sum_impr"];
      Ice::Long click = (Ice::Long) row["sum_click"];
      string key = GetKey(app_name,sponsor_id, group_id,pos_id,gender,stage);
      if(IsCredible(impr, click, app_no_gender)) {
        local_app_pool[key] = (double) click / (double) impr;
      }
    }
    }    
    /*******************app no_stage*************************************/  
    {
    sql.str("");
    sql << "select app_name,sponsor_id, group_id, pos_id, gender, sum(impr) as sum_impr, sum(click) as sum_click from app_demographic_ctr where time <= '" << begin_time  << "' and time >='" << end_time << "'" <<" group by app_name, group_id, pos_id, gender ";
    MCE_INFO("try to excute " << sql.str());
    string s= sql.str();
    Query q(&conn,true,s.c_str());
    mysqlpp::StoreQueryResult res = q.store();
    if (!res || res.num_rows() <= 0) {
      MCE_WARN("DemoLoaderCache Load Error " << sql.str() << ".  select  fail!");
    }
    MCE_INFO("[DemoLoaderCache]::Load   res size : " << res.num_rows());
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      string app_name = (string) row["app_name"];
      Ice::Long group_id = (Ice::Long) row["group_id"];
      int pos_id = (int) row["pos_id"];
      Ice::Long sponsor_id = (Ice::Long) row["sponsor_id"];
      Ice::Int gender = (int) row["gender"]; 
      Ice::Int stage = NO_USE;
      Ice::Long impr = (Ice::Long) row["sum_impr"];
      Ice::Long click = (Ice::Long) row["sum_click"];
      string key = GetKey(app_name,sponsor_id, group_id,pos_id,gender,stage);
      if(IsCredible(impr, click, app_no_stage)) {
        local_app_pool[key] = (double) click / (double) impr;
      }
    }
    }    
    /******************************app_all**********************************/
    {
    sql.str("");
    sql << "select app_name,sponsor_id, group_id, pos_id, sum(impr) as sum_impr, sum(click) as sum_click from app_demographic_ctr  where time <= '" << begin_time  << "' and time >='" << state_2_end_time << "'" << " group by app_name, group_id ,pos_id ";
    MCE_INFO("try to excute " << sql.str());
    string s= sql.str();
    Query q(&conn,true,s.c_str());
    mysqlpp::StoreQueryResult res = q.store();
    if (!res || res.num_rows() <= 0) {
      MCE_WARN("DemoLoaderCache Load Error " << sql.str() << ".  select  fail!");
    }
    MCE_INFO("[DemoLoaderCache]::Load   res size : " << res.num_rows());
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      string app_name= (string) row["app_name"];
      Ice::Long group_id = (Ice::Long) row["group_id"];
      int pos_id = (int) row["pos_id"];
      Ice::Long sponsor_id = (Ice::Long) row["sponsor_id"];
      Ice::Int gender = NO_USE;
      Ice::Int stage = NO_USE;
      Ice::Long impr = (Ice::Long) row["sum_impr"];
      Ice::Long click = (Ice::Long) row["sum_click"];
      string key = GetKey(app_name,sponsor_id, group_id,pos_id,gender,stage);
      if(IsCredible(impr, click, app_all_type)) {
        local_app_pool[key] = (double) click / (double) impr;
      }
    }
    }

    MCE_INFO("[DemoLoaderCache]:: app_pool size is " << local_app_pool.size());

    MCE_INFO("[DemoLoaderCache]:: stop demo_pool size is " << res_pool.size());

  } catch (std::exception& e) {
    MCE_WARN("LoadByTime --> load ad_quality error: " << e.what());
  } catch (...) {
    MCE_WARN("LoadByTime --> load ad_quality error!");
  }

  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    demo_pool.swap(res_pool);
    app_pool.swap(local_app_pool);
    MCE_INFO("[DemoLoaderCache]::LoadPool stop  size is " << demo_pool.size());
  }

}

}
}

