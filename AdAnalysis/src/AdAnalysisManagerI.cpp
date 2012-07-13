#include "AdAnalysisManagerI.h"
#include "ServiceI.h"
#include  <boost/lexical_cast.hpp>
#include "AdMatchAdapter.h"
#include "AdGateAdapter.h"
#include "AdUnion/gate/ad_logger.h"

using namespace std;
using namespace IceUtil;
using namespace xce::ad;
using namespace MyUtil;
using namespace com::xiaonei::xce;

void MyUtil::initialize(){
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&AdAnalysisManagerI::instance(), service.createIdentity("M", ""));
  InitAdLogger("ad", "../log/" + ServiceI::instance().getName() + "/ad_log", "INFO");
  InitAdPVLogger("ad_pv", "../log/" + ServiceI::instance().getName() + "/pv_val", "INFO");
  InitAdClickLogger("ad_click", "../log/" + ServiceI::instance().getName() + "/click_val", "INFO");
  TaskManager::instance().scheduleRepeated(new PushAvailableGroupsTimer(60 * 1000));
  TaskManager::instance().scheduleRepeated(new AdLogTimer());
  TaskManager::instance().scheduleRepeated(new LoadTimer());
}
string FormatedClickTime(time_t t)
{
  struct tm lt;
  localtime_r(&t, &lt);

  const int MAX_LEN = 24;
  char res[MAX_LEN];
  res[MAX_LEN - 1] = 0;
  snprintf(res, MAX_LEN - 1, "%d-%02d-%02d %02d:%02d:%02d", lt.tm_year + 1900,
      lt.tm_mon + 1, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec);

  return res;
}

bool Member::Available(){
  bool result = statues;
  MCE_INFO("Account::Available --> RESULT = " << result << " : member_id = " << member_id << " statues = " << statues);
  return result;
}

bool Plan::Available(){
  Date date = Date(time(NULL));
  time_t timestamp = date.time();
  int action_day = date.day();
  int action_hour = date.hour();
  bool running_area_day_check = (running_area_day == -1 || (1 << (action_day - 1) & running_area_day));
  bool running_area_hour_check = (running_area_hour == -1 || (1 << (action_hour - 1) & running_area_hour));
  bool time_check = timestamp >= start_time && (!end_time || timestamp <= end_time);
  bool result = is_active && time_check && running_area_day_check && running_area_hour_check;
  MCE_INFO("Plan::Available --> current_timestamp = " << timestamp << " plan_start_timestamp = " << start_time << " plan_end_timestamp = " << end_time <<" action_day = " << action_day << " action_hour = " << action_hour);
  MCE_INFO("Plan::Available --> RESULT = " << result << " : plan_id " << plan_id << " is_active = " << is_active << " time_check = " << time_check << " running_area_day check = " << running_area_day_check << " running_area_hour_check = " <<running_area_hour_check );
  return result;
}

bool Group::Available(){
  bool result = is_active && highest_bid >= 50;
  MCE_INFO("Group::Available --> RESULT = " << result << " group_id = " << group_id << " is_active = " << is_active << " highest_bid = " << highest_bid );
  return result;
}

AdAnalysisManagerI::AdAnalysisManagerI(){
  init_ok_ = false;
  Load();
}

