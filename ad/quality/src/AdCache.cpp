/*
 * AdCache.cpp
 *
 *  Created on: Nov 30, 2010
 *      Author: zhaohui.tang
 */

#include "AdCache.h"
#include "Date.h"
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

namespace xce {
namespace ad {

using namespace MyUtil;
using namespace std;
using namespace boost;

const string DB_INSTANCE = "ade_db";

/***********************************************************************************************************************/
bool PointsChecker::Check(const string & day_points, const string & hour_points) {
//  MCE_DEBUG("PointsChecker::Check --> day_points = " << day_points
//      << ",hour_points = " << hour_points);
  bool valid = CheckDay(day_points) && CheckHour(hour_points);
//  MCE_INFO("PointsChecker::Check --> valid = " << valid);
  return valid;
}

bool PointsChecker::CheckHour(const string & hour_point_str) {
  if (hour_point_str.empty()) {
    return true;
  }
  bitset < 24 > hour_points;
  try {
    hour_points = lexical_cast<int> (hour_point_str);
  } catch (...) {
    MCE_WARN("PointsChecker::CheckHour --> boost cast hour_point_str = "
        << hour_point_str << " error");
  }
  Date date = Date(time(NULL));
  int this_hour = date.hour();
  bool result = hour_points.test(this_hour);
//  MCE_DEBUG("PointsChecker::CheckHour --> this_hour = " << this_hour
//      << ", hours_points = " << hour_points << ",result = " << result);
  return result;
}

bool PointsChecker::CheckDay(const string & day_point_str) {
  if (day_point_str.empty()) {
    return true;
  }

  tokenizer<> tok(day_point_str);
  tokenizer<>::iterator tit = tok.begin();
  int year = lexical_cast<int> ((*(tit)).substr(0, 4));
  int month = lexical_cast<int> ((*(tit++)).substr(4, 2));
  map<int, bitset<32> > yearmonth_day;
  for (; tit != tok.end(); ++tit) {
    if ((*tit) != "0") {
      yearmonth_day[year * 100 + month] = lexical_cast<int> (*tit);
    }
    if (month == 12) {
      ++year;
      month = 1;
    } else {
      ++month;
    }
  }

  Date date = Date(time(NULL));
  int this_day = date.day();
  int this_month = date.month();
  int this_year = date.year();
  int yearmonth = this_year * 100 + this_month;
  map<int, bitset<32> >::iterator mit = yearmonth_day.find(yearmonth);
  if (mit != yearmonth_day.end()) {
    bool result = mit->second.test(this_day - 1); //0代表1号，以此类推
//    MCE_DEBUG("PointsChecker::CheckDay --> this year:month:day = " << this_year
//        << ":" << this_month << ":" << this_day << ",days = " << mit->second
//        << ",result " << result);
    return result;
  } else {
    return false;
  }
}

/***********************************************************************************************************************/
bool AdGroup::Available() {
  bool result = false;
  result = member_table_status_ && campaign_status_ && (member_status_ == 1)
      && (am_status_ == 1) && (audit_status_ == 1) && (delete_flag_ == 1)
      && (daily_sys_status_ == 1) && (total_sys_status_ == 1) && point_valid_ && ((max_price_ >= 100) || (member_id_ == 999));
//  MCE_DEBUG("~~~~~~~~~~~~~~~~~AdGroup::Available --> RESULT = " << result
//      << " group_id = " << group_id_ << " member_table_status_ = "
//      << member_table_status_ << " campain_statues_ = " << campaign_status_
//      << " member_status_ = " << member_status_ << " am_status_ = "
//      << am_status_ << " audit_status_ = " << audit_status_
//      << " delete_flag_ = " << delete_flag_ << " daily_sys_status_"
//      << daily_sys_status_ << " total_sys_status_ = " << total_sys_status_
//      << " point_valid_ = " << point_valid_ << " >> rotation_number = "
//      << rotation_number_);
  return result;
}

bool AdGroup::HasCreatives() {
  bool result = !(creative_ids_.empty());
  return result;
}

void AdGroup::UpdateCampainStatus() {
  AdCampaignPtr campaign_ptr = AdCampaignPool::instance().GetAdCampaign(campaign_id_);
  if (campaign_ptr) {
    campaign_status_ = campaign_ptr->Available();
  }
}

AdGroupPtr AdGroupPool::GetAdGroup(Ice::Long group_id) {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  AdGroupPtr ans = NULL;
  hash_map<Ice::Long, AdGroupPtr>::iterator it = group_pool_.find(group_id);
  if (it != group_pool_.end()) {
    ans = it->second;
  } else {
//    MCE_INFO("AdGroupPool::GetGetAdGroup --> miss gruop_id = " << group_id);
  }
  return ans;
}

void AdGroupPool::LoadAdGroup(hash_map<Ice::Long, AdGroupPtr> & group_pool) {
  Statement sql;
  sql << "select total_sys_status, daily_sys_status, display_type, adgroup_id, campaign_id, member_id, max_price, member_status, am_status, audit_status, delete_flag,day_points,hour_points, rotation_number from adgroup ";
  mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer, "adgroup").store(
      sql);
  if (res && res.num_rows() > 0) {
    for (size_t i = 0; i < res.num_rows(); ++i) {
      AdGroupPtr group = new AdGroup;
      mysqlpp::Row row = res.at(i);
      Ice::Long group_id = (Ice::Long) row["adgroup_id"];
      Ice::Long campaign_id = (Ice::Long) row["campaign_id"];
      Ice::Long member_id = (Ice::Long) row["member_id"];
      int member_status = (int) row["member_status"];
      int am_status = (int) row["am_status"];
      int audit_status = (int) row["audit_status"];
      int delete_flag = (int) row["delete_flag"];
      int max_price = (int) row["max_price"];
      int display_type = (int) row["display_type"];
      int daily_sys_status = (int) row["daily_sys_status"];
      int total_sys_status = (int) row["total_sys_status"];
      int rotation_number = (int) row["rotation_number"];
      string day_points = (mysqlpp::Null<string, mysqlpp::NullIsBlank>) row["day_points"];
      string hour_points = (mysqlpp::Null<string, mysqlpp::NullIsBlank>) row["hour_points"];
      bool campaign_status = false;
      AdCampaignPtr campaign_ptr = AdCampaignPool::instance().GetAdCampaign(campaign_id);
      if (campaign_ptr) {
        campaign_status = campaign_ptr->Available();
      }

      bool member_table_status = false;
      AdMemberPtr member_ptr = AdMemberPool::instance().GetAdMember(member_id);
      if (member_ptr) {
        member_table_status = member_ptr->Available(display_type);
      }

      bool point_valid = PointsChecker::instance().Check(day_points, hour_points);

      group->set_campaign_id(campaign_id);
      group->set_group_id(group_id);
      group->set_member_id(member_id);
      group->set_member_status(member_status);
      group->set_am_status(am_status);
      group->set_audit_status(audit_status);
      group->set_delete_flag(delete_flag);
      group->set_campaign_status(campaign_status);
      group->set_member_table_status(member_table_status);
      group->set_max_price(max_price);
      group->set_display_type(display_type);
      group->set_point_valid(point_valid);
      group->set_daily_sys_status(daily_sys_status);
      group->set_total_sys_status(total_sys_status);
      group->set_rotation_number(rotation_number);
      if (group->Available()) {
        group_pool[group_id] = group;
      } else {
//        MCE_DEBUG(
//            "AdGroupPool::Init --> because obj->Available is false so do not push it");
      }
      //MCE_INFO("AdGroupPool::Init --> group_id = " << group_id << " campaign_id = " << campaign_id << "campaign_status = " << campaign_status <<" member_id = " << member_id << " member_status = " << member_status << " am_status = " << am_status << " audit_status = " << audit_status << " delete_flag = " << delete_flag);
    }
  } else {
    MCE_INFO("AdGroupPool::Init --> no data in adgroup");
  }
}
void AdGroupPool::LoadCreative(hash_map<Ice::Long, AdGroupPtr> & group_pool) {
  Statement sql;
  sql << "select creative_id, adgroup_id, member_status, am_status, audit_status, delete_flag from creative";
  mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer, "creative").store(sql);
  if (res && res.num_rows() > 0) {
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      Ice::Long creative_id = (Ice::Long) row["creative_id"];
      Ice::Long group_id = (Ice::Long) row["adgroup_id"];
      int member_status = (int) row["member_status"];
      int am_status = (int) row["am_status"];
      int audit_status = (int) row["audit_status"];
      int delete_flag = (int) row["delete_flag"];
      if ((member_status == 1) && (am_status == 1) && (audit_status == 1) && (delete_flag == 1)) {
        hash_map<Ice::Long, AdGroupPtr>::iterator it = group_pool.find(group_id);
        if (it != group_pool.end()) {
          it->second->add_creative_id(creative_id);
        } else {
//          MCE_INFO("AdGroupPool::Init --> creative has group_id = " << group_id << " but adgroup no!");
        }
      }
    }
  }
}
void AdGroupPool::CheckIfHaveCreative(hash_map<Ice::Long, AdGroupPtr> & group_pool) {
  for (hash_map<Ice::Long, AdGroupPtr>::iterator git = group_pool.begin(); git != group_pool.end();) {
    if (!git->second->HasCreatives()) {
//      MCE_DEBUG("AdGroupPool::Init --> because obj->HasCreatives is false so erase it  groupid = " << git->first);
      group_pool.erase(git++);
    } else{
      ++git;
    }
  }
}

