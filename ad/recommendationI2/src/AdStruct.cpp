/*
 * AdStruct.cpp
 *
 *  Created on: Sep 3, 2010
 *      Author: antonio
 */

#include "AdStruct.h"
#include "Date.h"
#include "AdInvertedIndex.h"
#include <bitset>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/regex.hpp>
//#include "AdConfig.h"

namespace xce {
namespace ad {

using namespace MyUtil;
using namespace std;
using namespace boost;

const string DB_INSTANCE = "ade_db";

DictNodePtr DictNode::insert(const char& c, bool wordend) {
  Char2NodeMap::iterator it = node_.find(c);
  if (it == node_.end()) {
    DictNodePtr new_node = new DictNode();
    new_node->wordend_ = wordend;
    node_.insert(make_pair<char, DictNodePtr> (c, new_node));
    return new_node;
  }
  return it->second;
}

void DictNode::GetNext2(const char& c, DictNodePtr& ptr, bool lastisinteger, bool search_end){
  bool match = true;
  ostringstream os;
  if(match){
  //每次来的时候，应该按照字母优先级去匹配，优先级比×去匹配数字的优先级高，不然会出bug
  //正常的匹配逻辑，看是否有要匹配的字符串， 有就将Ptr指向其所对应的位置，没有就让ptr指向NULL
  ////////////////////////////////////////////////////////////////
    Char2NodeMap::iterator it;
    it = ptr->node_.find(c);
    if(it != ptr->node_.end()){
      ptr = it->second;
      os << "match : find " << c;
      MCE_DEBUG(os.str());
      if(search_end){
        if(!it->second->wordend_){
          os << "match: at end but end not empty ";
          MCE_DEBUG(os.str());
          ptr = NULL;
        }
      }
      //正常匹配，如果找到了，那么一定会return
    }
    else{
      //如果没有匹配上来的字母的话，那么要寻找×这个匹配逻辑
      Char2NodeMap::iterator it;
      it = node_.find('*');
      //首先要找×这个字符，看看是不是模糊匹配
      if(it != node_.end()){
        //如果找到了。
        os << " has find * ";
        if(c >= '0' && c <= '9'){
          //而且这次来的是数字，那么我们就会进行模糊匹配
          os << " is integer c = " << c;
          if(search_end){
            //如果这次请求的是结尾
            os << " is search end ";
            if(!it->second->wordend_){
              //但是urlmaping里面不是结尾，那么是错误的 ，不会匹配上的。
              os << " but not wordend;";
              ptr = NULL;
            }
            else{
              os << " and is wordend;";
              //如果是搜索到结尾，要让ptr移动到树里面的结尾， 用以能够最后取到referer_zones_
              ptr = it->second;
            }
          }
          os << " not search end return;";
          MCE_DEBUG(os.str());
          return;
        }
        else{
          //如果不是数字的话
          os << " and is not integer";
          if(lastisinteger){
            //如果上一次是模糊的匹配，也就是上一次匹配成功的是数字
            os << " last is an integer so ptr = ptr->next and match again";
            ptr = it->second;
            //那么就把指针向后移动一个
            it = ptr->node_.find(c);
            //去查有没有这个字符，进行二次匹配
            if(it != ptr->node_.end()){
              //
              ptr = it->second;
              os << "again match : find " << c;
              MCE_DEBUG(os.str());
              if(search_end){
                if(!it->second->wordend_){
                  os << "again match: at end but end not empty ";
                  MCE_DEBUG(os.str());
                  ptr = NULL;
                  return;
                }
              }
              return;
            }
          }
          os << " last is not an integer.....";
        }
        MCE_DEBUG(os.str());
      }
      os << "match : not find " << c << " ptr = NULL";
      MCE_DEBUG(os.str());
      ptr = NULL;
    }
  }
}

void DictNode::GetNext(const char& c, DictNodePtr& ptr, bool search_end){
  bool match = true;
  ostringstream os;
  Char2NodeMap::iterator it;
  it = node_.find('*');
  if(it != node_.end()){
    os << " has find * ";
    if(c >= '0' && c <= '9'){   // url_adzone_maping中的*表示一个整数
      os << " is integer c = " << c;
      if(search_end){
        os << " is search end ";
        if(!it->second->wordend_){
          os << " but not wordend;";
          ptr = NULL;
        }
        else{
          os << " and is wordend;";
          //如果是搜索到结尾，要让ptr移动到树里面的结尾， 用以能够最后取到referer_zones_
          ptr = it->second;
        }
      }
      os << " not search end return;";
      return;
    }
    os << " and is not integer ptr = ptr->next goto match;";
    ptr = it->second;
  }
  if(match){
    //正常的匹配逻辑，看是否有要匹配的字符串， 有就将Ptr指向其所对应的位置，没有就让ptr指向NULL
    it = ptr->node_.find(c);
    if(it != ptr->node_.end()){
      ptr = it->second;
      os << "match : find " << c;
      if(search_end){
        if(!it->second->wordend_){
          os << "match: at end but end not empty ";
          MCE_DEBUG(os.str());
          ptr = NULL;
        }
      }
    }
    else{
      os << "match : not find " << c << " ptr = NULL";
      ptr = NULL;
    }
  }
}

void DictTree::insert(const string& str, const AdZonePtr& zone_ptr) {
  MCE_INFO("DictTree::insert --> str = " << str << " zone_id = "
      << zone_ptr->id());
  int len = str.length();
  DictNodePtr index = root_;
  for (int i = 0; i < len; ++i) {
    index = index->insert(str.at(i), (i == len - 1));
  }
  index->referer_zones_.push_back(zone_ptr);
}

bool DictTree::search(const string& str, vector<AdZonePtr>& zones) {
  bool ans = true;
  int len = str.length();
  bool lastisinteger = false;
  DictNodePtr index = root_;
  for (int i = 0; i < len; ++i) {
    MCE_DEBUG("DictTree::search --> search i = " << i << " char = "
        << str.at(i));
    //index->GetNext(str.at(i), index, (i == len - 1));
    if(i){
      lastisinteger = (str.at(i - 1) >= '0' && str.at(i - 1) <= '9');
    }
    index->GetNext2(str.at(i), index, lastisinteger, (i == len - 1));
    if (!index) {
      ans = false;
      break;
    }
  }
  if (ans) {
    zones = index->referer_zones_;
  }
  MCE_INFO("DictTree::search --> str = " << str << " ans = " << ans);
  return ans;
}

/***********************************************************************************************************************/
bool PointsChecker::Check(const string & day_points, const string & hour_points) {
  MCE_DEBUG("PointsChecker::Check --> day_points = " << day_points
      << ",hour_points = " << hour_points);
  bool valid = CheckDay(day_points) && CheckHour(hour_points);
  MCE_INFO("PointsChecker::Check --> valid = " << valid);
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
  MCE_DEBUG("PointsChecker::CheckHour --> this_hour = " << this_hour
      << ", hours_points = " << hour_points << ",result = " << result);
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
    MCE_DEBUG("PointsChecker::CheckDay --> this year:month:day = " << this_year
        << ":" << this_month << ":" << this_day << ",days = " << mit->second
        << ",result " << result);
    return result;
  } else {
    return false;
  }
}

/***********************************************************************************************************************/
bool AdZone::Available() {
  bool result = false;
  result = (member_online_status_ == 1) && (am_online_status_ == 1)
      && (audit_status_ == 1) && (delete_flag_ == 1);
  return result;
}

bool AdZone::AddGroupToRotateList(int rotate_index, const AdGroupPtr& obj) {
  if (rotate_index >= 0 && rotate_index < (int) rotate_list_.size()) {
    rotate_list_.at(rotate_index) = obj;
    MCE_DEBUG("AdZone::AddGroupToRotateList --> zone_id = " << id_
        << "  rotate_index = " << rotate_index << " success group_id = "
        << obj->group_id());
    return true;
  }
  MCE_INFO("AdZone::AddGroupToRotateList --> rotate_index = " << rotate_index
      << " biger than rotate_list_.size = " << rotate_list_.size());
  return false;
}

AdGroupPtr AdZone::GetRotateGroups(int rotate_index, uint64_t user_key) {
  //MCE_INFO("AdZone::GetRotateGroups --> zone_id = " << id_ << " rotate_list_.size = " << GetRotateSize() << " rotate_index = " << rotate_index);
  AdGroupPtr ans = NULL;
  if (GetRotateSize() && (rotate_index >= 0 && rotate_index < (int) rotate_list_.size())) {
    if (rotate_list_[rotate_index]) {
      MCE_INFO("AdZone::GetRotateGroups --> user_key = " << user_key << " bid_unit_key = " << rotate_list_[rotate_index]->bid_unit_key());
      if (((Translator::instance().Match(user_key, rotate_list_[rotate_index]->bid_unit_key())))) {
        MCE_DEBUG("AdZone::GetRotateGroups --> matched success rotate_index = " << rotate_index << " user_key = " << user_key);
        ans = rotate_list_[rotate_index];
      } else {
        MCE_INFO("AdZone::GetRotateGroups --> user_key could not match adgroup->bidunit_key");
      }
    } else {
      MCE_INFO("AdZone::GetRotateGroups --> index = " << rotate_index
          << " no rotate ad, you can get cmp from Inve!");
    }
  }
  return ans;
}

int AdZone::GetRotateSize() {
  return rotate_list_.size();
}

void AdZone::ResizeRotateList(int size) {
  rotate_list_.resize(size);
}

bool AdZone::AddGroupToDefaultList(int default_index, const AdGroupPtr& obj) {
  if (default_index >= 0 && default_index < (int) default_list_.size()) {
    default_list_.at(default_index) = obj;
    MCE_DEBUG("AdZone::AddGroupToDefaultList --> zone_id = " << id_ << "  default_index = " << default_index << " success");
    return true;
  }
  MCE_INFO("AdZone::AddGroupToDefaultList --> default_index = " << default_index
      << " biger than default_list_.size = " << default_list_.size());
  return false;
}

AdGroupPtr AdZone::GetDefaultGroups() {
  AdGroupPtr ans = NULL;
  if (GetDefaultSize()) {
    int default_index = rand() % GetDefaultSize();
    if (default_index >= 0 && default_index < (int) default_list_.size()) {
      if (default_list_[default_index]) {
        ans = default_list_[default_index];
        //MCE_INFO("AdZone::GetDefaultGroups --> get success default_index = " << default_index);
      } else {
        MCE_INFO("AdZone::GetDefaultGroups --> index = " << default_index << " no default ad");
      }
    }
  }
  return ans;
}

set<AdGroupPtr> AdZone::GetDefaultGroups(int count) {
  set<AdGroupPtr> ans;
  int default_size = GetDefaultSize();
  if (default_size <= count) {
    ans.insert(default_list_.begin(), default_list_.end());
  } else {
    int now_index = rand() % default_size;
    for (int i = 0; i < count; i++) {
      ans.insert(default_list_[now_index%default_size]);
      now_index++;
    }
  }
  return ans;
}

int AdZone::GetDefaultSize() {
  return default_list_.size();
}

void AdZone::ResizeDefaultList(int size) {
  default_list_.resize(size);
}

bool AdZone::FindStage(int stage) {
  return stage_.find(stage) != stage_.end();
}

Ice::Long AdZone::id() {
  return id_;
}

//int AdZone::ad_count() {
//  return ad_count_;
//}

int AdZone::weight() {
  return weight_;
}

set<string> AdZone::referers() {
  return referer_;
}

void AdZone::set_id(Ice::Long id) {
  id_ = id;
}

//void AdZone::set_ad_count(int ad_count) {
//  ad_count_ = ad_count;
//}

void AdZone::add_stage(int stage) {
  stage_.insert(stage);
}

void AdZone::set_weight(int weight) {
  weight_ = weight;
}

void AdZone::set_referer(const string& ref) {
  referer_.insert(ref);
}

void AdZone::set_member_online_status(int member_online_status) {
  member_online_status_ = member_online_status;
}
void AdZone::set_am_online_status(int am_online_status) {
  am_online_status_ = am_online_status;
}
void AdZone::set_audit_status(int audit_status) {
  audit_status_ = audit_status;
}
void AdZone::set_delete_flag(int delete_flag) {
  delete_flag_ = delete_flag;
}

bool AdZone::AddGroupToGlobalList(int rotate_index, const AdGroupPtr& obj) {
  if (rotate_index >= 0 && rotate_index < (int) global_list_.size()) {
    global_list_.at(rotate_index) = obj;
    MCE_DEBUG("AdZone::AddGroupToGlobalList --> zone_id = " << id_
        << "  rotate_index = " << rotate_index << " success group_id = "
        << obj->group_id());
    return true;
  }
  MCE_INFO("AdZone::AddGroupToGlobalList --> rotate_index = " << rotate_index
      << " biger than rotate_list_.size = " << rotate_list_.size());
  return false;

}

set<AdGroupPtr> AdZone::GetGlobalGroups(const int count) {
  set<AdGroupPtr> ans;
  int global_size = global_list_.size();
  MCE_DEBUG("[GLOBAL] AdZone::GetGlobalGroups --> zone id = " << id_ << ", global_size = " << global_size);
  if (global_size <= count) {
    ans.insert(global_list_.begin(), global_list_.end());
  } else {
    int now_index = rand() % global_size;
    for (int i = 0; i < count; i++) {
      ans.insert(global_list_[now_index%global_size]);
      now_index++;
    }
  }
  return ans;
}

void AdZonePool::LoadAdzone(map<Ice::Long, AdZonePtr> & zones) {
  Statement sql;
  sql << "select adzone_id, member_online_status, audit_status, delete_flag, "
      "am_online_status, count, bp_count, priority, rotation_number "
      "from adzone where member_type = 0 or member_type = 2";
  // const string DB_INSTANCE = EngineConfig::instance().DB();
  mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer, "adzone").store(sql);
  if (res && res.num_rows() > 0) {
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      AdZonePtr zone = new AdZone();
      zone->set_id((Ice::Long) row["adzone_id"]);
      zone->set_member_online_status((int) row["member_online_status"]);
      zone->set_am_online_status((int) row["am_online_status"]);
      zone->set_audit_status((int) row["audit_status"]);
      zone->set_delete_flag((int) row["delete_flag"]);
      zone->set_brand_count((int) res[i]["count"]);
      zone->set_bp_count((int) res[i]["bp_count"]);
      zone->set_weight((int) res[i]["priority"]);

      string rotation_number = (string) res[i]["rotation_number"];
      vector<uint64_t> v = CommonUtil::instance().Split(rotation_number, boost::char_separator<char>(", ")); // 在逗号","后面加一个空格变为", "，可将空格也视为分隔符
      int sum = 0;
      for (vector<uint64_t>::iterator it = v.begin(); it != v.end(); ++it) {
        sum += (int) (*it);
      }
      zone->ResizeRotateList(sum);
      if (zone->Available()) {
        zones.insert(make_pair<Ice::Long, AdZonePtr> (zone->id(), zone));
      }
    }
  } else {
    MCE_INFO("AdZonePool::init --> no data in table adzone");
  }
}

