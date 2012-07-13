/*
 * AdInvertedIndex.cpp
 *
 *  Created on: Sep 2, 2010
 *      Author: antonio
 */

#include <algorithm>
#include <boost/lexical_cast.hpp>
#include "AdInvertedIndex.h"
#include "AdMatchManagerI.h"
#include <QueryRunner.h>
#include <DbDescriptor.h>
#include "../../util/src/AdCommonUtil.h"
#include "Log.h"
#include "AdConfig.h"

using namespace xce::ad;
using namespace com::xiaonei::xce;
using namespace boost;

template<typename T>
static string PrintHelper(const T & container) { //将容器内容打印成字符串
  static const int kPrintLimit = 3;
  ostringstream ios;
  ios << "(";
  bool first_item = true;
  int i = 0;
  for (typename T::const_iterator itr = container.begin(); itr
      != container.end(); ++itr,++i) {
    if (first_item == true) {
      first_item = false;
    } else {
      ios << ",";
    }
    if(typeid(*itr) == typeid(AdGroupPtr)){
      ios << (*itr)->group_id();
    }else{
      ios << *itr;
    }
//    ios << *itr;
    if(i >= kPrintLimit){
      ios << " ...... ";
      break;
    }
  }
  MCE_DEBUG("PrintHelper -- > i = " << i << " ,kPrintLimit = " << kPrintLimit);
  ios << ")";
  return ios.str();
}

