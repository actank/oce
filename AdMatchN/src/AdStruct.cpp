/*
 * AdStruct.cpp
 *
 *  Created on: Sep 3, 2010
 *      Author: antonio
 */
#include "AdMatchManagerI.h"
#include "AdStruct.h"
#include "Date.h"
#include "AdInvertedIndex.h"
#include <bitset>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/regex.hpp>
//先load campain 再group
namespace xce {
namespace ad {
using namespace MyUtil;
using namespace std;
using namespace boost;

DictNodePtr DictNode::insert(const char& c, bool wordend){
  Char2NodeMap::iterator it = node_.find(c);
  if(it == node_.end()){
    DictNodePtr new_node = new DictNode();
    new_node->wordend_ = wordend;
    node_.insert(make_pair<char, DictNodePtr>(c, new_node));
    return new_node;
  }
  else{
    if(wordend){
      it->second->wordend_ = wordend;
    }
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
      //MCE_DEBUG(os.str());
      if(search_end){
        if(!it->second->wordend_){
          os << "match: at end but end not empty ";
          //MCE_DEBUG(os.str());
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
          //MCE_DEBUG(os.str());
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
              //MCE_DEBUG(os.str());
              if(search_end){
                if(!it->second->wordend_){
                  os << "again match: at end but end not empty ";
                  //MCE_DEBUG(os.str());
                  ptr = NULL;
                  return;
                }
              }
              return;
            }
          }
          os << " last is not an integer.....";
        }
        //MCE_DEBUG(os.str());
      }
      os << "match : not find " << c << " ptr = NULL";
      //MCE_DEBUG(os.str());
      ptr = NULL;
    }
  }
}

void DictNode::GetNext(const char& c, DictNodePtr& ptr, bool search_end){
  Char2NodeMap::iterator it;
////////////////////////////////////////////////////////////////
  //查找* 如果找不到，则不更新ptr
  it = node_.find('*');
  if(it != node_.end()){
    //如果找到的话。看这次来匹配的是否是 /
    if('/' != c){
      //如果不是/的话，看是不是搜索到搜索串的结尾
      //如果是结尾，要判断从 * 进来 它后面还有没有要求。就是是否搜索到这个树的结尾，由于指针一直停留在/这个位置，它后面是*，
      //所以要看指针移动到*以后，它是否是后面没有东西了，如果没有了， 证明确实是搜索到了结尾，否则，就是只匹配了一部分。
      if(search_end){
        if(!it->second->wordend_){
          ptr = NULL;
        }
        else{
          //如果是搜索到结尾，要让ptr移动到树里面的结尾， 用以能够最后取到referer_zones_
          ptr = it->second;
        }
      }
      return;
    }
    else{
      //如果是/ 那么证明/*/这个区间的内容已经模糊匹配完毕。让指针移动到*这个位置上。更新 ptr
      ptr = it->second;
    }
  }
  if(ptr){
  //正常的匹配逻辑，看是否有要匹配的字符串， 有就将Ptr指向其所对应的位置，没有就让ptr指向NULL
  ////////////////////////////////////////////////////////////////
    it = ptr->node_.find(c);
    if(it != ptr->node_.end()){
      ptr = it->second;
      if(search_end){
        //看是否结尾，如果是结尾，如果不是结尾，则证明不匹配。
        if(!it->second->wordend_){
          ptr = NULL;
        }
      }
    }
    else{
      ptr = NULL;
    }
  }
}


void DictTree::insert(const string& str, const AdZonePtr& zone_ptr){
  //MCE_INFO("DictTree::insert --> str = " << str << " zone_id = " << zone_ptr->id());
  int len = str.length();
  DictNodePtr index = root;
  for(int i = 0; i < len; ++i){
    index = index->insert(str.at(i), (i == len - 1) );
  }
  index->referer_zones_.push_back(zone_ptr);
}


bool DictTree::search(const string& str, vector<AdZonePtr>& zones){
  bool ans = true;
  bool lastisinteger = false;
  int len = str.length();
  DictNodePtr index = root;
  for(int i = 0; i < len; ++i){
    if(i){
      lastisinteger = (str.at(i - 1) >= '0' && str.at(i - 1) <= '9');
    }
    //MCE_DEBUG("DictTree::search --> search i = " << i << " char = " << str.at(i));
    index->GetNext2(str.at(i), index, lastisinteger, (i == len - 1));
    if(!index){
      ans = false;
      break;
    }
  }
  if(ans){
    zones = index->referer_zones_;
  }
  //MCE_INFO("DictTree::search --> str = " << str << " ans = " << ans);
  return ans;
}


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
bool PointsChecker::Check(const string & day_points, const string & hour_points) {
  //MCE_DEBUG("PointsChecker::Check --> day_points = " << day_points << ",hour_points = " << hour_points);
  bool valid = CheckDay(day_points) && CheckHour(hour_points);
  //MCE_INFO("PointsChecker::Check --> valid = " << valid);
  return valid;
}

bool PointsChecker::CheckHour(const string & hour_point_str) {
  if (hour_point_str.empty()) {
    return true;
  }
  bitset<24> hour_points;
  try{
    hour_points = lexical_cast<int> (hour_point_str);
  }
  catch(...){
    MCE_WARN("PointsChecker::CheckHour --> boost cast hour_point_str = " << hour_point_str << " error");
  }
  Date date = Date(time(NULL));
  int this_hour = date.hour();
  bool result = hour_points.test(this_hour);
  //MCE_DEBUG("PointsChecker::CheckHour --> this_hour = " << this_hour << ", hours_points = " << hour_points << ",result = " << result);
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
	map<int,bitset<32> > yearmonth_day;
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
    //MCE_DEBUG("PointsChecker::CheckDay --> this year:month:day = " << this_year << ":" << this_month << ":" << this_day << ",days = " << mit->second << ",result " << result);
    return result;
  } else {
    return false;
  }
}
//////////////////////////////////////////////////////////////////////////////////////

bool AdZone::Available(){
  bool result = false;
  result = (member_online_status_ == 1) && (am_online_status_ == 1) && (audit_status_ == 1) && (delete_flag_ == 1);
  return result;
}

bool AdZone::AddGroupToRotateList(int rotate_index, const AdGroupPtr& obj) {
  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    if (rotate_index >= 0 && rotate_index < (int) rotate_list_.size()) {
      rotate_list_.at(rotate_index) = obj;
    }
  }
  return true;
  //MCE_WARN("AdZone::AddGroupToRotateList --> rotate_index = " << rotate_index << " biger than rotate_list_.size = " << rotate_list_.size());
  //return false;
}

AdGroupPtr AdZone::GetRotateGroups(int rotate_index, uint64_t user_key) {
  //MCE_INFO("AdZone::GetRotateGroups --> zone_id = " << id_ << " rotate_list_.size = " << GetRotateSize() << " rotate_index = " << rotate_index);
  AdGroupPtr ans = NULL;
  if (GetRotateSize()) {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    if (rotate_index >= 0 && rotate_index < (int) rotate_list_.size()) {
      if (rotate_list_[rotate_index]) {
        //MCE_INFO("AdZone::GetRotateGroups --> user_key = " << user_key << " bid_unit_key = " << rotate_list_[rotate_index]->bid_unit_key());
        if (((user_key & rotate_list_[rotate_index]->bid_unit_key())
              == user_key)) {
          //MCE_INFO("AdZone::GetRotateGroups --> matched success rotate_index = " << rotate_index << " user_key = " << user_key);
          ans = rotate_list_[rotate_index];
        } else {
          //MCE_INFO("AdZone::GetRotateGroups --> user_key could not match adgroup->bidunit_key");
        }
      } else {
        //MCE_INFO("AdZone::GetRotateGroups --> index = " << rotate_index <<" no rotate ad, you can get cmp from Inve!");
      }
    }
  }
  return ans;
}