void AdZonePool::LoadAdgroupAdzoneRef(map<Ice::Long, AdZonePtr> & zones) {
  map<Ice::Long, vector<AdGroupPtr> > zone2group = AdGroupPool::instance().GetRotateAds();
  map<Ice::Long, vector<AdGroupPtr> > zone2default = AdGroupPool::instance().GetDefaultAds();

  map<Ice::Long, vector<AdGroupPtr> > zone2global = AdGroupPool::instance().GetGlobalAds();

  MCE_INFO("AdZonePool::Init --> zone2group.size() = " << zone2group.size());
  MCE_INFO("AdZonePool::Init --> zone2default.size() = " << zone2default.size());
  if (!zone2group.empty()) {
    MCE_INFO("AdZonePool::Init --> zone2group in");
    //对于每个adzone放入轮播广告
    for (map<Ice::Long, vector<AdGroupPtr> >::iterator it = zone2group.begin(); it != zone2group.end(); ++it) {
      Ice::Long zone_id = it->first;
      int groups_size = it->second.size();
      map<Ice::Long, AdZonePtr>::iterator zoneit = zones.find(zone_id);
      if (zoneit != zones.end()) {
        AdZonePtr zone = zoneit->second;
        int rotate_size = zone->GetRotateSize();
        if (groups_size >= rotate_size) {//轮播广告个数>ad_count 重新设置轮播空间大小
          zone->ResizeRotateList(groups_size);
          MCE_DEBUG("AdZonePool::Init --> zone2group zone_id = " << zone_id << " resize = " << groups_size);
        }

        //依次放入轮播广告
        int index = 0;
        for (vector<AdGroupPtr>::iterator groupit = it->second.begin(); groupit
            != it->second.end(); ++groupit, ++index) {
          if (*groupit && !zone->AddGroupToRotateList(index, *groupit)) {
            MCE_INFO("AdZonePool::init --> add Carousel AdGroupPtr to Rotate error, AdZone->RotateSize() = "
                    << zone->GetRotateSize() << " index = " << index);
          }
        }
      } else {
        MCE_INFO("AdZonePool::init --> adgroup_adzone_ref has zoneid = "
            << zone_id << " but miss in adzone");
      }
    }
  } else {
    MCE_INFO("AdZonePool::init --> adzone2rotate is empty");
  }
  if (!zone2default.empty()) {
    //对于每个adzone放入默认广告
    for (map<Ice::Long, vector<AdGroupPtr> >::iterator it = zone2default.begin(); it != zone2default.end(); ++it) {
      Ice::Long zone_id = it->first;
      map<Ice::Long, AdZonePtr>::iterator zoneit = zones.find(zone_id);
      if (zoneit != zones.end()) {
        AdZonePtr zone = zoneit->second;
        zone->ResizeDefaultList(it->second.size());
        int index = 0;
        for (vector<AdGroupPtr>::iterator groupit = it->second.begin(); groupit
            != it->second.end(); ++groupit, ++index) {
          if (*groupit && !zone->AddGroupToDefaultList(index, *groupit)) {
            MCE_INFO( "AdZonePool::init --> add Default AdGroupPtr to Default error, AdZone->DefaultSize() = "
                    << zone->GetDefaultSize() << " index = " << index);
          }
        }
      } else {
        MCE_INFO("AdZonePool::init --> adgroup_adzone_ref has zoneid = " << zone_id << " but miss in adzone");
      }
    }
  } else {
    MCE_INFO("AdZonePool::init --> adzone2default is empty");
  }
  if (!zone2global.empty()) {
    //对于每个adzone放入默认广告
    for (map<Ice::Long, vector<AdGroupPtr> >::iterator it = zone2global.begin(); it != zone2global.end(); ++it) {
      Ice::Long zone_id = it->first;
      map<Ice::Long, AdZonePtr>::iterator zoneit = zones.find(zone_id);
      if (zoneit != zones.end()) {
        AdZonePtr zone = zoneit->second;
        zone->ResizeGlobalList(it->second.size());
        int index = 0;
        for (vector<AdGroupPtr>::iterator groupit = it->second.begin(); groupit
            != it->second.end(); ++groupit, ++index) {
          if (*groupit && !zone->AddGroupToGlobalList(index, *groupit)) {
            MCE_INFO( "AdZonePool::init --> add Global AdGroupPtr to Default error, AdZone->GlobalSize() = "
                    << zone->GetGlobalSize() << " index = " << index);
          }
        }
      } else {
        MCE_INFO("AdZonePool::init --> adgroup_adzone_ref has zoneid = " << zone_id << " but miss in adzone");
      }
    }
  } else {
    MCE_INFO("AdZonePool::init --> zone2global is empty");
  }
}

