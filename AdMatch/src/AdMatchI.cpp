/*
 * AdMatchI.cpp
 *
 *  Created on: Jul 20, 2010
 *      Author: yejingwei
 */

#include "AdMatchI.h"
#include "AdUtil.h"
#include "ServiceI.h"
#include "Date.h"
#include <QueryRunner.h>
#include <DbDescriptor.h>

#include <boost/tokenizer.hpp>
#include <AdAnalysisAdapter.h>

#include "client/linux/handler/exception_handler.h"

using namespace com::xiaonei::xce;
using namespace xce::ad;
using namespace boost;

bool MyCallback(const char *dump_path, const char *minidump_id, void *context,
    bool succeeded) {
  printf("got crash %s %s\n", dump_path, minidump_id);
  return true;
}

void MyUtil::initialize() {

  static google_breakpad::ExceptionHandler eh(".", 0, MyCallback, 0, true);

  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&AdMatchI::instance(), service.createIdentity("M",
      ""));
  TaskManager::instance().config(ThreadPoolConfig(1, 10));
  TaskManager::instance().execute(new InitTask);

}

//-----------------------------------------------------------------------------------------------


AdInfoSeq AdMatchI::GetAds(const string& userProfile, ::Ice::Int count,
    const string& referer, const Ice::Current&) {
  string tmpref = referer;
  return Matcher::instance().GetAds(userProfile, count, tmpref);
}

AdInfoSeq AdMatchI::GetAdsTest(::Ice::Int uid, ::Ice::Int stage,
    ::Ice::Int gender, ::Ice::Int age, const ::std::string& currentArea,
    const ::std::string& ipArea, ::Ice::Long school, ::Ice::Int grade,
    int count, const string& referer, const Ice::Current&) {
  string tmpref = referer;
  return Matcher::instance().GetAdsTest(uid, stage, gender, age, currentArea,
      ipArea, school, grade, count, tmpref);
}

void AdMatchI::Click(int uid, const string & ip, const string& text,
    const string & referer, const string & click_url, const Ice::Current&) {

  TaskManager::instance().execute(new ClickTask(uid, ip, text, referer,
      click_url));
}
void AdMatchI::LoadOneGroup(Ice::Long gid, const Ice::Current&) {
  TaskManager::instance().execute(new LoadOneGroupTask(gid));
}
void AdMatchI::ReloadOneGroup(Ice::Long gid, const Ice::Current&) {
  TaskManager::instance().execute(new ReloadOneGroupTask(gid));
}

void AdMatchI::DelGroup(Ice::Long gid, const Ice::Current&) {
  TaskManager::instance().execute(new DelGroupTask(gid));
}

bool AdMatchI::GetAvailableAds(const ::MyUtil::LongSeq& ids,
    const Ice::Current&) {
  TaskManager::instance().execute(new GetAvailableAdsTask(ids));
  return true;
}

void AdMatchI::ReportDefault(int uid, const string & ip, int adCount,
    const DefaultSeq & ads, const Ice::Current&) {
  MCE_DEBUG(__FILE__ << ":" << __LINE__);
  Matcher::instance().ReportDefault(uid, ip, adCount, ads);

}

bool AdMatchI::SetClickInfo(Ice::Long gid, int key, int click, int pv,
    const Ice::Current&) {
  return Matcher::instance().SetClickInfo(gid, key, click, pv);
}

//-----------------------------------------------------------------------------------------------
Matcher::Matcher() :
  ready_(false), price_time_(0), bid_time_(0), crtv_time_(0), allow_scanning_(
      false) {
}

AdInfoSeq Matcher::GetAds(const string & pro_str, int count, string & referer) {

  AdInfoSeq out;
  UserProfile profile;
  UserProfileClient::instance().Deserialize(pro_str, &profile);
  MCE_INFO("Matcher::GetAds --> uid = "<<profile.id()<<",age = " << profile.age() << ",gender = "<< profile.gender() <<
      ",stage = " << profile.stage() << ",grade = " << profile.grade() <<
      ",current_area = " << profile.current_area() << ",ip_area = " << profile.ip_area() << ",school =" << profile.school() << ",count = " << count);
  int uid = profile.id();
  if (!ready_) {
    MCE_WARN("Matcher::GetAds --> matcher is not ready, uid = " << uid << ",out size = " << out.size() << ",default add size = " << count - out.size());
    return out;
  }
  if (uid == 0) { //没有cookie的用户
    profile.set_age(23);
    profile.set_ip_area("0086000000000000");//全国
    profile.set_stage(4);
    profile.set_gender(1);//男性
    profile.set_ip(975044608); //北京的ip
  }
  Params params;
  params.uid = uid;
  if (!Preprocess(profile, params)) {
    MCE_WARN("Matcher::GetAds --> does not pass preprocess, id = " << uid << ",out size = " << out.size() << ",default add size = " << count - out.size());
    return out;
  }

  int m_key = AdUtil::GetUserKey(params);
  MCE_INFO("Matcher::GetAds --> uid = "<< uid <<", m_key = " << m_key);
  AdSetPtr setA = m_idx_.Get(m_key);
  if (!setA) {
    MCE_WARN("Matcher::GetAds --> m_idx_.Get(m_key) result is empty, uid = "<<uid<<",key = " << m_key);//map中没有这个key，一般不会发生
    setA = new AdSet;
  }

  MCE_DEBUG("Matcher::GetAdsTest --> setA size = " << setA->Size());
  AdSetPtr setB;
  if (params.stage == 2) {//学生
    if (params.current_area == 86000000000000) { //只提供全国
      setB = s_idx_.Get(-86000000000000);
      if (!setB) {
        MCE_WARN("Matcher::GetAdsTest --> s_idx_.Get(-86000000000000) size = 0 " );
        setB = new AdSet;
      }
    } else {
      AdSetPtr setChina = s_idx_.Get(-86000000000000);
      if (!setChina) {
        MCE_WARN("Matcher::GetAdsTest --> s_idx_.Get(-86000000000000) size = 0 " );
        setChina = new AdSet;
      }
      MCE_DEBUG("Matcher::GetAdsTest --> setChina size = " << setChina->Size());
      Ice::Long school_province = -(params.current_area / 10000000000
          * 10000000000);
      AdSetPtr setProvince = s_idx_.Get(school_province);

      Ice::Long school_city = -(params.current_area / 100000000 * 100000000);
      AdSetPtr setCity = s_idx_.Get(school_city);
      MCE_DEBUG("current_area = "<< params.current_area << ",school_province = " << school_province << ",school_city = " << school_city);

      AdSetPtr setSchool = s_idx_.Get(params.school);

      setB = setChina;

      if (setProvince) {
        setB = *setB + *setProvince;
        MCE_DEBUG("Matcher::GetAdsTest --> setProvince size = " << setProvince->Size());
      } else {
        MCE_DEBUG("Matcher::GetAdsTest --> setProvince empty " );
      }
      if (setCity) {
        setB = *setB + *setCity;
        MCE_DEBUG("Matcher::GetAdsTest --> setCity size = " << setCity->Size());
      } else {
        MCE_DEBUG("Matcher::GetAdsTest --> setCity empty " );
      }
      if (setSchool) {
        setB = *setB + *setSchool;
        MCE_DEBUG("Matcher::GetAdsTest --> setSchool size = " << setSchool->Size() << ",key = " << setSchool->key_);
      }

    }
  } else {//白领
    if (params.ip_area == 86000000000000) { //只提供全国
      setB = a_idx_.Get(86000000000000);
      if (!setB) {
        MCE_WARN("Matcher::GetAdsTest --> a_idx_.Get(86000000000000) size = 0 " ); //地域不用负号
        setB = new AdSet;
      }
    } else {
      AdSetPtr setChina = a_idx_.Get(86000000000000);
      if (!setChina) {
        MCE_WARN("Matcher::GetAdsTest --> a_idx_.Get(86000000000000) size = 0 " );
        setChina = new AdSet;
      }
      AdSetPtr setProvince = a_idx_.Get(params.ip_area / 10000000000
          * 10000000000);
      AdSetPtr setCity = a_idx_.Get(params.ip_area / 100000000 * 100000000);
      MCE_DEBUG("Matcher::GetAdsTest --> setChina size = " << setChina->Size());
      setB = setChina;

      if (setProvince) {
        MCE_DEBUG("Matcher::GetAdsTest --> setProvince size = " << setChina->Size());
        setB = *setB + *setProvince;
      }
      if (setCity) {
        setB = *setB + *setCity;
      }
    }
  }

  vector<pair<AdGroupPtr, FormatParam> > groups = FilterAdGroupB(setA, setB,
      count);
  int ip = profile.ip();

  out = Format(groups, m_key, count, profile, referer);
  MCE_DEBUG("Matcher::GetAds --> out size = " << out.size() << ",default add size = " << count - out.size());
  return out;
}