void AdGroupPool::Init() {
  MCE_DEBUG("AdGroupPool::Init");
  hash_map<Ice::Long, AdGroupPtr> group_pool;

  LoadAdGroup(group_pool);
  MCE_INFO("AdGroupPool::Init --> LoadAdGroup Over!");
  LoadCreative(group_pool);
  MCE_INFO("AdGroupPool::Init --> LoadCreative Over!");
  CheckIfHaveCreative(group_pool);
  MCE_INFO("AdGroupPool::Init --> CheckIfHaveCreative Over!");

  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    group_pool_.swap(group_pool);
  }
}
void AdGroupPool::UpdateCampainStatus() {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  for (hash_map<Ice::Long, AdGroupPtr>::iterator it = group_pool_.begin(); it != group_pool_.end(); ++it) {
    it->second->UpdateCampainStatus();
  }
}

/***********************************************************************************************************************/
bool AdMember::Available(int display_type) {
  bool tmp_val = is_formal_ && (validity_ == 1) && no_exceed_max_
      && am_online_status_ && ma_online_status_ && delete_flag_;
  if (display_type != 1) {
    tmp_val = have_money_ && tmp_val;
  }
  /*MCE_DEBUG("AdMember::Available --> RESULT = " << tmp_val << " member_id = " << member_id_ << " is_formal_ = " << is_formal_ << " validity_ = " <<
   validity_ << " no_exceed_max_ = " << no_exceed_max_ << " am_online_status_ = " << am_online_status_ << " ma_online_status_ = " <<
   ma_online_status_ << " delete_flag_ = " << delete_flag_ << " have_money_ = " << have_money_ << " display_type = " << display_type);*/
  return tmp_val;
}
AdMemberPtr AdMemberPool::GetAdMember(Ice::Long member_id) {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  AdMemberPtr ans = NULL;
  map<Ice::Long, AdMemberPtr>::iterator it = member_pool_.find(member_id);
  if (it != member_pool_.end()) {
    ans = it->second;
  } else {
   // MCE_INFO("AdMemberPool::GetAdMember --> miss member_id = " << member_id);
  }
  return ans;
}