void AdZonePool::LoadUrlAdzoneMaping(map<Ice::Long, AdZonePtr> & zones,
    DictTreePtr & trie, multimap<string, AdZonePtr> & ref_zone_map) {
  Statement sql;
  sql << "select url, stage, adzone_id from url_adzone_maping";
  // const string DB_INSTANCE = EngineConfig::instance().DB();
  mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer, "url_adzone_maping").store(sql);
  if (res && res.num_rows() > 0) {
    for (size_t i = 0; i < res.num_rows(); ++i) {
      string referer = (mysqlpp::Null<string, mysqlpp::NullIsBlank>) res[i]["url"];
      Ice::Long zone_id = (Ice::Long) res[i]["adzone_id"];
      map<Ice::Long, AdZonePtr>::iterator it = zones.find(zone_id);
      if (it != zones.end()) {
        it->second->set_referer(referer);
        it->second->add_stage((int) res[i]["stage"]);
        ref_zone_map.insert(make_pair<string, AdZonePtr> (referer, it->second));
        trie->insert(referer, it->second);
      } else {
        MCE_INFO("AdZonePool::init --> url_adzone_maping has zone_id = "
            << zone_id << " but miss in adzone!!!!");
      }
    }
  } else {
    MCE_INFO("AdZonePool::init --> no data in table url_adzone_maping");
  }

  MCE_INFO("AdZonePool::Init --> AFTER INIT zones.size = " << zones.size()
      << " ref_zone_map.size = " << ref_zone_map.size());
}
void AdZonePool::Init() {
  MCE_INFO("AdZonePool::init");
  map<Ice::Long, AdZonePtr> zones;
  LoadAdzone(zones);

  LoadAdgroupAdzoneRef(zones);

  DictTreePtr trie = new DictTree();
  multimap<string, AdZonePtr> ref_zone_map;
  LoadUrlAdzoneMaping(zones, trie, ref_zone_map);

  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    zone_pool_.swap(zones);
    ref_zone_map_.swap(ref_zone_map);
    trie_ = trie;
  }
}

