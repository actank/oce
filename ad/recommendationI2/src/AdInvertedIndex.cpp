/*
 * AdInvertedIndex.cpp
 *
 *  Created on: Sep 2, 2010
 *      Author: antonio
 */

#include "AdInvertedIndex.h"
#include <QueryRunner.h>
#include <DbDescriptor.h>
#include <boost/lexical_cast.hpp>
#include "../../util/src/AdCommonUtil.h"

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

void DimIndex::Add(const uint64_t& key, const AdGroupPtr& group, bool first) {
  IceUtil::RWRecMutex::WLock lock(mutex_);
  index_set_[key].insert(group);
}

void DimIndex::Add(const vector<uint64_t> & keys, const AdGroupPtr & group,
    bool first) {
  IceUtil::RWRecMutex::WLock lock(mutex_);
  for (vector<uint64_t>::const_iterator vit = keys.begin(); vit != keys.end(); ++vit) {
    index_set_[*vit].insert(group);
  }
}

void DimIndex::Remove(const uint64_t& key, const AdGroupPtr& group) {
  IceUtil::RWRecMutex::WLock lock(mutex_);
  map<uint64_t, set<AdGroupPtr> >::iterator it = index_set_.find(key);
  it->second.erase(group);
}

set<AdGroupPtr> DimIndex::Get(const uint64_t& key) {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  map<uint64_t, set<AdGroupPtr> >::iterator it = index_set_.find(key);
  if (it != index_set_.end()) {
    return it->second;
  }
  return set<AdGroupPtr> ();

}

void DimIndex::FilterInactive(set<AdGroupPtr> & groups) {
  for (set<AdGroupPtr>::iterator sit = groups.begin(); sit != groups.end();) {
    if (!(*sit)->Available()) {
      groups.erase(sit++);
    } else if ((*sit)->display_type() == 1) {   // Erase rotation Ad.
      groups.erase(sit++);
    } else if (!((*sit)->HasCreatives())) {
      groups.erase(sit++);
    } else {
      ++sit;
    }
  }
}

//---------------------------------------------------------------------------
set<AdGroupPtr> MultiDimIndex::GetMatchedGroup(const uint64_t& key) {
  set<AdGroupPtr> result;
  {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    for (map<uint64_t, set<AdGroupPtr> >::iterator mit = index_set_.begin(); mit
        != index_set_.end(); ++mit) {
      if (mit->second.empty()) {
        continue;
      }
      if (Translator::instance().Match(key, mit->first)) {
        result.insert(mit->second.begin(), mit->second.end());
      }
    }
  }
//  FilterInactive(result);
  MCE_INFO("[DIMINDEX] MultiDimIndex::GetMatchedGroup --> after Filter result.size = " << result.size());
  return result;
}

//---------------------------------------------------------------------------
set<AdGroupPtr> RegionIndex::RegionMatch(const uint64_t& key) {
  set<AdGroupPtr> result = Get(AreaUtil::instance().GetChina());

  uint64_t province = AreaUtil::instance().GetProvince(key);
  uint64_t city = AreaUtil::instance().GetCity(key);
  if (!province) { //全国的
//    FilterInactive(result);
    return result;
  }
  set<AdGroupPtr> province_result = Get(province);
  result.insert(province_result.begin(), province_result.end());
  if (!city) { //key精确到省
//    FilterInactive(result);
    return result;
  }
  set<AdGroupPtr> city_result = Get(city); //key精确到市
  result.insert(city_result.begin(), city_result.end());
//  FilterInactive(result);
  return result;

}

//---------------------------------------------------------------------------

