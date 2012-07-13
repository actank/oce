/*
 * FreshAdTesting.cpp
 *
 *  Created on: 2011-6-22
 *      Author: sunzz
 */
#include <connection.h>
#include <query.h>
#include <string>
#include <algorithm>
#include "AdConfig.h"
#include "QueryRunner.h"
#include "AdQualityUtil.h"
#include "FreshAdTesting.h"
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/regex.hpp>
using namespace boost;
namespace xce{
namespace ad{
using namespace mysqlpp;

Ice::Long GetTimeKey(Ice::Long time) {
  return time / 100;
}
void FreshAdGroup::init() {
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
}
void FreshAdGroup::calculate_display_count() {
  Ice::Long audit_time_key = GetTimeKey(audit_time_);
  map<Ice::Long, map<string,pair<Ice::Long,Ice::Long> > >::iterator it = time_zone_pos_pv_click_.begin();
  for(; it != time_zone_pos_pv_click_.end(); ++it) {
    if(it->first >= audit_time_key) {
      map<string, pair<Ice::Long, Ice::Long> > &zone_map = it->second;
      for(map<string, pair<Ice::Long, Ice::Long> >::iterator itz = zone_map.begin(); itz != zone_map.end(); ++itz) {
        //if(group_id_ == 100000015511){
        //  MCE_DEBUG("debug:gid:" << group_id_ << ",time:" << it->first
        //      << ",zone_key:" << itz->first << ",impr:" << itz->second.first <<",click:" <<itz->second.second);
        //}
        add_pv_click(itz->first, itz->second.first, itz->second.second);
      }
    }
  }
}
void FreshAdGroup::calculate_weight() {
  Ice::Long sum_impr = 0;
  double sum_click = 0;
  double ctr = average_ctr;
  for(map<string,pair<Ice::Long,Ice::Long> >::iterator it =  zone_pos_pv_click_.begin(); it != zone_pos_pv_click_.end(); ++it) {
    double ratio = 1.0;
    map<string,double>::iterator itc = click_discount_ratio_table.find(it->first);
    if(itc != click_discount_ratio_table.end()) {
      ratio = itc->second;
    }
    sum_impr += it->second.first;
    sum_click += it->second.second * ratio;
  }
  if(sum_impr > pv_limit_count) {
    is_test_over_ = true;
  }
  if(sum_impr > 0 && sum_click > 0) {
    ctr = double (sum_click) /double(sum_impr);
  } else if(sponsor_ctr_ > 0) {
    ctr = sponsor_ctr_;
  } else if(sum_impr > 10000) {
    ctr *= 0.8;
  }
  weight_ = ctr * (double) price_;
}
void FreshAdGroup::print_display_count() {
  ostringstream os;
  ostringstream oo;
  for(map<string, pair<Ice::Long,Ice::Long> >::iterator itz = zone_pos_pv_click_.begin();
      itz != zone_pos_pv_click_.end(); ++itz) {
    oo <<" " <<itz->first.substr(10,3) <<" " << itz->second.first << " " <<itz->second.second;
  }
  MCE_INFO("print_display_count gid:" <<group_id_ <<",audit_time:" << audit_time_<< ",sponsor_id:" << sponsor_id_ <<",sponsor ctr:" <<sponsor_ctr_
      <<",price:" <<price_ <<",is_test_over:" << is_test_over_<<" predict weight:" << weight_
      << " orign data:" << oo.str());
}

void FreshAdCache::LoadFreshAdMap(FreshAdQualityMap& fresh_ad_pool){
  IceUtil::RWRecMutex::RLock lock(mutex_);
  fresh_ad_pool = fresh_ad_pool_;
}

void FreshAdCache::LoadFreshAdPool() {

  MCE_INFO("FreshAdCache::LoadFreshData begin");
  FreshAdQualityMap fresh_ad_pool;
  //加载审核过的在线新广告
  LoadAuditedOnlineFreshAd();
  //加载广告的sponsor ctr
  LoadAdSponsorCtr();
  //加载新广告的每天的历史投放情况
  LoadFreshAdData();
  //计算新广告的总投放情况
  CalculateDisplayCount();
  //计算新广告的投放权重
  CalculateWeight();
  //打印新广告的信息
  PrintDisplayCount();
  //分配新广告的投放，根据ecpm
  AssignFreshAdImprRatio(fresh_ad_pool);
  //清空计算中间体
  Clear();

  IceUtil::RWRecMutex::WLock lock(mutex_);
  fresh_ad_pool_.swap(fresh_ad_pool);
  MCE_INFO("DemoLoaderCache::LoadFreshData stop");
}

void FreshAdCache::LoadAuditedOnlineFreshAd(){ //加载审核过的在线新广告
  MyUtil::Date begin_day = MyUtil::Date::seconds(time(NULL) - duration_day * 24 * 3600);  //10天的数据
  string begin_time = begin_day.str("%Y-%m-%d %H");
  /*{
    Statement sql;
    stringstream sql_string;//加载10天之内的在线新广告
    sql_string << "select adgroup_id as group_id, member_id, max_price,create_time from adgroup where create_time >='"
        << begin_time << "' AND  member_status = 1 AND am_status = 1 AND audit_status = 1 "
        "AND delete_flag= 1 AND display_type = 4 AND daily_sys_status = 1 AND total_sys_status = 1";
    sql << sql_string.str();
    MCE_INFO("FreshAdCache::LoadAuditedOnlineFreshAd try to execute:" << sql_string.str());
    try {
      mysqlpp::StoreQueryResult res = QueryRunner("ade_db", CDbRServer, "adgroup").store(sql);
      if (!res || res.num_rows() < 0){
        MCE_WARN("FreshAdCache::LoadAuditedOnlineFreshAd error");
        return;
      }
      else if (res.num_rows() == 0) {
        MCE_INFO("FreshAdCache::LoadAuditedOnlineFreshAd.NO create NEW ADS");
        return;
      }
      MCE_INFO("FreshAdCache::LoadAuditedOnlineFreshAd. create Fresh Size=: " << res.num_rows());
      //stringstream fresh_ad_list;
      for (size_t i = 0; i < res.num_rows(); ++i) {
        mysqlpp::Row row = res.at(i);
        Ice::Long group_id = (Ice::Long) row["group_id"];
        Ice::Long member_id = (Ice::Long) row["member_id"];
        Ice::Long max_price = (Ice::Long)row["max_price"];
        FreshAdGroupPtr f_ptr = new FreshAdGroup(group_id, member_id, max_price);
        string create_time = (string) row["create_time"];
        Ice::Long time = TimeForm2Long(create_time);
        f_ptr->set_create_time(time);
        adgroup_map_[group_id] = f_ptr;
        sponsor_set_.insert(member_id);

      }
    }catch (std::exception& e) {
      MCE_WARN("FreshAdCache::LoadAuditedOnlineFreshAd error: " << e.what());
    } catch (...) {
      MCE_WARN("FreshAdCache::LoadAuditedOnlineFreshAd error!");
    }
  }
  ///////////////////////////////////////////////////////////////////////////////////////
  MCE_INFO("FreshAdCache::LoadAuditedOnlineFreshAd load over, new create ads size:" << adgroup_map_.size());
  try {
    Connection conn(db_jebe_name,db_jebe_host,"ad","adjb###");
    stringstream sql;
    sql << "select adgroup_id as group_id, audit_time from audit_his where audit_time >='" <<
        begin_time << "' AND  audit_status  = 1";
    MCE_INFO("FreshAdCache::LoadAuditedOnlineFreshAd try to excute " << sql.str());
    string s= sql.str();
    Query q(&conn,true,s.c_str());
    mysqlpp::StoreQueryResult res = q.store();
    if (!res) {
      MCE_WARN("FreshAdCache::LoadAuditedOnlineFreshAd Error " << sql.str() << ".  select  fail!");
      return;
    }
    MCE_INFO("FreshAdCache::LoadAuditedOnlineFreshAd,audit gids Size=: " << res.num_rows());
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      Ice::Long group_id = (Ice::Long) row["group_id"];
      string audit_time = (string) row["audit_time"];
      map<Ice::Long, FreshAdGroupPtr>::iterator itf = adgroup_map_.find(group_id);
      if(itf != adgroup_map_.end()) {
        Ice::Long time = TimeForm2Long(audit_time);
        itf->second->set_audit_time(time);
      }
    }
  }catch (std::exception& e) {
    MCE_WARN("FreshAdCache::LoadAuditedOnlineFreshAd audit error: " << e.what());
  } catch (...) {
    MCE_WARN("FreshAdCache::LoadAuditedOnlineFreshAd audit error!");
  }*/
  set<Ice::Long> audit_gids;
  map<Ice::Long, Ice::Long> audit_gids_timekey;
  try {
    const char* db_jebe_host = AdDemoConfig::instance().db_jebe_host().c_str();
    const char* db_jebe_name = AdDemoConfig::instance().db_jebe_name().c_str();
    Connection conn(db_jebe_name,db_jebe_host,"ad","adjb###");
    stringstream sql;
    sql << "select adgroup_id as group_id, audit_time from ad_audit_his where audit_time >='" <<
        begin_time << "' AND  audit_status  = 1";
    MCE_INFO("FreshAdCache::LoadAuditedOnlineFreshAd try to excute " << sql.str());
    string s= sql.str();
    Query q(&conn,true,s.c_str());
    mysqlpp::StoreQueryResult res = q.store();
    if (!res) {
      MCE_WARN("FreshAdCache::LoadAuditedOnlineFreshAd Error " << sql.str() << ".  select  fail!");
      return;
    }
    MCE_INFO("FreshAdCache::LoadAuditedOnlineFreshAd,audit gids Size=: " << res.num_rows());
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      Ice::Long group_id = (Ice::Long) row["group_id"];
      string audit_time = (string) row["audit_time"];
      Ice::Long timekey = TimeForm2Long(audit_time);
      audit_gids.insert(group_id);
      audit_gids_timekey[group_id] = timekey;
    }
  }catch (std::exception& e) {
    MCE_WARN("FreshAdCache::LoadAuditedOnlineFreshAd audit error: " << e.what());
  } catch (...) {
    MCE_WARN("FreshAdCache::LoadAuditedOnlineFreshAd audit error!");
  }
  {
    string fresh_list = TransformForSql(audit_gids);
    Statement sql;
    stringstream sql_string;//加载广告数据
    sql_string << "select adgroup_id as group_id, member_id, max_price,create_time from adgroup where adgroup_id in "
        << fresh_list << " AND  member_status = 1 AND am_status = 1 AND audit_status = 1 "
        "AND delete_flag= 1 AND display_type = 4 AND daily_sys_status = 1 AND total_sys_status = 1";
    sql << sql_string.str();
    MCE_INFO("FreshAdCache::LoadAuditedOnlineFreshAd try to execute:" << sql_string.str());
    try {
      mysqlpp::StoreQueryResult res = QueryRunner("ade_db", CDbRServer, "adgroup").store(sql);
      if (!res || res.num_rows() < 0){
        MCE_WARN("FreshAdCache::LoadAuditedOnlineFreshAd error");
        return;
      }
      else if (res.num_rows() == 0) {
        MCE_INFO("FreshAdCache::LoadAuditedOnlineFreshAd.NO create NEW ADS");
        return;
      }
      MCE_INFO("FreshAdCache::LoadAuditedOnlineFreshAd. create Fresh Size=: " << res.num_rows());
      //stringstream fresh_ad_list;
      for (size_t i = 0; i < res.num_rows(); ++i) {
        mysqlpp::Row row = res.at(i);
        Ice::Long group_id = (Ice::Long) row["group_id"];
        Ice::Long member_id = (Ice::Long) row["member_id"];
        Ice::Long max_price = (Ice::Long)row["max_price"];
        FreshAdGroupPtr f_ptr = new FreshAdGroup(group_id, member_id, max_price);
        string create_time = (string) row["create_time"];
        Ice::Long time = TimeForm2Long(create_time);
        f_ptr->set_create_time(time);
        Ice::Long audit_time_key = 0;
        map<Ice::Long, Ice::Long>::iterator it =  audit_gids_timekey.find(group_id);
        if(it != audit_gids_timekey.end()) {
          audit_time_key = it->second;
        }
        f_ptr->set_audit_time(audit_time_key);
        adgroup_map_[group_id] = f_ptr;
        sponsor_set_.insert(member_id);

      }
    }catch (std::exception& e) {
      MCE_WARN("FreshAdCache::LoadAuditedOnlineFreshAd error: " << e.what());
    } catch (...) {
      MCE_WARN("FreshAdCache::LoadAuditedOnlineFreshAd error!");
    }
  }