AdZoneDict AdZonePool::GetAdZoneOrdered(const string& referer, const UserProfile& userprofile) {
  AdZoneDict ans;
  int user_stage = userprofile.stage();
  if(referer.empty()){
    return ans;
  }
  MCE_DEBUG("AdZonePool::GetAdZoneOrdered -->  referer = " << referer);
  vector<string> maybe;
  vector<AdZonePtr> trie_zones;
  int len = referer.length();
  bool trie_ans = false;
  string ok_referer = "";
  maybe.push_back(referer);
  for (int i = len - 1; i; --i) {
    if (referer.at(i) == '?' || referer.at(i) == '/') {
      maybe.push_back(referer.substr(0, i));
    }
  }
  {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    for (vector<string>::iterator it = maybe.begin(); it != maybe.end(); ++it) {
      MCE_DEBUG("AdZonePool::GetAdZoneOrdered --> trie.search url = " << (*it));
      try {
        trie_ans = trie_->search(*it, trie_zones);
      } catch (std::exception& e) {
        MCE_WARN("AdZonePool::GetAdZoneOrdered --> trie.search ERR  line:"
            << __LINE__ << " err:" << e.what());
      }
      if (trie_ans) {
        ok_referer = *it;
        MCE_DEBUG("AdZonePool::GetAdZoneOrdered --> trie.search : " << *it
            << " OK trie_zones.size = " << trie_zones.size());
        bool is_match = false;
        for(vector<AdZonePtr>::iterator trie_zone_it = trie_zones.begin(); trie_zone_it != trie_zones.end(); ++trie_zone_it){
          MCE_INFO("AdZonePool::GetAdZoneOrdered --> trie_zones.id = " << (*trie_zone_it)->id());
          if((*trie_zone_it)->Available() && (*trie_zone_it)->FindStage(user_stage)) {
            is_match = true;
          }
        }
        //break;
        if(is_match) {
          break; //匹配到maybe中某一个url，跳出循环
        } else {
          continue; //没有匹配到该url，继续循环
        }
      } else {
        MCE_INFO("AdZonePool::GetAdZoneOrdered --> trie.search : " << *it << " FAILED");
      }
    }
  }

  for (vector<AdZonePtr>::iterator it = trie_zones.begin(); it != trie_zones.end(); ++it) {
    bool available = (*it)->Available();
    if (available && (*it)->FindStage(user_stage)) {
      MCE_DEBUG("AdZonePool::GetAdZoneOrdered --> after equal_range referer = "
          << ok_referer << " find userstage = " << user_stage << " OK");
      ans.insert(make_pair<int, AdZonePtr> ((*it)->weight(), (*it)));
    } else {
      MCE_DEBUG("AdZonePool::GetAdZoneOrdered --> after equal_range referer = "
          << ok_referer << " find userstage = " << user_stage << " NOT MATCH");
    }
  }

  if (!ans.empty()) {
    MCE_DEBUG("AdZonePool::GetAdZoneOrdered --> ans.size = " << ans.size()
        << " referer = " << referer << " okreferer = " << ok_referer
        << " user.id = " << userprofile.id() << " user.stage = "
        << userprofile.stage() << " user.grade = " << userprofile.grade()
        << " user.gender = " << userprofile.gender() << " user.age = "
        << userprofile.age() << " user.school = " << userprofile.school()
        << " user.home_area = " << userprofile.home_area()
        << " user.current_area = " << userprofile.current_area());
    return ans;
  } else {
    MCE_INFO("AdZonePool::GetAdZoneOrdered --> myreferer = " << referer
        << " could not find any adzone");
  }

  return ans;
}

