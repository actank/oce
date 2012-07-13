/*
 * AdStruct.h
 *
 *  Created on: Nov 30, 2010
 *      Author: zhaohui.tang
 */

#ifndef AD_STRUCT_CACHE_H_
#define AD_STRUCT_CACHE_H_
#include <vector>
#include <map>
#include <ext/hash_map>
#include <set>
#include <bitset>
#include <IceUtil/RWRecMutex.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/Shared.h>
#include <IceUtil/Handle.h>
#include "TaskManager.h"
#include "QueryRunner.h"
#include "Singleton.h"

namespace xce {
namespace ad {
using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace __gnu_cxx;

const string DB_INSTANCE = "ade_db";

/***********************************************************************************************************************/
class PointsChecker : public MyUtil::Singleton<PointsChecker> {
public:
  bool Check(const string & day_points, const string & hour_points);
private:
  bool CheckHour(const string & day_points);
  bool CheckDay(const string & hour_points);
};

/***********************************************************************************************************************/
class AdMember : public IceUtil::Shared {
public:
  bool Available(int display_type = 1);

  Ice::Long member_id() const {
    return member_id_;
  }
  void set_member_id(Ice::Long member_id) {
    this->member_id_ = member_id;
  }
  int delete_flag() const {
    return delete_flag_;
  }
  void set_delete_flag(int delete_flag) {
    this->delete_flag_ = delete_flag;
  }
  int ma_online_status() const {
    return ma_online_status_;
  }
  void set_ma_online_status(int ma_online_status) {
    this->ma_online_status_ = ma_online_status;
  }
  int no_exceed_max() const {
    return no_exceed_max_;
  }
  void set_no_exceed_max(int no_exceed_max) {
    this->no_exceed_max_ = no_exceed_max;
  }
  int am_online_status() const {
    return am_online_status_;
  }
  void set_am_online_status(int am_online_status) {
    this->am_online_status_ = am_online_status;
  }
  int have_money() const {
    return have_money_;
  }
  void set_have_money(int have_money) {
    this->have_money_ = have_money;
  }
  int validity() const {
    return validity_;
  }
  void set_validity(int validity) {
    this->validity_ = validity;
  }
  int is_formal() const {
    return is_formal_;
  }
  void set_is_formal(int is_formal) {
    this->is_formal_ = is_formal;
  }
private:
  int is_formal_;
  int validity_;
  int have_money_;
  int no_exceed_max_;
  int am_online_status_;
  int ma_online_status_;
  int delete_flag_;
  Ice::Long member_id_;
};
typedef IceUtil::Handle<AdMember> AdMemberPtr;
class AdMemberPool : public MyUtil::Singleton<AdMemberPool> {
public:
  void Init();
  AdMemberPtr GetAdMember(Ice::Long member_id);
private:
  map<Ice::Long, AdMemberPtr> member_pool_;
  IceUtil::RWRecMutex mutex_;
};

/***********************************************************************************************************************/
//如果Member不可用，请业务系统update campign and ad_group表的status字段为不可用
class AdCampaign : public IceUtil::Shared {
public:
  AdCampaign() {
    end_time_ = 0;
    audit_status_ = 0;
    delete_flag_ = 0;
    sys_status_ = 0;
    member_status_ = 0;
    am_status_ = 0;
  }
  bool Available();