  MCE_INFO("FreshAdCache::LoadAuditedOnlineFreshAd stop , online new create group:" << adgroup_map_.size());


}
void FreshAdCache::LoadFreshAdData(){
  MyUtil::Date begin_day = MyUtil::Date::seconds(time(NULL) - duration_day * 24 * 3600);  //10天的数据
  Ice::Long begin_time = GetDay(begin_day);
  mysqlpp::StoreQueryResult res;
  try {
    const char* db_host = AdDemoConfig::instance().db_host().c_str();
    const char* db_name = AdDemoConfig::instance().db_name().c_str();
    Connection conn(db_name,db_host,"ad","rrad");
    stringstream sql;
    sql.str("");
    sql << "select adzone_id, adgroup_id as group_id, abs_pos , time ,sum(pv_count) as sum_impr , sum(click_count) as sum_click  from "
        " rt_DZPMGGS  where  time >= " << begin_time << " group by adzone_id, abs_pos, adgroup_id, time";
    MCE_INFO("try to excute " << sql.str());
    string s= sql.str();
    Query q(&conn,true,s.c_str());
    //mysqlpp::StoreQueryResult res = q.store();
    res = q.store();
    if (!res || res.num_rows() <= 0) {
      MCE_INFO("FreshAdCache::LoadFreshAdData all gids is truely fresh ");
    }
  }catch (std::exception& e) {
    MCE_WARN("FreshAdCache::LoadFreshAdData error: " << e.what());
  } catch (...) {
    MCE_WARN("FreshAdCache::LoadFreshAdData error!");
  }

  MCE_INFO("FreshAdCache::LoadFreshAdData res Size=: " << res.num_rows());
  if (res && res.num_rows() > 0) {
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      Ice::Long zone_id = (Ice::Long) row["adzone_id"];
      Ice::Long group_id = (Ice::Long) row["group_id"];
      int pos_id = (int) row["abs_pos"];
      Ice::Long impr = (Ice::Long) row["sum_impr"];
      Ice::Long click = (Ice::Long) row["sum_click"];
      Ice::Long time = (Ice::Long) row["time"];

      map<Ice::Long, FreshAdGroupPtr>::iterator it = adgroup_map_.find(group_id);
      if(it != adgroup_map_.end()) {
//        Ice::Long time_key = GetTimeKey(time);
        string pos_key = GetKey(zone_id, pos_id);
        it->second->add_time_pv_click(time, pos_key, impr, click);
      }
    }
  }
}