void AdZonePool::ReloadTimer::handle() {
  AdZonePool::instance().Init();
}

/***********************************************************************************************************************/
bool AdGroup::Available() {
  bool result = false;
  result = member_table_status_ && campaign_status_ && (member_status_ == 1)
      && (am_status_ == 1) && (audit_status_ == 1) && (delete_flag_ == 1)
      && (daily_sys_status_ == 1) && (total_sys_status_ == 1) && point_valid_ 
      && ((max_price_ >= 100) || (member_id_ == 999))
      && (FilteredGroupList::instance().IsNotExist(group_id_));

  MCE_DEBUG("~~~~~~~~~~~~~~~~~AdGroup::Available --> RESULT = " << result
      << " group_id = " << group_id_ << " member_table_status_ = "
      << member_table_status_ << " campain_statues_ = " << campaign_status_
      << " member_status_ = " << member_status_ << " am_status_ = "
      << am_status_ << " audit_status_ = " << audit_status_
      << " delete_flag_ = " << delete_flag_ << " daily_sys_status_"
      << daily_sys_status_ << " total_sys_status_ = " << total_sys_status_
      << " point_valid_ = " << point_valid_ << " >> rotation_number = "
      << rotation_number_);
  return result;
}

bool AdGroup::HasCreatives() {
  bool result = !(creative_ids_.empty());
  //MCE_DEBUG("AdGroup::HasCreatives --> RESULT = " << result );
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
    // MCE_INFO("AdGroupPool::GetGetAdGroup --> miss gruop_id = " << group_id);
  }
  return ans;
}