void AdAnalysisManagerI::PushAvailableGroups(){
  MyUtil::LongSeq result;
  IceUtil::RWRecMutex::RLock lock(cache_mutex_);
  for(GroupMap::const_iterator it = group_cache_.begin(); it != group_cache_.end(); ++it){
    if(Check(it->first)){
      result.push_back(it->first);
    }
  }
  MCE_INFO("AdAnalysisManagerI::PushAvailableGroups --> result size = " << result.size());
  try{
    AdMatchAdapter::instance().GetAvailableAds(result);
  }
  catch(Ice::Exception& e){
    MCE_WARN("AdAnalysisManagerI::PushAvailableGroups --> line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("AdAnalysisManagerI::PushAvailableGroups --> line:"<<__LINE__<<" err:"<<e.what());
  }
}

GroupPtr AdAnalysisManagerI::GetGroup(Ice::Long group_id){
  IceUtil::RWRecMutex::RLock lock(cache_mutex_);
  GroupPtr result;
  GroupMap::const_iterator it = group_cache_.find(group_id);
  if(it != group_cache_.end()){
    result = it->second;
    MCE_INFO("AdAnalysisManagerI::GetGroup --> group_id = " << group_id);
  }
  else {
    MCE_WARN("AdAnalysisManagerI::GetGroup --> MISS GROUP group_id = " << group_id);
  }
  return result;
}


MemberPtr AdAnalysisManagerI::GetMember(Ice::Long member_id){
  IceUtil::RWRecMutex::RLock lock(cache_mutex_);
  MemberPtr result;
  MemberMap::const_iterator it = member_cache_.find(member_id);
  if(it != member_cache_.end()){
    result = it->second;
    MCE_INFO("AdAnalysisManagerI::GetMember --> member_id = " << member_id);
  }
  else{
    MCE_WARN("AdAnalysisManagerI::GetMember --> MISS ACCOUNT member_id = " << member_id);
  }
  return result;
}



PlanPtr AdAnalysisManagerI::GetPlan(Ice::Long plan_id){
  IceUtil::RWRecMutex::RLock lock(cache_mutex_);
  PlanPtr result;
  PlanMap::const_iterator it = plan_cache_.find(plan_id);
  if(it != plan_cache_.end()){
    result = it->second;
    MCE_INFO("AdAnalysisManagerI::GetPlan --> plan_id = " << plan_id);
  }
  else{
    MCE_WARN("AdAnalysisManagerI::GetPlan --> MISS PLANID plan_id " << plan_id );
  }
  return result;
}

bool AdAnalysisManagerI::Check(Ice::Long group_id){
  bool result = false;
  GroupPtr group_ptr = GetGroup(group_id);
  if(group_ptr){
    Ice::Long member_id = group_ptr->member_id;
    Ice::Long plan_id = group_ptr->plan_id;
    PlanPtr plan_ptr = GetPlan(plan_id);
    MemberPtr member_ptr = GetMember(member_id);
    if(plan_ptr && member_ptr && member_ptr->Available() && plan_ptr->Available() && group_ptr->Available()){
        result = true;
    }
  }
  MCE_INFO("AdAnalysisManagerI::Check --> group_id = " << group_id << " result = " << result);
  return result;
}

void AdAnalysisManagerI::LoadGroup(Ice::Long group_id){
  Statement sql;
  sql << "select group_id, campaign_id, member_id, is_active, ad_zone_id, highest_bid from ad_group where group_id = " << group_id;
  mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer).store(sql);
  if (res && res.num_rows() > 0) {
    GroupPtr obj = new Group;
    mysqlpp::Row row = res.at(0);
    Ice::Long group_id = (Ice::Long)row["group_id"];
    Ice::Long plan_id = (Ice::Long)row["campaign_id"];
    Ice::Long member_id = (Ice::Long)row["member_id"];
    Ice::Long zone_id = (Ice::Long)row["ad_zone_id"];
    int highest_bid = (int)row["highest_bid"];
    bool is_active = (bool)row["is_active"];

    obj->group_id = group_id;
    obj->plan_id = plan_id;
    obj->is_active = is_active;
    obj->member_id = member_id;
    obj->zone_id = zone_id;
    obj->highest_bid = highest_bid;
    MCE_INFO("AdAnalysisManagerI::LoadGroup --> group_id = " << group_id << " plan_id = " << plan_id << " is_active = " << is_active << " member_id = " << member_id << " zone_id = " << zone_id << " highest_bid = " << highest_bid);

    {
      IceUtil::RWRecMutex::WLock lock(cache_mutex_);
      group_cache_[group_id] = obj;
    }
  }
  else{
    //for delete action
    {
      IceUtil::RWRecMutex::WLock lock(cache_mutex_);
      GroupMap::const_iterator it = group_cache_.find(group_id);
      if(it != group_cache_.end()){
        group_cache_[group_id]->is_active = false;
        MCE_WARN("AdAnalysisManagerI::LoadGroup --> DB ad_group NO group_id = " << group_id << " group_cache_ HAS " << group_id << " set it's is_active = false");
      }
      else{
        MCE_WARN("AdAnalysisManagerI::LoadGroup --> DB ad_group NO group_id = " << group_id << " group_cache_ NO " << group_id << " too!");
      }
    }
  }
}
void AdAnalysisManagerI::LoadMember(Ice::Long member_id){
  Statement sql;
  sql << "select id, status from ad_member where id = " << member_id;
  mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer).store(sql);
  if (res && res.num_rows() > 0) {
    mysqlpp::Row row = res.at(0);
    MemberPtr obj = new Member;
    Ice::Long member_id = (Ice::Long)row["id"];
    int statues = (int)row["status"];

    obj->member_id = member_id;
    obj->statues = statues;

    MCE_INFO("AdAnalysisManagerI::LoadMember --> member_id = " << member_id << " statues = " << statues);
    {
      IceUtil::RWRecMutex::WLock lock(cache_mutex_);
      member_cache_[member_id] = obj;
    }
  }
  else{
    //for delete action
    {
      IceUtil::RWRecMutex::WLock lock(cache_mutex_);
      MemberMap::const_iterator it = member_cache_.find(member_id);
      if(it != member_cache_.end()){
        member_cache_[member_id]->statues = 0;
        MCE_WARN("AdAnalysisManagerI::LoadMember --> DB ad_member NO member_id = " << member_id << " member_cache_ HAS " << member_id << " set it's status = 0");
      }
      else{
        MCE_WARN("AdAnalysisManagerI::LoadMember --> DB ad_member NO member_id = " << member_id << " member_cache_ NO " << member_id << " too!");
      }
    }
  }
}
void AdAnalysisManagerI::LoadPlan(Ice::Long plan_id){
  Statement sql;
  sql << "select * from ad_campaign where campaign_id = " << plan_id;
  mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer).store(sql);
  if (res && res.num_rows() > 0) {
    PlanPtr obj = new Plan;
    mysqlpp::Row row = res.at(0);
    Ice::Long plan_id = (Ice::Long)row["campaign_id"];
    Ice::Long member_id = (Ice::Long)row["member_id"];
    string start_time = (mysqlpp::Null<string, mysqlpp::NullIsBlank>)row["start_time"];
    string end_time = (mysqlpp::Null<string, mysqlpp::NullIsBlank>)row["end_time"];
    string running_area_hour = (mysqlpp::Null<string, mysqlpp::NullIsBlank>)row["running_area_hour"];
    string running_area_day = (mysqlpp::Null<string, mysqlpp::NullIsBlank>)row["running_area_day"];
    bool is_active = (bool)row["is_active"];
    obj->plan_id = plan_id;
    obj->member_id = member_id;
    if(running_area_hour.empty()){
      obj->running_area_hour = -1;
    }
    else{
      obj->running_area_hour = boost::lexical_cast<Ice::Int>(running_area_hour);
    }
    if(running_area_day.empty()){
      obj->running_area_day = -1;
    }
    else{
      obj->running_area_day = boost::lexical_cast<Ice::Int>(running_area_day);
    }


    if(start_time.empty()){
      obj->start_time = 0;
    }
    else{
      Date date_start_time = Date(start_time);
      obj->start_time = date_start_time.time();
    }
    if(end_time.empty()){
      obj->end_time = 0;
    }
    else{
      Date date_end_time = Date(end_time);
      obj->end_time = date_end_time.time();
    }
    obj->is_active = is_active;

    MCE_INFO("AdAnalysisManagerI::LoadPlan --> plan_id = " << plan_id << " member_id = " << member_id << " start_time = " << obj->start_time
        << " end_time = " << obj->end_time << " running_area_hour = " << running_area_hour << " running_area_day = " <<
        running_area_day << " is_active = " << is_active);

    {
      IceUtil::RWRecMutex::WLock lock(cache_mutex_);
      plan_cache_[plan_id] = obj;
    }

  }
  else{
    //for delete action
    {

      IceUtil::RWRecMutex::WLock lock(cache_mutex_);
      PlanMap::const_iterator it = plan_cache_.find(plan_id);
      if(it != plan_cache_.end()){
        plan_cache_[plan_id]->is_active = false;
        MCE_WARN("AdAnalysisManagerI::LoadPlan --> DB ad_campaign NO plan_id = " << plan_id << " plan_cache_ HAS " << plan_id << " set it's is_active = false");
      }
      else{
        MCE_WARN("AdAnalysisManagerI::LoadPlan --> DB ad_campaign NO plan_id = " << plan_id << " plan_cache_ NO " << plan_id << " too!");
      }
    }
  }
}