void AdMemberPool::Init() {
  MCE_DEBUG("AdMemberPool::Init");
  Statement sql;
  map<Ice::Long, AdMemberPtr> member_pool;
  sql << "select * from member";
  mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer, "member").store(sql);
  if (res && res.num_rows() > 0) {
    for (size_t i = 0; i < res.num_rows(); ++i) {
      AdMemberPtr member = new AdMember;
      mysqlpp::Row row = res.at(i);
      member->set_member_id((Ice::Long) row["member_id"]);
      member->set_is_formal((int) row["is_formal"]);
      member->set_validity((int) row["validity"]);
      member->set_have_money((int) row["have_money"]);
      member->set_no_exceed_max((int) row["no_exceed_max"]);
      member->set_am_online_status((int) row["am_online_status"]);
      member->set_ma_online_status((int) row["ma_online_status"]);
      member->set_delete_flag((int) row["delete_flag"]);
      if (member->Available()) {
        member_pool[member->member_id()] = member;
      }
    }
    MCE_INFO("AdMemberPool::Init --> AFTER INIT member_pool.size = " << member_pool.size());

    {
      IceUtil::RWRecMutex::WLock lock(mutex_);
      member_pool_.swap(member_pool);
    }
  } else {
    MCE_INFO("AdMemberPool::Init --> no data in ad_member");
  }
}