void Matcher::ProcessUrl(string & str) {
  for (int i = 0; i < str.length(); i++) {
    if (str[i] == '|') {
      str.replace(i, 1, "-");
    }
  }
}

bool Matcher::Preprocess(UserProfile & profile, Params & params) {

  params.stage = profile.stage();
  if (params.stage != 2 && params.stage != 4) {
    MCE_WARN("Matcher::GetAds --> uid = "<<profile.id()<<",stage = " << params.stage << ",not 2 , not 4");
    return false;
  }
  params.gender = profile.gender();
  if (params.gender != 0 && params.gender != 1) {
    MCE_WARN("Matcher::GetAds --> uid = "<<profile.id()<<",gender = " << params.gender << ",gender not 0 or 1");
    params.gender %= 2;
  }
  params.age = profile.age();
  if (params.age < 15 || params.age > 80) {
    MCE_WARN("Matcher::GetAds --> uid = "<<profile.id()<<",age = " << params.age << ",age not in range [15,80]");
    return false;
  }
  params.grade = profile.grade();
  if (params.stage == 2) {
    if (params.grade == 0) { //对于大学生，如果没有填写年级，默认为大一
      params.grade = 41;
    } else if (params.grade < 40) {//对于中小学生，不发广告
      MCE_INFO("Matcher::GetAds --> user is student , uid = "<<profile.id()<<",grade = " << params.grade << ",grade not in range");
      return false;
    } else if (params.grade > 45 && params.grade < 50) {
      MCE_WARN("Matcher::GetAds --> A user is student ,wrong grade , uid = "<<profile.id() << ",grade = " << params.grade);
      params.grade = 45;
    } else if (params.grade > 54 && params.grade < 60) {
      MCE_WARN("Matcher::GetAds --> B user is student ,wrong grade , uid = "<<profile.id() << ",grade = " << params.grade);
      params.grade = 53;
    } else if (params.grade > 61 && params.grade < 70) {
      MCE_WARN("Matcher::GetAds --> C user is student ,wrong grade , uid = "<<profile.id() << ",grade = " << params.grade);
      params.grade = 61;
    } else if (params.grade > 70) { //大于70
      MCE_WARN("Matcher::GetAds --> D user is student ,wrong grade , uid = "<<profile.id() << ",grade = " << params.grade);
      return false;
    }
  }
  if (params.stage == 2) {
    params.school = profile.school();
    if (profile.current_area().empty() || profile.current_area()
        == "0000000000000000") {
      MCE_DEBUG("Matcher::GetAds --> uid = "<< profile.id() <<",current_area is not provided ,use 86000000000000");
      params.current_area = 86000000000000;
    } else {
      params.current_area = lexical_cast<Ice::Long> (profile.current_area());
    }
  }
  if (params.stage == 4) {
    if (profile.ip_area().empty() || profile.ip_area() == "0000000000000000") {
      MCE_DEBUG("Matcher::GetAds --> uid = "<< profile.id() <<",ip_area = "<< profile.ip_area()<< ",use 86000000000000");
      params.ip_area = 86000000000000;
    } else {
      params.ip_area = lexical_cast<Ice::Long> (profile.ip_area());
    }
  }
  params.current_province = params.current_area / 10000000000 % 100;
  params.ip_province = params.ip_area / 10000000000 % 100;
  return true;
}

AdInfoSeq Matcher::GetAdsTest(::Ice::Int uid, ::Ice::Int stage,
    ::Ice::Int gender, ::Ice::Int age, const ::std::string& currentArea,
    const ::std::string& ipArea, ::Ice::Long school, ::Ice::Int grade,
    int count, const string & referer) {
  AdInfoSeq out;
  UserProfile profile;
  profile.set_age(age);
  profile.set_id(uid);
  profile.set_gender(gender);
  profile.set_stage(stage);
  profile.set_current_area(currentArea);
  profile.set_ip_area(ipArea);
  profile.set_school(school);
  profile.set_grade(grade);

  profile.set_ip(1896477159);

  string tmp;
  UserProfileClient::instance().Serialize(profile, &tmp);

  string tmpref = referer;
  return Matcher::GetAds(tmp, count, tmpref);

}

void Matcher::Init() {
  AdUtil::instance().Init();
  SetServiceIndex();
  Load(true);
  TaskManager::instance().scheduleRepeated(new ScanTimer);
  ready_ = true;
  allow_scanning_ = true;
}

bool Matcher::DelGroup(Ice::Long gid) {
  vector<int> keys;
  set<Ice::Long> schools, areas;
  {
    IceUtil::RWRecMutex::RLock lock(groups_mutex_);
    map<Ice::Long, AdGroupPtr>::iterator mit = group_set_.find(gid);
    if (mit == group_set_.end()) {
      //      MCE_WARN("Matcher::DelGroup --> gid not exist in group_set_ ,gid = " << gid);
      return false;
    } else {
      keys = mit->second->keys_;
      schools = mit->second->schools_;
      areas = mit->second->areas_;
    }
  }
  MCE_INFO("Matcher::DelGroup --> gid = "<<gid<<",keys size = " << keys.size() << ",schools size = " << schools.size() << ",areas size = " << areas.size());
  //  MCE_DEBUG("Matcher::DelGroup --> keys = " << PrintHelper(keys) << ",schools = " << PrintHelper(schools) << ",area = " << PrintHelper(areas));
  if (!keys.empty()) {
    for (vector<int>::iterator vit = keys.begin(); vit != keys.end(); ++vit) {
      m_idx_.Get(*vit)->Del(gid);
    }
  }
  if (!schools.empty()) {
    for (set<Ice::Long>::iterator vit = schools.begin(); vit != schools.end(); ++vit) {
      s_idx_.Get(*vit)->Del(gid);
    }
  }
  if (!areas.empty()) {
    for (set<Ice::Long>::iterator vit = areas.begin(); vit != areas.end(); ++vit) {
      a_idx_.Get(*vit)->Del(gid);
    }
  }

  {
    IceUtil::RWRecMutex::WLock lock(groups_mutex_);
    group_set_.erase(gid);
  }
  MCE_DEBUG("Matcher::DelGroup --> done , gid = " << gid);

  return true;
}

void Matcher::ReloadOneGroup(Ice::Long gid) {
  //  if (DelGroup(gid)) {
  //    MCE_DEBUG("Matcher::ReloadOneGroup --> gid = " << gid << ",del group success");
  //  } else {
  //    MCE_WARN("Matcher::ReloadOneGroup --> gid = " << gid << ",group does not exist");
  //  }
  DelGroup(gid);

  Ice::LongSeq ids;
  ids.push_back(gid);
  Load(false, ids);
}