void DimIndexMatcher::Load(hash_map<Ice::Long, AdGroupPtr> & groups,
    map < Ice::Long, vector<AdGroupPtr> > & zone2global) {
  MCE_INFO("DimIndexMatcher::Load --> input groups size = " << groups.size());
  Statement sql;

  sql << "select * from bid_unit where delete_flag = 1   ";
  const string DB_INSTANCE = "ade_db"; //EngineConfig::instance().DB();

  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(DB_INSTANCE, CDbRServer, "bid_unit").store(sql);//TODO
  } catch (Ice::Exception& e) {
    MCE_WARN("DimIndexMatcher::Init :" << e);
  } catch (std::exception& e) {
    MCE_WARN("DimIndexMatcher::Init :" << e.what());
  }

  if (!res) {
    MCE_INFO("DimIndexMatcher::Init --> StoryQueryResult wrong");
    return;
  }
  if (res.empty()) {
    MCE_INFO("DimIndexMatcher::Init --> no ads in bid_unit ");
    return;
  }

  map<Ice::Long, DimIndexGroup> zone_index;
  for (size_t i = 0; i < res.size(); ++i) {
    BidUnit bid;
    Ice::Long gid = lexical_cast<Ice::Long> ((mysqlpp::Null<string,
        mysqlpp::NullIsZero>) res.at(i)["adgroup_id"]);
    Ice::Long bid_id = lexical_cast<Ice::Long> ((mysqlpp::Null<string,
        mysqlpp::NullIsZero>) res.at(i)["bid_unit_id"]);
    Ice::Long member_id = lexical_cast<Ice::Long> ((mysqlpp::Null<string,
        mysqlpp::NullIsZero>) res.at(i)["member_id"]);
    bid.set_bid_unit_id(bid_id);
    bid.set_gender(lexical_cast<int> ((mysqlpp::Null<string, mysqlpp::NullIsZero>) res.at(i)["cast_gender"]));
    bid.set_stage(lexical_cast<int> ((mysqlpp::Null<string, mysqlpp::NullIsZero>) res.at(i)["cast_stage"]));
    bid.set_grade((mysqlpp::Null<string, mysqlpp::NullIsBlank>) (res.at(i)["cast_grade"]));
    bid.set_age((mysqlpp::Null<string, mysqlpp::NullIsBlank>) (res.at(i)["cast_age"]));
    bid.set_area((mysqlpp::Null<string, mysqlpp::NullIsBlank>) (res.at(i)["cast_area"]));
    string school_str = (mysqlpp::Null<string, mysqlpp::NullIsBlank>) (res.at(i)["cast_school"]);
    bid.set_school(school_str);

    uint64_t key = Translator::instance().GetIndex(bid);
    MCE_DEBUG("[GLOBAL] DimIndexMatcher::Init --> groupid = " << gid << ", bidunit_key = " << key);

    hash_map<Ice::Long, AdGroupPtr>::iterator mit = groups.find(gid);
    if (mit == groups.end()) {
      // MCE_INFO("DimIndexMatcher::Init --> could not find gid = " << gid);
      continue;
    }
    AdGroupPtr group = mit->second;

    group->set_bid_unit_key(key);
    group->set_bid_unit_id(bid_id);

    // if (group->display_type() == 1) { // This adgroup is Rotation adgroup
    if (group->display_type() != 4) { // This adgroup is Rotation adgroup
      // MCE_INFO("DimIndexMatcher::Init --> This adgroup is Rotation adgroup. gid = " << gid);
      continue;
    }

    if (member_id == 999) { // Filter default adgroups.
      // MCE_INFO("DimIndexMatcher::Init --> This adgroup is Default adgroup. gid = " << gid);
      continue;
    }

    vector<uint64_t> schools, areas;// 使用translator
    schools = SchoolUtil::instance().ParseSchools(school_str);
    areas = AreaUtil::instance().ParseAreas(bid.area());

    if (bid.stage() == 6) {
      schools.insert(schools.end(), areas.begin(), areas.end());
    }

    set<Ice::Long> zones = group->zone_ids();

    for (set<Ice::Long>::iterator sit = zones.begin(); sit != zones.end(); ++sit) {
      Ice::Long zone_id = *sit;
      zone_index[zone_id].multi_dim_index->Add(key, group, true);
      zone_index[zone_id].area_index->Add(areas, group, true);
      zone_index[zone_id].school_index->Add(schools, group, true);

      if (Translator::instance().IsGlobal(key)) {
        MCE_DEBUG("[GLOBAL] DimIndexMatcher::Init --> groupid = " << gid << ", bidunit_key = " << key << ", zoneid = " << zone_id);
        map<Ice::Long, vector<AdGroupPtr> >::iterator it = zone2global.find(zone_id);
        if (it != zone2global.end()) {
          it->second.push_back(group);
        } else {
          vector<AdGroupPtr> group_list;
          group_list.push_back(group);
          zone2global[zone_id] = group_list;
        }
      }
    }
  }

  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    zone_index_.swap(zone_index);
  }
}