map<Ice::Long, vector<AdGroupPtr> > AdGroupPool::GetRotateAds() {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  return zone2rotate_;
}
map<Ice::Long, vector<AdGroupPtr> > AdGroupPool::GetDefaultAds() {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  return zone2default_;
}

void AdGroupPool::LoadAdGroup(hash_map<Ice::Long, AdGroupPtr> & group_pool) {
  Statement sql;
  sql << "select total_sys_status, daily_sys_status, display_type, trans_type, adgroup_id, campaign_id, member_id, max_price, member_status, am_status, audit_status, delete_flag,day_points,hour_points, rotation_number from adgroup ";
  //const string DB_INSTANCE = EngineConfig::instance().DB();
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
      int trans_type = (int) row["trans_type"];
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
      group->set_trans_type(trans_type);
      group->set_point_valid(point_valid);
      group->set_daily_sys_status(daily_sys_status);
      group->set_total_sys_status(total_sys_status);
      group->set_rotation_number(rotation_number);
      if (group->Available()) {
        group_pool[group_id] = group;
      } else {
        MCE_DEBUG(
            "AdGroupPool::Init --> because obj->Available is false so do not push it");
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
  // const string DB_INSTANCE = EngineConfig::instance().DB();
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
          MCE_INFO("AdGroupPool::Init --> creative has group_id = " << group_id << " but adgroup no!");
        }
      }
    }
  }
}
void AdGroupPool::CheckIfHaveCreative(hash_map<Ice::Long, AdGroupPtr> & group_pool) {
  for (hash_map<Ice::Long, AdGroupPtr>::iterator git = group_pool.begin(); git != group_pool.end();) {
    if (!git->second->HasCreatives()) {
      MCE_DEBUG("AdGroupPool::Init --> because obj->HasCreatives is false so erase it  groupid = " << git->first);
      group_pool.erase(git++);
    } else{
      ++git;
    }
  }
}
void AdGroupPool::LoadAdgroupAdzoneRef(hash_map<Ice::Long, AdGroupPtr> & group_pool,
    map<Ice::Long, vector<AdGroupPtr> > & zone2rotate,
    map<Ice::Long, vector<AdGroupPtr> > & zone2default) {
  //load adzone_ids;
  Statement sql;
  sql << "select adzone_id, member_id, adgroup_id, member_status, am_status, delete_flag from adgroup_adzone_ref";
  //const string DB_INSTANCE = EngineConfig::instance().DB();
  mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer, "adgroup_adzone_ref").store(sql);
  if (res && res.num_rows() > 0) {
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      Ice::Long group_id = (Ice::Long) row["adgroup_id"];
      Ice::Long zone_id = (Ice::Long) row["adzone_id"];
      Ice::Long member_id = (Ice::Long) row["member_id"];
      int member_status = (int) row["member_status"];
      int am_status = (int) row["am_status"];
      int delete_flag = (int) row["delete_flag"];
      if ((member_status == 1) && (am_status == 1) && (delete_flag == 1)) {
        hash_map<Ice::Long, AdGroupPtr>::iterator group_pool_it = group_pool.find(group_id);
        if (group_pool_it != group_pool.end()) {
          AdGroupPtr group = group_pool_it->second;
          group->add_zone_id(zone_id);
          if ((member_id == 999) && (group->display_type() == 4)) {
            map<Ice::Long, vector<AdGroupPtr> >::iterator it = zone2default.find(zone_id);
            MCE_INFO("AdGroupPool::Init --> add to zone2default : group_id = " << group_id << " zone_id = " << zone_id);
            if (it == zone2default.end()) {
              vector<AdGroupPtr> v;
              zone2default.insert(make_pair<Ice::Long, vector<AdGroupPtr> > (zone_id, v));
            }
            zone2default[zone_id].push_back(group);
          } else if (group->display_type() == 1) {
            map<Ice::Long, vector<AdGroupPtr> >::iterator it = zone2rotate.find(zone_id);
              MCE_INFO("AdGroupPool::Init --> zone2rotate : group_id = " << group_id << " zone_id = " << zone_id);
              if (it == zone2rotate.end()) {
                vector<AdGroupPtr> v;
                zone2rotate.insert(make_pair<Ice::Long, vector<AdGroupPtr> > (zone_id, v));
              }
              int rotation_number = group->rotation_number();
              for (int rotation_index = 0; rotation_index < rotation_number; ++rotation_index) {
                zone2rotate[zone_id].push_back(group);
              }
          }
        } else {
          MCE_INFO("AdGroupPool::Init --> adgroup_adzone_ref has group_id = " << group_id << " but adgroup no!");
        }
      }
    }
  }
  MCE_INFO("AdGroupPool::Init --> zone2default.size = " << zone2default.size() << " zone2rotate.size = " << zone2rotate.size());

  ///////////////////////////////////////////////////////////////
  MCE_INFO("AdGroupPool::Init --> AFTER INIT group_pool.size = " << group_pool.size()
      << " zone2rotate_.size = " << zone2rotate_.size() << " zone2default.size = " << zone2default_.size());
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

  map < Ice::Long, vector<AdGroupPtr> > zone2rotate;
  map < Ice::Long, vector<AdGroupPtr> > zone2default;
  LoadAdgroupAdzoneRef(group_pool, zone2rotate, zone2default);
  MCE_INFO("AdGroupPool::Init --> LoadAdgroupAdzoneRef Over!");

  map<Ice::Long, vector<AdGroupPtr> > zone2global;
  DimIndexMatcher::instance().Load(group_pool, zone2global);

  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    group_pool_.swap(group_pool);
    zone2rotate_.swap(zone2rotate);
    zone2default_.swap(zone2default);
    zone2global_.swap(zone2global);
  }
}
void AdGroupPool::UpdateCampainStatus() {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  for (hash_map<Ice::Long, AdGroupPtr>::iterator it = group_pool_.begin(); it != group_pool_.end(); ++it) {
    it->second->UpdateCampainStatus();
  }
}