void Matcher::Load(bool is_first, const Ice::LongSeq& group_ids) {
  MCE_DEBUG("Matcher::Load --> is_first = " << is_first << ", group_ids size = " << group_ids.size() );
  map<Ice::Long, AdGroupPtr> ad_group_map;

  // load ad_group;

  Statement sql;

  //  sql << "select * from ad_group where is_active = 1";
  //  sql << "select ad_group.group_id , ad_group.highest_bid  "
  //      "from bid_unit, ad_campaign ,ad_group ,ad_member "
  //      "where (ad_campaign.end_time > now() or ad_campaign.end_time is NULL) "
  //      "and ad_campaign.campaign_id = ad_group.campaign_id "
  //      "and ad_campaign.member_id = ad_member.id "
  //      "and ad_member.status = 1 "
  //      "and bid_unit.adgroup_id = ad_group.group_id and ad_group.highest_bid >= 50 ";
  sql
      << "select ad_group.group_id as group_id ,ad_group.ad_zone_id as ad_zone_id,"
        " ad_group.highest_bid as highest_bid,"
        "ad_group.member_id as member_id ,"
        "ad_group.campaign_id as campaign_id ,ad_group.update_time as update_time "
        "from  ad_campaign ,ad_group ,ad_member "
        "where (ad_campaign.end_time > now() or ad_campaign.end_time is NULL) "
        "and ad_campaign.campaign_id = ad_group.campaign_id "
        "and ad_campaign.member_id = ad_member.id "
        "and ad_member.status = 1  "
        "and ad_campaign.is_active = 1 "
        "and ad_group.highest_bid >= 50 "
        "and ad_group.is_active = 1 and ad_group.member_id != 999";

  //  sql << " and member_id = " << tester_; //TODO 待删除
  if (!group_ids.empty()) {
    sql << " and group_id in " << PrintHelper(group_ids);
  }

  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(db_instance, CDbRServer).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("Matcher::Load :" << e << ", line:" << __LINE__);
  } catch (std::exception& e) {
    MCE_WARN("Matcher::Load :" << e.what() << ", line:" << __LINE__);
  }
  if (!res) {
    MCE_WARN("StoryQueryResult wrong");
    return;
  }
  if (res.empty()) {
    MCE_WARN("Matcher::Load --> no ads in bid_unit ");
    return;
  }
  MCE_DEBUG("Matcher::Load --> ad_group size = " << res.size());

  for (size_t i = 0; i < res.size(); ++i) {

    AdGroupPtr g = new AdGroup();
    g->member_id_ = (Ice::Long) (res.at(i)["member_id"]);
    g->campaign_id_ = (Ice::Long) (res.at(i))["campaign_id"];
    g->group_id_ = (Ice::Long) (res.at(i))["group_id"];
    g->max_price_ = (int) (res.at(i))["highest_bid"];
    g->adzone_id_ = (Ice::Long) (res.at(i))["ad_zone_id"];
    g->use_average_click_ = !is_first;

    ad_group_map[g->group_id_] = g;
  }

  MyUtil::LongSeq part_ids;
  set<Ice::Long> printIds;
  for (map<Ice::Long, AdGroupPtr>::iterator it = ad_group_map.begin(); it
      != ad_group_map.end();) {
    part_ids.push_back(it->second->group_id_);
    ++it;
    if (part_ids.size() >= 7000 || it == ad_group_map.end()) {
      BidParamsSeq bseq = LoadBidUnits(part_ids);
      map<Ice::Long, CreativeSetPtr> creative = LoadCreatives(part_ids);

      part_ids.clear();

      for (size_t i = 0; i < bseq.size(); ++i) {
        map<Ice::Long, AdGroupPtr>::iterator git = ad_group_map.find(
            bseq.at(i)->group_id);
        if (git == ad_group_map.end()) {
          continue;
        }
        map<Ice::Long, CreativeSetPtr>::iterator cit = creative.find(
            bseq.at(i)->group_id);
        if (cit == creative.end()) {
          continue;
        } else {
          git->second->creative_ = cit->second;
        }
        BidUnit bid_unit;
        Ice::Long bid_id = bseq.at(i)->bid_id;
        bid_unit.Init(bseq.at(i), gServiceIndex);
        git->second->keys_ = bid_unit.GetKeys();
        git->second->schools_ = bid_unit.schools_;
        git->second->areas_ = bid_unit.areas_;
        git->second->bid_id_ = bid_id;

        m_idx_.Build(git->second->keys_, git->second);
        s_idx_.Add(bid_unit.schools_, git->second);
        a_idx_.Add(bid_unit.areas_, git->second);
        {
          IceUtil::RWRecMutex::WLock lock(groups_mutex_);
          group_set_.insert(pair<Ice::Long, AdGroupPtr> (
              git->second->group_id_, git->second));
        }
        MyUtil::Date date((string) res.at(i)["update_time"]);
        price_time_ = max(price_time_, date.time());
        printIds.insert(bseq.at(i)->group_id);
      }
    }
  }

  MCE_DEBUG("Matcher::Load --> s_idx_ printinfo ");
  //  s_idx_.PrintInfo();

  MCE_DEBUG("Load done --> ids size = " << printIds.size());

  m_idx_.SetReady(true);
  s_idx_.SetReady(true);
  a_idx_.SetReady(true);
}

int Matcher::CountGroups() {
  IceUtil::RWRecMutex::RLock lock(groups_mutex_);
  return group_set_.size();
}
BidParamsSeq Matcher::LoadBidUnits(const Ice::LongSeq& group_id_seq) {
  BidParamsSeq seq;

  if (group_id_seq.empty()) {
    return seq;
  }
  Statement sql;

  sql << "select * from bid_unit where delete_flag = 1   "
      << " and adgroup_id in " << PrintHelper(group_id_seq);

  //  for (size_t i = 0; i < group_id_seq.size(); ++i) {
  //    if (i) {
  //      sql << ",";
  //    }
  //    sql << group_id_seq.at(i);
  //  }
  //  sql << ")";

  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(db_instance, CDbRServer).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("Matcher::LoadBidUnits :" << e << ", line:" <<__LINE__);
  } catch (std::exception& e) {
    MCE_WARN("Matcher::LoadBidUnits :" << e.what() << ", line:" << __LINE__);
  }

  if (!res) {
    MCE_WARN("StoryQueryResult wrong");
    return seq;
  }
  if (res.empty()) {
    MCE_WARN("Matcher::LoadBidUnits --> no ads in bid_unit ");
    return seq;
  }

  for (size_t i = 0; i < res.size(); ++i) {

    BidParamsPtr p = new BidParams();
    p->group_id = (Ice::Long) res.at(i)["adgroup_id"];
    p->bid_id = (Ice::Long) res.at(i)["bid_unit_id"];
    p->stages
        = (mysqlpp::Null<string, mysqlpp::NullIsBlank>) res.at(i)["cast_stage"];
    p->genders
        = (mysqlpp::Null<string, mysqlpp::NullIsBlank>) res.at(i)["cast_gender"];
    p->ages
        = (mysqlpp::Null<string, mysqlpp::NullIsBlank>) res.at(i)["cast_age"];
    p->schools
        = (mysqlpp::Null<string, mysqlpp::NullIsBlank>) res.at(i)["cast_school"];
    p->grades
        = (mysqlpp::Null<string, mysqlpp::NullIsBlank>) (res.at(i)["cast_grade"]);
    p->areas
        = (mysqlpp::Null<string, mysqlpp::NullIsBlank>) res.at(i)["cast_area"];
    seq.push_back(p);

    Date date((string) res.at(i)["update_time"]);
    time_t time = date.time();
    bid_time_ = max(bid_time_, time);
  }
  MCE_DEBUG("Matcher::LoadBidUnits --> bid_unit size = " << seq.size());
  return seq;
}