namespace xce {
namespace ad {

bool ShortIndex::Add(short key, AdGroupPtr group) {
  IceUtil::RWRecMutex::WLock lock(mutex_);
  MCE_DEBUG("[Add] before add: size = " << index_[key].size());
  index_[key].insert(group);
  MCE_DEBUG("[Add] after add: size = " << index_[key].size());
  return true;
}

bool ShortIndex::Add(vector<short>& keys, AdGroupPtr group) {
  IceUtil::RWRecMutex::WLock lock(mutex_);
  for(vector<short>::const_iterator it = keys.begin(); it != keys.end(); ++it) {
    index_[*it].insert(group);
  }
  return true;
}

bool ShortIndex::Del(vector<short>& keys, AdGroupPtr group) {
  IceUtil::RWRecMutex::WLock lock(mutex_);
  for(vector<short>::const_iterator it = keys.begin(); it != keys.end(); ++it) {
    index_[*it].erase(group);
  }
  return true;
}

set<AdGroupPtr> ShortIndex::Get(const short key) {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  map<short, set<AdGroupPtr> >::const_iterator it = index_.find(key);
  if(it != index_.end()) {
    return it->second;
  }
  return set<AdGroupPtr>();
}

set<AdGroupPtr> ShortIndex::GetWithAll(const short key) {
  set<AdGroupPtr> result;
  IceUtil::RWRecMutex::RLock lock(mutex_);
  map<short, set<AdGroupPtr> >::const_iterator it = index_.find(key);
  if(it != index_.end()) {
    const set<AdGroupPtr>& key_result = it->second;
    result.insert(key_result.begin(), key_result.end());
  }
  it = index_.find(-1);
  if(it != index_.end()) {
    const set<AdGroupPtr>& key_result = it->second;
    result.insert(key_result.begin(), key_result.end());
  }
  return result;
}


bool UIntIndex::Add(uint64_t& key, AdGroupPtr group) {
  IceUtil::RWRecMutex::WLock lock(mutex_);
  index_[key].insert(group);
  return true;
}

bool UIntIndex::Add(vector<uint64_t>& keys, AdGroupPtr group) {
  IceUtil::RWRecMutex::WLock lock(mutex_);
  for(vector<uint64_t>::const_iterator it = keys.begin(); it != keys.end(); ++it) {
    index_[*it].insert(group);
  }
  return true;
}

bool UIntIndex::Del(vector<uint64_t>& keys, AdGroupPtr group) {
  IceUtil::RWRecMutex::WLock lock(mutex_);
  for(vector<uint64_t>::const_iterator it = keys.begin(); it != keys.end(); ++it) {
    index_[*it].erase(group);
  }
  return true;
}

set<AdGroupPtr> UIntIndex::Get(const uint64_t key) {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  map<uint64_t, set<AdGroupPtr> >::const_iterator it = index_.find(key);
  if(it != index_.end()) {
    return it->second;
  }
  return set<AdGroupPtr>();
}

set<AdGroupPtr> UIntIndex::GetWithAll(const uint64_t key) {
  set<AdGroupPtr> result;
  IceUtil::RWRecMutex::RLock lock(mutex_);
  map<uint64_t, set<AdGroupPtr> >::iterator it = index_.find(key);
  if(it != index_.end()) {
    set<AdGroupPtr>& key_result =it->second;
    result.insert(key_result.begin(), key_result.end());
  }
  it = index_.find(0);
  if(it != index_.end()) {
    set<AdGroupPtr>& key_result =it->second;
    result.insert(key_result.begin(), key_result.end());
  }
  return result;
}

set<AdGroupPtr> AreaUIntIndex::GetWithAll(const uint64_t key) {
  //0086210100000000
  //0086210000000000
  //0086000000000000
  uint64_t province = (key/10000000000)*10000000000;
  uint64_t COUNTRY = 86000000000000;
  //MCE_DEBUG("key = " <<  key << ", province = " << province << ", COUNTRY = " << COUNTRY);
  
  set<AdGroupPtr> result;
  IceUtil::RWRecMutex::RLock lock(mutex_);
  map<uint64_t, set<AdGroupPtr> >::iterator it = index_.find(key);
  if(it != index_.end()) {
    set<AdGroupPtr>& key_result =it->second;
    result.insert(key_result.begin(), key_result.end());
  }
  if(province != key) {
    it = index_.find(province);
    if(it != index_.end()) {
      set<AdGroupPtr>& key_result =it->second;
      result.insert(key_result.begin(), key_result.end());
    }
  }
  if(province != COUNTRY) {
    it = index_.find(COUNTRY);
    if(it != index_.end()) {
      set<AdGroupPtr>& key_result =it->second;
      result.insert(key_result.begin(), key_result.end());
    }
  }
  return result;
}

bool AdInvertedIndex::Get(const UserProfile & profile, const Ice::Long zone_id, set<AdGroupPtr>& result_set) {
  vector<AdGroupPtr> result;
  int user_stage = profile.stage(); 

  DimIndexPtr dim_index;
  {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    map<Ice::Long, DimIndexPtr>::const_iterator it = zone_index_.find(zone_id);
    if (it != zone_index_.end()) {
      dim_index = it->second;
    }
  }
  if (!dim_index) {
    return false;
  }

  // 符合gender的广告
  const set<AdGroupPtr>& gender = dim_index->gender_index_->Get(profile.gender());
  // 符合stage的广告
  const set<AdGroupPtr>& stage = dim_index->stage_index_->Get((short)user_stage);

  // 求gender 和 stage 交集
  result.resize(min(gender.size(),stage.size()), NULL);
  vector<AdGroupPtr>::iterator it = set_intersection(gender.begin(), gender.end(), stage.begin(), stage.end(), result.begin());
  result_set.insert(result.begin(), it);

  // 符合age的广告
  const set<AdGroupPtr>& age = dim_index->age_index_->GetWithAll((short)profile.age());
  // 之前结果和age求交集
  it = set_intersection(result_set.begin(), result_set.end(), age.begin(), age.end(), result.begin());

  result_set.clear();
  result_set.insert(result.begin(), it);

  // 符合地域的广告 : 学生按school/current_area, 白领按ip_area.
  if (user_stage == 4) {
    uint64_t ip_area = lexical_cast<uint64_t> (profile.ip_area());
    set<AdGroupPtr> area = dim_index->area_index_->GetWithAll(ip_area);
    it = set_intersection(result_set.begin(), result_set.end(), area.begin(), area.end(), result.begin());
    result_set.clear();
    result_set.insert(result.begin(), it);
  } else {
    set<AdGroupPtr> grade = dim_index->grade_index_->GetWithAll((short)profile.grade());
    it = set_intersection(result_set.begin(), result_set.end(), grade.begin(), grade.end(), result.begin());
    result_set.clear();
    result_set.insert(result.begin(), it);
    uint64_t school_id = lexical_cast<uint64_t>(profile.school());
    set<AdGroupPtr> school = dim_index->school_index_->GetWithAll(school_id);
    //学生用户，使用current_area用作地域匹配
    uint64_t area_id = lexical_cast<uint64_t>(profile.current_area());
    //去地域集合找该地域
    const set<AdGroupPtr>& area = dim_index->area_index_->GetWithAll(area_id);
    //并集
    school.insert(area.begin(), area.end());
    //school.insert(all.begin(), all.end());
    it = set_intersection(result_set.begin(), result_set.end(), school.begin(), school.end(), result.begin());
    result_set.clear();
    result_set.insert(result.begin(), it);
  }

  if (result_set.empty()) {
    MCE_DEBUG("Get no ad: uid=" << profile.id() << " gender=" << profile.gender() << " age=" << profile.age() << " stage=" 
        << profile.stage() << " school=" << profile.school() << " grade=" << profile.grade() << " current_area=" 
        << profile.current_area() << " ip_area=" << profile.ip_area());
  }

  return true;
}

bool AdInvertedIndex::Load(hash_map<Ice::Long, AdGroupPtr>& groups) {
  if(load_all_) {
    //load_all_ = false;
    TotalLoad(groups);
  } else {
    IncLoad(groups);
  }
  return true;
}

bool AdInvertedIndex::TotalLoad(hash_map<Ice::Long, AdGroupPtr>& groups) {
  MCE_INFO("AdInvertedIndex::TotalLoad() --> START!  groups.size() = " << groups.size());
  Statement sql;

  sql << "select * from bid_unit where delete_flag = 1";
  const string DB_INSTANCE = EngineConfig::instance().DB();

  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(DB_INSTANCE, CDbRServer, "bid_unit").store(sql);//TODO
  } catch (Ice::Exception& e) {
    MCE_WARN("AdInvertedIndex::TotalLoad() error: " << e);
    return false;
  } catch (std::exception& e) {
    MCE_WARN("AdInvertedIndex::TotalLoad() std::exception: " << e.what());
    return false;
  } catch (...) {
    MCE_WARN("AdInvertedIndex::TotalLoad() unknown error");
    return false;
  }