void FreshAdCache::LoadAdSponsorCtr() {
  MyUtil::Date begin_day = MyUtil::Date::seconds(time(NULL) - duration_day * 24 * 3600);  //10天的数据
  Ice::Long begin_time = GetDay(begin_day);
  if(sponsor_set_.empty()) {
    return;
  }
  string fresh_list = TransformForSql(sponsor_set_);
  mysqlpp::StoreQueryResult res;
  try {
    const char* db_host = AdDemoConfig::instance().db_host().c_str();
    const char* db_name = AdDemoConfig::instance().db_name().c_str();
    Connection conn(db_name,db_host,"ad","rrad");
    stringstream sql;
    sql.str("");
    sql << "select member_id , sum(pv_count) as sum_impr , sum(click_count) as sum_click  from  rt_DZPMGGS  where  time >= " << begin_time
        << " AND  member_id  in " << fresh_list
        << " group by member_id";
    MCE_INFO("try to excute " << sql.str());
    string s= sql.str();
    Query q(&conn,true,s.c_str());
    //mysqlpp::StoreQueryResult res = q.store();
    res = q.store();
    if (!res || res.num_rows() <= 0) {
      MCE_INFO("FreshAdCache::LoadAdSponsorCtr all member gids is truely fresh ");
    }
  }catch (std::exception& e) {
    MCE_WARN("FreshAdCache::LoadAdSponsorCtr error: " << e.what());
  } catch (...) {
    MCE_WARN("FreshAdCache::LoadAdSponsorCtr error!");
  }
  MCE_INFO("FreshAdCache::LoadAdSponsorCtr sponsor Size=: " << res.num_rows());
  if (res && res.num_rows() > 0) {
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      Ice::Long sponsor_id = (Ice::Long) row["member_id"];
      Ice::Long impr = (Ice::Long) row["sum_impr"];
      Ice::Long click = (Ice::Long) row["sum_click"];
      if(impr > 0 && click > 0) {
        sponsor_map_[sponsor_id] = (double) click * 1.0 / (double) impr;
      }
    }
  }
  //设定sponsor ctr
  for(map<Ice::Long, FreshAdGroupPtr>::iterator it = adgroup_map_.begin(); it != adgroup_map_.end(); ++it) {
    map<Ice::Long, double>::iterator itf = sponsor_map_.find(it->second->sponsor_id());
    if(itf != sponsor_map_.end()) {
      if(itf->second > 0) {
        it->second->set_sponsor_ctr(itf->second);
      }
    }
  }
}
void FreshAdCache::CalculateDisplayCount() {
  for(map<Ice::Long, FreshAdGroupPtr>::iterator it = adgroup_map_.begin(); it != adgroup_map_.end(); ++it) {
    it->second->calculate_display_count();
  }
}
void FreshAdCache::CalculateWeight() {
  for(map<Ice::Long, FreshAdGroupPtr>::iterator it = adgroup_map_.begin(); it != adgroup_map_.end(); ++it) {
    it->second->calculate_weight();
  }
}
void FreshAdCache::PrintDisplayCount() {
  for(map<Ice::Long, FreshAdGroupPtr>::iterator it = adgroup_map_.begin(); it != adgroup_map_.end(); ++it) {
    it->second->print_display_count();
  }
}

void FreshAdCache::AssignFreshAdImprRatio(FreshAdQualityMap & fresh_pool) {
  map<Ice::Long, double > gid_weight;
  for(map<Ice::Long, FreshAdGroupPtr>::iterator it = adgroup_map_.begin(); it != adgroup_map_.end(); ++it) {
    if(!it->second->is_test_over()){
      double weight = it->second->weight();
      gid_weight[it->first] = weight * weight * weight; //扩大比例
      //gid_weight[it->first] = weight; //扩大比例
    }
  }
  fresh_pool["reserved"] = gid_weight;
}
}
}