map<Ice::Long, CreativeSetPtr> Matcher::LoadCreatives(
    const Ice::LongSeq& group_ids) {

  map<Ice::Long, CreativeSetPtr> creative_map;

  if (group_ids.empty()) {
    return creative_map;
  }

  Statement sql;
  mysqlpp::StoreQueryResult res;
  sql << "select * from creative where adgroup_id in "
      << PrintHelper(group_ids);

  try {
    res = QueryRunner(db_instance, CDbRServer).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("Matcher::LoadCreatives :" << e << ", line:" << __LINE__);
  } catch (std::exception& e) {
    MCE_WARN("Matcher::LoadCreatives :" << e.what() << ", line:" << __LINE__);
  }

  MCE_DEBUG("Matcher::LoadCreatives --> res size = " << res.size());
  for (size_t i = 0; i < res.size(); ++i) {
    Ice::Long gid = (Ice::Long) (res.at(i)["adgroup_id"]);

    map<Ice::Long, CreativeSetPtr>::iterator it = creative_map.find(gid);
    CreativeSetPtr creative_set;
    if (it == creative_map.end()) {
      creative_set = new CreativeSet();
      creative_map[gid] = creative_set;
    } else {
      creative_set = it->second;
    }

    Creative c;

    c.id = (Ice::Long) (res.at(i)["creative_id"]);
    MyUtil::Date date((string) res.at(i)["update_time"]);
    c.update_time = date.time();
    //    bid_time_ = max(bid_time_, time);
    crtv_time_ = max(crtv_time_, date.time());

    creative_set->set_[c.id] = c;
  }
  MCE_DEBUG(__FILE__ << ":" << __LINE__);
  return creative_map;
}

//采用新策略的选择，加大前三个广告的值
vector<pair<AdGroupPtr, FormatParam> > Matcher::FilterAdGroupB(
    const AdSetPtr& s1, const AdSetPtr& s2, int ad_count) {
  //  AdGroupSeq out;
  vector<pair<AdGroupPtr, FormatParam> > out;
  AdSetPtr merge_set;
  merge_set = *s1 & *s2;
  map<Ice::Long, set<Ice::Long> > member_map;
  MCE_DEBUG("Matcher::FilterAdGroup --> s1 size = " << s1->Size() << ",s2 size = " << s2->Size() << ",merge_set size = " << merge_set->Size());

  map<Ice::Long, SubGroupPtr>& submap = merge_set->GetSubGroupMap();
  map<Ice::Long, int> quality_map;
  multimap<int, SubGroupPtr> score_map; //分数_group 分数为负数
  map<Ice::Long, NextGroupPtr> next_map;

  int sum = 0;
  int sumA = 0, sumB = 0, pos = 0;
  int min_price = 1000000;
  map<Ice::Long, multimap<int, SubGroupPtr>::iterator> mark_map;
  map<Ice::Long, pair<int, int> > click_pv;
  for (map<Ice::Long, SubGroupPtr>::iterator mit = submap.begin(); mit
      != submap.end(); ++mit, ++pos) {
    Ice::Long gid = mit->first;
    pair<int, int> score_quality = mit->second->Score();
    int click = mit->second->click;
    int pv = mit->second->pv;

    int score = score_quality.first;
    multimap<int, SubGroupPtr>::iterator res = score_map.insert(pair<int,
        SubGroupPtr> (0 - score, mit->second));
    sum += score;

    mark_map[mit->first] = res;
    quality_map[mit->first] = score_quality.second;

    member_map[mit->second->group->member_id_].insert(
        mit->second->group->group_id_);//同一个member的gid
    min_price = min(min_price, mit->second->group->max_price_);
    click_pv[gid] = make_pair(click, pv);
  }

  for (multimap<int, SubGroupPtr>::iterator sit = score_map.begin(); sit
      != score_map.end(); ++sit) {
    multimap<int, SubGroupPtr>::iterator sit2 = sit;
    ++sit2;
    if (sit2 != score_map.end()) {
      NextGroupPtr next = new NextGroup;
      next->gid = sit2->second->group->group_id_;
      next->price = sit2->second->group->max_price_;
      next->quality = quality_map[next->gid];
      next->sub_group = sit2->second;
      next_map[sit->second->group->group_id_] = next;
    } else {
      NextGroupPtr next = new NextGroup;
      next->gid = 0;
      next->price = 50;
      next->quality = 0;
      next->sub_group = 0;
      next_map[sit->second->group->group_id_] = next;
    }
  }

  //  for (map<Ice::Long, set<Ice::Long> >::iterator mit = member_map.begin(); mit
  //      != member_map.end(); ++mit) {
  //    MCE_DEBUG("Matcher::FilterAdGroupB --> member_id = " << mit->first << ", it has groups gids = " << PrintHelper(mit->second));
  //  }

  int i = 0;
  for (multimap<int, SubGroupPtr>::iterator sit = score_map.begin(); sit
      != score_map.end() && i < 3; ++sit, ++i) {
    //    MCE_DEBUG("Matcher::FilterAdGroup -->  " << i << ": score = " << sit->first);
    sumA += abs(sit->first);
  }
  sumB = sum - sumA;
  MCE_DEBUG("Matcher::FilterAdGroup --> sum = " << sum << ",sumA = " << sumA << ",sumB = " << sumB);

  //  int k = 0;
  //  for (multimap<int, SubGroupPtr>::iterator sit = score_map.begin(); sit
  //      != score_map.end() && k < 6; ++sit, ++k) {
  //    MCE_DEBUG("Matcher::FilterAdGroup --> " << k << ": gid = " << sit->second->group->group_id_ << ",score = " << sit->first);
  //  }
  //  MCE_DEBUG("Matcher::FilterAdGroup --> sum score = " << sum << ",sumA = " << sumA << ",sumB = " << sumB);


  for (multimap<int, SubGroupPtr>::iterator mit = score_map.begin(); mit
      != score_map.end(); ++mit) {
    MCE_DEBUG("Matcher::FilterAdGroup --> gid = " << mit->second->group->group_id_ << ",score = " << mit->first << ",sum = " << sum );
  }

  if (score_map.size() > 3 && sumB * 3 > sumA) {
    MCE_DEBUG("Matcher::FilterAdGroup --> 3 * sumB / sumA = " << (double)3 * sumB / sumA);

    int i = 0;
    for (multimap<int, SubGroupPtr>::iterator mit = score_map.begin(); mit
        != score_map.end() && i < 3; ++i) {

      SubGroupPtr subg = mit->second;
      int score = mit->first * 3 * sumB / sumA;
      mark_map[mit->second->group->group_id_] = score_map.insert(make_pair(
          score, subg));
      score_map.erase(mit++);
    }
    sum = 4 * sumB;

    //    int k = 0;
    //    for (multimap<int, SubGroupPtr>::iterator sit = score_map.begin(); sit
    //        != score_map.end() && k < 6; ++sit, ++k) {
    //      MCE_DEBUG("Matcher::FilterAdGroup --> " << k << ": gid = " << sit->second->group->group_id_ << ",score = " << sit->first);
    //    }
    //    MCE_DEBUG("Matcher::FilterAdGroup --> sum score = " << sum);

  }

  //测试，打印pv概率
  for (multimap<int, SubGroupPtr>::iterator mit = score_map.begin(); mit
      != score_map.end(); ++mit) {
    MCE_DEBUG("Matcher::FilterAdGroup --> gid = " << mit->second->group->group_id_ << ",score = " << mit->first << ",sum = " << sum );
  }

  //  MCE_DEBUG("Matcher::FilterAdGroup --> score_map size = " << score_map.size() << ",mark_map size = " << mark_map.size());

  srand(time(NULL));

  for (int i = 0; i < ad_count && !score_map.empty(); ++i) {
    int rd = rand() % sum;

    int left = 0, right;
    for (multimap<int, SubGroupPtr>::iterator sit = score_map.begin(); sit
        != score_map.end(); ++sit) {
      right = left + abs(sit->first);
      if (left <= rd && rd < right) {
        SubGroupPtr g = sit->second;
        sum -= abs(sit->first);
        mark_map.erase(sit->second->group->group_id_);
        score_map.erase(sit++);

        int price;

        Ice::Long gid = g->group->group_id_;
        int this_quality = quality_map[g->group->group_id_];
        int this_price = g->group->max_price_;
        int next_quality, next_price;
        NextGroupPtr next = next_map[gid];
        if (next->gid != 0) {//该广告不是最后一个
          next_quality = next->quality;
          next_price = next->price;
          int factor = (((double) next_quality / this_quality) * sqrt(
              this_price * next_price) + 1); //中间值
          int factor_test = ((next_quality / this_quality) * sqrt(this_price
              * next_price) + 1);
          int other = (this_price + next_price) / 3 + 1;

          price = max(min_price, min(this_price, max(factor, other)));
          MCE_DEBUG("Matcher::FilterAdGroupB --> gid = " << g->group->group_id_ <<
              ",this_price = " << this_price << ",this quality = " << this_quality <<
              ",next gid = " << next->gid <<
              ",next_price = " << next_price <<",next quality = " << next_quality <<
              ",factor = " << factor <<
              ",factor_test = " << factor_test <<
              ",other price = "<< other<< ",result price = " << price );
        } else {
          price = max(min_price, min(this_price, (int) (sqrt(this_price * 50))
              + 1));
          MCE_DEBUG("Matcher::FilterAdGroupB --> gid = " << g->group->group_id_ <<
              ",this_price = " << this_price <<
              ",result price = " << price << ",it last ad");
        }
        int this_click, this_pv;
        this_click = click_pv[gid].first;
        this_pv = click_pv[gid].second;
        if (this_click == 1 && this_pv < 10100) {
          price = this_price * 0.7;
          MCE_DEBUG("Matcher::FilterAdGroupB --> this gid = " << gid << ",70 percent");
        } else if (next->gid != 0 && click_pv[next->gid].first == 1
            && click_pv[next->gid].second < 10100) {
          price = this_price * 0.7;
          MCE_DEBUG("Matcher::FilterAdGroupB --> next gid = " << next->gid << ",70 percent");
        }
        if (price < 50) {
          price = 50;
        }
        ++(g->pv);
        FormatParam param;
        param.cost = price;
        param.max_price = this_price;
        param.click = click_pv[gid].first;
        param.pv = click_pv[gid].second;

        out.push_back(make_pair(g->group, param));

        //删除相同member的group
        map<Ice::Long, set<Ice::Long> >::iterator mit = member_map.find(
            g->group->member_id_);

        if (mit != member_map.end()) {
          //          MCE_DEBUG("Matcher::FilterAdGroup -->gid = " << g->group->group_id_ << " ,member id = " << mit->first << ",it has gids size = " << mit->second.size()<< ",gids = " << PrintHelper(mit->second));
          mit->second.erase(g->group->group_id_);//删除本身
          //          MCE_DEBUG("Matcher::FilterAdGroup -->gid = " << g->group->group_id_ << " ,member id = " << mit->first << ",it has gids size = " << mit->second.size()<< ",gids = " << PrintHelper(mit->second));
          mit->second.begin();
          mit->second.end();
          for (set<Ice::Long>::iterator sit2 = mit->second.begin(); sit2
              != mit->second.end(); ++sit2) {
            map<Ice::Long, multimap<int, SubGroupPtr>::iterator>::iterator it =
                mark_map.find(*sit2);
            if (it != mark_map.end()) {
              sum -= abs(it->second->first);
              score_map.erase(it->second);
              mark_map.erase(it);
            }
          }
        }

        break;
      } else {
        left = right;
      }
    }
  }
  MCE_DEBUG( "Matcher::FilterAdGroup --> adgroupseq size = " << out.size() );
  return out;
}