int AdZone::GetRotateSize() {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  return rotate_list_.size();
}

void AdZone::ResizeRotateList(int size) {
  IceUtil::RWRecMutex::WLock lock(mutex_);
  rotate_list_.resize(size);
}

bool AdZone::AddGroupToDefaultList(int default_index, const AdGroupPtr& obj) {
  if (default_index >= 0 && default_index < (int) default_list_.size()) {
    default_list_.at(default_index) = obj;
    //MCE_INFO("AdZone::AddGroupToDefaultList --> zone_id = " << id_ << "  default_index = " << default_index << " success");
    return true;
  }
  //MCE_WARN("AdZone::AddGroupToDefaultList --> default_index = " << default_index << " biger than default_list_.size = " << default_list_.size());
  return false;
}

AdGroupPtr AdZone::GetDefaultGroups() {
  AdGroupPtr ans;
  if (GetDefaultSize()) {
    int default_index = rand() % GetDefaultSize();
    if (default_index >= 0 && default_index < (int) default_list_.size()) {
      if (default_list_[default_index]) {
        ans = default_list_[default_index];
        //MCE_INFO("AdZone::GetDefaultGroups --> get success default_index = " << default_index);
      } else {
        MCE_INFO("AdZone::GetDefaultGroups --> index = " << default_index <<" no default ad");
      }
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

/*
bool AdZone::status() {
  return status_;
}
*/

int AdZone::ad_count() {
  return ad_count_;
}

int AdZone::weight() {
  return weight_;
}

set<string> AdZone::referers() {
  return referer_;
}

void AdZone::set_id(Ice::Long id) {
  id_ = id;
}
/*
void AdZone::set_status(bool status) {
  status_ = status;
}*/

void AdZone::set_ad_count(int ad_count) {
  ad_count_ = ad_count;
}

void AdZone::set_stage(int stage) {
  stage_.insert(stage);
}

void AdZone::set_weight(int weight) {
  weight_ = weight;
}

void AdZone::set_referer(const string& ref) {
  referer_.insert(ref);
}

void AdZone::set_member_online_status(int member_online_status){
  member_online_status_ = member_online_status;
}
void AdZone::set_am_online_status(int am_online_status){
  am_online_status_ = am_online_status;
}
void AdZone::set_audit_status(int audit_status){
  audit_status_ = audit_status;
}
void AdZone::set_delete_flag(int delete_flag){
  delete_flag_ = delete_flag;
}

void AdZone::random_rotate() {
  int n_now = time(NULL);
  IceUtil::RWRecMutex::WLock lock(mutex_);
  if(member_type() == 1 && rotate_list_.size() > 0) {
    int num = rand()%rotate_list_.size();
    //MCE_WARN("random_rotate: adzone_id=" << id() << ", num = " << num << ", rotate_list_.size() = " << rotate_list_.size());
    for(int i = 0; i < num; ++i) {
      AdGroupPtr g = rotate_list_.front();
      rotate_list_.push_back(g);
      rotate_list_.erase(rotate_list_.begin());
      //MCE_WARN("random_rotate: i = " << i);
    }
  }
  //MCE_WARN("random_rotate: out!!!");
}

void AdZonePool::Init() {
  ///////////////////////////////////////////////Load adzone//////////////////////////////////////////////////////////////////////////////
  map<Ice::Long, AdZonePtr> zones;
  Statement sql;
  mysqlpp::StoreQueryResult res;
  sql
      << "select adzone_id, member_online_status, audit_status, delete_flag, "
      "am_online_status, count, member_type, priority, rotation_number from adzone "
      "where member_type = 1 or member_type = 0";
  res = QueryRunner(DB_INSTANCE, CDbRServer, "adzone").store(sql);
  if (res && res.num_rows() > 0) {
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      AdZonePtr zone = new AdZone();
      zone->set_id((Ice::Long) row["adzone_id"]);
      zone->set_member_online_status( (int) row["member_online_status"] );
      zone->set_am_online_status( (int) row["am_online_status"] );
      zone->set_audit_status( (int) row["audit_status"] );
      zone->set_delete_flag( (int) row["delete_flag"] );
      zone->set_member_type( (int) row["member_type"] );
/*      zone->set_status((bool) row["member_online_status"]
          && ((int) row["audit_status"] == 1) && (bool) row["delete_flag"]);*/
      zone->set_ad_count((int) res[i]["count"]);
      zone->set_weight((int) res[i]["priority"]);

      string rotation_number = (string) res[i]["rotation_number"];
      vector<uint64_t> v = Translator::instance().ParseAreas(rotation_number);
      if ((int) v.size() >= 3) {
        zone->set_self_rotation_number(v.at(2));
      }
      int sum = 0;
      for (vector<uint64_t>::iterator it = v.begin(); it != v.end(); ++it) {
        sum += (int) (*it);
      }
      zone->ResizeRotateList(sum);
      if(zone->Available()){
        zones.insert(make_pair<Ice::Long, AdZonePtr> (zone->id(), zone));
      }
    }
  } else {
    MCE_WARN("AdZonePool::init --> no data in table adzone");
  }

  ostringstream logs;
  logs << "AdZonePool::Init --> Available zones : ";
  for(map<Ice::Long, AdZonePtr>::iterator log_it = zones.begin(); log_it != zones.end(); ++log_it){
    logs << log_it->first << ", ";
  }
  //MCE_INFO(logs.str());
  logs.str("");

  ///////////////////////////////////////////////Load adgroup_adzone_ref//////////////////////////////////////////////////////////////////////
  sql.clear();
  map<Ice::Long, vector<AdGroupPtr> > zone2group = AdGroupPool::instance().GetRotateAds();
  map<Ice::Long, vector<AdGroupPtr> > zone2default = AdGroupPool::instance().GetDefaultAds();

  //MCE_INFO ("AdZonePool::Init --> zone2group.size() = " << zone2group.size());
  for (map<Ice::Long, vector<AdGroupPtr> >::iterator log_it = zone2group.begin(); log_it != zone2group.end(); ++log_it) {
      logs << "zone2rotate zoneid = " << log_it->first << " size = " << log_it->second.size() << ",";
  }
  //MCE_INFO(" AdZonePool::Init --> zone2group :  " << logs.str());
  logs.str("");

  //MCE_INFO("AdZonePool::Init --> zone2default.size() = " << zone2default.size());
  for (map<Ice::Long, vector<AdGroupPtr> >::iterator log_it = zone2default.begin(); log_it != zone2default.end(); ++log_it) {
   logs << "zone2default zoneid = " << log_it->first << " size = " << log_it->second.size() << ", ";
  }
  //MCE_INFO(" AdZonePool::Init --> zone2default :  " << logs.str());
  logs.str("");

  if (!zone2group.empty()) {
    //对于每个adzone放入轮播广告
    map<Ice::Long, vector<AdGroupPtr> >::iterator it;
    for (it = zone2group.begin(); it != zone2group.end(); ++it) {
      Ice::Long zone_id = it->first;
      int groups_size = it->second.size();
      //MCE_DEBUG("AdZonePool::Init --> zone2group for zone_id = " << zone_id);
      map<Ice::Long, AdZonePtr>::iterator zoneit = zones.find(zone_id);
      if (zoneit != zones.end()) {
        int rotate_size = zoneit->second->GetRotateSize();
        //MCE_DEBUG("AdZonePool::Init --> zone2group zone_id = " << zone_id << " rotate_size = " << rotate_size);
        if (groups_size >= rotate_size) {//轮播广告个数>ad_count 重新设置轮播空间大小
          zoneit->second->ResizeRotateList(groups_size);
          //MCE_DEBUG("AdZonePool::Init --> zone2group zone_id = " << zone_id << " resize = " << groups_size);
        }

        //依次放入轮播广告
        int index = 0;
        for (vector<AdGroupPtr>::iterator groupit = it->second.begin(); groupit
            != it->second.end(); ++groupit, ++index) {
          //MCE_INFO("AdZonePool::Init --> zone_id = " << zone_id << " group_id = " << (*groupit)->group_id());
          if (*groupit && !zoneit->second->AddGroupToRotateList(index, *groupit)) {
            //MCE_WARN("AdZonePool::init --> add Carousel AdGroupPtr to Rotate error, AdZone->RotateSize() = " << zoneit->second->GetRotateSize() << " index = " << index);
          }
        }
        //随机轮转一下轮播的位置
        zoneit->second->random_rotate(); 
      } else {
       // MCE_WARN("AdZonePool::init --> adgroup_adzone_ref has zoneid = " << zone_id << " but miss in adzone");
      }
    }
  } else {
    MCE_WARN("AdZonePool::init --> adzone2rotate is empty");
  }
  if (!zone2default.empty()) {
    //对于每个adzone放入默认广告
    map<Ice::Long, vector<AdGroupPtr> >::iterator it;
    for (it = zone2default.begin(); it != zone2default.end(); ++it) {
      Ice::Long zone_id = it->first;
      int groups_size = it->second.size();
      map<Ice::Long, AdZonePtr>::iterator zoneit = zones.find(zone_id);
      if (zoneit != zones.end()) {
        int default_size = it->second.size();
        zoneit->second->ResizeDefaultList(default_size);
        int index = 0;
        for (vector<AdGroupPtr>::iterator groupit = it->second.begin(); groupit
            != it->second.end(); ++groupit, ++index) {
          if (*groupit && !zoneit->second->AddGroupToDefaultList(index,
              *groupit)) {
            //MCE_WARN("AdZonePool::init --> add Default AdGroupPtr to Default error, AdZone->DefaultSize() = " << zoneit->second->GetDefaultSize() << " index = " << index);
          }
        }
      } else {
       // MCE_WARN("AdZonePool::init --> adgroup_adzone_ref has zoneid = " << zone_id << " but miss in adzone");
      }
    }
  } else {
    MCE_WARN("AdZonePool::init --> adzone2default is empty");
  }

  /////////////////////////////////Load url_adzone_maping////////////////////////////////////////////////////////////////////////////////////
  DictTreePtr trie = new DictTree();

  sql.clear();
  multimap<string, AdZonePtr> ref_zone_map;
  sql << "select url, stage, adzone_id from url_adzone_maping";
  res = QueryRunner(DB_INSTANCE, CDbRServer, "url_adzone_maping").store(sql);
  if (res && res.num_rows() > 0) {
    for (size_t i = 0; i < res.num_rows(); ++i) {

      string referer =
          (mysqlpp::Null<string, mysqlpp::NullIsBlank>) res[i]["url"];
      Ice::Long zone_id = (Ice::Long) res[i]["adzone_id"];
      map<Ice::Long, AdZonePtr>::iterator it = zones.find(zone_id);
      if (it != zones.end()) {
        it->second->set_referer(referer);
        it->second->set_stage((int) res[i]["stage"]);
        ref_zone_map.insert(make_pair<string, AdZonePtr> (referer, it->second));
        trie->insert(referer, it->second);
      } else {
        //MCE_WARN("AdZonePool::init --> url_adzone_maping has zone_id = " << zone_id << " but miss in adzone!!!!");
      }
    }
  } else {
    MCE_WARN("AdZonePool::init --> no data in table url_adzone_maping");
  }

  MCE_INFO("AdZonePool::Init --> AFTER INIT zones.size = " << zones.size() << " ref_zone_map.size = " << ref_zone_map.size());

  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    zone_pool_.swap(zones);
    ref_zone_map_.swap(ref_zone_map);
    trie_ = trie;
  }

}

AdZoneDict AdZonePool::GetAdZoneOrdered2(const string& referer, const int stage) {
  AdZoneDict ans;
  if(referer.empty()){
    return ans;
  }
  //MCE_INFO("AdZonePool::GetAdZoneOrdered -->  referer = " << referer);
  /************************************************/
  vector<string> maybe;
  vector<AdZonePtr> trie_zones;
  int len = referer.length();
  bool trie_ans;
  string ok_referer = "";
  maybe.push_back(referer);
  for(int i = len - 1; i; --i){
    if(referer.at(i) == '?' || referer.at(i) == '/' || referer.at(i) == '#'){
      string cutstr = referer.substr(0, i);
      if(cutstr == "http:/"){
        break;
      }
      //MCE_INFO("AdZonePool::GetAdZoneOrdered --> cutstr = " << cutstr);
      maybe.push_back(cutstr);
    }
  }
  {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    for(vector<string>::iterator it = maybe.begin(); it != maybe.end(); ++it){
      //MCE_INFO("AdZonePool::GetAdZoneOrdered --> trie.search url = " << (*it));
      try{
        trie_ans = trie_->search(*it, trie_zones);
      }
      catch(std::exception& e){
        MCE_WARN("AdZonePool::GetAdZoneOrdered2 --> trie.search ERR  line:"<<__LINE__<<" err:"<<e.what());
      }
      if(trie_ans){
        ok_referer = *it;
        //MCE_INFO("AdZonePool::GetAdZoneOrdered --> trie.search : " << *it << " OK trie_zones.size = " << trie_zones.size());
        bool is_match = false;
        for(vector<AdZonePtr>::iterator trie_zone_it = trie_zones.begin(); trie_zone_it != trie_zones.end(); ++trie_zone_it){
          //MCE_INFO("AdZonePool::GetAdZoneOrdered --> trie_zones.id = " << (*trie_zone_it)->id());
          if((*trie_zone_it)->Available() && (*trie_zone_it)->FindStage(stage)) {
            is_match = true;
          }
        }
        if(is_match) {
          break; //匹配到maybe中某一个url，跳出循环
        } else {
          continue; //没有匹配到该url，继续循环
        }
      }
      else{
        //MCE_INFO("AdZonePool::GetAdZoneOrdered --> trie.search : " << *it << " FAILED");
      }
    }
  }
  /************************************************/

  for (vector<AdZonePtr>::iterator it = trie_zones.begin(); it != trie_zones.end(); ++it) {
    bool available = (*it)->Available();
    if (available && (*it)->FindStage(stage)) {
      //MCE_INFO("AdZonePool::GetAdZoneOrdered --> after equal_range referer = " << ok_referer << " find userstage = " << stage << " OK id = " << (*it)->id());
      ans.insert(make_pair<int, AdZonePtr> ((*it)->weight(),(*it)));
    } else {
      //MCE_INFO("AdZonePool::GetAdZoneOrdered --> after equal_range referer = " << ok_referer << " find userstage = " << stage << " NOT MATCH");
    }
  }

  if (!ans.empty()) {
    //MCE_INFO("AdZonePool::GetAdZoneOrdered --> ans.size = " << ans.size() << " referer = " << referer << " okreferer = " << ok_referer); 
    return ans;
  } else {
    //MCE_INFO("AdZonePool::GetAdZoneOrdered --> myreferer = " << referer << " could not find any adzone");
  }
  return ans;
}


AdZoneDict AdZonePool::GetAdZoneOrdered(const string& referer, const int stage, const bool hide) {
  AdZoneDict ans;
  if(referer.empty()){
    return ans;
  }
  //MCE_INFO("AdZonePool::GetAdZoneOrdered -->  referer = " << referer);
  /************************************************/
  vector<string> maybe;
  vector<AdZonePtr> trie_zones;
  int len = referer.length();
  bool trie_ans;
  string ok_referer = "";
  maybe.push_back(referer);
  for(int i = len - 1; i; --i){
    if(referer.at(i) == '?' || referer.at(i) == '/' || referer.at(i) == '#'){
      string cutstr = referer.substr(0, i);
      if(cutstr == "http:/"){
        break;
      }
      //MCE_INFO("AdZonePool::GetAdZoneOrdered --> cutstr = " << cutstr);
      maybe.push_back(cutstr);
    }
  }
  {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    for(vector<string>::iterator it = maybe.begin(); it != maybe.end(); ++it){
      //MCE_INFO("AdZonePool::GetAdZoneOrdered --> trie.search url = " << (*it));
      try{
        trie_ans = trie_->search(*it, trie_zones);
      }
      catch(std::exception& e){
        MCE_WARN("AdZonePool::GetAdZoneOrdered --> trie.search ERR  line:"<<__LINE__<<" err:"<<e.what());
      }
      if(trie_ans){
        ok_referer = *it;
        //MCE_INFO("AdZonePool::GetAdZoneOrdered --> trie.search : " << *it << " OK trie_zones.size = " << trie_zones.size());
        bool is_match = false;
        for(vector<AdZonePtr>::iterator trie_zone_it = trie_zones.begin(); trie_zone_it != trie_zones.end(); ++trie_zone_it){
          //MCE_INFO("AdZonePool::GetAdZoneOrdered --> trie_zones.id = " << (*trie_zone_it)->id());
          if((*trie_zone_it)->Available() && (*trie_zone_it)->FindStage(stage)) {
            is_match = true;
          }
        }
        if(is_match) {
          break; //匹配到maybe中某一个url，跳出循环
        } else {
          continue; //没有匹配到该url，继续循环
        }
      } else{
        //MCE_INFO("AdZonePool::GetAdZoneOrdered --> trie.search : " << *it << " FAILED");
      }
    }
  }
  /************************************************/

  for (vector<AdZonePtr>::iterator it = trie_zones.begin(); it != trie_zones.end(); ++it) {
    bool available = (*it)->Available();
    if (available && (*it)->FindStage(stage)) {
      //MCE_INFO("AdZonePool::GetAdZoneOrdered --> after equal_range referer = " << ok_referer << " find userstage = " << stage << " OK id = " << (*it)->id());
      //vip 去广告策略，凡是vip并且设置了去广告后，删除03, 04两个广告位
      if(!hide || (*it)->id() != 100000000003 && (*it)->id() != 100000000004) {
        //MCE_DEBUG("[VIP_DEBUG] referer = " << ok_referer << ", zone_id = " << (*it)->id());
        ans.insert(make_pair<int, AdZonePtr> ((*it)->weight(),(*it)));
      }
    } else {
      //MCE_INFO("AdZonePool::GetAdZoneOrdered --> after equal_range referer = " << ok_referer << " find userstage = " << stage << " NOT MATCH");
    }
  }

  /*if (!ans.empty()) {
    //MCE_INFO("AdZonePool::GetAdZoneOrdered --> ans.size = " << ans.size() << " referer = " << referer << " okreferer = " << ok_referer); 
    return ans;
  } else {
    //MCE_INFO("AdZonePool::GetAdZoneOrdered --> myreferer = " << referer << " could not find any adzone");
  }*/
  return ans;
}

void AdZonePool::ReloadTimer::handle() {
  AdZonePool::instance().Init();
}

/***********************************************************************************************************************/

bool AdGroup::Available(bool log) {
  bool result = false;
  result = member_table_status_ && campaign_status_ && (member_status_ == 1)
      && (am_status_ == 1) && (audit_status_ == 1) && (delete_flag_ == 1) && (daily_sys_status_ == 1) && (total_sys_status_ == 1)
      && point_valid_;
  if(log){
    //MCE_INFO("AdGroup::Available --> RESULT = " << result << " group_id = " << group_id_ << " member_table_status_ = " << member_table_status_ << " campain_statues_ = " << campaign_status_ << " member_status_ = " << member_status_ << " am_status_ = " << am_status_ << " audit_status_ = " << audit_status_ << " delete_flag_ = " << delete_flag_ << " daily_sys_status_" << daily_sys_status_ << " total_sys_status_ = " << total_sys_status_ << " point_valid_ = " << point_valid_ << " >> rotation_number = " << rotation_number_);
  }
  return result;
}

bool AdGroup::HasCreatives(){
  bool result = !(creative_ids_.empty());
  //MCE_DEBUG("AdGroup::HasCreatives --> RESULT = " << result );
  return result;
}

Ice::Long AdGroup::GetNextCreative(int rotate_index){
  if(!creative_ids_.empty()){
    int size = creative_ids_.size(), start = 0;
    //int stop = rotate_index % size + 1;
    creative_index_++;
/*    if(creative_index_ >= size){
      creative_index_ = 0;
    }*/
    int stop = (creative_index_) % size + 1;
    Ice::Long ans = *(creative_ids_.begin());
    for(set<Ice::Long>::iterator it = creative_ids_.begin(); it != creative_ids_.end() && start < stop; ++it, ++start){
      ans = *it;
    }
    //MCE_INFO("AdGroup::GetNextCreative --> userid = " << userid << " group_id =  " << group_id_ << "  creative_size = " << size << " rotate_index = " << rotate_index << " start = " << start << " stop = " << stop << " ansid = " << ans);
    return ans;
  }
  return 0;
}

void AdGroup::UpdateCampainStatues() {
  AdCampaignPtr campaign_ptr = AdCampaignPool::instance().GetAdCampaign(
      campaign_id_);
  if (campaign_ptr) {
    campaign_status_ = campaign_ptr->Available();
  }
}

AdGroupPtr AdGroupPool::GetAdGroup(Ice::Long group_id) {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  AdGroupPtr ans;
  map<Ice::Long, AdGroupPtr>::iterator it = group_pool_.find(group_id);
  if (it != group_pool_.end()) {
    ans = it->second;
  } else {
    MCE_WARN("AdGroupPool::GetGetAdGroup --> miss gruop_id = " << group_id);
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

void AdGroupPool::Init() {

  //MCE_DEBUG("AdGroupPool::Init");
  //load ad_group;
  Statement sql;
  map<Ice::Long, AdGroupPtr> group_pool;
  map<Ice::Long, vector<AdGroupPtr> > zone2rotate;
  map<Ice::Long, vector<AdGroupPtr> > zone2default;

   //sql<< "select display_type, adgroup_id, campaign_id, member_id, max_price, member_status, am_status, audit_status, delete_flag,day_points,hour_points from adgroup where adgroup_id in (10000000890001, 10000000860001, 10000000350002, 10000000280001, 10000000180001, 10000000340001)";
  sql << "select total_sys_status, daily_sys_status, display_type, trans_type, "
        "adgroup_id, campaign_id, member_id, max_price, member_status, am_status, "
        "audit_status, delete_flag,day_points,hour_points, rotation_number from adgroup ";
  //sq   << "select display_type, adgroup_id, campaign_id, member_id, max_price, member_status, am_status, audit_status, delete_flag,day_points,hour_points from adgroup   ";
  mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer, "adgroup").store(
      sql);
  if (res && res.num_rows() > 0) {
    for (size_t i = 0; i < res.num_rows(); ++i) {
      AdGroupPtr obj = new AdGroup;
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
      int daily_sys_status =(int)row["daily_sys_status"];
      int total_sys_status = (int)row["total_sys_status"];
      int rotation_number = (int)row["rotation_number"];
      string day_points = (mysqlpp::Null<string, mysqlpp::NullIsBlank>) row["day_points"];
      string
          hour_points =
              (mysqlpp::Null<string, mysqlpp::NullIsBlank>)row["hour_points"];
      bool campaign_status = false;
      AdCampaignPtr campaign_ptr = AdCampaignPool::instance().GetAdCampaign(
          campaign_id);
      if (campaign_ptr) {
        campaign_status = campaign_ptr->Available();
      }

      bool member_table_status = false;
      AdMemberPtr member_ptr = AdMemberPool::instance().GetAdMember(member_id);
      if (member_ptr) {
        member_table_status = member_ptr->Available(display_type);
      }

      bool point_valid = PointsChecker::instance().Check(day_points,hour_points);

      obj->set_campaign_id(campaign_id);
      obj->set_group_id(group_id);
      obj->set_member_id(member_id);
      obj->set_member_status(member_status);
      obj->set_am_status(am_status);
      obj->set_audit_status(audit_status);
      obj->set_delete_flag(delete_flag);
      obj->set_campaign_status(campaign_status);
      obj->set_member_table_status(member_table_status);
      obj->set_max_price(max_price);
      obj->set_display_type(display_type);
      obj->set_trans_type(trans_type);
      obj->set_point_valid(point_valid);
      obj->set_daily_sys_status(daily_sys_status);
      obj->set_total_sys_status(total_sys_status);
      obj->set_rotation_number(rotation_number);
      if(obj->Available(true)){
        //MCE_DEBUG("AdGroupPool::Init --> group " << group_id << " member_id = " << obj->member_id() << " is ok display type = " << obj->display_type());
        group_pool[group_id] = obj;
      }
      else{
        //MCE_DEBUG("AdGroupPool::Init --> because " << group_id << " not Available so do not push it display_type = " << obj->display_type());
      }
      //MCE_INFO("AdGroupPool::Init --> group_id = " << group_id << " campaign_id = " << campaign_id << "campaign_status = " << campaign_status <<" member_id = " << member_id << " member_status = " << member_status << " am_status = " << am_status << " audit_status = " << audit_status << " delete_flag = " << delete_flag);
    }
  } else {
    MCE_WARN("AdGroupPool::Init --> no data in adgroup");
  }

  //MCE_INFO("AdGroupPool::Init --> after Available filter has " << group_pool.size() << " groups");
  //load creative;
  sql.clear();
  sql
      << "select creative_id, adgroup_id, member_status, am_status, audit_status, delete_flag from creative";
  res = QueryRunner(DB_INSTANCE, CDbRServer, "creative").store(sql);
  if (res && res.num_rows() > 0) {
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      Ice::Long creative_id = (Ice::Long) row["creative_id"];
      Ice::Long group_id = (Ice::Long) row["adgroup_id"];
      int member_status = (int) row["member_status"];
      int am_status = (int) row["am_status"];
      int audit_status = (int) row["audit_status"];
      int delete_flag = (int) row["delete_flag"];
      if ((member_status == 1) && (am_status == 1) && (audit_status == 1)
          && (delete_flag == 1)) {
        map<Ice::Long, AdGroupPtr>::iterator it = group_pool.find(group_id);
        if (it != group_pool.end()) {
          it->second->set_creative_ids(creative_id);
        } else {
          //MCE_WARN("AdGroupPool::Init --> creative has group_id = " << group_id << " but adgroup no!");
        }
      }
    }
  }

  ///////////////////adgroup's has creative check////////////////
  for(map<Ice::Long, AdGroupPtr>::iterator git = group_pool.begin(); git != group_pool.end();){
    if(!git->second->HasCreatives()){
      //MCE_DEBUG("AdGroupPool::Init --> because obj->HasCreatives is false so erase it  groupid = " << git->first);
      group_pool.erase(git++);
    }
    else{
      ++git;
    }
  }

  //MCE_DEBUG("AdGroupPool::Init --> after creative filter has " << group_pool.size() << " groups");

  //load adzone_ids;
  sql.clear();
  sql
      << "select adzone_id, member_id, adgroup_id, member_status, am_status, delete_flag from adgroup_adzone_ref";
  res = QueryRunner(DB_INSTANCE, CDbRServer, "adgroup_adzone_ref").store(sql);
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
        map<Ice::Long, AdGroupPtr>::iterator it = group_pool.find(group_id);
        if (it != group_pool.end()) {
          //MCE_INFO("AdGroupPool::Init --> OK group ids = " << group_id << " it's member_id = " << it->second->member_id() << " display_type = " << it->second->display_type());
          it->second->set_zond_ids(zone_id);
          int display_type = it->second->display_type();
          if ((member_id == 999) && (display_type != 4)) {
            map<Ice::Long, vector<AdGroupPtr> >::iterator it =
                zone2default.find(zone_id);
            map<Ice::Long, AdGroupPtr>::iterator git =
                group_pool.find(group_id);
            if (git != group_pool.end()) {
              //MCE_INFO("AdGroupPool::Init --> add to zone2default : group_id = " << group_id << " zone_id = " << zone_id);
              if (it == zone2default.end()) {
                vector<AdGroupPtr> v;
                zone2default.insert(make_pair<Ice::Long, vector<AdGroupPtr> > (
                    zone_id, v));
              }
              zone2default[zone_id].push_back(git->second);
            } else {
              //MCE_WARN("AdGroupPool::Init --> adgroup_adzone_ref has group : " << group_id << " but adgroup no");
            }
          } else if (display_type == 1) {
            map<Ice::Long, vector<AdGroupPtr> >::iterator it =
                zone2rotate.find(zone_id);
            map<Ice::Long, AdGroupPtr>::iterator git =
                group_pool.find(group_id);
            if (git != group_pool.end()) {
               //MCE_INFO("AdGroupPool::Init --> zone2rotate : group_id = " << group_id << " zone_id = " << zone_id);
              if (it == zone2rotate.end()) {
                vector<AdGroupPtr> v;
                zone2rotate.insert(make_pair<Ice::Long, vector<AdGroupPtr> > (
                    zone_id, v));
              }
              int rotation_number = git->second->rotation_number();
              for(int rotation_index = 0; rotation_index < rotation_number; ++rotation_index){
                zone2rotate[zone_id].push_back(git->second);
              }
            } else {
              //MCE_WARN("AdGroupPool::Init --> adgroup_adzone_ref has group : " << group_id << " but adgroup no");
            }
          }
        } else {
          //MCE_WARN("AdGroupPool::Init --> adgroup_adzone_ref has group_id = " << group_id << " but adgroup no!");
        }
      }
    }
  }
  //MCE_INFO("AdGroupPool::Init --> zone2default.size = " << zone2default.size() << " zone2rotate.size = " << zone2rotate.size());

  ///////////////////////////////////////////////////////////////
  //MCE_INFO("AdGroupPool::Init --> AFTER INIT group_pool.size = " << group_pool.size() << " zone2rotate_.size = " << zone2rotate_.size() << " zone2default.size = " << zone2default_.size());

  DimIndexMatcher::instance().Load(group_pool);
  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    group_pool_.swap(group_pool);
    zone2rotate_.swap(zone2rotate);
    zone2default_.swap(zone2default);
  }
}
void AdGroupPool::UpdateCampainStatues() {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  for (map<Ice::Long, AdGroupPtr>::iterator it = group_pool_.begin(); it
      != group_pool_.end(); ++it) {
    it->second->UpdateCampainStatues();
  }
}

