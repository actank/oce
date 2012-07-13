/*
 * AdStruct.h
 *
 *  Created on: Aug 25, 2010
 *      Author: antonio
 */

#ifndef ADSTRUCT_H_
#define ADSTRUCT_H_
#include <vector>
#include <map>
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
#include "time.h"
#include "FeedMemcProxy/client/user_profile_client.h"
#include <ext/hash_map>

namespace xce {
namespace ad {
using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace __gnu_cxx;

//qa test
class AdZone;
typedef IceUtil::Handle<AdZone> AdZonePtr;
typedef vector<AdZone> AdZoneSeq;
typedef map<int, AdZonePtr> AdZoneDict;
//weight2AdZonePtr


class DictNode;
class DictTree;
typedef IceUtil::Handle<DictNode> DictNodePtr;
typedef IceUtil::Handle<DictTree> DictTreePtr;
typedef hash_map<char, DictNodePtr> Char2NodeMap;

class DictNode : public IceUtil::Shared{
public:
  DictNode(){
    wordend_ = false;
  }
  DictNodePtr insert(const char& c, bool wordend);
  void GetNext(const char& c, DictNodePtr&, bool search_end = false);
  void GetNext2(const char& c, DictNodePtr&, bool lastisinteger , bool search_end = false);
  vector<AdZonePtr> referer_zones_;
  Char2NodeMap node_;
  bool wordend_;
};

class DictTree: public IceUtil::Shared{
public:
  DictTree(){
    root = new DictNode();
  }
  void insert(const string& str, const AdZonePtr& zone_ptr);
  bool search(const string& str, vector<AdZonePtr>& zones);
private:
  DictNodePtr root;
};




///////////////////////////////////////////////////////////////////////////////////////////////
class PointsChecker: public MyUtil::Singleton<PointsChecker>{
public:
  bool Check(const string & day_points,const string & hour_points);
private:
  bool CheckHour(const string & day_points);
  bool CheckDay(const string & hour_points);
};

///////////////////////////////////////////////////////////////////////////////////////////////
class AdGroup;
typedef IceUtil::Handle<AdGroup> AdGroupPtr;
typedef vector<AdGroupPtr> AdGroupSeq;

class AdZone: public IceUtil::Shared {
public:
  AdZone(){
    member_online_status_ = 0;
    am_online_status_ = 0;
    audit_status_ = 0;
    delete_flag_ = 0;
    //MyUtil::TaskManager::instance().schedule(new RandomRotateTimer(this, 10*60*1000));//10分钟
  }
  AdGroupPtr GetRotateGroups(int rotate_index, uint64_t user_key);
  bool AddGroupToRotateList(int rotate_index, const AdGroupPtr& obj);
  int GetRotateSize();
  void ResizeRotateList(int size);
  bool Available();
  AdGroupPtr GetDefaultGroups();
  bool AddGroupToDefaultList(int rotate_index, const AdGroupPtr& obj);
  int GetDefaultSize();
  void ResizeDefaultList(int size);

  bool FindStage(int stage);

  Ice::Long id();
  //bool status();
  int ad_count();
  int weight();
  set<string> referers();
  void set_id(Ice::Long id);
  //void set_status(bool status);
  void set_ad_count(int count);
  void set_stage(int stage);
  void set_weight(int weight);
  void set_referer(const string& ref);
  void set_member_online_status(int member_online_status);
  void set_am_online_status(int am_online_status);
  void set_audit_status(int audit_status);
  void set_delete_flag(int delete_flag);
  int self_rotation_number() const {
    return self_rotation_number_;
  }
  void set_self_rotation_number(int self_rotation_number) {
    this->self_rotation_number_ = self_rotation_number;
  }
  int member_type() const {
    return member_type_;
  }
  void set_member_type(int member_type) {
    this->member_type_ = member_type;
  }
  void random_rotate();
private:
  Ice::Long id_; //zone id
  //bool status_; //是否可用
  int ad_count_; //可显示的广告数
  int weight_; //广告位展示权重
  set<int> stage_;//能展示这个广告位的阶段
  set<string> referer_;//这个广告位请求的referer
  vector<AdGroupPtr> rotate_list_;//广告位轮播的广告们
  IceUtil::RWRecMutex mutex_;
  int update_time_;//轮播随机的时间
  vector<AdGroupPtr> default_list_;
  int self_rotation_number_; // 自助广告所占轮播数
  int member_type_;