void AdAnalysisManagerI::Load(){
  MCE_DEBUG("AdAnalysisManagerI::Load");
  TaskManager::instance().execute(new LoadTask);
}

void AdAnalysisManagerI::Click(const AdClickParameterPtr& para,const Ice::Current& current){

  Ice::Long group_id = para->groupid;
  Ice::Long creative_id = para->creativeid;
  Ice::Int click_price = para->clickprice;
  Ice::Long bit_unit_id = para->bitunitid;
  int userid = para->userid;
  string ip = para->ip;
  string click_url = para->clickurl;
  string ubc = para->ubc;
  string referer = para->referer;
  string pv_time = para->pvtime;


  GroupPtr group_ptr = GetGroup(group_id);

  //creative_id   site_id， 开始时间,结束时间   cheat   4个没有
  if(group_ptr){
	  if(click_price > group_ptr->highest_bid){
		  MCE_WARN("AdAnalysisManagerI::Click -- > AdMatch server can not restart so click_price > highest_bid  ==> " << click_price << " > " << group_ptr->highest_bid);
		  click_price = group_ptr->highest_bid;
	  }
     AD_INFO(creative_id << ',' << group_id << ','
      << group_ptr->plan_id << ',' << group_ptr->member_id << ',' << group_ptr->zone_id << ','
      << /*group_ptr->site_id*/'0' << ',' << /*group_ptr->site_id*/'0' << ',' << group_ptr->zone_id << ','
      << click_price << ',' << FormatedClickTime(time(NULL)) << ',' << click_price << ','
      << click_price << ',' << "1" << ',' << "lm" << ','
      << "0," <<  bit_unit_id << ',' << ip << ',' << userid << ',' << '0');
     MCE_INFO("AdAnalysisManagerI::Click --> group_id = " << group_id << " click_price = " << click_price << " ip = " << ip);
     //1000002681000100000,10000026810001,1000002681,100000003921,  1,              1000030001,   1000030001,     1,                50,       2010-08-08 15:54:33,50,50,1,lm,0,0,10.7.16.13, 100000003921,0,2010-08-08 15:54:00,2010-08-08 15:54:59
     //1000004648000100001,10000046480001,1000004648,100000007704,  100000000001,   0,            0,              100000000001,     65,       2010-08-13 21:32:50,65,65,1,lm,0,0,23.23.12.43,100000007704,0,2010-08-13 21:32:00,2010-08-13 21:32:59

     MyUtil::Date date(time(NULL));
     MyUtil::Date pv_data(boost::lexical_cast<time_t>(pv_time));
     ADCK_INFO(creative_id << "," << date.toDateTime() << "," << bit_unit_id<< "," << click_price << "," << ip << "," << group_ptr->zone_id << ",1000000_" << userid << "," << ubc << "," << click_url << "," << "1,-," << group_ptr->member_id << "," << "," << group_ptr->highest_bid << "," << "2,"<< referer <<",," << pv_data.toDateTime() <<"," << userid << ",2,1,1," << ip << ",,," << group_ptr->highest_bid << ",1,lm,0," << bit_unit_id << ",0,1,4");
  }
  else{
    MCE_WARN("AdAnalysisManagerI::Click --> no group_id " << group_id << "!!!!!!!!!");
  }
}
void AdAnalysisManagerI::Pv(const AdPvParameterSeq& paraseq, const Ice::Current& current){
	int abs_pos = 1;
  for(AdPvParameterSeq::const_iterator it = paraseq.begin(); it != paraseq.end(); ++it){
    Ice::Long group_id = (*it)->groupid;
    string http = (*it)->http;
    string ip = (*it)->ip;
    string referer = (*it)->referer;
    Ice::Long bit_unit_id = (*it)->bitunitid;
    Ice::Long creative_id = (*it)->creativeid;
    referer = "\"" + referer + "\"";
    GroupPtr group_ptr = GetGroup(group_id);
    if(group_ptr){
      MyUtil::Date date = MyUtil::Date(time(NULL));
      string str_zone_id = boost::lexical_cast<string>(group_ptr->zone_id);
      string webid = "00000000";
      if(str_zone_id.length() >= 8){
        webid = str_zone_id.substr(0, 8);
      }
      ADPV_INFO("AdAnalysis0@10.3.17.135||" << ip << "||" << date.toDateTime() << "||\"GET " << http << " HTTP/1.0\"||" << "200||" << "undefined||" << "undefined||" << "undefined||" << "2||" << "1||" << "none||" << referer << "||" << str_zone_id << "^B^B^B^B^B^B^B" <<"||" << creative_id << "^B" << group_ptr->member_id << "^B" << bit_unit_id << "^B0^B" << group_ptr->highest_bid << "^B" << referer << "^B" << abs_pos << "" << "^B1" << "^B2||4");
    }
    else{
      MCE_WARN("AdAnalysisManagerI::Pv --> no group_id " << group_id << "!!!!!!!!!");
    }
		++abs_pos;
  }
}