void AdGroupPool::UpdateCheckTimer::handle() {
  AdGroupPool::instance().Init();
}
void AdGroupPool::UpdateCampaignStatusTimer::handle() {
  AdGroupPool::instance().UpdateCampainStatues();
}
/***********************************************************************************************************************/
bool AdMember::Available(int display_type) {
  bool tmp_val = is_formal_ && (validity_ == 1) && am_online_status_ && ma_online_status_ && delete_flag_;
  if(display_type != 1){
    tmp_val = no_exceed_max_ && have_money_ && tmp_val;
  }
  //MCE_DEBUG("AdMember::Available --> RESULT = " << tmp_val << " member_id = " << member_id_ << " is_formal_ = " << is_formal_ << " validity_ = " <<
  //    validity_ << " no_exceed_max_ = " << no_exceed_max_ << " am_online_status_ = " << am_online_status_ << " ma_online_status_ = " <<
   //   ma_online_status_ << " delete_flag_ = " << delete_flag_ << " have_money_ = " << have_money_ << " display_type = " << display_type);
  return tmp_val;
}
AdMemberPtr AdMemberPool::GetAdMember(Ice::Long member_id) {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  AdMemberPtr ans;
  map<Ice::Long, AdMemberPtr>::iterator it = member_pool_.find(member_id);
  if (it != member_pool_.end()) {
    ans = it->second;
  } else {
    //MCE_WARN("AdMemberPool::GetAdMember --> miss member_id = " << member_id);
  }
  return ans;
}