  if (!res) {
    MCE_INFO("AdInvertedIndex::TotalLoad() --> StoryQueryResult wrong");
    return false;
  }
  if (res.empty()) {
    MCE_INFO("AdInvertedIndex::TotalLoad() --> no ads in bid_unit ");
    return false;
  }

  map<Ice::Long, DimIndexPtr> zone_index;
  for(size_t i = 0; i < res.size(); ++i) {
    Ice::Long gid = lexical_cast<Ice::Long> ((mysqlpp::Null<string, mysqlpp::NullIsZero>) res.at(i)["adgroup_id"]);
    Ice::Long bid_id = lexical_cast<Ice::Long> ((mysqlpp::Null<string, mysqlpp::NullIsZero>) res.at(i)["bid_unit_id"]);
    Ice::Long member_id = lexical_cast<Ice::Long> ((mysqlpp::Null<string, mysqlpp::NullIsZero>) res.at(i)["member_id"]);
    int gender = lexical_cast<int> ((mysqlpp::Null<string, mysqlpp::NullIsZero>) res.at(i)["cast_gender"]);
    int stage = lexical_cast<int> ((mysqlpp::Null<string, mysqlpp::NullIsZero>) res.at(i)["cast_stage"]);
    string grade = (mysqlpp::Null<string, mysqlpp::NullIsBlank>) (res.at(i)["cast_grade"]);
    string age = (mysqlpp::Null<string, mysqlpp::NullIsBlank>) (res.at(i)["cast_age"]);
    string school = (mysqlpp::Null<string, mysqlpp::NullIsBlank>) (res.at(i)["cast_school"]);
    string area = (mysqlpp::Null<string, mysqlpp::NullIsBlank>) (res.at(i)["cast_area"]);
    hash_map<Ice::Long, AdGroupPtr>::iterator mit = groups.find(gid);
    if(mit == groups.end()) {
      continue;
    }
    AdGroupPtr group = mit->second;

    group->set_bid_unit_id(bid_id);

    if (member_id == 999) { // Filter default adgroups.
      continue;
    }

    if (EngineConfig::instance().IsForBrand()? (group->display_type() != 1 && group->display_type() != 4): (group->display_type() == 4)) {
    // if (group->display_type() == 4) { // This adgroup is cpm/cpc adgroup
      set<Ice::Long> zones = group->zone_ids();
      vector<short> short_keys;
      vector<uint64_t> uint_keys;
      for (set<Ice::Long>::iterator sit = zones.begin(); sit != zones.end(); ++sit) {
        Ice::Long zone_id = *sit;
        if (zone_index.find(zone_id) == zone_index.end()) {
          zone_index.insert(make_pair(zone_id, new DimIndex()));
        }
        DimIndexPtr dim_index = zone_index[zone_id];
        if(Translator::ProcessGender(gender, short_keys)) {
          dim_index->gender_index_->Add(short_keys, group);
          short_keys.clear();
        }
        if(Translator::ProcessStage(stage, short_keys)) {
          dim_index->stage_index_->Add(short_keys, group);
          short_keys.clear();
        }
        if(Translator::ProcessAge(age, short_keys)) {
          dim_index->age_index_->Add(short_keys, group);
          short_keys.clear();
        }
        if(stage == 2) {//只有stage == 2时才允许指定学校投放
          vector<uint64_t> uint_keys2;
          if(Translator::ProcessSchool(school, uint_keys, uint_keys2)) {
            dim_index->area_index_->Add(uint_keys, group);
            dim_index->school_index_->Add(uint_keys2, group);
            uint_keys.clear();
            uint_keys2.clear();
          }
          if(Translator::ProcessGrade(grade, short_keys)) {
            dim_index->grade_index_->Add(short_keys, group);
            short_keys.clear();
          }
        } else if((stage == 4) && Translator::ProcessArea(area, uint_keys)) {
          dim_index->area_index_->Add(uint_keys, group);
          uint_keys.clear();
        } else if(stage == 6 && Translator::ProcessArea(area, uint_keys)) {
          dim_index->area_index_->Add(uint_keys, group);
          uint_keys.clear();
          short_keys.push_back(-1);
          dim_index->grade_index_->Add(short_keys, group);
          short_keys.clear();
        }
      }
    } else if(group->display_type() == 1) {
      BidUnitPtr bid_unit = new BidUnit(gender, stage, grade, age, school, area);
      group->set_bid_unit(bid_unit);
    }
  }

  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    zone_index_.swap(zone_index);
    MCE_INFO("AdInvertedIndex::TotalLoad() --> DONE!");
  }
  return true;
}

