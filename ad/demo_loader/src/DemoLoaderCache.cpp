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
#include "AdQualityUtil.h"
#include "FreshAdTesting.h"
#include "AdQualityScore.h"
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

  xce::ad::AdDemoConfig::instance().LoadConfig();
  MyUtil::TaskManager::instance().schedule(new xce::ad::AdDemoConfig::ConfigLoadTimer(0));
  xce::ad::DemoLoaderCache::instance().LoadSystemParameter();
  TaskManager::instance().schedule(new xce::ad::DemoLoaderSystemParaTimer());
  MyUtil::TaskManager::instance().schedule(new xce::ad::FreshAdCacheTimer(0));    
  MyUtil::TaskManager::instance().schedule(new xce::ad::ScoreAdCacheTimer(0));
  MyUtil::TaskManager::instance().schedule(new xce::ad::DemoLoaderAverageEcpmTimer(0));
  MyUtil::TaskManager::instance().schedule(new xce::ad::DemoLoaderDisplayLessAdTimer(0));
  MyUtil::TaskManager::instance().schedule(new xce::ad::DemoLoaderSortPosCacheTimer(0));
  xce::ad::DemoLoaderCache::instance().LoadPool();
  TaskManager::instance().schedule(new xce::ad::DemoLoaderCacheTimer());
  //xce::ad::DemoLoaderCache::instance().LoadAppPool();
  //TaskManager::instance().schedule(new xce::ad::AppLoaderCacheTimer());

  xce::ad::DemoLoaderCache::instance().LoadCpmPool();
  TaskManager::instance().schedule(new xce::ad::CpmLoaderCacheTimer());
  xce::ad::DemoLoaderCache::instance().LoadLimitedAdPool();
  TaskManager::instance().schedule(new xce::ad::DemoLoaderLimitedAdTimer());

}

namespace xce{
namespace ad{
using namespace mysqlpp;

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
AdSortPosMap DemoLoaderCache::GetAppSortPos(const ::Ice::Current& current){
  MCE_INFO("DemoLoaderCache::GetAppSortPos--> call from " << current.con->toString());
  AdSortPosMap result;
  IceUtil::RWRecMutex::RLock lock(mutex_);
  result = app_sort_pos;
  MCE_INFO("DemoLoaderCache::GetAppSortPos,data size:"<<result.size());
  return result;
}
CpmAdgroupMap DemoLoaderCache::GetCpmAdGroupMap(const ::Ice::Current& current){
  MCE_INFO("DemoLoaderCache::GetCpmAdGroupMap--> call from " << current.con->toString());
  CpmAdgroupMap result;
  IceUtil::RWRecMutex::RLock lock(mutex_);
  result = cpm_groups;
  MCE_INFO("DemoLoaderCache::GetCpmAdGroupMap,data size:"<<result.size());
  return result;
}
LimitedAdGroupMap DemoLoaderCache::GetLimitedAdGroupMap(const ::Ice::Current& current){
  MCE_INFO("DemoLoaderCache::GetLimitedAdGroupMap--> call from " << current.con->toString());
  LimitedAdGroupMap result;
  IceUtil::RWRecMutex::RLock lock(mutex_);
  result = limited_groups;
  MCE_INFO("DemoLoaderCache::GetLimitedAdGroupMap,data size:"<<result.size());
  return result;
}

FreshAdQualityMap DemoLoaderCache::GetFreshAdMap(const ::Ice::Current& current){
  MCE_INFO("DemoLoaderCache::GetFreshAdMap--> call from " << current.con->toString());
  FreshAdQualityMap result;
  FreshAdCache::instance().LoadFreshAdMap(result);
  MCE_INFO("DemoLoaderCache::GetFreshAd,pool size:"<<result.size()); 
  return result;
}
AdQualityScoreMap DemoLoaderCache::GetAdsQuality(const ::Ice::Current& current){
  MCE_INFO("DemoLoaderCache::GetAdQualityScore--> call from " << current.con->toString());
  AdQualityScoreMap result;
  ScoreAdCache::instance().LoadScoreAdMap(result);
  MCE_INFO("DemoLoaderCache::GetAdQualityScore,pool size:"<<result.size()); 
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

void DemoLoaderCache::LoadSystemParameter(){
  MCE_INFO("DemoLoaderCache::LoadSystemParameter begin");
  map<string,string> system_para;
  try{
    const char* db_host = AdDemoConfig::instance().db_host().c_str();
    const char* db_name = AdDemoConfig::instance().db_name().c_str();
    Connection conn(db_name,db_host,"ad","rrad");
    MCE_INFO("DemoLoaderCache::LoadSystemParameter,db host:" << AdDemoConfig::instance().db_host()<< ",db_name:" << db_name);

    stringstream sql;
    sql.str("");
    sql << " select * from system_parameter";
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
      string key = (string) row["key"];
      string value = (string) row["value"];
      MCE_INFO("[DemoLoaderCache]:: key=" << key << ", value=" << value);
      system_para[key] = value;
    }
    MCE_INFO("[DemoLoaderCache]:: stop SystemParameters size is " << system_para.size());
  } catch (std::exception& e) {
    MCE_WARN("LoadByTime --> load system parameters error: " << e.what());
  } catch (...) {
    MCE_WARN("LoadByTime --> load system parameters error!");
  }
  
  IceUtil::RWRecMutex::WLock lock(mutex_);
  system_para_.swap(system_para);
  MCE_INFO("DemoLoaderCache::LoadSystemParameter stop,system_para size:" << system_para.size());
}

string DemoLoaderCache::GetParameterByName(string para_name) {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  map<string, string>::iterator its = system_para_.find(para_name);
  if(its != system_para_.end()) {
    return its->second;
  }
  return ""; 
}

void DemoLoaderCache::LoadLimitedAdPool(){
  MCE_INFO("DemoLoaderCache::LoadLimitedAdPool begin");
  LimitedAdGroupMap limited_ad_map;
  MyUtil::Date begin = MyUtil::Date::seconds(time(NULL));
  string begin_time = begin.str("%Y-%m-%d %H:%M:%S");
  Ice::Long begin_time_pos = GetDay(begin.year(),begin.month(),begin.day());
  MyUtil::Date end = MyUtil::Date::seconds(time(NULL) - 10 * HOURCOUNT * 3600);
  Ice::Long end_time_pos = GetDay(end.year(),end.month(),end.day());

  try{
    const char* db_host = AdDemoConfig::instance().db_host().c_str();
    const char* db_name = AdDemoConfig::instance().db_name().c_str();
    Connection conn(db_name,db_host,"ad","rrad");
    MCE_INFO("DemoLoaderCache::LoadLimitedAdPool,db host:" << AdDemoConfig::instance().db_host()<< ",db_name:" << db_name);

    stringstream sql;
    sql.str("");
    sql << "select adzone_id, adgroup_id as group_id, abs_pos, sum(pv_count) as sum_impr, sum(sum_click_price) as sum_cost from "
        << " rt_DZPMGGS where time >= " << end_time_pos << " and adzone_id in (100000000001,100000000063,100000000070,100000000072) and member_id <> 999"
        << " group by adzone_id,adgroup_id,abs_pos";
    MCE_INFO("try to excute " << sql.str());
    string s= sql.str();
    Query q(&conn,true,s.c_str());
    mysqlpp::StoreQueryResult res = q.store();
    if (!res || res.num_rows() <= 0) {
      MCE_WARN("DemoLoaderCache Load Error " << sql.str() << ".  select  fail!");
    }
    MCE_INFO("[DemoLoaderCache]::Load   res size : " << res.num_rows());
    map<Ice::Long,map<Ice::Long,map<int,pair<Ice::Long, Ice::Long> > > > temp_map;
    map<Ice::Long, double> ecpm_map;
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      Ice::Long zone_id = (Ice::Long) row["adzone_id"];
      Ice::Long group_id = (Ice::Long) row["group_id"];
      int pos_id = (int) row["abs_pos"];
      Ice::Long impr = (Ice::Long) row["sum_impr"];
      Ice::Long cost = (Ice::Long) row["sum_cost"];
      temp_map[group_id][zone_id][pos_id] = make_pair(impr,cost);
    }
    ExpandEcpmByZone(temp_map,ecpm_map);
    double ecpm_line2 = 0;
    double ecpm_line1 = 0;
    string ret = GetParameterByName("ecpm_line2");
    if(ret != ""){
      ecpm_line2 = boost::lexical_cast<double>(ret);
    }
    ret = GetParameterByName("ecpm_line1");
    if(ret != ""){
      ecpm_line1 = boost::lexical_cast<double>(ret);
    }
    MCE_INFO("[DemoLoaderCache]:: ecpm_map size is " << ecpm_map.size());
    FreshAdQualityMap fresh_gids_map;
    FreshAdCache::instance().LoadFreshAdMap(fresh_gids_map);
    FreshAdQualitySet fresh_gids = fresh_gids_map["reserved"];

    for(map<Ice::Long, double>::iterator iter = ecpm_map.begin(); iter != ecpm_map.end(); ++iter){
      map<Ice::Long, double>::iterator it = fresh_gids.find(iter->first);
      MCE_INFO("Limited_ecpm-gid-" << iter->first << "-ecpm-" << iter->second)
      if(it != fresh_gids.end()) {
        continue;
      }
      if(iter->second < ecpm_line1){
        limited_ad_map[iter->first] = 1;
      }else if(iter->second < ecpm_line2){
        limited_ad_map[iter->first] = 2;
      }
    }
    MCE_INFO("[DemoLoaderCache]:: stop LimitedAdPool size is " << limited_ad_map.size());
  } catch (std::exception& e) {
    MCE_WARN("LoadByTime --> load limited ad pool error: " << e.what());
  } catch (...) {
    MCE_WARN("LoadByTime --> load limited ad pool error!");
  }
  