void AdMemberPool::Init() {
  MCE_DEBUG("AdMemberPool::Init");
  Statement sql;
  map<Ice::Long, AdMemberPtr> member_pool;
  sql << "select * from member";
  mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer, "member").store(
      sql);
  if (res && res.num_rows() > 0) {
    for (size_t i = 0; i < res.num_rows(); ++i) {
      AdMemberPtr obj = new AdMember;
      mysqlpp::Row row = res.at(i);
      obj->member_id_ = (Ice::Long) row["member_id"];
      obj->is_formal_ = (int) row["is_formal"];
      obj->validity_ = (int) row["validity"];
      obj->have_money_ = (int) row["have_money"];
      obj->no_exceed_max_ = (int) row["no_exceed_max"];
      obj->am_online_status_ = (int) row["am_online_status"];
      obj->ma_online_status_ = (int) row["ma_online_status"];
      obj->delete_flag_ = (int) row["delete_flag"];
      if(obj->Available()){
        member_pool[obj->member_id_] = obj;
      }
    }
    //MCE_INFO("AdMemberPool::Init --> AFTER INIT member_pool.size = " << member_pool.size());

    {
      IceUtil::RWRecMutex::WLock lock(mutex_);
      member_pool_.swap(member_pool);
    }
  } else {
    MCE_WARN("AdMemberPool::Init --> no data in ad_member");
  }
}