set<AdGroupPtr> DimIndexMatcher::Get(Ice::Long zone, UserProfile & profile) {
  TimeStat ts;
  MCE_INFO("[RANKER] [DIMINDEX] DimIndexMatcher::Get --> zoneid = " << zone);
//  UserProfile profile;
//  UserProfileClient::instance().Deserialize(pro_str, &profile);
  set<AdGroupPtr> result;

  UserBase user = UserBase::MakeUser(profile);
  set<AdGroupPtr> intersect;

  DimIndexGroup dimgroup;
  {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    map<Ice::Long, DimIndexGroup>::iterator mit = zone_index_.find(zone);
    if (mit == zone_index_.end()) {
      return result;
    }
    dimgroup = mit->second;
  }

  uint64_t m_key = Translator::instance().GetIndex(user);

  result = dimgroup.multi_dim_index->GetMatchedGroup(m_key);

  if (user.stage() == 2) {
    uint64_t school_area = 0;
    uint64_t school = 0;
    try {
      school_area = lexical_cast<uint64_t> (profile.current_area());
      school = lexical_cast<uint64_t> (profile.school());
    } catch (...) {
      MCE_WARN("[DIMINDEX] DimIndexMatcher::Get --> boost cast current_area = " << profile.current_area() << " school = " << profile.school() << "   TO uint64_t err");
    }
    set<AdGroupPtr> A = dimgroup.school_index->GetMatchedGroup(school_area);
    set<AdGroupPtr> B = dimgroup.school_index->GetMatchedGroup(school);
    A.insert(B.begin(), B.end());
    intersect = DimIndex::MergeGroupSet(A, result);
  } else {
    uint64_t current_area = 0;
    try {
      current_area = lexical_cast<uint64_t> (profile.ip_area());
    } catch (...) {
      MCE_WARN("[DIMINDEX] DimIndexMatcher::Get --> boost cast ip_area = " << profile.ip_area() << "   TO uint64_t err");
    }
    set<AdGroupPtr> area_result = dimgroup.area_index->GetMatchedGroup(current_area);

    intersect = DimIndex::MergeGroupSet(area_result, result);
  }
  MCE_INFO("DimIndexMatcher --> ~~~~~~~~~~~~~~~~GET_TIME_TEST = " << ts.getTime());
  return intersect;
}

map<Ice::Long, AdGroupPtr> DimIndexMatcher::Get(Ice::Long zone_id, const uint64_t& key) {
  MCE_INFO("DimIndexMatcher::Get --> zone_id = " << zone_id << ", user key = " << key);
  TimeStat ts;
  map<Ice::Long, AdGroupPtr> result;

  DimIndexGroup dimgroup;
  {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    map<Ice::Long, DimIndexGroup>::iterator mit = zone_index_.find(zone_id);
    if (mit == zone_index_.end()) {
      return result;
    }
    dimgroup = mit->second;
  }

  set<AdGroupPtr> gs = dimgroup.multi_dim_index->GetMatchedGroup(key);
  for (set<AdGroupPtr>::const_iterator sit = gs.begin(); sit != gs.end(); sit++) {
    if (key == 10414574138294274) {
        MCE_DEBUG("DimIndexMatcher::Get --> zone_id = " << zone_id
            << ", user key = " << key << ", group_id = "
            << (*sit)->group_id());
    }
    result[(*sit)->group_id()] = *sit;
  }

  MCE_INFO("DimIndexMatcher::Get --> zone_id = " << zone_id << ", user key = " << key << ", used time = " << ts.getTime());
  return result;
}