vector<pair<AdGroupPtr, int> > Matcher::FilterAdGroup(const AdSetPtr& s1,
    const AdSetPtr& s2, int ad_count) {
  //  AdGroupSeq out;
  vector<pair<AdGroupPtr, int> > out;
  AdSetPtr merge_set;
  merge_set = *s1 & *s2;
  //  MCE_DEBUG("Matcher::FilterAdGroup --> s1 size = " << s1->Size() << ",s2 size = " << s2->Size() << ",merge_set size = " << merge_set->Size());

  map<Ice::Long, SubGroupPtr>& submap = merge_set->GetSubGroupMap();

  multimap<int, SubGroupPtr> score_map;
  int sum = 0;
  map<Ice::Long, multimap<int, SubGroupPtr>::iterator> mark_map;
  for (map<Ice::Long, SubGroupPtr>::iterator mit = submap.begin(); mit
      != submap.end(); ++mit) {

    pair<int, int> score_quality = mit->second->Score();
    int score = score_quality.first;
    multimap<int, SubGroupPtr>::iterator res = score_map.insert(pair<int,
        SubGroupPtr> (0 - score, mit->second));
    //    MCE_DEBUG("Matcher::FilterAdGroup --> gid = " << mit->first << ",score = " << score);
    sum += score;
    mark_map[mit->first] = res;
  }
  //  MCE_DEBUG("Matcher::FilterAdGroup --> sum score = " << sum);

  srand(time(NULL));

  for (int i = 0; i < ad_count && !score_map.empty(); ++i) {

    int rd = rand() % sum;
    int left = 0, right;
    for (multimap<int, SubGroupPtr>::iterator sit = score_map.begin(); sit
        != score_map.end(); ++sit) {

      right = left + abs(sit->first);
      if (left <= rd && rd < right) {
        SubGroupPtr g = sit->second;

        sum -= abs(sit->first);
        mark_map.erase(sit->second->group->group_id_);
        score_map.erase(sit++);

        //

        int price = 50;
        int next_price = sit->second->group->max_price_;
        if (sit != score_map.end()) {
          price = min(g->group->max_price_, (next_price + g->group->max_price_)
              / 2); //TODO 增加质量项
        } else {
          price = (50 + g->group->max_price_) / 2;
        }

        ++(g->pv);//TODO 是否移到format中执行?
        out.push_back(make_pair(g->group, price));
        //        map<Ice::Long, set<Ice::Long> >::iterator cit = compete_ref_.find(
        //            g->group->group_id_);
        //        if (cit != compete_ref_.end()) {
        //          set<Ice::Long> & competers = cit->second;//竞争者
        //          for (set<Ice::Long>::iterator sit = competers.begin(); sit
        //              != competers.end(); ++sit) {
        //
        //            map<Ice::Long, multimap<int, SubGroupPtr>::iterator>::iterator it =
        //                mark_map.find(*sit);
        //            if (it != mark_map.end()) {
        //              sum -= abs(it->second->first);
        //              score_map.erase(it->second);
        //              mark_map.erase(it);
        //            }
        //          }
        //        }
        break;
      } else {
        left = right;
      }
    }
  }
  MCE_DEBUG( "Matcher::FilterAdGroup --> adgroupseq size = " << out.size() );

  return out;
}

//text格式：  index_key_creative_bidid_cost_pos_maxprice_pv_click_pvtime_ubc