void AdMemberPool::UpdateCheckTimer::handle() {
  AdMemberPool::instance().Init();
}

/***********************************************************************************************************************/
bool AdCampaign::Available() {
  Date date = Date(time(NULL));
  time_t timestamp = date.time();
  bool time_check = timestamp >= begin_time_ && (!end_time_ || timestamp
      <= end_time_);
  bool result = delete_flag_ && (audit_status_ == 1) && time_check && (member_status_ == 1) && (am_status_ == 1) && (sys_status_ == 1);
  return result;
}

AdCampaignPtr AdCampaignPool::GetAdCampaign(Ice::Long campaign_id) {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  AdCampaignPtr ans;
  map<Ice::Long, AdCampaignPtr>::iterator it = campaign_pool_.find(campaign_id);
  if (it != campaign_pool_.end()) {
    ans = it->second;
  } else {
    //MCE_WARN("AdCampaignPool::GetAdCampaign --> miss campaign_id = " << campaign_id);
  }
  return ans;
}

void AdCampaignPool::Init() {
  //MCE_DEBUG("AdCampaignPool::Init");
  Statement sql;
  map<Ice::Long, AdCampaignPtr> campaign_pool;
  sql
      << "select sys_status, member_status, am_status, campaign_id, member_id, begin_time, end_time, cast_calender, cast_hour, delete_flag, audit_status from campaign";
  mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer, "campaign").store(
      sql);
  if (res && res.num_rows() > 0) {
    for (size_t i = 0; i < res.num_rows(); ++i) {
      AdCampaignPtr obj = new AdCampaign;
      mysqlpp::Row row = res.at(i);
      Ice::Long campaign_id = (Ice::Long) row["campaign_id"];
      Ice::Long member_id = (Ice::Long) row["member_id"];
      string begin_time =
          (mysqlpp::Null<string, mysqlpp::NullIsBlank>) row["begin_time"];
      string end_time =
          (mysqlpp::Null<string, mysqlpp::NullIsBlank>) row["end_time"];
      int running_area_hour = 0;
      int running_area_day = 0;
      //      string cast_calender =
      //          (mysqlpp::Null<string, mysqlpp::NullIsBlank>) row["cast_calender"];
      //      string cast_hour =
      //          (mysqlpp::Null<string, mysqlpp::NullIsBlank>) row["cast_hour"];
      bool delete_flag = (bool) row["delete_flag"];
      int audit_status = (int) row["audit_status"];
      int sys_status =(int)row["sys_status"];
      int member_status = (int)row["member_status"];
      int am_status = (int)row["am_status"];

      obj->campaign_id_ = campaign_id;
      obj->member_id_ = member_id;
      obj->delete_flag_ = delete_flag;
      obj->audit_status_ = audit_status;
      obj->sys_status_ = sys_status;
      obj->member_status_ = member_status;
      obj->am_status_ = am_status;

      //      Date date = Date(time(NULL));
      //      int action_year = date.year();
      //      int action_month = date.month();
      //      int action_day = date.day();
      //
      //      if (!cast_calender.empty()) {
      //        bool find_ok = false;
      //        vector<uint64_t> cast_calender_list = Translator::instance().Split(
      //            cast_calender);
      //        if (!cast_calender_list.empty()) {
      //          string db_date =
      //              boost::lexical_cast<string>(cast_calender_list.at(0));
      //          int db_year = boost::lexical_cast<int>(db_date.substr(0, 4));
      //          int db_month = boost::lexical_cast<int>(db_date.substr(4, 2));
      //          int db_day = boost::lexical_cast<int>(db_date.substr(6, 2));
      //          if (action_year >= db_year && action_month >= db_month && action_day
      //              >= db_day) {
      //            //下标 = 年份差 * 12 + 月份差 + 1
      //            int index = (12 * action_year - db_year) + (action_month - db_month
      //                + 1);
      //            if (index <= ((int) cast_calender_list.size() - 1)) {
      //              running_area_day = boost::lexical_cast<Ice::Long>(
      //                  cast_calender_list.at(index));
      //              find_ok = true;
      //            }
      //          }
      //        }
      //        if (!find_ok) {
      //          running_area_day = -1;
      //        }
      //      } else {
      //        running_area_day = -1;
      //      }
      //      if (!cast_hour.empty()) {
      //        running_area_hour = boost::lexical_cast<Ice::Long>(cast_hour);
      //      } else {
      //        running_area_hour = -1;
      //      }
      if (begin_time.empty()) {
        obj->begin_time_ = 0;
      } else {
        Date date_begin_time = Date(begin_time);
        obj->begin_time_ = date_begin_time.time();
      }
      if (end_time.empty()) {
        obj->end_time_ = 0;
      } else {
        Date date_end_time = Date(end_time);
        obj->end_time_ = date_end_time.time();
      }
      if(obj->Available()){
        campaign_pool[campaign_id] = obj;
      }
    }
    //MCE_INFO("AdCampaignPool::Init --> AFTER INIT campaign_pool.size = " << campaign_pool.size());

    {
      IceUtil::RWRecMutex::WLock lock(mutex_);
      campaign_pool_.swap(campaign_pool);
    }
  } else {
    MCE_WARN("AdCampaignPool::Init --> no data in ad_campaign");
  }
}
void AdCampaignPool::UpdateCheckTimer::handle() {
  AdCampaignPool::instance().Init();
}
/***********************************************************************************************************************/
void AdEffectTimer::handle(){
  AdEffectLoader::instance().Load();
}