void AdAnalysisManagerI::LoadCacheData(){
  MCE_DEBUG("AdAnalysisManagerI::LoadCacheData : ");
  GroupMap group_cache;
  MemberMap member_cache;
  PlanMap plan_cache;

  Statement sql;
  MCE_DEBUG("LOAD ad_group");
  sql << "select group_id, campaign_id, member_id, is_active,ad_zone_id, highest_bid from ad_group";
  mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer).store(sql);
  if (res && res.num_rows() > 0) {
    for (size_t i = 0; i < res.num_rows(); ++i) {
      GroupPtr obj = new Group;
      mysqlpp::Row row = res.at(i);
      Ice::Long group_id = (Ice::Long)row["group_id"];
      Ice::Long plan_id = (Ice::Long)row["campaign_id"];
      Ice::Long member_id = (Ice::Long)row["member_id"];
      Ice::Long zone_id = (Ice::Long)row["ad_zone_id"];
      int highest_bid = (int)row["highest_bid"];
      bool is_active = (bool)row["is_active"];

      obj->group_id = group_id;
      obj->plan_id = plan_id;
      obj->is_active = is_active;
      obj->member_id = member_id;
      obj->zone_id = zone_id;
      obj->highest_bid = highest_bid;
      group_cache[group_id] = obj;
      MCE_DEBUG("LOAD GROUP : group_id = " << group_id << " plan_id = " << plan_id << " is_active = " << is_active << " member_id = " << member_id << " zone_id = " << zone_id << " highest_bid = " << highest_bid);
     }
  }
  MCE_DEBUG("LOAD ad_campaign");
  sql.clear();
  sql << "select * from ad_campaign";
  res = QueryRunner(DB_INSTANCE, CDbRServer).store(sql);
  if (res && res.num_rows() > 0) {
    for (size_t i = 0; i < res.num_rows(); ++i) {
      PlanPtr obj = new Plan;
      mysqlpp::Row row = res.at(i);
      Ice::Long plan_id = (Ice::Long)row["campaign_id"];
      Ice::Long member_id = (Ice::Long)row["member_id"];

      string start_time = (mysqlpp::Null<string, mysqlpp::NullIsBlank>)row["start_time"];
      string end_time = (mysqlpp::Null<string, mysqlpp::NullIsBlank>)row["end_time"];


      string running_area_hour = (mysqlpp::Null<string, mysqlpp::NullIsBlank>)row["running_area_hour"];
      string running_area_day = (mysqlpp::Null<string, mysqlpp::NullIsBlank>)row["running_area_day"];
      bool is_active = (bool)row["is_active"];
      obj->plan_id = plan_id;
      obj->member_id = member_id;

      if(running_area_hour.empty()){
        obj->running_area_hour = -1;
      }
      else{
        obj->running_area_hour = boost::lexical_cast<Ice::Int>(running_area_hour);
      }
      if(running_area_day.empty()){
        obj->running_area_day = -1;
      }
      else{
        obj->running_area_day = boost::lexical_cast<Ice::Int>(running_area_day);
      }


      if(start_time.empty()){
        obj->start_time = 0;
      }
      else{
        Date date_start_time = Date(start_time);
        obj->start_time = date_start_time.time();
      }
      if(end_time.empty()){
        obj->end_time = 0;
      }
      else{
        Date date_end_time = Date(end_time);
        obj->end_time = date_end_time.time();
      }
      obj->is_active = is_active;

      plan_cache[plan_id] = obj;
      MCE_DEBUG("LOAD campaign : plan_id = " << plan_id << " member_id = " << member_id << " start_time = " << obj->start_time
          << " end_time = " << obj->end_time << " running_area_hour = " << running_area_hour << " running_area_day = " <<
          running_area_day << " is_active = " << is_active);
    }
  }
  MCE_DEBUG("LOAD AD_MEMBER");
  sql.clear();
  sql << "select id, status from ad_member";
  res = QueryRunner(DB_INSTANCE, CDbRServer).store(sql);
  if (res && res.num_rows() > 0) {
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      MemberPtr obj = new Member;
      Ice::Long member_id = (Ice::Long)row["id"];
      int statues = (int)row["status"];

      obj->member_id = member_id;
      obj->statues = statues;
      member_cache[member_id] = obj;
      MCE_DEBUG("LOAD AD_MEMBER : member_id = " << member_id << " statues = " << statues);
    }
  }

  {
    IceUtil::RWRecMutex::WLock lock(cache_mutex_);
    init_ok_ = true;
    group_cache_.swap(group_cache);
    member_cache_.swap(member_cache);
    plan_cache_.swap(plan_cache);
  }
}