  int member_online_status_;
  int am_online_status_;
  int audit_status_;
  int delete_flag_;

};


const static int zone_reload_timestamp = 1 * 60 * 1000;

class AdZonePool: public MyUtil::Singleton<AdZonePool> {
public:
  AdZonePool() {
    trie_ = new DictTree();
  }
  void Init();
  AdZoneDict GetAdZoneOrdered(const string& referer, const int stage, const bool hide = false);
  AdZoneDict GetAdZoneOrdered2(const string& referer, const int stage);
  class ReloadTimer: public MyUtil::Timer {
  public:
    ReloadTimer() :
      Timer(zone_reload_timestamp) {
    }
    ;
    virtual void handle();
  };
private:
  DictTreePtr trie_;
  multimap<string, AdZonePtr> ref_zone_map_;
  map<Ice::Long, AdZonePtr> zone_pool_;
  IceUtil::RWRecMutex mutex_;
};
///////////////////////////////////////////////////////////////////////////////////////////////
class AdMember: public IceUtil::Shared {
public:
  bool Available(int display_type = 1);

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
class AdMemberPool: public MyUtil::Singleton<AdMemberPool> {
public:
  void Init();
  AdMemberPtr GetAdMember(Ice::Long member_id);
  class UpdateCheckTimer: public MyUtil::Timer {
  public:
    UpdateCheckTimer() :
      Timer(1 * 60 * 1000) {
    }
    ;
    virtual void handle();
  };
private:
  map<Ice::Long, AdMemberPtr> member_pool_;
  IceUtil::RWRecMutex mutex_;
};

///////////////////////////////////////////////////////////////////////////////////////////////
//如果Member不可用，请业务系统update campign and ad_group表的status字段为不可用
class AdCampaign: public IceUtil::Shared {
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

  Ice::Long campaign_id_;
  Ice::Long member_id_;
  time_t begin_time_;
  time_t end_time_;
//  Ice::Long running_area_hour_;
//  Ice::Long running_area_day_;
  int sys_status_;
  int member_status_;
  int am_status_;
  int audit_status_;
  bool delete_flag_;

};
typedef IceUtil::Handle<AdCampaign> AdCampaignPtr;

class AdCampaignPool: public MyUtil::Singleton<AdCampaignPool> {
public:
  void Init();
  AdCampaignPtr GetAdCampaign(Ice::Long campaign_id);
  class UpdateCheckTimer: public MyUtil::Timer {
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
///////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////
class AdGroup: public IceUtil::Shared {
public:
  Ice::Long GetNextCreative(int rotate_index);
  bool Available(bool log = false);
  bool HasCreatives();
  void UpdateCampainStatues();
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
  void set_zond_ids(Ice::Long zone_id) {
    zone_ids_.insert(zone_id);
  }
  void set_campaign_status(bool campaign_status) {
    campaign_status_ = campaign_status;
  }
  void set_member_table_status(bool member_table_status){
    member_table_status_ = member_table_status;
  }
  void set_creative_ids(Ice::Long cretive_id) {
    creative_ids_.insert(cretive_id);
  }
  void set_bid_unit_id(Ice::Long bid_unit_id) {
    bid_unit_id_ = bid_unit_id;
  }
  void set_point_valid(bool valid){
    point_valid_ = valid;
  }
  void set_daily_sys_status(int daily_sys_status){
    daily_sys_status_ = daily_sys_status;
  }
  void set_total_sys_status(int total_sys_status){
    total_sys_status_ = total_sys_status;
  }
  void set_rotation_number(int rotation_number){
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
  int rotation_number(){
    return rotation_number_;
  }
  set<Ice::Long> zone_ids() {
    return zone_ids_;
  }
  set<Ice::Long> creative_ids() {
    return creative_ids_;
  }
  Ice::Long bid_unit_id() {
    return bid_unit_id_;
  }
  int daily_sys_status(){
    return daily_sys_status_;
  }
  int total_sys_status(){
    return total_sys_status_;
  }
  bool campaign_status() {
    return campaign_status_;
  }
  bool member_table_status(){
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
    bid_unit_key_ = 0;
    bid_unit_id_ = 0;
    rotation_number_ = 1;
    creative_index_ = 0;
  }
private:
  uint64_t bid_unit_key_;
  Ice::Long bid_unit_id_;
  Ice::Long group_id_;
  Ice::Long member_id_;
  Ice::Long campaign_id_;
  int member_status_;
  int am_status_;
  int audit_status_;
  int delete_flag_;
  int display_type_;//轮播,cpm,cpc
  int trans_type_; // 结算方式：10:cpc，20：cpm, 0: 其他
  int max_price_;
  int daily_sys_status_;
  int total_sys_status_;
  int rotation_number_;
  bool campaign_status_;
  bool member_table_status_;
  set<Ice::Long> zone_ids_;
  set<Ice::Long> creative_ids_;
  int creative_index_;
  bool point_valid_;//是否在排期内

};

class AdGroupPool: public MyUtil::Singleton<AdGroupPool> {
public:
  void Init();
  AdGroupPtr GetAdGroup(Ice::Long group_id);