void AdEffectLoader::AddIndex(const AdEffectIndexPtr& index){
  //MCE_DEBUG("AdEffectLoader::AddIndex --> key = " << index->user_key_ << " group_id = " << index->group_id_);
  IceUtil::Mutex::Lock lock(mutex_);
  index_set_.insert(index);
/*  for(set<AdEffectIndexPtr>::iterator logit = index_set_.begin(); logit != index_set_.end(); ++logit){
    MCE_DEBUG("AdEffectLoader::AddIndex --> after insert has : key = " << (*logit)->user_key_ << " group_id = " << (*logit)->group_id_);
  }*/
}

void AdEffectLoader::Load(){
  set<AdEffectIndexPtr> index_set;
  Statement sql;
  /*for(set<AdEffectIndexPtr>::iterator logit = index_set_.begin(); logit != index_set_.end(); ++logit){
    MCE_DEBUG("AdEffectLoader::Load -->  befor swap has : key = " << (*logit)->user_key_ << " group_id = " << (*logit)->group_id_);
  }*/
  {
    IceUtil::Mutex::Lock lock(mutex_);
    index_set_.swap(index_set);
  }
  //MCE_DEBUG("AdEffectLoader::Load index_set.size = " << index_set.size());
  int server_index = AdMatchManagerI::instance().ServerIndex();
  //MCE_DEBUG("AdEffectLoader::Load --> server_index = " << server_index);
  for(set<AdEffectIndexPtr>::iterator it = index_set.begin(); it != index_set.end(); ++it){
    //MCE_DEBUG("AdEffectLoader::Load --> Load key = " << (*it)->user_key_ << " group_id = " << (*it)->group_id_);
    sql << "select * from ad_effect_stat where user_key = " << (*it)->user_key_ << " and group_id = " << (*it)->group_id_ << " and server_index = " << server_index;
    mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer, "ad_effect_stat").store(sql);
    if (res && res.num_rows() > 0) {
      for (size_t i = 0; i < res.num_rows(); ++i) {
        AdEffectPtr obj = new AdEffect();
        mysqlpp::Row row = res.at(i);
        Ice::Long key = (Ice::Long) row["user_key"];
        Ice::Long group_id = (Ice::Long) row["group_id"];
        Ice::Long pv_count = (Ice::Long) row["pv_count"];
        Ice::Long click_count = (Ice::Long) row["click_count"];
        obj->set_pv(pv_count);
        obj->set_click(click_count);
        obj->set_load_fromdb(true);//TODO
        //MCE_INFO("AdEffectLoader::Load --> FIND FROM DB userkey = " << key << " group_id = " << group_id << " pv_count = " << obj->pv() << " click_count = " << obj->click() << " load_fromdb = " << obj->load_fromdb());
        AdEffectStat::instance().UpdateAdEffect(key, group_id, obj);
      }
    }
    else{
      AdEffectPtr obj = new AdEffect();
      obj->set_load_fromdb(true);//TODO
      //MCE_INFO("AdEffectLoader::Load --> MISS FROM DB userkey = " << (*it)->user_key_ << " group_id = " << (*it)->group_id_ << " pv_count = " << obj->pv() << " click_count = " << obj->click() << " load_fromdb = " << obj->load_fromdb());
      AdEffectStat::instance().UpdateAdEffect((*it)->user_key_, (*it)->group_id_, obj);
    }
    sql.clear();
  }
}