void AdGroupPool::UpdateCheckTimer::handle() {
  AdGroupPool::instance().Init();
}
void AdGroupPool::UpdateCampaignStatusTimer::handle() {
  AdGroupPool::instance().UpdateCampainStatus();
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
  //const string DB_INSTANCE = EngineConfig::instance().DB();
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

void AdMemberPool::UpdateCheckTimer::handle() {
  AdMemberPool::instance().Init();
}

/***********************************************************************************************************************/
bool AdCampaign::Available() {
  Date date = Date(time(NULL));
  time_t timestamp = date.time();
  bool time_check = timestamp >= begin_time_ && (!end_time_ || timestamp <= end_time_);
  bool result = delete_flag_ && (audit_status_ == 1) && time_check
      && (member_status_ == 1) && (am_status_ == 1) && (sys_status_ == 1);

  //MCE_INFO("AdCampaign::Available --> current_timestamp = " << timestamp << " plan_begin_timestamp = " << begin_time_ << " plan_end_timestamp = " << end_time_);
  MCE_DEBUG("AdCampaign::Available --> RESULT = " << result
      << " : campaign_id_ = " << campaign_id_ << " delete_flag_ = "
      << delete_flag_ << " audit_status_ = " << audit_status_
      << " time_check = " << time_check << " member_status_ = "
      << member_status_ << " am_status_ = " << am_status_ << " sys_status_ = "
      << sys_status_ << " current_timestamp = " << timestamp
      << " plan_begin_timestamp = " << begin_time_ << " plan_end_timestamp = "
      << end_time_);
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
  //const string DB_INSTANCE = EngineConfig::instance().DB();
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
void AdCampaignPool::UpdateCheckTimer::handle() {
  AdCampaignPool::instance().Init();
}
/***********************************************************************************************************************/

/***********************************************************************************************************************/
void SchoolPool::Init() {
  map < Ice::Long, uint64_t > pool;
  Statement sql;
  sql << "select * from school";
  //const string DB_INSTANCE = EngineConfig::instance().DB();
  mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer, "school").store(
      sql);
  if (res && res.num_rows() > 0) {
    //    IceUtil::RWRecMutex::WLock lock(mutex_);
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      Ice::Long school = (Ice::Long) row["id"];
      uint64_t area = (uint64_t) row["area_code"];
      pool[school] = area;
    }
  }
  MCE_DEBUG("SchoolPool::Init --> load done , pool size = " << pool_.size());
  MCE_DEBUG("SchoolPool::Init --> load done , pool size = " << pool.size());
  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    pool_.swap(pool);
  }
}

uint64_t SchoolPool::GetSchoolAreaCode(Ice::Long school) {
  {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    map<Ice::Long, uint64_t>::iterator mit = pool_.find(school);
    if (mit != pool_.end()) {
      return mit->second;
    }
  }
  return 0;
}

/***********************************************************************************************************************/
void CompeteRef::Init() {
  Load();
  is_ready_ = true;
}
bool CompeteRef::is_ready() {
  return is_ready_;
}

void CompeteRef::Load() {

  Statement sql;
  sql << "select * from ad_compete_ref ";
  mysqlpp::StoreQueryResult res;
  try {
    //const string DB_INSTANCE = EngineConfig::instance().DB();
    res = QueryRunner(DB_INSTANCE, CDbRServer, "ad_compete_ref").store(sql);//TODO
  } catch (Ice::Exception& e) {
    MCE_WARN("CompeteRef::Init :" << e);
  } catch (std::exception& e) {
    MCE_WARN("CompeteRef::Init :" << e.what());
  }

  if (!res) {
    MCE_INFO("CompeteRef::Init --> StoryQueryResult wrong");
    return;
  }
  if (res.empty()) {
    MCE_INFO("CompeteRef::Init --> nothing in db ");
  }
  map < Ice::Long, vector<Ice::Long> > tmpref;
  for (size_t i = 0; i < res.size(); ++i) {
    Ice::Long cp_a = (Ice::Long)(res.at(i)["first_campaign_id"]);
    Ice::Long cp_b = (Ice::Long)(res.at(i)["second_campaign_id"]);
    tmpref[cp_a].push_back(cp_b);
    tmpref[cp_b].push_back(cp_a);
    //MCE_DEBUG("CompeteRef::Load --> add cp_a : " << cp_a << " cmp " << " cp_b : " << cp_b);
  }
  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    ref_.swap(tmpref);
    MCE_INFO("CompeteRef::Load --> load done,tmpref size = " << ref_.size());
  }

}