  bool delete_flag() const {
    return delete_flag_;
  }
  void set_delete_flag(bool delete_flag) {
    this->delete_flag_ = delete_flag;
  }
  int audit_status() const {
    return audit_status_;
  }
  void set_audit_status(int audit_status) {
    this->audit_status_ = audit_status;
  }
  int am_status() const {
    return am_status_;
  }
  void set_am_status(int am_status) {
    this->am_status_ = am_status;
  }
  int member_status() const {
    return member_status_;
  }
  void set_member_status(int member_status) {
    this->member_status_ = member_status;
  }
  int sys_status() const {
    return sys_status_;
  }
  void set_sys_status(int sys_status) {
    this->sys_status_ = sys_status;
  }
  time_t end_time() const {
    return end_time_;
  }
  void set_end_time(time_t end_time) {
    this->end_time_ = end_time;
  }
  time_t begin_time() const {
    return begin_time_;
  }
  void set_begin_time(time_t begin_time) {
    this->begin_time_ = begin_time;
  }
  Ice::Long member_id() const {
    return member_id_;
  }
  void set_member_id(Ice::Long member_id) {
    this->member_id_ = member_id;
  }
  Ice::Long campaign_id() const {
    return campaign_id_;
  }
  void set_campaign_id(Ice::Long campaign_id) {
    this->campaign_id_ = campaign_id;
  }

private:
  Ice::Long campaign_id_;
  Ice::Long member_id_;
  time_t begin_time_;
  time_t end_time_;
  int sys_status_;
  int member_status_;
  int am_status_;
  int audit_status_;
  bool delete_flag_;
};
typedef IceUtil::Handle<AdCampaign> AdCampaignPtr;

class AdCampaignPool : public MyUtil::Singleton<AdCampaignPool> {
public:
  void Init();
  AdCampaignPtr GetAdCampaign(Ice::Long campaign_id);
private:
  map<Ice::Long, AdCampaignPtr> campaign_pool_;
  IceUtil::RWRecMutex mutex_;

};

/***********************************************************************************************************************/
class AdGroup : public IceUtil::Shared {
public:
  bool Available();
  bool HasCreatives();
  void UpdateCampainStatus();
  void set_bid_unit_key(uint64_t bid_unit_key) {
    bid_unit_key_ = bid_unit_key;
  }
  void set_group_id(Ice::Long group_id) {
    group_id_ = group_id;
  }
  void set_member_id(Ice::Long member_id) {
    member_id_ = member_id;
  }
  void set_campaign_id(Ice::Long campaign_id) {
    campaign_id_ = campaign_id;
  }
  void set_member_status(int member_status) {
    member_status_ = member_status;
  }
  void set_am_status(int am_status) {
    am_status_ = am_status;
  }
  void set_audit_status(int audit_status) {
    audit_status_ = audit_status;
  }
  void set_delete_flag(int delete_flag) {
    delete_flag_ = delete_flag;
  }
  void set_display_type(int display_type) {
    display_type_ = display_type;
  }
  void set_max_price(int max_price) {
    max_price_ = max_price;
  }
  void set_campaign_status(bool campaign_status) {
    campaign_status_ = campaign_status;
  }
  void set_member_table_status(bool member_table_status) {
    member_table_status_ = member_table_status;
  }
  void add_creative_id(Ice::Long cretive_id) {
    creative_ids_.insert(cretive_id);
  }
  void set_bid_unit_id(Ice::Long bid_unit_id) {
    bid_unit_id_ = bid_unit_id;
  }
  void set_point_valid(bool valid) {
    point_valid_ = valid;
  }
  void set_daily_sys_status(int daily_sys_status) {
    daily_sys_status_ = daily_sys_status;
  }
  void set_total_sys_status(int total_sys_status) {
    total_sys_status_ = total_sys_status;
  }
  void set_rotation_number(int rotation_number) {
    rotation_number_ = rotation_number;
  }
  uint64_t bid_unit_key() {
    return bid_unit_key_;
  }
  Ice::Long group_id() {
    return group_id_;
  }
  Ice::Long member_id() {
    return member_id_;
  }
  Ice::Long campaign_id() {
    return campaign_id_;
  }
  int member_status() {
    return member_status_;
  }
  int am_status() {
    return am_status_;
  }
  int audit_status() {
    return audit_status_;
  }
  int delete_flag() {
    return delete_flag_;
  }
  int display_type() {
    return display_type_;
  }
  int max_price() {
    return max_price_;
  }
  int rotation_number() {
    return rotation_number_;
  }
  set<Ice::Long> creative_ids() {
    return creative_ids_;
  }
  Ice::Long bid_unit_id() {
    return bid_unit_id_;
  }
  int daily_sys_status() {
    return daily_sys_status_;
  }
  int total_sys_status() {
    return total_sys_status_;
  }
  bool campaign_status() {
    return campaign_status_;
  }
  bool member_table_status() {
    return member_table_status_;
  }
  bool operator >(const AdGroup& group) const {
    return group_id_ > group.group_id_;
  }
  bool operator ==(const AdGroup& group) const {
    return group_id_ == group.group_id_;
  }
  bool operator <(const AdGroup& group) const {
    return group_id_ < group.group_id_;
  }
  AdGroup() {
    am_status_ = 0;
    audit_status_ = 0;
    delete_flag_ = 0;
    campaign_status_ = 0;
    member_table_status_ = 0;
    daily_sys_status_ = 0;
    total_sys_status_ = 0;
    bid_unit_key_ = 0;
    bid_unit_id_ = 0;
    rotation_number_ = 1;
  }
  bool IsDefault() const {
    return member_id_ == 999;
  }
  bool IsBrand() const {
    return display_type_ != 4;
  }
private:
  uint64_t bid_unit_key_;
  Ice::Long bid_unit_id_;
  Ice::Long group_id_;
  Ice::Long member_id_;
  Ice::Long campaign_id_;
  int member_status_; // 客户自己设置的该adgroup的状态
  int am_status_;
  int audit_status_;
  int delete_flag_;
  int display_type_;//轮播,cpm,cpc
  int max_price_;
  int daily_sys_status_;
  int total_sys_status_;
  int rotation_number_;
  bool campaign_status_; // 所属campaign的状态
  bool member_table_status_; // 所属member的状态
  set<Ice::Long> creative_ids_;

  bool point_valid_;//是否在排期内
};

typedef IceUtil::Handle<AdGroup> AdGroupPtr;
typedef vector<AdGroupPtr> AdGroupSeq;

class AdGroupPool : public MyUtil::Singleton<AdGroupPool> {
public:
  void Init();
  AdGroupPtr GetAdGroup(Ice::Long group_id);
  bool HasAdGroup(Ice::Long group_id) {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    return group_pool_.find(group_id) != group_pool_.end();
  }

  void UpdateCampainStatus();
private:
  hash_map<Ice::Long, AdGroupPtr> group_pool_;
  IceUtil::RWRecMutex mutex_;

  void LoadAdGroup(hash_map<Ice::Long, AdGroupPtr> & group_pool);
  void LoadCreative(hash_map<Ice::Long, AdGroupPtr> & group_pool);
  void CheckIfHaveCreative(hash_map<Ice::Long, AdGroupPtr> & group_pool); // adgroup's has creative check
};

} // end namespace ad
} // end namespace xce

#endif /* AD_STRUCT_CACHE_H_ */