AdInfoSeq Matcher::Format(vector<pair<AdGroupPtr, FormatParam> > & group_info,
    int m_key, int ad_count, UserProfile & profile, string & referer) {
  ProcessUrl(referer);
  time_t pv_time;
  pv_time = time(0);

  int uid = profile.id();
  int ip = profile.ip();
  int age = profile.age();
  int gender = profile.gender();
  int birth_year = 2010 - age;//TODO
  int stage = profile.stage();
  string current_area = profile.current_area();
  if (current_area.empty()) {
    current_area = "0";
  }
  string ip_area = profile.ip_area();
  if (ip_area.empty()) {
    ip_area = "0";
  }
  Ice::Long school = profile.school();
  int grade = profile.grade();
  AdPvParameterSeq pv_seq;

  AdInfoSeq ads;
  set<Ice::Long> tmp;
  MCE_DEBUG("Matcher::Format --> uid = " << uid << ",referer = " << referer << ",ip = " << ip);
  int pos = 1;
  for (vector<pair<AdGroupPtr, FormatParam> >::iterator git =
      group_info.begin(); git != group_info.end(); ++git, ++pos) {
    map<Ice::Long, Creative> creatives;
    Ice::Long bid_id, adzone_id;
    {
      IceUtil::RWRecMutex::RLock lock(groups_mutex_);
      bid_id = git->first->bid_id_;
      creatives = git->first->creative_->set_;
      adzone_id = git->first->adzone_id_;

    }
    srand(time(NULL));
    if (creatives.empty()) {
      MCE_WARN("Matcher::Format --> gid  = " << git->first->group_id_ << ", has no creative");
      AdInfo ad;
      ad.creative = -1;//没有创意id,用－1来代替
      ads.push_back(ad);
      continue;
    }
    int rd = rand() % creatives.size();
    map<Ice::Long, Creative>::iterator mit = creatives.begin();
    int i = 0;
    while (i++ != rd) {
      mit++;
    }
    AdInfo ad;
    ad.creative = mit->first;
    tmp.insert(ad.creative);
    time_t t = mit->second.update_time;
    ad.updateTime = t;
    ostringstream text_ios;

    text_ios << gServiceIndex << "_" << m_key << "_" << mit->first << "_"
        << bid_id << "_" << git->second.cost << "_" << pos << "_"
        << git->second.max_price << "_" << git->second.click << "_"
        << git->second.pv << "_" << pv_time;

    AdPvParameterPtr pv_param = new AdPvParameter;
    pv_param->groupid = git->first->group_id_;
    pv_param->referer = referer;//TODO
    pv_param->creativeid = ad.creative;

    int a, b, c, d; //将整数ip转化成string
    unsigned ip_u = (unsigned) ip;

    a = ip_u / 16777216;
    b = (ip_u - a * 16777216) / 65536;
    c = (ip_u - a * 16777216 - b * 65536) / 256;
    d = (ip_u - a * 16777216 - b * 65536 - c * 256);
    ostringstream ios2;
    ios2 << a << "." << b << "." << c << "." << d;
    pv_param->ip = ios2.str();

    string valid_area;
    if (stage == 2) {
      valid_area = current_area;
    } else {
      valid_area = ip_area;
    }

    //拼http
    ostringstream http_ios;
    ostringstream ubc_ios;
    string ubc;
    ubc_ios << "1000000_" << uid << "|" << gender << "|" << birth_year
        << "-01-01" << "|" << age << "|" << stage << "|" << valid_area << "|"
        << school << "_" << current_area << "|" << grade << "|0|0|" << ip_area;

    ubc = ubc_ios.str();

    http_ios << "/show?userid=" << uid << "&tt=0&type=.js&adzoneid="
        << adzone_id << "&count=" << ad_count << "&rowoffset=0&ip="
        << pv_param->ip << "&jbc=cookie|a|b&ubc=" << ubc;

    pv_param->http = http_ios.str();
    pv_param->bitunitid = bid_id;
    pv_seq.push_back(pv_param);

    text_ios << "_" << ubc;

    ad.text = text_ios.str();
    ads.push_back(ad);

    MCE_INFO("Matcher::Format --> text = " << ad.text);
    MCE_INFO("Matcher::Format --> http = " << pv_param->http);
    MCE_INFO("Matcher::Format --> uid = " << uid << ",gid = " << pv_param->groupid << ",creative_id = " << pv_param->creativeid <<
        ",bidunit_id = " << bid_id << ",ip = " << pv_param->ip <<
        ",cost = " << git->second.cost << ",max_price = " << git->second.max_price << ",click = " << git->second.click <<
        ",pv = " << git->second.pv << ",pos = " << pos);
  }
  AdAnalysisAdapter::instance().Pv(pv_seq);
  return ads;
}

void Matcher::ReportDefault(int uid, const string & ip, int ad_count,
    const DefaultSeq & ads) {
  MCE_DEBUG("Matcher::ReportDefault --> uid " << uid << ",ip = " << ip << ",ad_count = " << ad_count << ",ads size = " << ads.size());
  int gender = 0;
  int stage = 4;
  Ice::Long current_area = 0;
  Ice::Long school = 0;
  Ice::Long ip_area = 0;

  int age = 25;
  int grade = 41;
  int birth_year = 1970;

  ostringstream http_ios;
  ostringstream ubc_ios;
  string ubc;
  ubc_ios << "1000000_" << uid << "|" << gender << "|" << birth_year
      << "-01-01" << "|" << age << "|" << stage << "|" << current_area << "|"
      << school << "_" << current_area << "|" << grade << "|0|0|" << ip_area;

  ubc = ubc_ios.str();
  AdPvParameterSeq pv_seq;
  for (DefaultSeq::const_iterator dit = ads.begin(); dit != ads.end(); ++dit) {
    DefaultAd ad = *dit;

    AdPvParameterPtr pv_param = new AdPvParameter;
    pv_param->groupid = ad.groupId;

    string referer = ad.refer;
    ProcessUrl(referer);
    pv_param->referer = referer;//TODO

    pv_param->creativeid = ad.crtvId;
    pv_param->bitunitid = ad.bidId;
    pv_param->ip = ip;

    http_ios << "/show?userid=" << uid << "&tt=0&type=.js&adzoneid="
        << ad.adzoneId << "&count=" << ad_count << "&rowoffset=0&ip="
        << pv_param->ip << "&jbc=cookie|a|b&ubc=" << ubc;
    pv_param->http = http_ios.str();
    http_ios.clear();
    pv_seq.push_back(pv_param);
    MCE_DEBUG("Matcher::ReportDefault --> gid = " << pv_param->groupid << ",http = " << pv_param->http << ",refer = " << pv_param->referer);
    MCE_INFO("Matcher::ReportDefault --> uid = " << uid << ",gid = " << pv_param->groupid << ",creative_id = " << pv_param->creativeid <<
        ",bidunit_id = " << ad.bidId << ",ip = " << pv_param->ip <<
        ",cost = " << 0 << ",max_price = " << 0 << ",click = " << 0 <<
        ",pv = " << 0 << ",is default ad");
  }
  AdAnalysisAdapter::instance().Pv(pv_seq);

}