void AdAnalysisManagerI::ReloadGroupById(Ice::Long group_id, const Ice::Current& Current){
  MCE_INFO("AdAnalysisManagerI::ReloadGroupById --> group_id = " << group_id);
  TaskManager::instance().schedule(new ReloadGroupTimer(group_id));
}
void AdAnalysisManagerI::ReloadMemberById(Ice::Long member_id, const Ice::Current& Current){
  MCE_INFO("AdAnalysisManagerI::ReloadMemberById --> member_id = " << member_id);
  TaskManager::instance().schedule(new ReloadMemberTimer(member_id));
}
void AdAnalysisManagerI::ReloadCampaignById(Ice::Long plan_id, const Ice::Current& Current){
  MCE_INFO("AdAnalysisManagerI::ReloadCampaignById --> plan_id = " << plan_id);
  TaskManager::instance().schedule(new ReloadPlanTimer(plan_id));
}
void AdAnalysisManagerI::ReloadCreativeById(Ice::Long creative_id, const Ice::Current& Current){
  MCE_INFO("AdAnalysisManagerI::ReloadCreativeById --> creative_id = " << creative_id);
  TaskManager::instance().schedule(new ReloadCreativeTimer(creative_id));
}


// for tw test ----------------------------------
void AdAnalysisManagerI::Click2(const AdClickParameterPtr& para, const string& append_str,const Ice::Current& current){
  MCE_DEBUG("Click2 --> append_str:" << append_str);
  Ice::Long group_id = para->groupid;
  Ice::Long creative_id = para->creativeid;
  Ice::Int click_price = para->clickprice;
  Ice::Long bit_unit_id = para->bitunitid;
  int userid = para->userid;
  string ip = para->ip;
  string click_url = para->clickurl;
  string ubc = para->ubc;
  string referer = para->referer;
  string pv_time = para->pvtime;


  GroupPtr group_ptr = GetGroup(group_id);

  //creative_id   site_id， 开始时间,结束时间   cheat   4个没有
  if(group_ptr){
	  if(click_price > group_ptr->highest_bid){
		  MCE_WARN("AdAnalysisManagerI::Click -- > AdMatch server can not restart so click_price > highest_bid  ==> " << click_price << " > " << group_ptr->highest_bid);
		  click_price = group_ptr->highest_bid;
	  }
     AD_INFO(creative_id << ',' << group_id << ','
      << group_ptr->plan_id << ',' << group_ptr->member_id << ',' << group_ptr->zone_id << ','
      << /*group_ptr->site_id*/'0' << ',' << /*group_ptr->site_id*/'0' << ',' << group_ptr->zone_id << ','
      << click_price << ',' << FormatedClickTime(time(NULL)) << ',' << click_price << ','
      << click_price << ',' << "1" << ',' << "lm" << ','
      << "0," <<  bit_unit_id << ',' << ip << ',' << userid << ',' << '0');
     MCE_INFO("AdAnalysisManagerI::Click --> group_id = " << group_id << " click_price = " << click_price << " ip = " << ip);
     //1000002681000100000,10000026810001,1000002681,100000003921,  1,              1000030001,   1000030001,     1,                50,       2010-08-08 15:54:33,50,50,1,lm,0,0,10.7.16.13, 100000003921,0,2010-08-08 15:54:00,2010-08-08 15:54:59
     //1000004648000100001,10000046480001,1000004648,100000007704,  100000000001,   0,            0,              100000000001,     65,       2010-08-13 21:32:50,65,65,1,lm,0,0,23.23.12.43,100000007704,0,2010-08-13 21:32:00,2010-08-13 21:32:59

     MyUtil::Date date(time(NULL));
     MyUtil::Date pv_data(boost::lexical_cast<time_t>(pv_time));
     ADCK_INFO(creative_id << "," << date.toDateTime() << "," << bit_unit_id<< "," << click_price << "," << ip << "," << group_ptr->zone_id << ",1000000_" << userid << "," << ubc << "," << click_url << "," << "1,-," << group_ptr->member_id << "," << "," << group_ptr->highest_bid << "," << "2,"<< referer <<",," << pv_data.toDateTime() <<"," << userid << ",2,1,1," << ip << ",,," << group_ptr->highest_bid << ",1,lm,0," << bit_unit_id << ",0,1,4," << append_str);
  }
  else{
    MCE_WARN("AdAnalysisManagerI::Click --> no group_id " << group_id << "!!!!!!!!!");
  }
}
void AdAnalysisManagerI::Pv2(const AdPvParameterSeq& paraseq,const MyUtil::StrSeq& append_str, const Ice::Current& current){
	MCE_DEBUG("Pv2 --> append_str:" << append_str.size());
	int abs_pos = 1;
	int valid_pos = 1;
  for(AdPvParameterSeq::const_iterator it = paraseq.begin(); it != paraseq.end(); ++it){
    Ice::Long group_id = (*it)->groupid;
    string http = (*it)->http;
    string ip = (*it)->ip;
    string referer = (*it)->referer;
    Ice::Long bit_unit_id = (*it)->bitunitid;
    Ice::Long creative_id = (*it)->creativeid;

    GroupPtr group_ptr = GetGroup(group_id);
    if(group_ptr){
      MyUtil::Date date = MyUtil::Date(time(NULL));
      string str_zone_id = boost::lexical_cast<string>(group_ptr->zone_id);
      string webid = "00000000";
      if(str_zone_id.length() >= 8){
        webid = str_zone_id.substr(0, 8);
      }
      string append_info = append_str.at(abs_pos-1);
      int pos = 0;
      if (append_info.find_first_of("_NOTFOUND") != string::npos) {
        pos = valid_pos++;
      }
      ADPV_INFO("AdAnalysis0@10.3.17.135||" << ip << "||" << date.toDateTime() << "||\"GET " << http << " HTTP/1.0\"||" << "200||" << "undefined||" << "undefined||" << "undefined||" << "2||" << "1||" << "none||" << referer << "||" << str_zone_id << "^B^B^B^B^B^B^B" <<"||" << creative_id << "^B" << group_ptr->member_id << "^B" << bit_unit_id << "^B0^B" << group_ptr->highest_bid << "^B" << referer << "^B" << pos << "" << "^B1" << "^B2||4||"<< append_info);
    }
    else{
      MCE_WARN("AdAnalysisManagerI::Pv --> no group_id " << group_id << "!!!!!!!!!");
    }
		++abs_pos;
  }
}


