/*
 * AdInvertedIndex.cpp
 *
 *  Created on: Sep 2, 2010
 *      Author: antonio
 */

#include "AdInvertedIndex.h"
#include "AdMatchManagerI.h"
#include <QueryRunner.h>
#include <DbDescriptor.h>
#include <boost/lexical_cast.hpp>

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

    if(i >= kPrintLimit){
      ios << " ...... ";
      break;
    }
  }
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
    } else if (((*sit)->display_type() == 1) || ((*sit)->display_type() == 4)) {
      groups.erase(sit++);
    } else if (!((*sit)->HasCreatives())) {
      groups.erase(sit++);
    }else if ((*sit)->member_id() == 999) {
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
      if ((key & (mit->first)) == key) {
        result.insert(mit->second.begin(), mit->second.end());
      }
    }
  }
  FilterInactive(result);
  return result;
}

//---------------------------------------------------------------------------

set<AdGroupPtr> AreaIndex::GetMatchedGroup(const uint64_t& key) {
  set<AdGroupPtr> result = Get(AreaUtil::instance().GetChina());

  uint64_t province = AreaUtil::instance().GetProvince(key);
  uint64_t city = AreaUtil::instance().GetCity(key);
  if (!province) { //全国的
    FilterInactive(result);
    return result;
  }
  set<AdGroupPtr> province_result = Get(province);
  result.insert(province_result.begin(), province_result.end());
  if (!city) { //key精确到省
    FilterInactive(result);
    return result;
  }
  set<AdGroupPtr> city_result = Get(city); //key精确到市
  result.insert(city_result.begin(), city_result.end());
  FilterInactive(result);
  return result;

}

//---------------------------------------------------------------------------

set<AdGroupPtr> SchoolIndex::GetMatchedGroup(const uint64_t& key) {
  if (AreaUtil::instance().IsArea(key)) { //地域条件
    set<AdGroupPtr> result = Get(AreaUtil::instance().GetChina());
    uint64_t province = AreaUtil::instance().GetProvince(key);
    uint64_t city = AreaUtil::instance().GetCity(key);
    if (province == 0) { //全国的
      FilterInactive(result);
      return result;
    }
    set<AdGroupPtr> province_result = Get(province);
    result.insert(province_result.begin(), province_result.end());
    if (city == 0) {
      FilterInactive(result);
      return result;
    }

    set<AdGroupPtr> city_result = Get(city);
    result.insert(city_result.begin(), city_result.end());
    FilterInactive(result);
    return result;
  } else { //指定学校
    set<AdGroupPtr> result = Get(key);
    FilterInactive(result);
    return result;
  }
}
//-----------------------------------------------------------------------------

void DimIndexMatcher::Load(map<Ice::Long, AdGroupPtr> & groups) {
  Statement sql;

  sql << "select * from bid_unit where delete_flag = 1   ";

  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(DB_INSTANCE, CDbRServer, "bid_unit").store(sql);//TODO
  } catch (Ice::Exception& e) {
    MCE_WARN("DimIndexMatcher::Init :" << e);
  } catch (std::exception& e) {
    MCE_WARN("DimIndexMatcher::Init :" << e.what());
  }

  if (!res) {
    MCE_WARN("DimIndexMatcher::Init --> StoryQueryResult wrong");
    return;
  }
  if (res.empty()) {
    MCE_WARN("DimIndexMatcher::Init --> no ads in bid_unit ");
    return;
  }

  map<Ice::Long, DimIndexGroup> zone_index;
  for (size_t i = 0; i < res.size(); ++i) {
    BidUnit bid;
    Ice::Long gid = lexical_cast<Ice::Long> ((mysqlpp::Null<string,
        mysqlpp::NullIsZero>) res.at(i)["adgroup_id"]);
    Ice::Long bid_id = lexical_cast<Ice::Long> ((mysqlpp::Null<string,
        mysqlpp::NullIsZero>) res.at(i)["bid_unit_id"]);
    bid.bid_unit_id = bid_id;
    bid.gender = lexical_cast<int> (
        (mysqlpp::Null<string, mysqlpp::NullIsZero>) res.at(i)["cast_gender"]);
    bid.stage = lexical_cast<int> (
        (mysqlpp::Null<string, mysqlpp::NullIsZero>) res.at(i)["cast_stage"]);
    bid.grade
        = (mysqlpp::Null<string, mysqlpp::NullIsBlank>) (res.at(i)["cast_grade"]);
    bid.age
        = (mysqlpp::Null<string, mysqlpp::NullIsBlank>) (res.at(i)["cast_age"]);
    bid.area
        = (mysqlpp::Null<string, mysqlpp::NullIsBlank>) (res.at(i)["cast_area"]);
    string
        school_str =
            (mysqlpp::Null<string, mysqlpp::NullIsBlank>) (res.at(i)["cast_school"]);
    bid.school = school_str;
    map<Ice::Long, AdGroupPtr>::iterator mit = groups.find(gid);
    if (mit == groups.end()) {
      continue;
    }
    AdGroupPtr group = mit->second;

    vector<uint64_t> schools, areas;//TODO 使用translator
    schools = Translator::instance().ParseSchools(school_str);
    areas = Translator::instance().ParseAreas(bid.area);

    if (bid.stage == 6) {
      schools.insert(schools.end(), areas.begin(), areas.end());
    }

    uint64_t key = Translator::instance().GetIndex(bid);
    set<Ice::Long> zones = group->zone_ids();
    //MCE_DEBUG("DimIndexMatcher::Load --> gid = " << gid << ",key = " << key << ",zones size = " << zones.size() << " __bid_unit_id = " << bid_id << ",");

    for (set<Ice::Long>::iterator sit = zones.begin(); sit != zones.end(); ++sit) {
      zone_index[*sit].multi_dim_index->Add(key, group, true);
      zone_index[*sit].area_index->Add(areas, group, true);
      zone_index[*sit].school_index->Add(schools, group, true);
    }

    group->set_bid_unit_key(key);
    group->set_bid_unit_id(bid_id);
  }
  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    zone_index_.swap(zone_index);
  }
}