  IceUtil::RWRecMutex::WLock lock(mutex_);
  limited_groups.swap(limited_ad_map);
  MCE_INFO("DemoLoaderCache::LoadLimitedAdPool stop,limited_groups size:" << limited_groups.size());
}

void DemoLoaderCache::LoadCpmPool() {
  MCE_INFO("DemoLoaderCache::LoadCpmPool begin");
  CpmAdgroupMap cpm_groups_tmp;
  map<Ice::Long, map<Ice::Long, CpmAd> > cpm_ad_map;
  Ice::Long min_start_time = 30001225;

  stringstream cpm_sql;
  cpm_sql << "select * from cpm_adgroup";
  MCE_INFO("DemoLoaderCache::LoadCpmPool try to excute " << cpm_sql.str());
  string cpm_s= cpm_sql.str();
  
  Ice::Long t = time(NULL);
  MyUtil::Date yestoday_date = MyUtil::Date::seconds(t - 24 * 3600);
  MyUtil::Date today_date = MyUtil::Date::seconds(t);
  string yestoday = yestoday_date.str("%Y%m%d");
  string today = today_date.str("%Y%m%d");
  string cur_hour = today_date.str("%Y%m%d%H");
  double today_time = today_date.toSecondsDouble();
  set<Ice::Long> cpm_ad_ids;
  try {
    const char* db_jebe_host = AdDemoConfig::instance().db_jebe_host().c_str();
    MCE_INFO("DemoLoaderCache::LoadCpmPool Db name " << db_jebe_host);
    Connection conn("test",db_jebe_host,"ad","adjb###");
    Query cpm_q(&conn,true,cpm_s.c_str());
    mysqlpp::StoreQueryResult cpm_res = cpm_q.store();
    if (!cpm_res || cpm_res.num_rows() <= 0) {
      MCE_INFO("DemoLoaderCache::LoadCpmPool Error " << cpm_sql.str() << ".  select 0 rows!");
    }
    MCE_INFO("DemoLoaderCache::LoadCpmPool res size : " << cpm_res.num_rows());
    
    for (size_t i = 0; i < cpm_res.num_rows(); ++i) {
      mysqlpp::Row row = cpm_res.at(i);
      int type = (int) row["type"];
      if(type == cpc_type){
        continue;
      }
      Ice::Long adgroup_id = (Ice::Long) row["adgroup_id"];
      int is_valid = (int) row["is_valid"];
      if(is_valid == 0){
        CpmAdgroup cpm_adgroup;
        stringstream key_stream;
        key_stream << adgroup_id;
        cpm_groups_tmp[key_stream.str()] = cpm_adgroup;
        continue;
      }
      Ice::Long adzone_id = (Ice::Long) row["adzone_id"];
      Ice::Long contract_start_time = (int) row["contract_start_time"];
      Ice::Long start_time = (int) row["start_time"];
      int duration = (int) row["duration"];
      Ice::Long pv = (Ice::Long) row["pv"];
      Ice::Long cost = (Ice::Long) row["cost"];
      int price = (int) row["price"];
      CpmAd cpmAd;
      cpmAd.type_ = type;
      cpmAd.adgroup_id_ = adgroup_id;
      cpmAd.adzone_id_ = adzone_id;
      cpmAd.contract_start_time_ = contract_start_time;
      cpmAd.start_time_ = start_time;
      cpmAd.duration_ = duration;
      
      int year = start_time / 10000;
      int month = start_time % 10000 / 100;
      int day = start_time % 100;
      stringstream tstr;
      tstr << year << "-" << month << "-" << day << " 00:00:00";
      MyUtil::Date start_date = MyUtil::Date(tstr.str());
      double start_time_d = start_date.toSecondsDouble();
      cpmAd.past_days_ = (today_time - start_time_d) / (3600 * 24);
      //未来的计划或者过时的每天计划和严格执行的计划都放弃
      int is_strict = (int) row["is_strict"];
      if(today_time < start_time_d || ((duration == 1 || is_strict == 1) && cpmAd.past_days_ >= duration)){
        CpmAdgroup cpm_adgroup;
        stringstream key_stream;
        key_stream << adgroup_id;
        cpm_groups_tmp[key_stream.str()] = cpm_adgroup;
        continue;
      }
      cpm_ad_ids.insert(adgroup_id);
      if(contract_start_time < min_start_time){
        min_start_time = contract_start_time;
      }
      
      cpmAd.pv_ = pv;
      cpmAd.cost_ = cost;
      cpmAd.past_pv_ = 0;
      cpmAd.past_click_ = 0;
      cpmAd.past_cost_ = 0;
      cpmAd.today_pv_ = 0;
      cpmAd.today_click_ = 0;
      cpmAd.today_cost_ = 0;
      cpmAd.cur_hour_pv_ = 0;
      cpmAd.cur_hour_click_ = 0;
      cpmAd.cur_hour_cost_ = 0;
      cpmAd.price_ = price;
      MCE_INFO("cpm_ad: adgroup_id=" << adgroup_id 
          << ",zone_id=" << adzone_id 
          << ",contract_start_time=" << contract_start_time 
          << ",start_time=" << start_time 
          << ",duration=" << duration 
          << ",pv=" << pv 
          << ",cost=" << cost 
          << ",price=" << price);
      map<Ice::Long, map<Ice::Long, CpmAd> >::iterator itr = cpm_ad_map.find(adgroup_id);
      if(itr != cpm_ad_map.end()) {
        map<Ice::Long, CpmAd>& zone_map = itr->second;
        map<Ice::Long, CpmAd>::iterator itr2 = zone_map.find(adzone_id);
        if(itr2 != zone_map.end()) {
          MCE_WARN("ambiguous cpm plan for adgroup(" << adgroup_id << ") in adzone(" << adzone_id << ")");
        }
      }
      cpm_ad_map[adgroup_id][adzone_id] = cpmAd;
    } 
  }catch (std::exception& e) {
    MCE_WARN("DemoLoaderCache::LoadCpmPool error: " << e.what());
  } catch (...) {
    MCE_WARN("DemoLoaderCache::LoadCpmPool error!");
  }

  stringstream ctr_sql;
  if(cpm_ad_ids.empty()){
    CpmAdgroupMap cpm_member_groups;
    LoadCpmMemberPool(cpm_member_groups);
    if (! cpm_member_groups.empty()) {
      cpm_groups_tmp.insert(cpm_member_groups.begin(), cpm_member_groups.end());
    }     
    IceUtil::RWRecMutex::WLock lock(mutex_);
    cpm_groups.swap(cpm_groups_tmp);
    MCE_INFO("DemoLoaderCache::LoadCpmPool stop,cpm pool size:" << cpm_groups.size());
    return;
  }
  string cpm_ads_list = TransformForSql(cpm_ad_ids);
  ctr_sql << " SELECT FLOOR(creative_id/100000) as adgroup_id,adzone_id,SUM(pv_count) as sum_pv_count,SUM(click_count) as sum_click_count,SUM(sum_click_price) as sum_click_price"
          << " FROM Log_DCZY "
          << " WHERE FLOOR(creative_id/100000) IN " << cpm_ads_list 
          << " AND TIME >= " << min_start_time << " and time <= " << yestoday
          << " GROUP BY FLOOR(creative_id/100000),adzone_id";
  MCE_INFO("DemoLoaderCache::LoadCpmPool try to excute " << ctr_sql.str());
  string ctr_s= ctr_sql.str();
  try {
    const char* db_jebe_host = AdDemoConfig::instance().db_jebe_host().c_str();
    Connection conn("statdb",db_jebe_host,"ad","adjb###");
    Query ctr_q(&conn,true,ctr_s.c_str());
    mysqlpp::StoreQueryResult ctr_res = ctr_q.store();
    if (!ctr_res || ctr_res.num_rows() <= 0) {
      MCE_WARN("DemoLoaderCache::LoadCpmPool Error " << ctr_sql.str() << ".  select  fail!");
    }
    MCE_INFO("DemoLoaderCache::LoadCpmPool res size : " << ctr_res.num_rows());
    
    for (size_t i = 0; i < ctr_res.num_rows(); ++i) {
      mysqlpp::Row row = ctr_res.at(i);
      Ice::Long adgroup_id = (Ice::Long) row["adgroup_id"];
      Ice::Long adzone_id = (Ice::Long) row["adzone_id"];
      Ice::Long sum_pv_count = (Ice::Long) row["sum_pv_count"];
      Ice::Long sum_click_count = (Ice::Long) row["sum_click_count"];
      Ice::Long sum_click_price = (Ice::Long) row["sum_click_price"];
      map<Ice::Long, map<Ice::Long, CpmAd> >::iterator itr = cpm_ad_map.find(adgroup_id);
      if(itr != cpm_ad_map.end()) {
        map<Ice::Long, CpmAd>& zone_map = itr->second;
        map<Ice::Long, CpmAd>::iterator itr2 = zone_map.find(adzone_id);
        if(itr2 != zone_map.end()) {
          itr2->second.past_pv_ += sum_pv_count;
          itr2->second.past_click_ += sum_click_count;
          itr2->second.past_cost_ += sum_click_price;
        }
        itr2 = zone_map.find(-1);
        if(itr2 != zone_map.end()) {
          itr2->second.past_pv_ += sum_pv_count;
          itr2->second.past_click_ += sum_click_count;
          itr2->second.past_cost_ += sum_click_price;
        }
      }
    } 
  }catch (std::exception& e) {
    MCE_WARN("DemoLoaderCache::LoadCpmPool error: " << e.what());
  } catch (...) {
    MCE_WARN("DemoLoaderCache::LoadCpmPool error!");
  }

  stringstream tctr_sql;
  tctr_sql << " SELECT FLOOR(creative_id/100000) as adgroup_id,adzone_id,SUM(pv_count) as sum_pv_count,SUM(click_count) as sum_click_count,SUM(sum_click_price) as sum_click_price"
           << " FROM Log_DCZY "
           << " WHERE FLOOR(creative_id/100000) IN " << cpm_ads_list 
           << " AND TIME = " << today
           << " GROUP BY FLOOR(creative_id/100000),adzone_id";
  MCE_INFO("DemoLoaderCache::LoadCpmPool try to excute " << tctr_sql.str());
  string tctr_s= tctr_sql.str();
  try {
    const char* db_jebe_host = AdDemoConfig::instance().db_jebe_host().c_str();
    Connection conn("statdb",db_jebe_host,"ad","adjb###");
    Query ctr_q(&conn,true,tctr_s.c_str());
    mysqlpp::StoreQueryResult ctr_res = ctr_q.store();
    if (!ctr_res || ctr_res.num_rows() <= 0) {
      MCE_WARN("DemoLoaderCache::LoadCpmPool Error " << tctr_sql.str() << ".  select  fail!");
    }
    MCE_INFO("DemoLoaderCache::LoadCpmPool res size : " << ctr_res.num_rows());
    
    for (size_t i = 0; i < ctr_res.num_rows(); ++i) {
      mysqlpp::Row row = ctr_res.at(i);
      Ice::Long adgroup_id = (Ice::Long) row["adgroup_id"];
      Ice::Long adzone_id = (Ice::Long) row["adzone_id"];
      Ice::Long sum_pv_count = (Ice::Long) row["sum_pv_count"];
      Ice::Long sum_click_count = (Ice::Long) row["sum_click_count"];
      Ice::Long sum_click_price = (Ice::Long) row["sum_click_price"];
      map<Ice::Long, map<Ice::Long, CpmAd> >::iterator itr = cpm_ad_map.find(adgroup_id);
      if(itr != cpm_ad_map.end()) {
        map<Ice::Long, CpmAd>& zone_map = itr->second;
        map<Ice::Long, CpmAd>::iterator itr2 = zone_map.find(adzone_id);
        if(itr2 != zone_map.end()) {
          itr2->second.today_pv_ += sum_pv_count;
          itr2->second.today_click_ += sum_click_count;
          itr2->second.today_cost_ += sum_click_price;
        }
        itr2 = zone_map.find(-1);
        if(itr2 != zone_map.end()) {
          itr2->second.today_pv_ += sum_pv_count;
          itr2->second.today_click_ += sum_click_count;
          itr2->second.today_cost_ += sum_click_price;
        }
      }
    }
  }catch (std::exception& e) {
    MCE_WARN("DemoLoaderCache::LoadCpmPool error: " << e.what());
  } catch (...) {
    MCE_WARN("DemoLoaderCache::LoadCpmPool error!");
  }

  stringstream hctr_sql;
  hctr_sql << " SELECT adgroup_id,adzone_id,SUM(pv_count) as sum_pv_count,SUM(click_count) as sum_click_count,SUM(sum_click_price) as sum_click_price"
           << " FROM Log_HGZ "
           << " WHERE adgroup_id IN " << cpm_ads_list 
           << " AND TIME = " << cur_hour
           << " GROUP BY adgroup_id,adzone_id";
  MCE_INFO("DemoLoaderCache::LoadCpmPool try to excute " << hctr_sql.str());
  string hctr_s= hctr_sql.str();
  try {
    const char* db_jebe_host = AdDemoConfig::instance().db_jebe_host().c_str();
    Connection conn("statdb",db_jebe_host,"ad","adjb###");
    Query ctr_q(&conn,true,hctr_s.c_str());
    mysqlpp::StoreQueryResult ctr_res = ctr_q.store();
    if (!ctr_res || ctr_res.num_rows() <= 0) {
      MCE_WARN("DemoLoaderCache::LoadCpmPool Error " << hctr_sql.str() << ".  select  fail!");
    }
    MCE_INFO("DemoLoaderCache::LoadCpmPool res size : " << ctr_res.num_rows());
    
    for (size_t i = 0; i < ctr_res.num_rows(); ++i) {
      mysqlpp::Row row = ctr_res.at(i);
      Ice::Long adgroup_id = (Ice::Long) row["adgroup_id"];
      Ice::Long adzone_id = (Ice::Long) row["adzone_id"];
      Ice::Long sum_pv_count = (Ice::Long) row["sum_pv_count"];
      Ice::Long sum_click_count = (Ice::Long) row["sum_click_count"];
      Ice::Long sum_click_price = (Ice::Long) row["sum_click_price"];
      map<Ice::Long, map<Ice::Long, CpmAd> >::iterator itr = cpm_ad_map.find(adgroup_id);
      if(itr != cpm_ad_map.end()) {
        map<Ice::Long, CpmAd>& zone_map = itr->second;
        map<Ice::Long, CpmAd>::iterator itr2 = zone_map.find(adzone_id);
        if(itr2 != zone_map.end()) {
          itr2->second.cur_hour_pv_ += sum_pv_count;
          itr2->second.cur_hour_click_ += sum_click_count;
          itr2->second.cur_hour_cost_ += sum_click_price;
        }
        itr2 = zone_map.find(-1);
        if(itr2 != zone_map.end()) {
          itr2->second.cur_hour_pv_ += sum_pv_count;
          itr2->second.cur_hour_click_ += sum_click_count;
          itr2->second.cur_hour_cost_ += sum_click_price;
        }
      }
    }
  }catch (std::exception& e) {
    MCE_WARN("DemoLoaderCache::LoadCpmPool error: " << e.what());
  } catch (...) {
    MCE_WARN("DemoLoaderCache::LoadCpmPool error!");
  }
  
  for(map<Ice::Long, map<Ice::Long, CpmAd> >::iterator iter = cpm_ad_map.begin(); iter != cpm_ad_map.end(); ++iter){
    for(map<Ice::Long, CpmAd>::iterator sub_iter = iter->second.begin(); sub_iter != iter->second.end(); ++sub_iter){
      CpmAd& cpm_ad = sub_iter->second;
      
      Ice::Long all_reserve_pv = cpm_ad.pv_ - cpm_ad.past_pv_;
      if(cpm_ad.duration_ == 1 && cpm_ad.past_days_ == 0){
        all_reserve_pv = cpm_ad.pv_;
      }
      Ice::Long daily_pv = 0;
      if(cpm_ad.past_days_ >= cpm_ad.duration_){
        daily_pv = all_reserve_pv;
      }else{
        daily_pv = all_reserve_pv / (cpm_ad.duration_ - cpm_ad.past_days_);
      }
      Ice::Long today_reserve_pv = daily_pv - cpm_ad.today_pv_;

      int past_hours = boost::lexical_cast<int>(cur_hour) - boost::lexical_cast<int>(today) * 100;
      Ice::Long now_reserve_pv = today_reserve_pv + cpm_ad.cur_hour_pv_;
      Ice::Long hourly_pv = now_reserve_pv / (24 - past_hours);
      Ice::Long cur_hour_reserve_pv = hourly_pv - cpm_ad.cur_hour_pv_;

      CpmAdgroup cpm_adgroup;
      cpm_adgroup.groupid = iter->first;
      cpm_adgroup.pv = cur_hour_reserve_pv;
      cpm_adgroup.charge = cpm_ad.price_;
      if(cpm_ad.pv_ == 0){
        cpm_adgroup.cpm = 0;
      }else{
        cpm_adgroup.cpm = cpm_ad.cost_ / (double)cpm_ad.pv_;
      }
      
      Ice::Long all_past_pv = cpm_ad.past_pv_ + cpm_ad.today_pv_;
      Ice::Long all_past_click = cpm_ad.past_click_ + cpm_ad.today_click_;
      Ice::Long reserve_pv = all_reserve_pv - cpm_ad.today_pv_;
      if(((all_past_pv > 100000 && all_past_click > 0) || (all_past_click > 20 && all_past_pv > 0) ) && reserve_pv > 0){
        double ctr = all_past_click /(double)all_past_pv;
        if(cpm_ad.type_ == cpm_type){
          double reserve_cpm = (cpm_ad.cost_ - cpm_ad.past_cost_ - cpm_ad.today_cost_) / (double)reserve_pv;
          cpm_adgroup.charge = reserve_cpm / ctr;
        }else if(cpm_ad.type_ == cpa_type){
          cpm_adgroup.cpm = ctr * cpm_adgroup.charge; 
        }
      }
     
      if(cpm_adgroup.charge <= 0){
        cpm_adgroup.charge = 1;
      }
      stringstream key_stream;
      key_stream << iter->first;
      cpm_groups_tmp[key_stream.str()] = cpm_adgroup;
      key_stream << "_" << sub_iter->first;
      cpm_groups_tmp[key_stream.str()] = cpm_adgroup;
      MCE_INFO("cpm_ad_group: adgroup_id=" << cpm_adgroup.groupid 
          << ",type=" << cpm_ad.type_
          << ",zone_id=" << sub_iter->first
          << ",past_days=" << cpm_ad.past_days_
          << ",reserve_pv=" << reserve_pv
          << ",daily_pv=" << daily_pv
          << ",today_reserve_pv=" << today_reserve_pv
          << ",hourly_pv=" << hourly_pv
          << ",cur_hour_reserve_pv=" << cpm_adgroup.pv 
          << ",charge=" << cpm_adgroup.charge 
          << ",cpm=" << cpm_adgroup.cpm);
    }
  }
    
  CpmAdgroupMap cpm_member_groups;
  LoadCpmMemberPool(cpm_member_groups);
  if (! cpm_member_groups.empty()) {
    cpm_groups_tmp.insert(cpm_member_groups.begin(), cpm_member_groups.end());
  } 
  IceUtil::RWRecMutex::WLock lock(mutex_);
  cpm_groups.swap(cpm_groups_tmp);
  MCE_INFO("DemoLoaderCache::LoadCpmPool stop,cpm pool size:" << cpm_groups.size());
}

void DemoLoaderCache::LoadCpmMemberPool(CpmAdgroupMap &cpm_member_groups){
  /* for online  todo , config in db */
  
  Ice::Long nuomi_member_id = 100000009136;
  Ice::Long nuomi_daily_pv = 7500000;
  Ice::Long nuomi_adzone_id = 100000000060;
   
  /* for test */
  /*
  //Ice::Long nuomi_member_id = 198000014166;  // test offline
  Ice::Long nuomi_member_id = 100000019715; // test online
  Ice::Long nuomi_daily_pv = 100; // <= 0 not ; > 0 yes
  Ice::Long nuomi_adzone_id = 100000000073;
  */
  Ice::Long t = time(NULL);
  MyUtil::Date yestoday_date = MyUtil::Date::seconds(t);
  MyUtil::Date today_date = MyUtil::Date::seconds(t);
  string today = today_date.str("%Y%m%d");
  stringstream hctr_sql;

  hctr_sql << " SELECT member_id, adzone_id, SUM(pv_count) as sum_pv_count"
           << " FROM  Log_DMZD"
           << " WHERE member_id = " << nuomi_member_id
           //<< " AND adzone_id = " << nuomi_adzone_id 
           << " AND TIME = " << today
           << " GROUP BY member_id" ;
           //<< " GROUP BY member_id, adzone_id" ;
  MCE_INFO("DemoLoaderCache::LoadCpmMemberPool try to excute " << hctr_sql.str());
  string hctr_s= hctr_sql.str();
  try {
    const char* db_jebe_host = AdDemoConfig::instance().db_jebe_host().c_str();
    Connection conn("statdb",db_jebe_host,"ad","adjb###");
    Query ctr_q(&conn,true,hctr_s.c_str());
    mysqlpp::StoreQueryResult ctr_res = ctr_q.store();
    if (!ctr_res || ctr_res.num_rows() <= 0) {
      MCE_WARN("DemoLoaderCache::LoadCpmMemberPool Error " << hctr_sql.str() << ".  select  fail!");
    }
    MCE_INFO("DemoLoaderCache::LoadCpmMemberPool res size : " << ctr_res.num_rows());

    for (size_t i = 0; i < ctr_res.num_rows(); ++i) {
      mysqlpp::Row row = ctr_res.at(i);
      Ice::Long member_id = (Ice::Long) row["member_id"];
      //Ice::Long adzone_id = (Ice::Long) row["adzone_id"];
      Ice::Long sum_pv_count = (Ice::Long) row["sum_pv_count"];
      CpmAdgroup cpm_adgroup;
      cpm_adgroup.pv = -100;
      stringstream key_stream;
      key_stream << member_id << "_member_" << nuomi_adzone_id;
      MCE_INFO("DemoLoaderCache::LoadCpmMemberPool key = " << key_stream.str()  << " sum_pv_pv = " << sum_pv_count);
      if(sum_pv_count < nuomi_daily_pv) {
        cpm_adgroup.pv = nuomi_daily_pv - sum_pv_count;
      }
      cpm_member_groups[key_stream.str()] = cpm_adgroup;
      MCE_INFO("DemoLoaderCache::LoadCpmMemberPool add: " << key_stream.str() << " pv = " << cpm_adgroup.pv);
    }
  }catch (std::exception& e) {
    MCE_WARN("DemoLoaderCache::LoadCpmMemberPool error: " << e.what());
  } catch (...) {
    MCE_WARN("DemoLoaderCache::LoadCpmMemberPool error!");
  }
 
  if (cpm_member_groups.empty()) { 
    CpmAdgroup cpm_adgroup;
    cpm_adgroup.pv = nuomi_daily_pv; //默认
    stringstream key_stream;
    key_stream << nuomi_member_id << "_member_" << nuomi_adzone_id;
    cpm_member_groups[key_stream.str()] = cpm_adgroup;
    MCE_INFO("DemoLoaderCache::LoadCpmMemberPool add: " << key_stream.str() << " pv = " << cpm_adgroup.pv);
  }
  CpmAdgroup cpm_adgroup;
  cpm_adgroup.pv = -100; //过滤掉member
  stringstream key_stream;
  key_stream << nuomi_member_id;
  cpm_member_groups[key_stream.str()] = cpm_adgroup;
  MCE_INFO("DemoLoaderCache::LoadCpmMemberPool add: " << key_stream.str() << " pv = " << cpm_adgroup.pv);
  MCE_INFO("DemoLoaderCache::LoadCpmMemberPool load cpm_member size = " << cpm_member_groups.size());
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
    Ice::Long begin_pos = GetTime(ctr_begin.year(),ctr_begin.month(),ctr_begin.day(), ctr_begin.hour());
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
   const char* db_jebe_host = AdDemoConfig::instance().db_jebe_host().c_str();
   const char* db_jebe_name = AdDemoConfig::instance().db_jebe_name().c_str();
   Connection conn(db_jebe_name,db_jebe_host,"ad","adjb###");
   stringstream sql;
   sql << "select adgroup_id as group_id from ad_audit_his where audit_time >='" <<
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
  click_discount_ratio_table[pos_751] = 1;
  click_discount_ratio_table[pos_752] = 1.920088852;
  click_discount_ratio_table[pos_753] = 5.376857573;
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
        string key = GetKey(itz->first,itp->first);
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

        string c_key = GetKey(itz->first,itp->first);
        map<string,double>::iterator itc = click_discount_ratio_table.find(c_key);
        double ratio = 1.0;
        if(itc != click_discount_ratio_table.end()) {
          ratio = itc->second;
        }

        string key = GetKey(itz->first,spid,gid,itp->first,gender,stage);

        if(IsCredible(impr, click, type)) {
          res_pool[key] = (double) click / (double) impr;
        } else if(IsCredible(sum_impr, sum_click, type)){
          //res_pool[key] = (double) sum_click / (double) (sum_impr * ratio);
          int pv_th = GetPvThreshold(type);
          int click_th = GetClickThreshold(type);
          double impr_weight = (double) impr / (double) pv_th;
          double click_weight = (double) click / (double) click_th;
          double weight = impr_weight > click_weight ? impr_weight : click_weight;
          res_pool[key] = weight * ((double) click / (double) impr) + (1 - weight) * ((double) sum_click / (double) (sum_impr * ratio));
        }
      }
    }
  }
  MCE_INFO("DemoLoaderCache::ExpandCtrByPos,stop:temp size:" << temp_pool.size() <<",res_pool size:" << res_pool.size());
}
void DemoLoaderCache::ExpandCtrByZone(map<string,map<Ice::Long,map<int,pair<Ice::Long, Ice::Long> > > >& temp_pool,AdQualityMap& res_pool,int type){
  MCE_INFO("DemoLoaderCache::ExpandCtrByZone,begin:temp size:" << temp_pool.size() <<",res_pool size:" << res_pool.size());

  map<string,double> click_discount_ratio_table;
  click_discount_ratio_table[pos_751] = 1.233590330;
  click_discount_ratio_table[pos_752] = 2.368603041;
  click_discount_ratio_table[pos_753] = 6.632839508;
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
        string key = GetKey(itz->first,itp->first);
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

        string c_key = GetKey(itz->first,itp->first);
        map<string,double>::iterator itc = click_discount_ratio_table.find(c_key);
        double ratio = 1.0;
        if(itc != click_discount_ratio_table.end()) {
          ratio = itc->second;
        }

        string key = GetKey(itz->first,spid,gid,itp->first,gender,stage);
        if(IsCredible(impr, click, type)) {
          res_pool[key] = (double) click / (double) impr;
        } else if(IsCredible(sum_impr, sum_click, type)){
          //res_pool[key] = (double) sum_click / (double) (sum_impr * ratio);
          int pv_th = GetPvThreshold(type);
          int click_th = GetClickThreshold(type);
          double impr_weight = (double) impr / (double) pv_th;
          double click_weight = (double) click / (double) click_th;
          double weight = impr_weight > click_weight ? impr_weight : click_weight;
          res_pool[key] = weight * ((double) click / (double) impr) + (1 - weight) * ((double) sum_click / (double) (sum_impr * ratio));
        }
      }
    }

  }
  MCE_INFO("DemoLoaderCache::ExpandCtrByZone,stop:temp size:" << temp_pool.size() <<",res_pool size:" << res_pool.size());
}
void DemoLoaderCache::ExpandEcpmByZone(map<Ice::Long,map<Ice::Long,map<int,pair<Ice::Long, Ice::Long> > > >& temp_pool,map<Ice::Long, double>& res_pool){
  MCE_INFO("DemoLoaderCache::ExpandEcpmByZone,begin:temp size:" << temp_pool.size() <<",res_pool size:" << res_pool.size());

  map<string,double> click_discount_ratio_table;
  click_discount_ratio_table[pos_751] = 1.233590330;
  click_discount_ratio_table[pos_752] = 2.368603041;
  click_discount_ratio_table[pos_753] = 6.632839508;
  click_discount_ratio_table[pos_721] = 2.075563365;
  click_discount_ratio_table[pos_722] = 3.419027415;
  click_discount_ratio_table[pos_723] = 5.980428574;
  click_discount_ratio_table[pos_701] = 2.702867005;
  click_discount_ratio_table[pos_702] = 5.534257896;
  click_discount_ratio_table[pos_703] = 7.06495615;
  click_discount_ratio_table[pos_631] = 1;
  click_discount_ratio_table[pos_632] = 1.358939264;
  click_discount_ratio_table[pos_633] = 1.711976347;
/*for test
  click_discount_ratio_table[pos_011] = 0.5;
  click_discount_ratio_table[pos_012] = 0.5;
  click_discount_ratio_table[pos_013] = 0.5;
*/
  click_discount_ratio_table[pos_011] = 2.3;
  click_discount_ratio_table[pos_012] = 4.5;
  click_discount_ratio_table[pos_013] = 6.3;
  for(map<Ice::Long,map<Ice::Long,map<int,pair<Ice::Long, Ice::Long> > > >::iterator it = temp_pool.begin(); it!= temp_pool.end();++it) {
    Ice::Long gid = it->first;
    Ice::Long sum_impr = 0;
    double sum_cost = 0;

    map<Ice::Long,map<int,pair<Ice::Long, Ice::Long> > >& zone_map = it->second;
    for(map<Ice::Long,map<int,pair<Ice::Long, Ice::Long> > >::iterator itz = zone_map.begin(); itz != zone_map.end(); ++itz) {
      map<int,pair<Ice::Long, Ice::Long> >& pos_map = itz->second;

      for(map<int,pair<Ice::Long, Ice::Long> >::iterator itp = pos_map.begin(); itp != pos_map.end(); ++itp) {
        string key = GetKey(itz->first,itp->first);
        map<string,double>::iterator itc = click_discount_ratio_table.find(key);
        double ratio = 1.0;
        if(itc != click_discount_ratio_table.end()) {
          ratio = itc->second;
        }else{
          continue;
        }
        sum_impr += itp->second.first;
        sum_cost += itp->second.second * ratio;
      }
    }
    double ecpm = sum_cost / (double) sum_impr * 1000;
    MCE_INFO("Limited_pv-gid-" << gid << "-pv-" << sum_impr);
    if(sum_impr >= 100000) {
      res_pool[gid] = ecpm;
    }
  }
  MCE_INFO("DemoLoaderCache::ExpandEcpmByZone,stop:temp size:" << temp_pool.size() <<",res_pool size:" << res_pool.size());
}
void DemoLoaderCache::ExpandCtrByPosAndZone(map<string,map<Ice::Long,map<int,pair<Ice::Long, Ice::Long> > > >& temp_pool,AdQualityMap& res_pool,int type){
  MCE_INFO("DemoLoaderCache::ExpandCtrByPosAndZone,begin:temp size:" << temp_pool.size() <<",res_pool size:" << res_pool.size());

  map<string,double> click_discount_ratio_table_by_pos;
  click_discount_ratio_table_by_pos[pos_751] = 1;
  click_discount_ratio_table_by_pos[pos_752] = 1.920088852;
  click_discount_ratio_table_by_pos[pos_753] = 5.376857573;
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
  click_discount_ratio_table_by_zone[pos_751] = 1.233590330;
  click_discount_ratio_table_by_zone[pos_752] = 2.368603041;
  click_discount_ratio_table_by_zone[pos_753] = 6.632839508;
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
        string key = GetKey(itz->first,itp->first);
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
          
          int pv_th = GetPvThreshold(type);
          int click_th = GetClickThreshold(type);
          double impr_weight = (double) impr / (double) pv_th;
          double click_weight = (double) click / (double) click_th;
          double weight = impr_weight > click_weight ? impr_weight : click_weight;
          
          string c_key = GetKey(itz->first,itp->first);
          if(IsCredible(sum_impr_by_pos, sum_click_by_pos, type)){
            map<string,double>::iterator itc = click_discount_ratio_table_by_pos.find(c_key);
            double ratio = 1.0;
            if(itc != click_discount_ratio_table_by_pos.end()) {
              ratio = itc->second;
            }
            res_pool[key] = weight * ((double) click / (double) impr) + (1 - weight) * ((double) sum_click_by_pos / (double) (sum_impr_by_pos * ratio));
            //res_pool[key] = (double) sum_click_by_pos / (double) (sum_impr_by_pos * ratio);
          }else if(IsCredible(sum_impr, sum_click, type)){
            map<string,double>::iterator itc = click_discount_ratio_table_by_zone.find(c_key);
            double ratio = 1.0;
            if(itc != click_discount_ratio_table_by_zone.end()) {
              ratio = itc->second;
            }
            //res_pool[key] = (double) sum_click / (double) (sum_impr * ratio);
            res_pool[key] = weight * ((double) click / (double) impr) + (1 - weight) * ((double) sum_click / (double) (sum_impr * ratio));
            
          }
        }
      }
    }
  }
  MCE_INFO("DemoLoaderCache::ExpandCtrByPosAndZone,stop:temp size:" << temp_pool.size() <<",res_pool size:" << res_pool.size());
}