// ----------------------------------------------



void LoadTask::handle(){
	AdAnalysisManagerI::instance().LoadCacheData();
}
void ReloadGroupTimer::handle(){
  AdAnalysisManagerI::instance().LoadGroup(group_id_);
}
void ReloadMemberTimer::handle(){
  AdAnalysisManagerI::instance().LoadMember(member_id_);
}
void ReloadPlanTimer::handle(){
  AdAnalysisManagerI::instance().LoadPlan(plan_id_);
}
void ReloadCreativeTimer::handle(){
  try{
    MCE_INFO("ReloadCreativeTimer::handle --> CALL AdGateAdapter.ReloadCreativeById(" << creative_id_ <<") Success");
    AdGateAdapter::instance().ReloadCreativeById(creative_id_);
  }
  catch(Ice::Exception& e){
    MCE_WARN("ReloadCreativeTimer::handle CALL AdGateAdapter.ReloadCreativeById(" << creative_id_ <<") Fail --> line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("ReloadCreativeTimer::handle CALL AdGateAdapter.ReloadCreativeById(" << creative_id_ <<") Fail--> line:"<<__LINE__<<" err:"<<e.what());
  }
}


void PushAvailableGroupsTimer::handle(){
  //MyUtil::Date d = MyUtil::Date(time(NULL));
  //MCE_INFO("PushAvailableGroupsTimer::handle --> start = " << d.toDateTime() );
  AdAnalysisManagerI::instance().PushAvailableGroups();
  //d = MyUtil::Date(time(NULL));
  //MCE_INFO("PushAvailableGroupsTimer::handle --> end = " << d.toDateTime());
}