AdEffectPtr AdEffectPool::GetAdEffect(Ice::Long group_id) {
  {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    map<Ice::Long, AdEffectPtr>::iterator it = effect_pool_.find(group_id);
    if (it != effect_pool_.end()) {
      //MCE_DEBUG("AdEffectPool::GetAdEffect --> this AdEffectPtr exist, gid = " << group_id);
      it->second->set_last_update_time(time(NULL));
      return it->second;
    }
  }
  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    //MCE_DEBUG("AdEffectPool::GetAdEffect --> this AdEffectPtr does not exist, gid = " << group_id);
    AdEffectPtr p = new AdEffect();
    effect_pool_[group_id] = p;

    AdEffectIndexPtr index = new AdEffectIndex();
    index->user_key_ = key_;
    index->group_id_ = group_id;
    AdEffectLoader::instance().AddIndex(index);

    return p;
  }
}

void AdEffectPool::StorageInfo(uint64_t key) {
	//MCE_DEBUG("AdEffectPool::StorageInfo --> key = " << key);
  Statement sql;
  ostringstream os;
  IceUtil::RWRecMutex::RLock lock(mutex_);
  map<Ice::Long, AdEffectPtr>::iterator it;
  int server_index = AdMatchManagerI::instance().ServerIndex();
  for (it = effect_pool_.begin(); it != effect_pool_.end(); ++it) {
    int pv_threshold = it->second->pv_threshold();
    int click_threshold = it->second->click_threshold();
		//MCE_DEBUG("AdEffectPool::StorageInfo --> key = " << key << " group_id = " << it->first << " load_fromdb = " << it->second->load_fromdb() << " pv_threshold = " << pv_threshold << " click_threshold = " << click_threshold << " pv_inc = " << it->second->pv_inc() << " click_inc = " << it->second->click_inc() << " pv_count = " << it->second->pv() << " click_count = " << it->second->click());
    if (it->second->load_fromdb() && (it->second->pv_inc() >= pv_threshold || it->second->click_inc()
        >= click_threshold)) {
      it->second->set_pv_inc(0);
      it->second->set_click_inc(0);
      os
          << "insert into ad_effect_stat (user_key, group_id, pv_count, click_count, server_index) values("
          << key << "," << it->first << ", " << it->second->pv() << ", "
          << it->second->click() << ", " << server_index
          << ") on duplicate key update pv_count = values(pv_count), click_count = values(click_count)";
      //MCE_DEBUG("AdEffectPool::StorageInfo --> key = " << key << "  sql = " << os.str());
      sql << os.str();
      mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbWServer, "ad_effect_stat").store(sql);
    }

    os.str("");
    sql.clear();
  }
}

