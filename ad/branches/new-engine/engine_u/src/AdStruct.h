/*
 * AdStruct.h
 *
 *  Created on: Aug 25, 2010
 *      Author: antonio
 */

#ifndef ADSTRUCT_H_
#define ADSTRUCT_H_

#include <time.h>
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
#include "AdTranslator.h"
#include "QueryRunner.h"
#include "Singleton.h"
#include "Util.h"
#include "FeedMemcProxy/client/user_profile_client.h"

namespace xce {
namespace ad {
using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace __gnu_cxx;

class StatFunc {
  string name_;
  timeval start_, end_;
  //static int count;
public:
  StatFunc(string name) : name_(name) {
    gettimeofday(&start_, NULL);
  }

  ~StatFunc() {
    gettimeofday(&end_, NULL);
    float timeuse=1000000*(end_.tv_sec-start_.tv_sec)+end_.tv_usec
        -start_.tv_usec;
    timeuse/=1000;
    if(timeuse >= 20){
      MCE_INFO(name_ << " time usage:" << timeuse << " ms");
    } else {
      MCE_DEBUG(name_ << " time usage:" << timeuse << " ms");
    }
  }
};

class AdZone;
typedef IceUtil::Handle<AdZone> AdZonePtr;
typedef vector<AdZone> AdZoneSeq;
typedef map<int, AdZonePtr> AdZoneDict;

class DictNode;
class DictTree;
typedef IceUtil::Handle<DictNode> DictNodePtr;
typedef IceUtil::Handle<DictTree> DictTreePtr;
typedef hash_map<char, DictNodePtr> Char2NodeMap;

class DictNode : public IceUtil::Shared {
public:
  DictNode() {
    wordend_ = false;
  }
  DictNodePtr insert(const char& c, bool wordend);
  void GetNext2(const char& c, DictNodePtr&, bool lastisinteger , bool search_end = false);
  void GetNext(const char& c, DictNodePtr&, bool search_end = false);

  vector<AdZonePtr> referer_zones_;
  Char2NodeMap node_;
  bool wordend_;
};

class DictTree : public IceUtil::Shared {
public:
  DictTree() {
    root_ = new DictNode();
  }
  void insert(const string& str, const AdZonePtr& zone_ptr);
  bool search(const string& str, vector<AdZonePtr>& zones);
private:
  DictNodePtr root_;
};

/***********************************************************************************************************************/
class PointsChecker : public MyUtil::Singleton<PointsChecker> {
public:
  bool Check(const string & day_points, const string & hour_points);
private:
  bool CheckHour(const string & day_points);
  bool CheckDay(const string & hour_points);
};

/***********************************************************************************************************************/
class AdGroup;
typedef IceUtil::Handle<AdGroup> AdGroupPtr;
typedef vector<AdGroupPtr> AdGroupSeq;

class SelectedAdGroup;
typedef IceUtil::Handle<SelectedAdGroup> SelectedAdGroupPtr;
typedef vector<SelectedAdGroupPtr> SelectedAdGroupSeq;

class AdZone : public IceUtil::Shared {
public:
  AdZone() {
    member_online_status_ = 0;
    am_online_status_ = 0;
    audit_status_ = 0;
    delete_flag_ = 0;
  }
  AdGroupPtr GetRotateGroups(int rotate_index, const UserProfile& user_profile);
  bool AddGroupToRotateList(int rotate_index, const AdGroupPtr& obj);
  int GetRotateSize();
  void ResizeRotateList(int size);
  bool Available();
  AdGroupPtr GetDefaultGroups();
  set<AdGroupPtr> GetDefaultGroups(int count);
  bool AddGroupToDefaultList(int rotate_index, const AdGroupPtr& obj);
  int GetDefaultSize();
  void ResizeDefaultList(int size);

  bool FindStage(int stage);