void DemoLoaderCache::ExpandCtrByPosAndZoneForAmplify(map<string,map<Ice::Long,map<int,pair<Ice::Long, Ice::Long> > > >& temp_pool,
    AdQualityMap& res_pool,map<Ice::Long,Ice::Long>& amplify_gids_price){
  MCE_INFO("DemoLoaderCache::ExpandCtrByPosAndZoneForAmplify,begin:temp size:" << temp_pool.size() <<",res_pool size:" << res_pool.size());

  map<string,double> click_discount_ratio_table_by_pos;
  click_discount_ratio_table_by_pos[pos_751] = 1;
  click_discount_ratio_table_by_pos[pos_752] = 1.920088852;
  click_discount_ratio_table_by_pos[pos_753] = 5.376857573;
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
  click_discount_ratio_table_by_zone[pos_751] = 1.233590330;
  click_discount_ratio_table_by_zone[pos_752] = 2.368603041;
  click_discount_ratio_table_by_zone[pos_753] = 6.632839508;
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
    Ice::Long group_id = boost::lexical_cast<Ice::Long>(tmp[1]);
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
        string key = GetKey(itz->first,itp->first);
        map<string,double>::iterator itc = click_discount_ratio_table_by_pos.find(key);
        double ratio = 1.0;
        if(itc != click_discount_ratio_table_by_pos.end()) {
          ratio = itc->second;
        }else{
          continue;
        }
        sum_impr_by_pos += itp->second.first;

        sum_click_by_pos += itp->second.second * ratio ;
        itc = click_discount_ratio_table_by_zone.find(key);
        if(itc != click_discount_ratio_table_by_zone.end()) {
          ratio = itc->second;
        }else{
          continue;
        }
        sum_impr += itp->second.first;
        sum_click += itp->second.second * ratio ;
      }
      gid_map[itz->first] = make_pair(sum_impr_by_pos,sum_click_by_pos);
    }

    /*double ctr = 0.0;
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
    }*/
    double price = amplify_gids_price[group_id];
    for(map<Ice::Long,map<int,pair<Ice::Long, Ice::Long> > >::iterator itz = zone_map.begin(); itz != zone_map.end(); ++itz) {
      map<int,pair<Ice::Long, Ice::Long> >& pos_map = itz->second;
      Ice::Long zone_id = itz->first;

      for(map<int,pair<Ice::Long, Ice::Long> >::iterator itp = pos_map.begin(); itp != pos_map.end(); ++itp) {
        Ice::Long impr = itp->second.first;
        Ice::Long click = itp->second.second;
        int pos_id = itp->first;
        double ctr = 0.0;
        if(impr > 0) {
          ctr = (double)click / (double)impr;
        }

        double ecpm = price * ctr;

        map<Ice::Long, pair<Ice::Long,double> >::iterator it =  ecpm_table.find(zone_id * 10 + pos_id);
        double local_ecpm_thr = 0.0;
        if (it!= ecpm_table.end() && (*it).second.first > 0){
          local_ecpm_thr = (*it).second.second / (*it).second.first;
        }
        local_ecpm_thr = local_ecpm_thr * AdDemoConfig::instance().GetFreshEcpmRatio();

        string key = GetKey(zone_id, spid, group_id, pos_id, gender, stage);
        if (res_pool.find(key) != res_pool.end()){
          MCE_DEBUG("DemoLoaderCache::ExpandCtrByPosAndZoneForAmplify The ad has got enough impr. Load no fresh ad with low thr,gid="
              << group_id << " impr=" << impr << " click=" << click << " ecpm=" << ecpm << " ecpm thr=" << local_ecpm_thr
              << " ecpm_ratio="<< AdDemoConfig::instance().GetFreshEcpmRatio() << " at zone=" << zone_id << " pos=" << pos_id);
          continue;
        }
        if(IsFreshCredible(impr, click, zone_id, pos_id ,ecpm,local_ecpm_thr)) {
          res_pool[key] = (double) click / (double) impr;
          MCE_INFO("DemoLoaderCache::ExpandCtrByPosAndZoneForAmplify Load fresh ad with low thr by self_pos,gid=" << group_id << " impr=" << impr
              << " click=" << click << " ecpm=" << ecpm << " ecpm thr=" << local_ecpm_thr
              << " ecpm_ratio="<< AdDemoConfig::instance().GetFreshEcpmRatio() << " at zone=" << zone_id << " pos=" << pos_id);
        } else {
          Ice::Long sum_impr_by_pos = 0;
          double sum_click_by_pos = 0;
          map<Ice::Long,pair<Ice::Long, double> >::iterator itg = gid_map.find(zone_id);
          if(itg != gid_map.end()){
            sum_impr_by_pos = itg->second.first;
            sum_click_by_pos = itg->second.second;
          }
          if(sum_impr_by_pos > 0) {
            ctr = (double)sum_click_by_pos / (double)sum_impr_by_pos;
          }
          ecpm = price * ctr;
          string c_key = GetKey(zone_id,pos_id);
          ///////new add
          map<string,double>::iterator itc = click_discount_ratio_table_by_pos.find(c_key);
          double ratio = 1.0;
          if(itc != click_discount_ratio_table_by_pos.end()) {
            ratio = itc->second;
          }
          ecpm = ecpm /ratio;
          sum_click_by_pos = sum_click_by_pos /ratio;
          ////////new add
          if(IsFreshCredible(sum_impr_by_pos, sum_click_by_pos, zone_id, pos_id ,ecpm,local_ecpm_thr)) {
            /////new cancle
//            map<string,double>::iterator itc = click_discount_ratio_table_by_pos.find(c_key);
//            double ratio = 1.0;
//            if(itc != click_discount_ratio_table_by_pos.end()) {
//              ratio = itc->second;
//            }
            res_pool[key] = (double) sum_click_by_pos / (double) (sum_impr_by_pos * ratio);
            MCE_INFO("DemoLoaderCache::ExpandCtrByPosAndZoneForAmplify Load fresh ad with low thr by all_pos,gid=" << group_id << " impr=" << sum_impr_by_pos
                << " click=" << sum_click_by_pos << " ecpm=" << ecpm << " ecpm thr=" << local_ecpm_thr
                << " ecpm_ratio="<< AdDemoConfig::instance().GetFreshEcpmRatio() << " at zone=" << zone_id << " pos=" << pos_id);
          //}else if(IsCredible(sum_impr, sum_click, type)){
          }else {
            if(sum_impr > 0) {
              ctr = (double) sum_click / (double) sum_impr;
            }
            ///////////new add
            ecpm =(double) price * (double)ctr;
            double ratio = 1.0;
            if(itc != click_discount_ratio_table_by_zone.end()) {
              ratio = itc->second;
            }
            ecpm = ecpm /ratio;
            sum_click = sum_click / ratio;
            /////////new add
            if(IsFreshCredible(sum_impr, sum_click, zone_id, pos_id ,ecpm,local_ecpm_thr)){
              /////new cancle
//              map<string,double>::iterator itc = click_discount_ratio_table_by_zone.find(c_key);
//              double ratio = 1.0;
//              if(itc != click_discount_ratio_table_by_zone.end()) {
//                ratio = itc->second;
//              }
              res_pool[key] = (double) sum_click / (double) (sum_impr * ratio);
              MCE_INFO("DemoLoaderCache::ExpandCtrByPosAndZoneForAmplify Load fresh ad with low thr by all_zone,gid=" << group_id << " impr=" << sum_impr
                  << " click=" << sum_click << " ecpm=" << ecpm << " ecpm thr=" << local_ecpm_thr
                  << " ecpm_ratio="<< AdDemoConfig::instance().GetFreshEcpmRatio() << " at zone=" << zone_id << " pos=" << pos_id);
            }
          }
        }
      }
    }
  }
  MCE_INFO("DemoLoaderCache::ExpandCtrByPosAndZoneForAmplify,stop:temp size:" << temp_pool.size() <<",res_pool size:" << res_pool.size());
}
void DemoLoaderCache::LoadPool() {
  AdQualityMap res_pool;

  MCE_INFO("[DemoLoaderCache]::try to load.LoadPool begin");
  MyUtil::Date begin = MyUtil::Date::seconds(time(NULL));
  string begin_time = begin.str("%Y-%m-%d %H:%M:%S");
  Ice::Long begin_time_pos = GetDay(begin.year(),begin.month(),begin.day());
  MyUtil::Date end = MyUtil::Date::seconds(time(NULL) - DAYCOUNT * HOURCOUNT * 3600);
  Ice::Long end_time_pos = GetDay(end.year(),end.month(),end.day());
  MyUtil::Date state_2_end = MyUtil::Date::seconds(time(NULL) - 2 * HOURCOUNT * 3600);
  string end_time = end.str("%Y-%m-%d %H:%M:%S");
  string state_2_end_time = state_2_end.str("%Y-%m-%d %H:%M:%S");
  Ice::Long state_2_end_time_pos = GetDay(state_2_end.year(),state_2_end.month(),state_2_end.day());
  map<Ice::Long , Ice::Long> gid_spid_map;

  try {
    const char* db_host = AdDemoConfig::instance().db_host().c_str();
    const char* db_name = AdDemoConfig::instance().db_name().c_str();
    //MCE_INFO("DemoLoaderCache::db_host config: " << db_host << ", again : " << AdDemoConfig::instance().db_host());
    Connection conn(db_name,db_host,"ad","rrad");
    MCE_INFO("DemoLoaderCache::LoadPool,db host:" << AdDemoConfig::instance().db_host()<< ",db_name:" << db_name);
    stringstream sql;
    {
        Ice::Long max_time = begin_time_pos;
        max_time =(((max_time * 100 + begin.hour()) * 100 + begin.minute())* 100 + begin.second());
        MCE_INFO("[DemoLoaderCache]::LoadPool begin,start time:"<< max_time <<",begin time:" << begin_time);
        res_pool["max_time"] = (double) max_time;
    }

    {/****************no_gender*****************************/
      sql.str("");
      sql
        << "select adzone_id, member_id, adgroup_id as group_id, abs_pos, stage, sum(pv_count) as sum_impr, sum(click_count) as sum_click from "
        " rt_DZPMGGS where time <= " << begin_time_pos << " and time >= "
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
        gid_spid_map[group_id] = sponsor_id;
        string key = GetKey(zone_id,sponsor_id, group_id,pos_id,gender,stage);
        if(IsCredible(impr, click, no_gender)) {
          res_pool[key] = (double) click / (double) impr;
        }
      }
    }
    /*******************no_stage*************************************/  
    {
      sql.str("");
      sql << "select adzone_id,member_id, adgroup_id as group_id, abs_pos, gender, sum(pv_count) as sum_impr, sum(click_count) as sum_click from rt_DZPMGGS  where time <= " << begin_time_pos  << " and time >=" << end_time_pos << " " <<" group by adzone_id, adgroup_id, abs_pos, gender ";
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
      sql << "select adzone_id,member_id, abs_pos, gender,stage, sum(pv_count) as sum_impr, sum(click_count) as sum_click from rt_DZPMGGS  where time <= " << begin_time_pos  << " and time >=" << end_time_pos << "" <<" group by adzone_id, member_id, abs_pos, stage,gender ";
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
      sql << "select adzone_id,member_id, adgroup_id as group_id, abs_pos, gender, stage, sum(pv_count) as sum_impr, sum(click_count) as sum_click from rt_DZPMGGS  where time <= " << begin_time_pos  << " and time >=" << end_time_pos << "" <<" group by adzone_id , adgroup_id , abs_pos , gender,stage ";
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
      sql << "select adzone_id,member_id, adgroup_id as group_id, abs_pos, sum(pv_count) as sum_impr, sum(click_count) as sum_click from rt_DZPMGGS  where time <= " << begin_time_pos  << " and time >=" << state_2_end_time_pos << "" << " group by adzone_id, adgroup_id ,abs_pos ";
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

    MCE_INFO("[DemoLoaderCache]:: stop demo_pool size is " << res_pool.size());

  } catch (std::exception& e) {
    MCE_WARN("LoadByTime --> load demo pool error: " << e.what());
  } catch (...) {
    MCE_WARN("LoadByTime --> load demo pool error!");
  }

  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    demo_pool.swap(res_pool);
    MCE_INFO("[DemoLoaderCache]::LoadPool stop  size is " << demo_pool.size());
  }

}