//text格式：  index_key_creative_bidid_cost_maxprice_pv_click_pvtime_ubc  ubc中可能有空格
//默认广告只有到cost的信息
void Matcher::Click(int uid, const string & ip, const string& text,
    string & referer, string & click_url) {
  ProcessUrl(referer);
  ProcessUrl(click_url);
  MCE_DEBUG("Matcher::Click --> uid = " << uid << ",ip = " << ip << ",text = " << text << ",referer = " << referer << ",click_url");
  //根据广告系统，一个group只能投放一个年龄段
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
  boost::char_separator<char> sep("_");
  tokenizer tok(text, sep);
  //  tokenizer<>::iterator tit = tok.begin();
  int size = 0;
  for (tokenizer::iterator tit = tok.begin(); tit != tok.end(); ++tit) {
    ++size;
  }

  tokenizer::iterator tit = tok.begin();
  int key, cost;
  unsigned index;
  Ice::Long creative, gid, bid_id;
  index = lexical_cast<int> (*tit++);
  if (index != gServiceIndex) {
    MCE_WARN("Matcher::Click --> wrong index in text, index = " << index << ", gServiceIndex = " << gServiceIndex);
    return;
  }
  key = lexical_cast<int> (*tit++);
  creative = lexical_cast<Ice::Long> (*tit++);
  bid_id = lexical_cast<Ice::Long> (*tit++);
  cost = lexical_cast<int> (*tit++);

  gid = creative / 100000;

  AdClickParameterPtr p = new AdClickParameter();
  p->groupid = gid;
  p->creativeid = creative;
  p->userid = uid;

  p->ip = ip;
  p->referer = "\"" + referer + "\"";
  p->clickurl = "\"" + click_url + "\"";

  if (key != 0) { // 非默认广告
    int pos = lexical_cast<int> (*tit++);
    int max_price = lexical_cast<int> (*tit++);
    int click = lexical_cast<int> (*tit++);
    int pv = lexical_cast<int> (*tit++);
    string pv_time = (*tit++);
    p->pvtime = pv_time;
    string ubc;
    ostringstream ubc_ios;
    bool first = true;
    for (; tit != tok.end(); ++tit) {
      string tmp = lexical_cast<string> (*tit);
      if (first == true) {
        first = false;
      } else {
        ubc_ios << "_";
      }
      ubc_ios << tmp;
    }
    ubc = ubc_ios.str();
    p->ubc = ubc;
    MCE_DEBUG("Matcher::Click --> ubc = " << ubc);
    m_idx_.Click(key, gid);
    p->clickprice = cost;
    p->bitunitid = bid_id;
    p->clickpos = pos;
    MCE_INFO("Matcher::Click --> uid = "<<uid << ",gid = "<< gid << ",creative = " << creative << ",bidunit_id = " << bid_id
        << ",key = " << key << ",ip = " << ip << ",cost = " << cost
        << ",max_price = " << max_price << ",click = " << click << ",pv = " << pv << ",pos = " << pos);

    AdAnalysisAdapter::instance().Click(p);
  } else {//默认广告
    int pos = 1;
    p->pvtime = "0";
    p->clickpos = pos;
    MCE_INFO("Matcher::Click --> uid = "<<uid << ",gid = "<< gid << ",creative = " << creative << ",bidunit_id = " << bid_id
        << ",key = " << key << ",ip = " << ip << ",cost = " << 0
        << ",max_price = " << 0 << ",click = " << 0 << ",pv = " << 0 << ",pos = " << pos);
    ostringstream tmpios;
    tmpios << "1000000_" << uid << "|0|1970-01-01|0|0|0|0_0|0|0|0|0";
    //    string ubc_default = string("1000000_") + uid + "|0|1970-01-01|0|0|0|0_0|0|0|0|0";
    p->ubc = tmpios.str();//默认的ubc
    p->clickprice = 0;

    p->bitunitid = 0;

    AdAnalysisAdapter::instance().Click(p);

    return;
  }

  //  if (key == 0) {
  //    p->pvtime = "";
  //  } else {
  //    string pv_time = (*tit++);
  //    p->pvtime = pv_time;
  //    string ubc;
  //    ostringstream ubc_ios;
  //    bool first = true;
  //    for (; tit != tok.end(); ++tit) {
  //      string tmp = lexical_cast<string> (*tit);
  //      if (first == true) {
  //        first = false;
  //      } else {
  //        ubc_ios << "_";
  //      }
  //      ubc_ios << tmp;
  //    }
  //    ubc = ubc_ios.str();
  //    p->ubc = ubc;
  //    MCE_DEBUG("Matcher::Click --> ubc = " << ubc);
  //  }
  //
  //  if (key == 0) {//默认广告的
  //    MCE_INFO("Matcher::Click --> uid = "<<uid << ",creative = " << creative << ",key = " << key << ",ip = " << ip << ",cost = " << -1 << ",it's a default ad");
  //    ostringstream tmpios;
  //    MCE_DEBUG(__FILE__ << ":" << __LINE__);
  //    tmpios << "1000000_" << uid << "|0|1970-01-01|0|0|0|0_0|0|0|0|0";
  //    //    string ubc_default = string("1000000_") + uid + "|0|1970-01-01|0|0|0|0_0|0|0|0|0";
  //    MCE_DEBUG(__FILE__ << ":" << __LINE__);
  //    p->ubc = tmpios.str();//默认的ubc
  //    MCE_DEBUG(__FILE__ << ":" << __LINE__);
  //    p->clickprice = -1;
  //    MCE_DEBUG(__FILE__ << ":" << __LINE__);
  //    p->bitunitid = -1;
  //
  //    MCE_DEBUG(__FILE__ << ":" << __LINE__);
  //    AdAnalysisAdapter::instance().Click(p);
  //    MCE_DEBUG(__FILE__ << ":" << __LINE__);
  //
  //    return;
  //  }
  //
  //  m_idx_.Click(key, gid);
  //  p->clickprice = cost;
  //  p->bitunitid = bid_id;
  //  MCE_INFO("Matcher::Click --> uid = "<<uid << ",creative = " << creative << ",key = " << key << ",ip = " << ip << ",cost = " << cost);
  //  //cost有可能为0，但还是要调AdAnalysisAdapter
  //  AdAnalysisAdapter::instance().Click(p);

}

bool Matcher::SetClickInfo(Ice::Long gid, int key, int click, int pv) {
  MCE_WARN("Matcher::SetClickInfo --> gid = " << gid <<", key = " << key << ",click = " << click << ",pv =" << pv);

  AdSetPtr adSet = m_idx_.Get(key);
  if (!adSet) {
    MCE_WARN("Matcher::SetClickInfo --> m_idx_.Get(key), can not get this AdSet, key = " << key << ",gid = " << gid);
  }
  adSet->SetClickInfo(gid, click, pv);
  return true;
}

void Matcher::SetServiceIndex() {
  ServiceI &service = ServiceI::instance();
  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
  gServiceIndex = atoi(props->getProperty("ServerIndex").c_str());
  tester_ = atol(props->getProperty("ServerTester").c_str());
  //  MCE_INFO("Matcher::SetServiceIndex --> Tester string = " << props->getProperty("Tester"));
  MCE_INFO("Matcher::SetServiceIndex --> ServerTester = " << props->getProperty("ServerTester"));
  MCE_INFO("Matcher::SetServiceIndex --> gServiceIndex = " << gServiceIndex );
}

void Matcher::GetAvailableAds(const MyUtil::LongSeq & ids) {

  if (!allow_scanning_) {
    MCE_INFO("Matcher::GetAvailableAds --> not allow scanning ");
    return;
  }
  allow_scanning_ = false;
  //  MCE_INFO("Matcher::GetAvailableAds --> AvailableAds size = " << ids.size() << ", ids = " << PrintHelper(ids));
  //  return;
  set<Ice::Long> availables(ids.begin(), ids.end());
  set<Ice::Long> invalids;
  //  map<Ice::Long, AdGroupPtr> clone;

  {
    IceUtil::RWRecMutex::RLock lock(groups_mutex_);

    for (map<Ice::Long, AdGroupPtr>::iterator mit = group_set_.begin(); mit
        != group_set_.end(); ++mit) {
      set<Ice::Long>::iterator sit = availables.find(mit->first);
      if (sit == availables.end()) {
        invalids.insert(mit->first);
        //      availables.erase(sit);
      } else {
        availables.erase(sit);
      }
    }
  }
  MCE_INFO("Matcher::GetAvailableAds --> new active ids size = " << availables.size());
  MCE_INFO("Matcher::GetAvailableAds --> inactive ids size = " << invalids.size());
  //invalids为不能用的id
  for (set<Ice::Long>::iterator sit = invalids.begin(); sit != invalids.end(); ++sit) {
    DelGroup(*sit);
  }

  //availables剩下新增的gid

  if (!availables.empty()) {
    Ice::LongSeq tmp(availables.begin(), availables.end());
    Load(false, tmp);
  }
  MCE_DEBUG("Matcher::GetAvailableAds --> total ad group count = " << CountGroups());
  allow_scanning_ = true;
}