  Ice::Long id();
  int bp_count() const {
    return bp_count_;
  }
  void set_bp_count(int bp_count) {
    this->bp_count_ = bp_count;
  }
  int brand_count() const {
    return brand_count_;
  }
  void set_brand_count(int brand_count) {
    this->brand_count_ = brand_count;
  }
  AdGroupPtr only_first() {
    return only_first_;
  }
  void set_only_first(AdGroupPtr only_first) {
    only_first_ =  only_first;
  }
  int member_type() const {
    return member_type_;
  }
  void set_member_type(int member_type) {
    this->member_type_ = member_type;
  }
  int weight();
  set<string> referers();
  void set_id(Ice::Long id);
  void add_stage(int stage);
  void set_weight(int weight);
  void set_referer(const string& ref);
  void set_member_online_status(int member_online_status);
  void set_am_online_status(int am_online_status);
  void set_audit_status(int audit_status);
  void set_delete_flag(int delete_flag);
  vector<AdGroupPtr> rotate_list() {
    return rotate_list_;
  }
  int self_rotation_number() const {
    return self_rotation_number_;
  }
  void set_self_rotation_number(int self_rotation_number) {
    this->self_rotation_number_ = self_rotation_number;
  }
  void random_rotate();
private:
  Ice::Long id_; //zone id
  int brand_count_; // 可显示的品牌广告数
  int bp_count_; // 可显示的中小自助广告数
  int weight_; //广告位展示权重
  set<int> stage_;//能展示这个广告位的阶段
  set<string> referer_;//这个广告位请求的referer
  vector<AdGroupPtr> rotate_list_;//广告位轮播的广告们
  vector<AdGroupPtr> default_list_;

  vector<AdGroupPtr> global_list_; // 投放给所有用户的广告
  AdGroupPtr only_first_;

  int self_rotation_number_; // 自助广告所占轮播数
  int member_type_;

  int member_online_status_;
  int am_online_status_;
  int audit_status_;
  int delete_flag_;
};

const static int zone_reload_timestamp = 1 * 60 * 1000;

class AdZonePool : public MyUtil::Singleton<AdZonePool> {
public:
  AdZonePool() {
    trie_ = new DictTree();
  }
  void Init();
  AdZoneDict GetAdZoneOrdered(const string& referer, const UserProfile& userprofile, bool hide = false);
  class ReloadTimer : public MyUtil::Timer {
  public:
    ReloadTimer() :
      Timer(zone_reload_timestamp) {
    }
    virtual void handle();
  };
  AdZonePtr GetAdZone(Ice::Long id) {
    map<Ice::Long, AdZonePtr>::iterator it = zone_pool_.find(id);
    if (it != zone_pool_.end()) {
      return it->second;
    } else {
      return NULL;
    }
  }
private:
  DictTreePtr trie_;
  multimap<string, AdZonePtr> ref_zone_map_;
  map<Ice::Long, AdZonePtr> zone_pool_;
  IceUtil::RWRecMutex mutex_;

  void LoadAdzone(map<Ice::Long, AdZonePtr> & zones);
  void LoadAdgroupAdzoneRef(map<Ice::Long, AdZonePtr> & zones);
  void LoadUrlAdzoneMaping(map<Ice::Long, AdZonePtr> & zones, DictTreePtr & trie, multimap<string, AdZonePtr> & ref_zone_map);
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
  class UpdateCheckTimer : public MyUtil::Timer {
  public:
    UpdateCheckTimer() :
      Timer(1 * 60 * 1000) {
    }
    virtual void handle();
  };
private:
  map<Ice::Long, AdMemberPtr> member_pool_;
  IceUtil::RWRecMutex mutex_;
};
/***********************************************************************************************************************/
class AdStyle : public IceUtil::Shared {
public:

  Ice::Long style_id() const {
    return style_id_;
  }
  void set_style_id(Ice::Long style_id) {
    this->style_id_ = style_id;
  }
  void set_ad_cnt( int ad_cnt){
    this->ad_cnt_ = ad_cnt;
  }
  int ad_cnt() const{
    return ad_cnt_; 
  }
  string note() const{
    return this->note_;
  }
  void set_note(string note ){
    this->note_ = note;
  }
  void set_submit_time(string submit_time){
    this->submit_time_ = submit_time;
  }
  string submit_time() const{
    return submit_time_;
  }
  void set_big_widgets(vector<int>& big_widgets){
    this->big_widgets_ = big_widgets;
  }
  vector<int> big_widgets() const{
    return this->big_widgets_; 
  }
  void set_small_widgets(vector<int>& small_widgets){
    this->small_widgets_ = small_widgets;
  }
  vector<int> small_widgets() const{
    return this->small_widgets_; 
  }
  static vector<int> String2intvec(string str , string sep);

private:
  Ice::Long style_id_;
  int ad_cnt_;
  string note_;
  string submit_time_;
  vector<int> big_widgets_;
  vector<int> small_widgets_;
};
typedef IceUtil::Handle<AdStyle> AdStylePtr;
class AdStylePool : public MyUtil::Singleton<AdStylePool> {
public:
  void Init();
  AdStylePtr GetAdStyle(Ice::Long style_id);
  class UpdateCheckTimer : public MyUtil::Timer {
  public:
    UpdateCheckTimer() :
      Timer(1 * 60 * 1000) {
    }
    virtual void handle();
  };
private:
  map<Ice::Long, AdStylePtr> style_pool_;
  IceUtil::RWRecMutex mutex_;
};

/***********************************************************************************************************************/

class AdExperiment : public IceUtil::Shared {
public:

  Ice::Long experiment_id() const {
    return experiment_id_;
  }
  void set_experiment_id(Ice::Long experiment_id) {
    this->experiment_id_ = experiment_id;
  }
  void set_style_id( Ice::Long style_id){
    this->style_id_ = style_id;
  }
  Ice::Long style_id() const{
    return style_id_; 
  }
  void set_status( int status){
    this->status_ = status;
  }
  int status() const{
    return status_; 
  }
  void set_traffic( int traffic){
    this->traffic_ = traffic;
  }
  int traffic() const{
    return traffic_; 
  }
  void set_zone_id( Ice::Long zone_id){
    this->zone_id_ = zone_id;
  }
  Ice::Long zone_id() const{
    return zone_id_; 
  }
  void set_submit_time(string submit_time){
    this->submit_time_ = submit_time;
  }
  string submit_time() const{
    return submit_time_;
  }
  void set_modify_time(string modify_time){
    this->modify_time_ = modify_time;
  }
  string modify_time() const{
    return modify_time_;
  }
private:
  Ice::Long experiment_id_;
  Ice::Long zone_id_;
  Ice::Long style_id_;
  int traffic_;
  int status_;
  string modify_time_;
  string submit_time_;
};
typedef IceUtil::Handle<AdExperiment> AdExperimentPtr;
class AdExperimentPool : public MyUtil::Singleton<AdExperimentPool> {
public:
  void Init();
  AdExperimentPtr GetAdExperiment(Ice::Long experiment_id);
  vector<AdExperimentPtr> ListAdExperiment();
  static bool compare(AdExperimentPtr exp1 , AdExperimentPtr exp2){
	  if( exp1->zone_id() == exp2->zone_id()){
		  return exp1->traffic() < exp2->traffic();
	  }else{
		  return exp1->zone_id()< exp1->zone_id();
	  }
  }
  class UpdateCheckTimer : public MyUtil::Timer {
  public:
    UpdateCheckTimer() :
      Timer(1 * 60 * 1000) {
    }
    virtual void handle();
  };
private:
  map<Ice::Long, AdExperimentPtr> experiment_pool_;
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
  class UpdateCheckTimer : public MyUtil::Timer {
  public:
    UpdateCheckTimer() :
      Timer(1 * 60 * 1000) {
    }
    ;
    virtual void handle();
  };
private:
  map<Ice::Long, AdCampaignPtr> campaign_pool_;
  IceUtil::RWRecMutex mutex_;

};

/***********************************************************************************************************************/
class AdGroup : public IceUtil::Shared {
public:
  Ice::Long GetCreative(int uid, int rotate_index);
  bool Available();
  bool HasCreatives();
  void UpdateCampainStatus();
  void set_group_id(Ice::Long group_id) {
    group_id_ = group_id;
  }
  void set_widget_id(Ice::Long widget_id) {
    widget_id_ = widget_id;
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
  void add_zone_id(Ice::Long zone_id) {
    zone_ids_.insert(zone_id);
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
  void add_widget_id(Ice::Long creative_id,Ice::Long widget_id) {
    MCE_DEBUG("add_widget_id: creative_id = " << creative_id << ", widget_id = " << widget_id);
    creative2widget_[creative_id]=widget_id;
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
  void set_bid_unit(BidUnitPtr bid_unit) {
    bid_unit_ = bid_unit;
  }
  BidUnitPtr bid_unit() {
    return bid_unit_;
  }
  Ice::Long group_id() {
    return group_id_;
  }
  Ice::Long member_id() {
    return member_id_;
  }
  Ice::Long widget_id() {
    return widget_id_;
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
  set<Ice::Long> zone_ids() {
    return zone_ids_;
  }
  Ice::Long zone_id() {
    if (zone_ids_.empty()) {
      return 0;
    }
    return *zone_ids_.begin();
  }
  set<Ice::Long> creative_ids() {
    return creative_ids_;
  }
  map<Ice::Long,Ice::Long> creative2widget() {
    return creative2widget_;
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
  int trans_type() const {
    return trans_type_;
  }
  void set_trans_type(int trans_type) {
    this->trans_type_ = trans_type;
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
    bid_unit_id_ = 0;
    rotation_number_ = 1;
    demo_state_ = -1;
  }

  AdGroup(Ice::Long group_id) : group_id_(group_id), member_id_(0), campaign_id_(0),widget_id_(0) {
    am_status_ = 0;
    audit_status_ = 0;
    delete_flag_ = 0;
    campaign_status_ = 0;
    member_table_status_ = 0;
    daily_sys_status_ = 0;
    total_sys_status_ = 0;
    bid_unit_id_ = 0;
    rotation_number_ = 1;
    demo_state_ = -1;
  }

  bool HasThisAdzone(Ice::Long zone_id) const {
    return zone_ids_.find(zone_id) != zone_ids_.end();
  }
  bool IsDefault() const {
    return member_id_ == 999;
  }
  void SetDemoState(int state) {
    demo_state_ = state;
  }
  int GetDemoState() {
    return demo_state_;
  }
private:
  Ice::Long bid_unit_id_;
  Ice::Long group_id_;
  Ice::Long member_id_;
  Ice::Long campaign_id_;
  Ice::Long widget_id_;
  int member_status_; // 客户自己设置的该adgroup的状态
  int am_status_;
  int audit_status_;
  int delete_flag_;
  int display_type_;//轮播,cpm,cpc
  int trans_type_; // 结算方式：10:cpc，20：cpm, 0: 其他
  int max_price_;
  int daily_sys_status_;
  int total_sys_status_;
  int rotation_number_;
  int demo_state_;
  bool campaign_status_; // 所属campaign的状态
  bool member_table_status_; // 所属member的状态
  set<Ice::Long> zone_ids_;
  set<Ice::Long> creative_ids_;
  map<Ice::Long,Ice::Long> creative2widget_;

  bool point_valid_;//是否在排期内

  BidUnitPtr bid_unit_;
};

class AdGroupPool : public MyUtil::Singleton<AdGroupPool> {
public:
  void Init();
  AdGroupPtr GetAdGroup(Ice::Long group_id);

  map<Ice::Long, vector<AdGroupPtr> > GetRotateAds();
  map<Ice::Long, vector<AdGroupPtr> > GetDefaultAds();
  map<Ice::Long, AdGroupPtr> GetFirstAds() {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    return zone2first_;
  }
  void UpdateCampainStatus();
  class UpdateCheckTimer : public MyUtil::Timer {
  public:
    UpdateCheckTimer() :
      Timer(10 * 60 * 1000) {
    }
    virtual void handle();
  };
  class UpdateCampaignStatusTimer : public MyUtil::Timer {
  public:
    UpdateCampaignStatusTimer() :
      Timer(1 * 60 * 1000) {
    }
    virtual void handle();
  };
private:
  hash_map<Ice::Long, AdGroupPtr> group_pool_;
  map<Ice::Long, vector<AdGroupPtr> > zone2rotate_;
  map<Ice::Long, vector<AdGroupPtr> > zone2default_;
  map<Ice::Long, AdGroupPtr> zone2first_;
  IceUtil::RWRecMutex mutex_;

  void LoadAdGroup(hash_map<Ice::Long, AdGroupPtr> & group_pool);
  void LoadCreative(hash_map<Ice::Long, AdGroupPtr> & group_pool);
  void LoadAdgroupAdzoneRef(hash_map<Ice::Long, AdGroupPtr> & group_pool,
      map< Ice::Long, vector<AdGroupPtr> > & zone2rotate,
      map< Ice::Long, vector<AdGroupPtr> > & zone2default,
      map<Ice::Long, AdGroupPtr> & zone2first);
  void CheckIfHaveCreative(hash_map<Ice::Long, AdGroupPtr> & group_pool); // adgroup's has creative check
};

/***********************************************************************************************************************/
class SchoolPool : public MyUtil::Singleton<SchoolPool> {
public:
  void Init();
  uint64_t GetSchoolAreaCode(Ice::Long school);
private:
  map<Ice::Long, uint64_t> pool_;
  IceUtil::RWRecMutex mutex_;
};

/***********************************************************************************************************************/
class CompeteRef : public MyUtil::Singleton<CompeteRef> {
public:
  CompeteRef() :
    is_ready_(false) {
  }
  void Init();
  void Load();
  vector<Ice::Long> Get(Ice::Long campaign);
  bool is_ready();

  class ReloadTimer : public MyUtil::Timer {
  public:
    ReloadTimer() :
      Timer(5 * 60 * 1000) {
    }
    virtual void handle();
  };

private:
  map<Ice::Long, vector<Ice::Long> > ref_;
  IceUtil::RWRecMutex mutex_;
  bool is_ready_;
};

/***********************************************************************************************************************/
class UserBind {
public:
  Ice::Long adzone_id() const {
    return adzone_id_;
  }
  void set_adzone_id(Ice::Long adzone_id) {
    this->adzone_id_ = adzone_id;
  }
  Ice::Long user_id() const {
    return user_id_;
  }
  void set_user_id(Ice::Long user_id) {
    this->user_id_ = user_id;
  }
  bool operator >(const UserBind& ul) const {
    if (user_id_ != ul.user_id_)
      return user_id_ > ul.user_id_;
    return adzone_id_ > ul.adzone_id_;
  }
  bool operator ==(const UserBind& ul) const {
    return ( (user_id_ == ul.user_id_) && (adzone_id_ == ul.adzone_id_));
  }
  bool operator <(const UserBind& ul) const {
    if(user_id_ != ul.user_id_)
      return user_id_ < ul.user_id_;
    return adzone_id_ < ul.adzone_id_;
  }
private:
  Ice::Long user_id_;
  Ice::Long adzone_id_;
};

class UserBindPool : public MyUtil::Singleton<UserBindPool> {
private:
  typedef map<UserBind, Ice::Long> UserBindMap;
public:
  void BindUser(int user_id, Ice::Long creative_id, Ice::Long zone_id);
  void UnbindUser(int user_id, Ice::Long zone_id);
  void NotifyBindUser(int user_id, Ice::Long creative_id, Ice::Long zone_id);
  void NotifyUnbindUser(int user_id, Ice::Long zone_id);
  void Init();
  typedef UserBindMap::iterator iterator;
  iterator find(UserBind user_bind);
//  iterator begin();
  iterator end();
private:
  UserBindMap user_bind_cache_; //UserBind --> creative_id
  IceUtil::RWRecMutex mutex_;
};

class SelectedAdGroup : public IceUtil::Shared {
public:
  SelectedAdGroup(AdGroupPtr p) : group_(p), cost_(0), pos_(0),
                                  selected_creative_id_(0), selected_widget_id_(0),
                                  ecpm_(0), demo_state_(-1) {
    map<Ice::Long, Ice::Long> c2w = p->creative2widget();
    if(!c2w.empty()) {
      selected_creative_id_ = c2w.begin()->first;
      selected_widget_id_ = c2w.begin()->second;
    }
  }

  bool operator<(const SelectedAdGroup& right) const {
    return group_ < right.group_;
  }
  bool operator==(const SelectedAdGroup& right) const {
    return group_ < right.group_;
  }
//  bool operator<(const SelectedAdGroup& right) const {
//    return group_ < right.group_;
//  }
  AdGroupPtr group_;
  int cost_;
  int pos_;
  Ice::Long selected_creative_id_; //样式实验选中的创意
  Ice::Long selected_widget_id_;   //样式实验选中的widget_id
  double ecpm_;
  string append_info_;
  string extra_;
  int demo_state_;
};

class FilteredGroupList: public MyUtil::Singleton<FilteredGroupList> {
public:
  void Init() {
    MCE_INFO("FilteredGroupList init start");
    string file_name = "../etc/filtered_group_list";
    try {
      ifstream config(file_name.c_str());
      string line;
      if (config) {
        while (config >> line) {
          MCE_INFO("FilteredGroupList init group id : " << line);
          set_.insert(boost::lexical_cast<Ice::Long>(line));
        }
      }
    } catch (Ice::Exception& e) {
      MCE_WARN("FilteredGroupList init error : " << e);
    } catch (...) {
      MCE_WARN("FilteredGroupList init error : unkown error \"...\"");
    }
    MCE_INFO("FilteredGroupList init end");
  }
  bool IsNotExist(Ice::Long gid) {
    return set_.find(gid) == set_.end();
  }
private:
  set<Ice::Long> set_;
};
} // end namespace ad
} // end namespace xce

#endif /* ADSTRUCT_H_ */