void DemoLoaderCache::LoadAppPool() {
  AdQualityMap local_app_pool;

  MCE_INFO("[DemoLoaderCache]::try to load.LoadAppPool begin");
  MyUtil::Date begin = MyUtil::Date::seconds(time(NULL));
  string begin_time = begin.str("%Y-%m-%d %H:%M:%S");
  Ice::Long begin_time_pos = GetDay(begin.year(),begin.month(),begin.day());
  MyUtil::Date end = MyUtil::Date::seconds(time(NULL) - DAYCOUNT * HOURCOUNT * 3600);
  Ice::Long end_time_pos = GetDay(end.year(),end.month(),end.day());
  MyUtil::Date state_2_end = MyUtil::Date::seconds(time(NULL) - 2 * HOURCOUNT * 3600);
  string end_time = end.str("%Y-%m-%d %H:%M:%S");
  string state_2_end_time = state_2_end.str("%Y-%m-%d %H:%M:%S");
  Ice::Long state_2_end_time_pos = GetDay(state_2_end.year(),state_2_end.month(),state_2_end.day());
  map<Ice::Long , Ice::Long> gid_spid_map;

  try {
    const char* db_host = AdDemoConfig::instance().db_host().c_str();
    const char* db_name = AdDemoConfig::instance().db_name().c_str();
    //MCE_INFO("DemoLoaderCache::db_host config: " << db_host << ", again : " << AdDemoConfig::instance().db_host());
    Connection conn(db_name,db_host,"ad","rrad");
    MCE_INFO("DemoLoaderCache::LoadAppPool,db host:" << AdDemoConfig::instance().db_host()<< ",db_name:" << db_name);
    stringstream sql;

    /*************************Load App Data**************************/
    /*************************************app two feature**********************************/
   // Connection app_conn("test","10.3.16.119","ad","rrad");
    map<string,pair<Ice::Long,Ice::Long> > app_spid_pool;
    {
      sql.str("");
      //sql << "select app_name , sponsor_id, group_id, pos_id, gender, stage, sum(impr) as sum_impr, sum(click) as sum_click from app_demographic_ctr where time <= '" << begin_time  << "' and time >='" << end_time << "'" <<" group by app_name , group_id , pos_id , gender,stage ";
      sql << "select app as app_name, member_id as sponsor_id, adgroup_id as group_id, abs_pos as pos_id, gender, stage, "
          "sum(pv_count) as sum_impr, sum(click_count) as sum_click "
          "from Log_DABGGSD where  time >= " << end_time_pos <<" group by app , adgroup_id , abs_pos , gender,stage ";
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
        /*Ice::Long sponsor_id = 0;
        map<Ice::Long, Ice::Long>::iterator itg = gid_spid_map.find(group_id);
        if(itg != gid_spid_map.end()) {
          sponsor_id = itg->second;
        }*/
        Ice::Int gender = (int) row["gender"];
        Ice::Int stage = (Ice::Int) row["stage"];
        Ice::Long impr = (Ice::Long) row["sum_impr"];
        Ice::Long click = (Ice::Long) row["sum_click"];
        string key = GetKey(zone_name,sponsor_id, group_id,pos_id,gender,stage);
        if(IsCredible(impr, click, app_two_feature)) {
          local_app_pool[key] = (double) click / (double) impr;
        }
        ///////////////////////////////////////////////
        key = GetKey(zone_name,sponsor_id, NO_USE,pos_id,gender,stage);
        map<string,pair<Ice::Long, Ice::Long> >::iterator it = app_spid_pool.find(key);
        if(it != app_spid_pool.end()) {
          it->second.first += impr;
          it->second.second += click;
        } else {
          app_spid_pool[key] = make_pair(impr,click);
        }


      }
    }    
    MCE_INFO("[DemoLoaderCache]:: app_pool after two feature size is " << local_app_pool.size());
    /*****************************app spid_type***********************************************/
    {
      sql.str("");
      //sql << "select app_name,sponsor_id, pos_id, gender,stage, sum(impr) as sum_impr, sum(click) as sum_click from app_demographic_ctr where time <= '" << begin_time  << "' and time >='" << end_time << "'" <<" group by app_name, sponsor_id, pos_id, stage,gender "; 
     sql << "select app as app_name, member_id as sponsor_id,abs_pos as pos_id, gender, stage, sum(pv_count) as sum_impr, "
          " sum(click_count) as sum_click from Log_DABGGSD where  time >= " << end_time_pos
          <<" group by app, member_id, abs_pos, stage,gender ";
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
    MCE_INFO("[DemoLoaderCache]:: app_pool after spid size is " << local_app_pool.size());
    /****************app no_gender*****************************/
    {
      sql.str("");
      //sql << "select app_name,sponsor_id, group_id, pos_id, stage, sum(impr) as sum_impr, sum(click) as sum_click from app_demographic_ctr where time <= '" << begin_time  << "' and time >='" << end_time << "'" <<" group by app_name,group_id,pos_id,stage ";
      sql << "select app as app_name, member_id as sponsor_id, adgroup_id as group_id, abs_pos as pos_id, stage, sum(pv_count) as sum_impr, "
          " sum(click_count) as sum_click from Log_DABGGSD where time >= " << end_time_pos <<" group by app, adgroup_id, abs_pos,stage ";
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
    MCE_INFO("[DemoLoaderCache]:: app_pool after stage size is " << local_app_pool.size());
    /*******************app no_stage*************************************/  
    {
      sql.str("");
      //sql << "select app_name,sponsor_id, group_id, pos_id, gender, sum(impr) as sum_impr, sum(click) as sum_click from app_demographic_ctr where time <= '" << begin_time  << "' and time >='" << end_time << "'" <<" group by app_name, group_id, pos_id, gender ";
      sql << "select app as app_name,member_id as sponsor_id, adgroup_id as group_id, abs_pos as pos_id, gender, "
          "sum(pv_count) as sum_impr, sum(click_count) as sum_click "
          "from Log_DABGGSD where  time >= " << end_time_pos  <<" group by app, adgroup_id, abs_pos, gender ";
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
    MCE_INFO("[DemoLoaderCache]:: app_pool after gender size is " << local_app_pool.size());
    /******************************app_all**********************************/
    {
      sql.str("");
      //sql << "select app_name,sponsor_id, group_id, pos_id, sum(impr) as sum_impr, sum(click) as sum_click from app_demographic_ctr  where time <= '" << begin_time  << "' and time >='" << state_2_end_time << "'" << " group by app_name, group_id ,pos_id ";
      sql << "select app as app_name, member_id as sponsor_id, adgroup_id as group_id, abs_pos as pos_id, "
          "sum(pv_count) as sum_impr, sum(click_count) as sum_click from Log_DABGGSD "
          " where time >= " << state_2_end_time_pos << " group by app , adgroup_id ,abs_pos ";
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

  } catch (std::exception& e) {
    MCE_WARN("LoadByTime --> load App error: " << e.what());
  } catch (...) {
    MCE_WARN("LoadByTime --> load App error!");
  }

  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    app_pool.swap(local_app_pool);
    MCE_INFO("[DemoLoaderCache]::LoadAppPool stop  size is " << app_pool.size());
  }

}

bool SortFunc(const pair<int,double> & x1,const pair<int,double> & x2){
  return x1.second > x2.second;
}
void DemoLoaderCache::LoadAppSortPos(){
  MCE_INFO("[DemoLoaderCache]::try to load.");

  MyUtil::Date begin = MyUtil::Date::seconds(time(NULL) - 3 * 24 * 3600);
  Ice::Long begin_pos = GetDay(begin.year(),begin.month(),begin.day());

  AdSortPosMap swap_sort_pos;
  map<string, vector<pair<int,double> > > app_pos_ctr;

  try{
    const char* db_host = AdDemoConfig::instance().db_host().c_str();
    const char* db_name = AdDemoConfig::instance().db_name().c_str();
    //MCE_INFO("DemoLoaderCache::db_host config: " << db_host << ", again : " << AdDemoConfig::instance().db_host());
    Connection app_conn(db_name,db_host,"ad","rrad");
    MCE_INFO("DemoLoaderCache::LoadAppSortPos,db host:" << AdDemoConfig::instance().db_host()<< ",db_name:" << db_name);

    vector<pair<int,double> > sort_pos;

    stringstream sql;
    {
      //sql << "select app_name , pos_id, sum(impr) as sum_impr, sum(click) as sum_click from app_pos_data where time <= '" << begin_time  << "' and time >='" << end_time << "'" <<" group by app_name , pos_id ";
      sql << "select app as app_name , abs_pos as pos_id, sum(pv_count) as sum_impr, sum(click_count) as sum_click from Log_DABGGSD "
          " where time >= "  << begin_pos  <<" group by app_name , pos_id ";
      MCE_INFO("try to excute " << sql.str());
      string s= sql.str();
      Query q(&app_conn,true,s.c_str());
      mysqlpp::StoreQueryResult res = q.store();
      if (!res || res.num_rows() <= 0) {
        MCE_WARN("DemoLoaderCache Load AppSortPos Error " << sql.str() << ".  select  fail!");
      }
      MCE_INFO("[DemoLoaderCache]::LoadAppSortPos res size : " << res.num_rows());
      for (size_t i = 0; i < res.num_rows(); ++i) {
        mysqlpp::Row row = res.at(i);
        string app_name = (string) row["app_name"];
        //MCE_INFO("app name is  " << app_name);
        int pos_id = (int) row["pos_id"];
        if(pos_id > 0) {
          //MCE_INFO("pos id is  " << pos_id);
          Ice::Long impr = (Ice::Long) row["sum_impr"];
          Ice::Long click = (Ice::Long) row["sum_click"];
          if(impr > 80000 || click >12) {
            double ctr = 0;
            if (impr > 0){
              ctr = click * 1.0 / impr;
            }
            map<string,vector<pair<int, double> > > ::iterator ita = app_pos_ctr.find(app_name);
            if(ita != app_pos_ctr.end()) {
              ita->second.push_back(make_pair(pos_id, ctr));
            } else {
              vector<pair<int,double> > vec;
              vec.push_back(make_pair(pos_id, ctr));
              app_pos_ctr[app_name] = vec;
            }
          }
        }
      }
    }
    MCE_INFO("[DemoLoaderCache]:: LoadAppSortPos app sort pos size is " << app_pos_ctr.size());
  } catch (std::exception& e) {
    MCE_WARN("LoadAppSortPos error: " << e.what());
  } catch (...) {
    MCE_WARN("LoadAppSortPos error!");
  }
  for(map<string, vector<pair<int,double> > >::iterator it = app_pos_ctr.begin(); it != app_pos_ctr.end();++it) {
    vector<pair<int,double> > &sort_pos = it->second;
    vector<int> pos_seq;
    if(sort_pos.size() == 3) { //三个位置上都有数据
      sort(sort_pos.begin(),sort_pos.end(),SortFunc);
      for(vector<pair<int,double> >::iterator its = sort_pos.begin(); its != sort_pos.end(); ++ its) {
        pos_seq.push_back((*its).first);
      }
    } else { //数据不够，则采用默认顺序
      pos_seq.push_back(1);
      pos_seq.push_back(2);
      pos_seq.push_back(3);
    }
    swap_sort_pos[it->first] = pos_seq;
  }

  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    app_sort_pos.swap(swap_sort_pos);
  }


}

}
}