void Matcher::ScanChangedPrices() {
  // price
  Statement sql;
  mysqlpp::StoreQueryResult res;

  //查询是否改变最高出价
  sql
      << "select * from ad_group  where highest_bid >= 50 and update_time > FROM_UNIXTIME("
      << price_time_ << ") and member_id != 999";
  try {
    res = QueryRunner(db_instance, CDbRServer).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("Matcher::ScanChangedPrices --> price :" << e << ", line:" << __LINE__);
  } catch (std::exception& e) {
    MCE_WARN("Matcher::ScanChangedPrices --> price:" << e.what() << ", line:" << __LINE__);
  }
  if (!res) {
    MCE_WARN("StoryQueryResult wrong --> price");
  }
  if (res.empty()) {
    MCE_INFO("Matcher::ScanChangedPrices --> price :empty ");
    return;
  }
  MCE_DEBUG("Matcher::ScanChangedPrices --> changed price size = " << res.size());
  Ice::LongSeq ids;
  for (unsigned i = 0; i < res.size(); ++i) {
    Ice::Long gid = (Ice::Long) (res.at(i)["group_id"]);

    //    int is_active = (int) (res.at(i)["is_active"]);
    //    if(is_active == 0){
    //      IceUtil::RWRecMutex::RLock lock(groups_mutex_);
    //      map<Ice::Long, AdGroupPtr>::iterator mit = group_set_.find(gid);
    //      if (mit == group_set_.end()) {
    //        MCE_WARN("Matcher::ScanChangedPrices --> this group does not exist, group id = " << gid);
    //        continue;
    //      }
    //    }

    //    MCE_DEBUG(__FILE__ << ":" << __LINE__ << ", gid = " << gid);
    int price = (int) (res.at(i)["highest_bid"]);
    MyUtil::Date date((string) res.at(i)["update_time"]);
    time_t time = date.time();
    price_time_ = max(price_time_, time);//更新时间
    AdGroupPtr group;
    {
      IceUtil::RWRecMutex::RLock lock(groups_mutex_);
      map<Ice::Long, AdGroupPtr>::iterator mit = group_set_.find(gid);
      if (mit == group_set_.end()) {
        //        MCE_WARN("Matcher::ScanChangedPrices --> this group does not exist, group id = " << gid);
        continue;
      } else {
        group = mit->second;
      }
    }
    group->max_price_ = price;
    ids.push_back(gid);
  }
  MCE_DEBUG("Matcher::ScanChangedPrices --> changed price ids size " << ids.size());
}

void Matcher::ScanChangedBidUnits() {
  //bid_unit;

  Statement sql;
  mysqlpp::StoreQueryResult res;

  sql << "select * from bid_unit where update_time > FROM_UNIXTIME("
      << bid_time_ << ") and member_id != 999";

  try {
    res = QueryRunner(db_instance, CDbRServer).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("Matcher::ScanChangedBidUnits -->bid:" << e << ", line:" << __LINE__);
  } catch (std::exception& e) {
    MCE_WARN("Matcher::ScanChangedBidUnits -->bid:" << e.what() << ", line:" << __LINE__);
  }
  if (!res) {
    MCE_WARN("StoryQueryResult wrong -->bid");
    return;
  }
  if (res.empty()) {
    MCE_INFO("Matcher::ScanChangedBidUnits --> empty ");
    return;
  }
  MCE_DEBUG("Matcher::ScanChangedBidUnits --> res size = " << res.size());
  Ice::LongSeq ids;
  for (unsigned i = 0; i < res.size(); ++i) {
    Ice::Long gid = (Ice::Long) (res.at(0)["adgroup_id"]);
    MyUtil::Date date((string) res.at(i)["update_time"]);
    //    MCE_DEBUG("Matcher::ScanChangedBidUnits --> gid = " << gid << ",time = " << date.time());
    DelGroup(gid);
    ids.push_back(gid);
  }
  if (!ids.empty()) {
    Load(false, ids);
  }
  MCE_DEBUG("Matcher::ScanChangedPrices --> changed price ids size = " << ids.size());
}

void Matcher::ScanChangedCreatives() {
  Statement sql;
  mysqlpp::StoreQueryResult res;
  sql << "select * from creative where update_time > FROM_UNIXTIME("
      << crtv_time_ << ")";
  try {
    res = QueryRunner(db_instance, CDbRServer).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("Matcher::ScanChangedCreatives --> err:" << e << ", line:" << __LINE__);
  } catch (std::exception& e) {
    MCE_WARN("Matcher::ScanChangedCreatives --> err:" << e.what() << ", line:" << __LINE__);
  }
  if (!res) {
    MCE_WARN("StoryQueryResult wrong --> creative");
    return;
  }
  if (res.empty()) {
    MCE_INFO("Matcher::ScanChangedCreatives --> res empty ");
    return;
  }
  MCE_DEBUG("Matcher::ScanChangedCreatives --> res size = " << res.size());
  Ice::LongSeq ids;
  //  multimap<Ice::Long, Creative> group_creatives;
  for (size_t i = 0; i < res.size(); ++i) {
    Ice::Long gid = (Ice::Long) (res.at(0)["adgroup_id"]);
    Ice::Long cid = (Ice::Long) (res.at(0)["creative_id"]);
    MyUtil::Date date((string) res.at(i)["update_time"]);
    time_t time = date.time();
    crtv_time_ = max(crtv_time_, time);
    Creative c;
    c.update_time = time;
    c.id = cid;
    map<Ice::Long, AdGroupPtr>::iterator mit;
    {
      IceUtil::RWRecMutex::RLock lock(groups_mutex_);
      mit = group_set_.find(gid);
      if (mit == group_set_.end()) {
        continue;
      } else {
        AdGroupPtr group;
        group = mit->second;
        group->creative_->set_[cid] = c;
        ids.push_back(gid);
      }
    }
  }
  //  MCE_DEBUG("Matcher::ScanChangedCreatives --> changed creative ids = " << PrintHelper(ids));
}

bool Matcher::ready() {
  return ready_;
}
//-----------------------------------------------------------------------------------------------

void InitTask::handle() {
  Matcher::instance().Init();
}

//-----------------------------------------------------------------------------------------------
void ClickTask::handle() {
  try {
    Matcher::instance().Click(uid_, ip_, text_, referer_, click_url_);
  } catch (Ice::Exception& e) {
    MCE_WARN("ClickTask::handle :" << e);
  } catch (std::exception& e) {
    MCE_WARN("ClickTask::handle :" << e.what());
  }

}
//-----------------------------------------------------------------------------------------------


void GetAvailableAdsTask::handle() {
  Matcher::instance().GetAvailableAds(ids_);

}

void LoadOneGroupTask::handle() {
  Ice::LongSeq ids;
  ids.push_back(gid_);
  Matcher::instance().Load(false, ids);
}

void DelGroupTask::handle() {
  Matcher::instance().DelGroup(gid_);
}

void ReloadOneGroupTask::handle() {
  Matcher::instance().ReloadOneGroup(gid_);

}

void ScanTimer::handle() {
  MCE_DEBUG("ScanTimer::handle");
  try {
    Matcher::instance().ScanChangedBidUnits();
    Matcher::instance().ScanChangedPrices();
    Matcher::instance().ScanChangedCreatives();
  } catch (Ice::Exception& e) {
    MCE_WARN("ScanTimer::handle --> error: " << e << ", line:" << __LINE__);
  } catch (std::exception& e) {
    MCE_WARN("ScanTimer::handle --> error: " << e.what() << ", line:" << __LINE__);
  }
  //Matcher::instance().ScanChanged();
}