  map<Ice::Long, vector<AdGroupPtr> > GetRotateAds();
  map<Ice::Long, vector<AdGroupPtr> > GetDefaultAds();
  void UpdateCampainStatues();
  class UpdateCheckTimer: public MyUtil::Timer {
  public:
    UpdateCheckTimer() :
      Timer(1 * 60 * 1000) {
    }
    ;
    virtual void handle();
  };
  class UpdateCampaignStatusTimer: public MyUtil::Timer {
  public:
    UpdateCampaignStatusTimer() :
      Timer(1 * 60 * 1000) {
    }
    ;
    virtual void handle();
  };
private:
  map<Ice::Long, AdGroupPtr> group_pool_;
  map<Ice::Long, vector<AdGroupPtr> > zone2rotate_;
  map<Ice::Long, vector<AdGroupPtr> > zone2default_;
  IceUtil::RWRecMutex mutex_;
};
///////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////

class AdEffectIndex : public IceUtil::Shared{
public:
  uint64_t user_key_;
  Ice::Long group_id_;
  bool operator >(const AdEffectIndex& index) const {
    return group_id_ > index.group_id_;
  }
  bool operator ==(const AdEffectIndex& index) const {
    return (group_id_ == index.group_id_) && (user_key_ == index.user_key_);
  }
  bool operator <(const AdEffectIndex& index) const {
    return group_id_ < index.group_id_;
  }
};
typedef IceUtil::Handle<AdEffectIndex> AdEffectIndexPtr;

class AdEffectTimer : public MyUtil::Timer{
public:
  AdEffectTimer() :
    Timer(1 * 30 * 1000) {
  }
  ;
  virtual void handle();
};

class AdEffectLoader : public MyUtil::Singleton<AdEffectLoader>{
public:
  void AddIndex(const AdEffectIndexPtr& index);
  void Load();
private:
  set<AdEffectIndexPtr> index_set_;
  IceUtil::Mutex mutex_;
};

class AdEffect: public IceUtil::Shared {
public:
  AdEffect() :
    pv_(1), click_(1), pv_inc_(0), click_inc_(0), load_fromdb_(0), last_update_time_(time(NULL)) {
  }
  ;
  AdEffect(Ice::Long pv, Ice::Long click) :
    pv_(pv), click_(click) {
  }
  ;