User DimIndexMatcher::MakeUser(UserProfile & profile) {
  Ice::Long uid = profile.id();

  LogHelper::instance().CookieEmptyCheck(profile);

  User user;
  user.age = profile.age();
  if (profile.stage() == 2) {
    user.area = profile.current_area();
  } else {
    user.area = profile.ip_area();
  }
  user.gender = profile.gender();
  user.stage = profile.stage();
  user.grade = profile.grade();
  return user;
}
set<AdGroupPtr> DimIndexMatcher::Get(Ice::Long zone, const string & pro_str) {
  TimeStat ts;
  TimeStat tts;
  UserProfile profile;
  ts.reset();
  UserProfileClient::instance().Deserialize(pro_str, &profile);
  set<AdGroupPtr> result;

  User user = MakeUser(profile);
  set<AdGroupPtr> intersect;

  ts.reset();
  DimIndexGroup dimgroup;
  {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    map<Ice::Long, DimIndexGroup>::iterator mit = zone_index_.find(zone);
    if (mit == zone_index_.end()) {
      //MCE_INFO("DimIndexMatcher::Get --> no zone : " << zone << " return");
      return result;
    }
    dimgroup = mit->second;
  }

  uint64_t m_key = Translator::instance().GetIndex(user);

  ts.reset();
  result = dimgroup.multi_dim_index->GetMatchedGroup(m_key);
  if(result.empty()){
    return intersect;
  }
  //MCE_DEBUG("DimIndexMatcher::Get --> m_key result size = " << result.size() << ",result = " << PrintHelper(result));
  if (user.stage == 2) {
    uint64_t school_area = 0;
    uint64_t school = 0;
    try {
      school_area = lexical_cast<uint64_t> (profile.current_area());
      school = lexical_cast<uint64_t> (profile.school());
    } catch (...) {
      MCE_WARN("DimIndexMatcher::Get --> boost cast current_area = " << profile.current_area() << " school = " << profile.school() << "   TO uint64_t err");
    }
    set<AdGroupPtr> A = dimgroup.school_index->GetMatchedGroup(school_area);
    set<AdGroupPtr> B = dimgroup.school_index->GetMatchedGroup(school);
    //MCE_DEBUG("DimIndexMatcher::Get --> school_area result = " << A.size() << ",A = " << PrintHelper(A));
    //MCE_DEBUG("DimIndexMatcher::Get --> school result = " << B.size() << ",B = " << PrintHelper(B));
    A.insert(B.begin(), B.end());
    intersect = DimIndex::MergeGroupSet(A, result);
    //MCE_DEBUG("DimIndexMatcher::Get --> intersect result = " << intersect.size() << ",intersect = " << PrintHelper(intersect));
  } else {
    uint64_t current_area = 0;
    try {
      current_area = lexical_cast<uint64_t> (profile.ip_area());
    } catch (...) {
      MCE_WARN("DimIndexMatcher::Get --> boost cast ip_area = " << profile.ip_area() << "   TO uint64_t err");
    }
    ts.reset();
    set<AdGroupPtr> area_result = dimgroup.area_index->GetMatchedGroup(
        current_area);

    ts.reset();
    intersect = DimIndex::MergeGroupSet(area_result, result);
    //MCE_DEBUG("DimIndexMatcher --> GET_TIME_TEST DimIndex::MergeGroupSet = " << ts.getTime());
    //MCE_DEBUG("DimIndexMatcher::Get --> area_result = " << area_result.size() << "  intersect size = " << intersect.size());

  }
  MCE_DEBUG("DimIndexMatcher --> ~~~~~~~~~~~~~~~~TOTAL GET_TIME_TEST  = " << tts.getTime());
  return intersect;

}
//----------------------------------------------------------------------------
uint64_t AreaUtil::GetProvince(uint64_t area) {
  static uint64_t i = 10000000000;
  uint64_t province = area / i * i;
  if (province >= 86010000000000 && province <= 86990000000000) {
    return province;
  } else {
    return 0;
  }
}

uint64_t AreaUtil::GetCity(uint64_t area) {
  if (area == GetProvince(area)) { //city的位为0
    return 0;
  } else {
    return area;//保证city的位之后的所有位都为0
  }
}

bool AreaUtil::IsArea(uint64_t area_or_school) {
  return area_or_school > 999999999999;//学校码小于这个数，地域码都大于这个数
}

uint64_t AreaUtil::GetChina() {
  return 86000000000000;
}