vector<Ice::Long> CompeteRef::Get(Ice::Long campaign) {
  if (is_ready_) {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    map<Ice::Long, vector<Ice::Long> >::iterator mit = ref_.find(campaign);
    if (mit != ref_.end()) {
      MCE_DEBUG("CompeteRef::Get --> success campaign = " << campaign
          << " ans.size() = " << mit->second.size());
      return mit->second;
    }
    MCE_DEBUG("CompeteRef::Get --> FAILED no competes with campaign id = "
        << campaign);
  }
  return vector<Ice::Long> ();
}

void CompeteRef::ReloadTimer::handle() {
  if (!CompeteRef::instance().is_ready()) {
    MCE_INFO("CompeteRef::Get --> is not ready");
    return;
  }
  CompeteRef::instance().Load();
}

/***********************************************************************************************************************/
void UserBindPool::Init() {
  UserBindMap user_bind_cache;
  Statement sql;
  sql << "select * from ad_user_bind";
  //const string DB_INSTANCE = EngineConfig::instance().DB();
  mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer, "ad_user_bind").store(sql);
  if (res && res.num_rows() > 0) {
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      int user_id = (int) row["userid"];
      Ice::Long creative_id = (Ice::Long) row["creative_id"];
      Ice::Long zone_id = (Ice::Long) row["adzone_id"];
      UserBind user_bind;
      user_bind.set_user_id(user_id);
      user_bind.set_adzone_id(zone_id);
      user_bind_cache[user_bind] = creative_id;
    }
  }
  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    user_bind_cache_.swap(user_bind_cache);
  }
}
void UserBindPool::BindUser(int user_id, Ice::Long creative_id,
    Ice::Long zone_id) {
  MCE_INFO("UserBindPool::BindUser --> userid = " << user_id
      << " creativeid = " << creative_id << " zoneid = " << zone_id);
  Statement sql;
  ostringstream os;
  os << "insert into ad_user_bind (userid, creative_id, adzone_id) values("
      << user_id << "," << creative_id << ", " << zone_id
      << ") on duplicate key update creative_id = values(creative_id)";
  MCE_INFO("UserBindPool::BindUser --> userid = " << user_id
      << " creativeid = " << creative_id << " zoneid = " << zone_id << " sql = "
      << os.str());
  sql << os.str();
  //const string DB_INSTANCE = EngineConfig::instance().DB();
  mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbWServer, "ad_user_bind").store(sql);
}
void UserBindPool::UnbindUser(int user_id, Ice::Long zone_id) {
  MCE_INFO("UserBindPool::UnbindUser --> userid = " << user_id);
  Statement sql;
  ostringstream os;
  os << "delete from ad_user_bind where userid = " << user_id
      << " and adzone_id = " << zone_id;
  MCE_INFO("UserBindPool::UnbindUser --> userid = " << user_id << " zoneid = "
      << zone_id << " sql = " << os.str());
  sql << os.str();
  //const string DB_INSTANCE = EngineConfig::instance().DB();
  mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbWServer, "ad_user_bind").store(sql);
}
void UserBindPool::NotifyBindUser(int user_id, Ice::Long creative_id, Ice::Long zone_id) {
  MCE_INFO("UserBindPool::NotifyBindUser --> userid = " << user_id
      << " creativeid = " << creative_id << " zoneid = " << zone_id);
  IceUtil::RWRecMutex::WLock lock(mutex_);
  UserBind user_bind;
  user_bind.set_user_id(user_id);
  user_bind.set_adzone_id(zone_id);
  UserBindMap::iterator it = user_bind_cache_.find(user_bind);
  if (it == user_bind_cache_.end()) {
    MCE_INFO("UserBindPool::NotifyBindUser --> add to user_bind_cache_ ul.userid = "
            << user_bind.user_id() << " ul.zoneid = " << zone_id);
    user_bind_cache_.insert(make_pair<UserBind, Ice::Long> (user_bind, creative_id));
  } else {
    it->second = creative_id;
  }
}
void UserBindPool::NotifyUnbindUser(int user_id, Ice::Long zone_id) {
  MCE_INFO("UserBindPool::NotifyUnbindUser --> user_id = " << user_id
      << " zoneid = " << zone_id);
  IceUtil::RWRecMutex::WLock lock(mutex_);
  UserBind user_bind;
  user_bind.set_user_id(user_id);
  user_bind.set_adzone_id(zone_id);
  UserBindMap::iterator it = user_bind_cache_.find(user_bind);
  if (it != user_bind_cache_.end()) {
    user_bind_cache_.erase(it);
  }
}
UserBindPool::iterator UserBindPool::find(UserBind user_bind) {
  return user_bind_cache_.find(user_bind);
}
//UserBindPool::iterator UserBindPool::begin() {
//  return user_bind_cache_.begin();
//}
UserBindPool::iterator UserBindPool::end() {
  return user_bind_cache_.end();
}

} // end namespace ad
} // end namespace xce