bool AdInvertedIndex::IncLoad(hash_map<Ice::Long, AdGroupPtr>& groups) {
  MCE_INFO("AdInvertedIndex::IncLoad() --> START!  groups.size() = " << groups.size());
  Statement sql;

  sql << "select adgroup_id, bid_unit_id, member_id, cast_gender, cast_stage, "
      << " cast_grade, cast_age, cast_school, cast_area, unix_timestamp(last_update_time), delete_flag "
      << " from bid_unit where last_update_time > (now() - 300) order by bid_unit_id";//最近5分钟的更改
  const string DB_INSTANCE = EngineConfig::instance().DB();

  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(DB_INSTANCE, CDbRServer, "bid_unit").store(sql);//TODO
  } catch (std::exception& e) {
    MCE_WARN("AdInvertedIndex::IncLoad() std::exception: " << e.what());
    return false;
  } catch (...) {
    MCE_WARN("AdInvertedIndex::IncLoad() unknown error");
    return false;
  }

  if (!res) {
    MCE_INFO("AdInvertedIndex::IncLoad() --> StoryQueryResult wrong");
    return false;
  }
  if (res.empty()) {
    MCE_INFO("AdInvertedIndex::IncLoad() --> no change data, DONE!");
    return true;
  }
  bool has_element = false;
  for(size_t i = 0; i < res.size(); ++i) {
    //unix_timestamp(last_update_time)
    long last_update_time = lexical_cast<long> ((mysqlpp::Null<string, mysqlpp::NullIsZero>) res.at(i)["unix_timestamp(last_update_time)"]);
    if(last_update_time >= last_update_time_) {
      has_element = true;
      break;
    }
  }
  if(!has_element) {
    MCE_INFO("AdInvertedIndex::IncLoad() --> no change data, DONE!");
    return true;
  }
  map<Ice::Long, DimIndexPtr> zone_index;
  {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    zone_index = zone_index_;
  }
  for(size_t i = 0; i < res.size(); ++i) {
    Ice::Long gid = lexical_cast<Ice::Long> ((mysqlpp::Null<string, mysqlpp::NullIsZero>) res.at(i)["adgroup_id"]);
    Ice::Long bid_id = lexical_cast<Ice::Long> ((mysqlpp::Null<string, mysqlpp::NullIsZero>) res.at(i)["bid_unit_id"]);
    Ice::Long member_id = lexical_cast<Ice::Long> ((mysqlpp::Null<string, mysqlpp::NullIsZero>) res.at(i)["member_id"]);
    int gender = lexical_cast<int> ((mysqlpp::Null<string, mysqlpp::NullIsZero>) res.at(i)["cast_gender"]);
    int stage = lexical_cast<int> ((mysqlpp::Null<string, mysqlpp::NullIsZero>) res.at(i)["cast_stage"]);
    string grade = (mysqlpp::Null<string, mysqlpp::NullIsBlank>) (res.at(i)["cast_grade"]);
    string age = (mysqlpp::Null<string, mysqlpp::NullIsBlank>) (res.at(i)["cast_age"]);
    string school = (mysqlpp::Null<string, mysqlpp::NullIsBlank>) (res.at(i)["cast_school"]);
    string area = (mysqlpp::Null<string, mysqlpp::NullIsBlank>) (res.at(i)["cast_area"]);
    bool delete_flag =  lexical_cast<bool>( (mysqlpp::Null<string, mysqlpp::NullIsBlank>) (res.at(i)["delete_flag"]));
    //unix_timestamp(last_update_time)
    long last_update_time = lexical_cast<long> ((mysqlpp::Null<string, mysqlpp::NullIsZero>) res.at(i)["unix_timestamp(last_update_time)"]);
    if(last_update_time > last_update_time_) {
      last_update_time_ = last_update_time;
    }
    if(!delete_flag) {//删除，此时不检查是否在有效广告组列表groups中, 但会从自己的有效列表中查找
      map<Ice::Long, AdGroupPtr>::iterator it = cached_group_.find(bid_id);
      if(it != cached_group_.end() && it->second->display_type() == 4) {
        AdGroupPtr group = it->second;
        set<Ice::Long> zones = group->zone_ids();
        vector<short> short_keys;
        vector<uint64_t> uint_keys;
        for (set<Ice::Long>::iterator sit = zones.begin(); sit != zones.end(); ++sit) {
          Ice::Long zone_id = *sit;
          map<Ice::Long, DimIndexPtr>::iterator zit = zone_index.find(zone_id);
          if (zit == zone_index.end()) {//无此广告位
            continue;
          }
          DimIndexPtr dim_index = zit->second;
          if(Translator::ProcessGender(gender, short_keys)) {
            dim_index->gender_index_->Del(short_keys, group);
            short_keys.clear();
          }
          if(Translator::ProcessStage(stage, short_keys)) {
            dim_index->stage_index_->Del(short_keys, group);
            short_keys.clear();
          }
          if(Translator::ProcessAge(age, short_keys)) {
            dim_index->age_index_->Del(short_keys, group);
            short_keys.clear();
          }
          if(stage == 2) {//只有stage == 2时才允许指定学校投放
            vector<uint64_t> uint_keys2;
            if(Translator::ProcessSchool(school, uint_keys, uint_keys2)) {
              dim_index->area_index_->Del(uint_keys, group);
              dim_index->school_index_->Del(uint_keys2, group);
              uint_keys.clear();
              uint_keys2.clear();
            }
            if(Translator::ProcessGrade(grade, short_keys)) {
              dim_index->grade_index_->Del(short_keys, group);
              short_keys.clear();
            }
          } else if((stage == 4) && Translator::ProcessArea(area, uint_keys)) {
            dim_index->area_index_->Del(uint_keys, group);
            uint_keys.clear();
          } else if(stage == 6 && Translator::ProcessArea(area, uint_keys)) {
            dim_index->area_index_->Del(uint_keys, group);
            uint_keys.clear();
            short_keys.push_back(-1);
            dim_index->grade_index_->Del(short_keys, group);
            short_keys.clear();
          }
        }//end of for per adzone
      }// if(it != cached_group_.end() && it->second->display_type() == 4) {
      //清除cache列表
      cached_group_.erase(it);
    } else {//新增
      hash_map<Ice::Long, AdGroupPtr>::iterator mit = groups.find(gid);
      if(mit == groups.end()) {
        continue;
      }
      map<Ice::Long, AdGroupPtr>::iterator it = cached_group_.find(bid_id);
      if(it != cached_group_.end() || mit->second->display_type() != 4) {
        continue;
      }
      AdGroupPtr group = mit->second;
      group->set_bid_unit_id(bid_id);
      if (member_id == 999) { // Filter default adgroups.
        continue;
      }
      if (group->display_type() == 4) { // This adgroup is cpm/cpc adgroup
        set<Ice::Long> zones = group->zone_ids();
        vector<short> short_keys;
        vector<uint64_t> uint_keys;
        for (set<Ice::Long>::iterator sit = zones.begin(); sit != zones.end(); ++sit) {
          Ice::Long zone_id = *sit;
          if (zone_index.find(zone_id) == zone_index.end()) {
            zone_index.insert(make_pair(zone_id, new DimIndex()));
          }
          DimIndexPtr dim_index = zone_index[zone_id];
          if(Translator::ProcessGender(gender, short_keys)) {
            dim_index->gender_index_->Add(short_keys, group);
            short_keys.clear();
          }
          if(Translator::ProcessStage(stage, short_keys)) {
            dim_index->stage_index_->Add(short_keys, group);
            short_keys.clear();
          }
          if(Translator::ProcessAge(age, short_keys)) {
            dim_index->age_index_->Add(short_keys, group);
            short_keys.clear();
          }
          if(stage == 2) {//只有stage == 2时才允许指定学校投放
            vector<uint64_t> uint_keys2;
            if(Translator::ProcessSchool(school, uint_keys, uint_keys2)) {
              dim_index->area_index_->Add(uint_keys, group);
              dim_index->school_index_->Add(uint_keys2, group);
              uint_keys.clear();
              uint_keys2.clear();
            }
            if(Translator::ProcessGrade(grade, short_keys)) {
              dim_index->grade_index_->Add(short_keys, group);
              short_keys.clear();
            }
          } else if((stage == 4) && Translator::ProcessArea(area, uint_keys)) {
            dim_index->area_index_->Add(uint_keys, group);
            uint_keys.clear();
          } else if(stage == 6 && Translator::ProcessArea(area, uint_keys)) {
            dim_index->area_index_->Add(uint_keys, group);
            uint_keys.clear();
            short_keys.push_back(-1);
            dim_index->grade_index_->Add(short_keys, group);
            short_keys.clear();
          }
        }
        cached_group_[bid_id] = group;
      } else if(group->display_type() == 1) {
        BidUnitPtr bid_unit = new BidUnit(gender, stage, grade, age, school, area);
        group->set_bid_unit(bid_unit);
      }
    }
  }

  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    zone_index_.swap(zone_index);
    MCE_INFO("AdInvertedIndex::IncLoad() --> DONE!");
  }
  return true;
}


}
}