/***********************************************************************************************************************/
bool AdCampaign::Available() {
  Date date = Date(time(NULL));
  time_t timestamp = date.time();
  bool time_check = timestamp >= begin_time_ && (!end_time_ || timestamp <= end_time_);
  bool result = delete_flag_ && (audit_status_ == 1) && time_check
      && (member_status_ == 1) && (am_status_ == 1) && (sys_status_ == 1);

  //MCE_INFO("AdCampaign::Available --> current_timestamp = " << timestamp << " plan_begin_timestamp = " << begin_time_ << " plan_end_timestamp = " << end_time_);
//  MCE_DEBUG("AdCampaign::Available --> RESULT = " << result
//      << " : campaign_id_ = " << campaign_id_ << " delete_flag_ = "
//      << delete_flag_ << " audit_status_ = " << audit_status_
//      << " time_check = " << time_check << " member_status_ = "
//      << member_status_ << " am_status_ = " << am_status_ << " sys_status_ = "
//      << sys_status_ << " current_timestamp = " << timestamp
//      << " plan_begin_timestamp = " << begin_time_ << " plan_end_timestamp = "
//      << end_time_);
  return result;
}

AdCampaignPtr AdCampaignPool::GetAdCampaign(Ice::Long campaign_id) {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  AdCampaignPtr ans = NULL;
  map<Ice::Long, AdCampaignPtr>::iterator it = campaign_pool_.find(campaign_id);
  if (it != campaign_pool_.end()) {
    ans = it->second;
  } else {
    // MCE_INFO("AdCampaignPool::GetAdCampaign --> miss campaign_id = " << campaign_id);
  }
  return ans;
}

void AdCampaignPool::Init() {
  MCE_DEBUG("AdCampaignPool::Init");
  Statement sql;
  map<Ice::Long, AdCampaignPtr> campaign_pool;
  sql << "select sys_status, member_status, am_status, campaign_id, member_id, begin_time, end_time, cast_calender, cast_hour, delete_flag, audit_status from campaign";
  mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer, "campaign").store(
      sql);
  if (res && res.num_rows() > 0) {
    for (size_t i = 0; i < res.num_rows(); ++i) {
      AdCampaignPtr obj = new AdCampaign;
      mysqlpp::Row row = res.at(i);
      Ice::Long campaign_id = (Ice::Long) row["campaign_id"];
      Ice::Long member_id = (Ice::Long) row["member_id"];
      string begin_time = (mysqlpp::Null<string, mysqlpp::NullIsBlank>) row["begin_time"];
      string end_time = (mysqlpp::Null<string, mysqlpp::NullIsBlank>) row["end_time"];
      bool delete_flag = (bool) row["delete_flag"];
      int audit_status = (int) row["audit_status"];
      int sys_status = (int) row["sys_status"];
      int member_status = (int) row["member_status"];
      int am_status = (int) row["am_status"];

      obj->set_campaign_id(campaign_id);
      obj->set_member_id(member_id);
      obj->set_delete_flag(delete_flag);
      obj->set_audit_status(audit_status);
      obj->set_sys_status(sys_status);
      obj->set_member_status(member_status);
      obj->set_am_status(am_status);

      if (begin_time.empty()) {
        obj->set_begin_time(0);
      } else {
        Date date_begin_time = Date(begin_time);
        obj->set_begin_time(date_begin_time.time());
      }
      if (end_time.empty()) {
        obj->set_end_time(0);
      } else {
        Date date_end_time = Date(end_time);
        obj->set_end_time(date_end_time.time());
      }

      //MCE_INFO("AdAnalysisManagerI::Campaign --> campaign_id_ = " << campaign_id << " member_id_ = " << member_id << " begin_time_ = " << begin_time
      //    << " end_time_ = " << end_time << " delete_flag_ = " << delete_flag << " audit_status_ = " << audit_status <<
      //    " cast_calender = " << cast_calender << " running_area_day_ = " << running_area_day << " cast_hour = " << cast_hour << " running_area_hour = " << running_area_hour);
      if (obj->Available()) {
        campaign_pool[campaign_id] = obj;
      }
    }
    MCE_INFO("AdCampaignPool::Init --> AFTER INIT campaign_pool.size = " << campaign_pool.size());

    {
      IceUtil::RWRecMutex::WLock lock(mutex_);
      campaign_pool_.swap(campaign_pool);
    }
  } else {
    MCE_INFO("AdCampaignPool::Init --> no data in ad_campaign");
  }
}

} // end namespace ad
} // end namespace xce