  double GetClickRate() {
    return (double) click_ / (double) pv_;
  }
  ;
  void set_pv(Ice::Long pv) {
    pv_ = pv;
  }
  void set_click(Ice::Long click) {
    click_ = click;
  }
  void set_load_fromdb(bool load_fromdb){
    load_fromdb_ = load_fromdb;
  }
  bool load_fromdb(){
    return load_fromdb_;
  }
  void IncPv() {
    ++pv_;
    ++pv_inc_;
  }
  void IncClick() {
    ++click_;
    ++click_inc_;
  }
  Ice::Long pv() {
    return pv_;
  }
  Ice::Long click() {
    return click_;
  }
  int pv_inc(){
    return pv_inc_;
  }
  int click_inc(){
    return click_inc_;
  }
  void set_pv_inc(int pv_inc){
    pv_inc_ = pv_inc;
  }
  void set_click_inc(int click_inc){
    click_inc_ = click_inc;
  }
  void reduce_pv_inc(int tmp){
    pv_inc_ -= tmp;
  }
  void reduce_click_inc(int tmp){
    click_inc_ -= tmp;
  }
  int pv_threshold(){
    return 100;
    //return 10000;
    //return 1;
  }
  int click_threshold(){
    return 1;
  }
  time_t last_update_time(){
    return last_update_time_;
  }
  void set_last_update_time(time_t last_update_time){
    last_update_time_ = last_update_time;
  }
private:
  time_t last_update_time_;
  bool load_fromdb_;
  Ice::Long pv_;
  Ice::Long click_;
  int pv_inc_;
  int click_inc_;
};
typedef IceUtil::Handle<AdEffect> AdEffectPtr;

class AdEffectPool: public IceUtil::Shared {
public:
  void StorageInfo(uint64_t key);
  void KickTimeoutEffect();
  AdEffectPtr GetAdEffect(Ice::Long group_id);
  void AddEffect(Ice::Long group_id, const AdEffectPtr& obj);
  void IncPv(Ice::Long group_id);
  void IncClick(Ice::Long group_id);
  uint64_t key(){
    return key_;
  }
  void set_key(uint64_t key){
    key_ = key;
  }
private:
  map<Ice::Long, AdEffectPtr> effect_pool_;
  uint64_t key_;
  IceUtil::RWRecMutex mutex_;
};
typedef IceUtil::Handle<AdEffectPool> AdEffectPoolPtr;

class AdEffectStat: public MyUtil::Singleton<AdEffectStat> {
public:
  AdEffectStat(){
    kick_timestamp_ = 1 * 60 * 60;
    //kick_timestamp_ = 30;
  }
  void Init();
  void StorageInfo();
  void KickTimeoutEffect();
  AdEffectPoolPtr GetEffectPool(uint64_t key);
  void UpdateAdEffect(uint64_t key, Ice::Long group_id, const AdEffectPtr& ptr);
  void IncPv(uint64_t key, Ice::Long group_id);
  void IncClick(uint64_t key, Ice::Long group_id);
  void set_kick_timestamp(time_t kick_timestamp){
    kick_timestamp_ = kick_timestamp;
  }
  time_t kick_timestamp(){
    return kick_timestamp_;
  }
  class StorageInfoTimer: public MyUtil::Timer {
  public:
    StorageInfoTimer() :
      Timer(1 * 60 * 1000) {
    }
    ;
    virtual void handle();
  };
  class KickTimer: public MyUtil::Timer {
  public:
    KickTimer() :
      Timer(120 * 60 * 1000)
        //Timer(1 * 60 * 1000)
    {
    }
    ;
    virtual void handle();
  };
private:
  map<uint64_t, AdEffectPoolPtr> pool_;
  IceUtil::RWRecMutex mutex_;
  time_t kick_timestamp_;
};
///////////////////////////////////////////////////////////////////////////////////////////////

class SchoolPool: public MyUtil::Singleton<SchoolPool> {
public:
  void Init();
  uint64_t GetSchoolAreaCode(Ice::Long school);
private:
  map<Ice::Long, uint64_t> pool_;
  IceUtil::RWRecMutex mutex_;
};

}
}

#endif /* ADSTRUCT_H_ */