void AdEffectPool::KickTimeoutEffect(){
  vector< map<Ice::Long, AdEffectPtr>::iterator > kickpool;
  vector< map<Ice::Long, AdEffectPtr>::iterator > :: iterator kickpool_iterator;
  {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    map<Ice::Long, AdEffectPtr>::iterator it;
    time_t now = time(NULL);
    time_t kick_timestamp = AdEffectStat::instance().kick_timestamp();
    //MCE_DEBUG("AdEffectPool::KickTimeoutEffect --> effect_pool.size = " << effect_pool_.size());
    for (it = effect_pool_.begin(); it != effect_pool_.end(); ++it) {
      if(now - it->second->last_update_time() >= kick_timestamp){
        //MCE_DEBUG("AdEffectPool::KickTimeoutEffect --> group_id = " << it->first << "  last update time = " << it->second->last_update_time() << " now = " << now << " kick_timestamp = " << kick_timestamp << " tmp = " << now - it->second->last_update_time() << " so erase");
        kickpool.push_back(it);
      }
    }
  }
  if(!kickpool.empty()){
    IceUtil::RWRecMutex::WLock lock(mutex_);
    for(kickpool_iterator = kickpool.begin(); kickpool_iterator != kickpool.end(); ++kickpool_iterator){
      effect_pool_.erase((*kickpool_iterator));
    }
  }
}

void AdEffectPool::AddEffect(Ice::Long group_id, const AdEffectPtr& obj) {
  IceUtil::RWRecMutex::WLock lock(mutex_);
  map<Ice::Long, AdEffectPtr>::iterator it = effect_pool_.find(group_id);
  if(it != effect_pool_.end()){
    effect_pool_[group_id] = obj;
  }
  else{
    effect_pool_.insert(make_pair<Ice::Long, AdEffectPtr>(group_id, obj));
  }
}

void AdEffectPool::IncPv(Ice::Long group_id) {
  AdEffectPtr ptr = GetAdEffect(group_id);
  ptr->IncPv();
  //MCE_INFO("AdEffectPool::IncPv --> group_id = " << group_id << " after inc pv = " << ptr->pv());
}

void AdEffectPool::IncClick(Ice::Long group_id) {

  AdEffectPtr ptr = GetAdEffect(group_id);
  ptr->IncClick();
  //MCE_INFO("AdEffectPool::IncClick --> group_id = " << group_id << " after inc Click = " << ptr->click() << " click_inc = " << ptr->click_inc());
}

AdEffectPoolPtr AdEffectStat::GetEffectPool(uint64_t key) {
  {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    map<uint64_t, AdEffectPoolPtr>::iterator it = pool_.find(key);
    if (it != pool_.end()) {
      //MCE_DEBUG("AdEffectStat::GetEffectPool --> this AdEffectPoolPtr exist, key = " << key);
      return it->second;
    }
  }
  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    //MCE_DEBUG("AdEffectStat::GetEffectPool --> this AdEffectPoolPtr does not exist , create one, key = " << key);
    AdEffectPoolPtr p = new AdEffectPool;
    p->set_key(key);
    pool_[key] = p;
    return p;
  }
}
void AdEffectStat::UpdateAdEffect(uint64_t key, Ice::Long group_id, const AdEffectPtr& ptr){
  AdEffectPoolPtr pool_ptr = GetEffectPool(key);
  pool_ptr->AddEffect(group_id, ptr);
}

void AdEffectStat::Init() {
  return;
  Statement sql;
  map<uint64_t, AdEffectPoolPtr> pool;
  sql << "select * from ad_effect_stat";
  mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer, "ad_effect_stat").store(
      sql);
  if (res && res.num_rows() > 0) {
    for (size_t i = 0; i < res.num_rows(); ++i) {
      AdEffectPtr obj = new AdEffect();
      mysqlpp::Row row = res.at(i);
      Ice::Long key = (Ice::Long) row["user_key"];
      Ice::Long group_id = (Ice::Long) row["group_id"];
      Ice::Long pv_count = (Ice::Long) row["pv_count"];
      Ice::Long click_count = (Ice::Long) row["click_count"];
      obj->set_pv(pv_count);
      obj->set_click(click_count);
      map<uint64_t, AdEffectPoolPtr>::iterator it = pool.find(key);
      if (it == pool.end()) {
        AdEffectPoolPtr effect_pool_ptr = new AdEffectPool();
        pool.insert(make_pair<uint64_t, AdEffectPoolPtr> (key, effect_pool_ptr));
      }
      //MCE_INFO("AdEffectStat::Init --> userkey = " << key << " group_id = " << group_id << " pv_count = " << pv_count << " click_count = " << click_count);
      pool[key]->AddEffect(group_id, obj);
    }
  }
  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    pool_.swap(pool);
  }
}

void AdEffectStat::StorageInfo() {
  MCE_INFO("AdEffectStat::StorageInfo");
  IceUtil::RWRecMutex::RLock lock(mutex_);
  map<uint64_t, AdEffectPoolPtr>::iterator it;
  for (it = pool_.begin(); it != pool_.end(); ++it) {
    it->second->StorageInfo(it->first);
  }
}

void AdEffectStat::KickTimeoutEffect(){
  //MCE_INFO("AdEffectStat::KickTimeoutEffect");
  IceUtil::RWRecMutex::RLock lock(mutex_);
  map<uint64_t, AdEffectPoolPtr>::iterator it;
  for (it = pool_.begin(); it != pool_.end(); ++it) {
    it->second->KickTimeoutEffect();
  }
}

void AdEffectStat::IncPv(uint64_t key, Ice::Long group_id) {
  //MCE_DEBUG("AdEffectStat::IncPv --> key = " << key << " group_id = " << group_id);
  AdEffectPoolPtr pool_ptr = GetEffectPool(key);
  pool_ptr->IncPv(group_id);
}

void AdEffectStat::IncClick(uint64_t key, Ice::Long group_id) {
  AdEffectPoolPtr pool_ptr = GetEffectPool(key);
  pool_ptr->IncClick(group_id);
  //MCE_DEBUG("AdEffectStat::IncClick --> key = " << key << " group_id = " << group_id);
}

void AdEffectStat::StorageInfoTimer::handle() {
  AdEffectStat::instance().StorageInfo();
}

void AdEffectStat::KickTimer::handle(){
  AdEffectStat::instance().KickTimeoutEffect();
}

void SchoolPool::Init() {
  map<Ice::Long, uint64_t> pool;
  Statement sql;
  sql << "select * from school";
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
}
}
