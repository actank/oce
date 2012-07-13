#include "AdMatchManagerI.h"
#include "AdLogAdapter.h"
#include "AdMatchNAdapter.h"
#include "AdInvertedIndex.h"
#include "AdRank.h"
#include "AdStruct.h"
#include "ServiceI.h"
#include "AdAnalysis.h"
#include "Date.h"
#include <QueryRunner.h>
#include <DbDescriptor.h>
#include <boost/tokenizer.hpp>
#include "client/linux/handler/exception_handler.h"
#include <boost/algorithm/string/regex.hpp>

#include "AdAnalysisAdapter.h"
#include "AdEngineDebugI.h"
#include "AdRecommendIAdapter.h"
#include "AdEngineBAdapter.h"

using namespace com::xiaonei::xce;
using namespace xce::ad;
using namespace boost;

std::string xce::ad::DB_INSTANCE = "ade_db";

bool MyCallback(const char *dump_path, const char *minidump_id, void *context, bool succeeded) {
  printf("got crash %s %s\n", dump_path, minidump_id);
  return true;
}

void MyUtil::initialize() {
  srand(time(NULL));
  static google_breakpad::ExceptionHandler eh(".", 0, MyCallback, 0, true);
  ServiceI& service = ServiceI::instance();
  int index = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("ServerIndex", -1);
  xce::ad::DB_INSTANCE = service.getCommunicator()->getProperties()->getPropertyWithDefault("DbInstance", "ade_db");
  service.getAdapter()->add(&AdMatchManagerI::instance(), service.createIdentity("M",""));
  service.getAdapter()->add(&AdEngineDebugI::instance(), service.createIdentity("D",""));
  AdMatchManagerI::instance().SetServerIndex(index);
  Translator::instance().set_server_index(index);
  TaskManager::instance().execute(new InitTask());
  TaskManager::instance().scheduleRepeated(new ReloadTimer());
}

string LogHelper::MakeText(const LogParas& para){
  ostringstream ans;
  string spliter = "^C";
  ans << para.server_index_ << spliter << para.bid_unit_key_ << spliter << para.creative_id_ << spliter << para.bid_unit_id_ << spliter << para.cost_ << spliter << para.click_pos_ << spliter
      << para.max_price_ << spliter << para.pv_time_ << spliter << MakeUbc(para.user_profile_) << spliter << para.zone_id_ << spliter << para.user_key_;
  return ans.str();
}

LogParas LogHelper::PaserText(const string& text){
  LogParas ans;
  if(!text.empty()){
    vector<string> para_strs;
    boost::algorithm::split_regex( para_strs, text,regex( "\\^C" ) ) ;
    if(para_strs.size() >= 11){
      try{
        ans.server_index_ = boost::lexical_cast<int>(para_strs[0]);
        ans.bid_unit_key_ = boost::lexical_cast<Ice::Long>(para_strs[1]);
        ans.creative_id_ = boost::lexical_cast<Ice::Long>(para_strs[2]);
        ans.bid_unit_id_ = boost::lexical_cast<Ice::Long>(para_strs[3]);
        ans.cost_ = boost::lexical_cast<int>(para_strs[4]);
        ans.click_pos_ = boost::lexical_cast<int>(para_strs[5]);
        ans.max_price_ = boost::lexical_cast<int>(para_strs[6]);
        ans.pv_time_ = boost::lexical_cast<time_t>(para_strs[7]);
        ans.ubc_ = para_strs[8];
        ans.zone_id_ = boost::lexical_cast<uint64_t>(para_strs[9]);
        ans.user_key_ = boost::lexical_cast<uint64_t>(para_strs[10]);
      }
      catch(...){
        MCE_WARN("LogHelper::PaserText --> cast error ");
      }
    }
  }
  return ans;
}

void LogHelper::Process(string& str){
  for (int i = 0; i < (int)str.length(); i++) {
    if (str[i] == '|') {
      str.replace(i, 1, "-");
    }
  }
  str = "\"" + str + "\"";
}

string LogHelper::MakeUbc(const UserProfile& upf){
  ostringstream ubc_ios;

  int birth_year = Date(time(NULL)).year() - upf.age();
  int user_stage = upf.stage();
  string valid_area = "";
  if(4 == user_stage){
    valid_area = upf.ip_area();
  }
  else{
    valid_area = upf.current_area();
  }
  if (valid_area.empty()) {
    valid_area = "-";
  }
  string current_area = (upf.current_area().empty()) ? "-" : upf.current_area();
  string ip_area = (upf.ip_area().empty()) ? "-" : upf.ip_area();

  ubc_ios << "1000000_" << upf.id() << "|" << upf.gender() << "|" << birth_year
      << "-01-01" << "|" << upf.age() << "|" << upf.stage() << "|" << valid_area << "|"
      << upf.school() << "_" << current_area << "|" << upf.grade() << "|0|0|" << ip_area;
  return ubc_ios.str();
}

string LogHelper::MakeHttp(const LogParas& para){
  ostringstream http_ios;
  http_ios << "/show?userid=" << para.user_profile_.id() << "&tt=0&type=.js&adzoneid="
          << para.zone_id_ << "&count=" << para.ad_count_ << "&rowoffset=0&ip="
          << MakeIp(para.user_profile_.ip()) << "&jbc=cookie|a|b&ubc=" << MakeUbc(para.user_profile_);
  return http_ios.str();
}

string LogHelper::MakeIp(int ip){
  int a, b, c, d;
  unsigned ip_u = (unsigned) ip;
  a = ip_u / 16777216;
  b = (ip_u - a * 16777216) / 65536;
  c = (ip_u - a * 16777216 - b * 65536) / 256;
  d = (ip_u - a * 16777216 - b * 65536 - c * 256);
  ostringstream ios2;
  ios2 << a << "." << b << "." << c << "." << d;
  return ios2.str();
}

void LogHelper::CookieEmptyCheck(UserProfile& upf){
  int userid = upf.id();
  if(!upf.id()){
    upf.set_id(888);
  }
  if(upf.age() > 100 || upf.age() < 15){
    upf.set_age(20 + AdMatchManagerI::instance().ServerIndex());
  }
  if(upf.ip_area().empty() || "0000000000000000" == upf.ip_area()){
    upf.set_ip_area("0086110000000000");
  }
  if(!upf.stage()){
    upf.set_stage(4);
  }
  if(!upf.ip()){
    upf.set_ip(975044608);
  }
  if(upf.current_area().empty()){
    upf.set_current_area("0086110000000000");
  }


  ///////////////////////////
  //if(!upf.home_area().empty()) {
  //  upf.set_ip_area(upf.home_area()); //for test only
  //}
  ///////////////////////////
}

string LogHelper::FormatedClickTime(time_t t)
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

void AdMatchManagerI::NotifyBindUser(int userid, Ice::Long creativeid, Ice::Long zoneid, const Ice::Current& Current){
  //MCE_INFO("AdMatchManagerI::NotifyBindUser --> userid = " << userid << " creativeid = " << creativeid << " zoneid = " << zoneid);
  IceUtil::RWRecMutex::WLock lock(mutex_);
  UserBind ul;
  ul.userid = userid;
  ul.zoneid = zoneid;
  UserBindMap::iterator it = user_bind_cache_.find(ul);
  if(it == user_bind_cache_.end()){
    //MCE_INFO("AdMatchManagerI::NotifyBindUser --> add to user_bind_cache_ ul.userid = " << ul.userid << " ul.zoneid = " << zoneid);
    user_bind_cache_.insert(make_pair<UserBind, Ice::Long>(ul, creativeid));
  }
  else{
    it->second = creativeid;
  }
}
void AdMatchManagerI::NotifyUnbindUser(int userid, Ice::Long zoneid, const Ice::Current& Current){
  //MCE_INFO("AdMatchManagerI::NotifyUnbindUser --> userid = " << userid << " zoneid = " << zoneid);
  IceUtil::RWRecMutex::WLock lock(mutex_);
  UserBind ul;
  ul.userid = userid;
  ul.zoneid = zoneid;
  UserBindMap::iterator it = user_bind_cache_.find(ul);
  if(it != user_bind_cache_.end()){
    user_bind_cache_.erase(it);
  }
}

void AdMatchManagerI::BindUser(int userid, Ice::Long creativeid, Ice::Long zoneid, const Ice::Current& current){
  //MCE_INFO("AdMatchManagerI::BindUser --> userid = " << userid << " creativeid = " << creativeid << " zoneid = " << zoneid);
  Statement sql;
  ostringstream os;
  os << "insert into ad_user_bind (userid, creative_id, adzone_id) values("
      << userid << "," << creativeid << ", " << zoneid << ") on duplicate key update creative_id = values(creative_id)";
  //MCE_INFO("AdMatchManagerI::BindUser --> userid = " << userid << " creativeid = " << creativeid << " zoneid = " << zoneid << " sql = " << os.str());
  sql << os.str();
  mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbWServer, "ad_user_bind").store(sql);
  try{
    AdMatchNAdapter::instance().NotifyBindUser(userid, creativeid, zoneid);
  }
  catch(Ice::Exception& e){
    MCE_WARN("AdMatchManagerI::BindUser --> call AdMatchNAdapter.NotifyBindUser ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("AdMatchManagerI::BindUser --> call AdMatchNAdapter.NotifyBindUser ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
  try{
    AdEngineBAdapter::instance().BindUser(userid, creativeid, zoneid);
  }
  catch(Ice::Exception& e){
    MCE_WARN("AdMatchManagerI::BindUser --> call AdEngineBAdapter.BindUser ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("AdMatchManagerI::BindUser --> call AdEngineBAdapter.BindUser ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
}

void AdMatchManagerI::UnbindUser(int userid, Ice::Long zone_id, const Ice::Current& current){
  //MCE_INFO("AdMatchManagerI::UnbindUser --> userid = " << userid);
  Statement sql;
  ostringstream os;
  os << "delete from ad_user_bind where userid = " << userid << " and adzone_id = " << zone_id;
  //MCE_INFO("AdMatchManagerI::UnbindUser --> userid = " << userid << " zoneid = " << zone_id << " sql = " << os.str());
  sql << os.str();
  mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbWServer, "ad_user_bind").store(sql);
  try{
    AdMatchNAdapter::instance().NotifyUnbindUser(userid, zone_id);
  }
  catch(Ice::Exception& e){
    MCE_WARN("AdMatchManagerI::BindUser --> call AdMatchNAdapter.NotifyUnbindUser ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("AdMatchManagerI::BindUser --> call AdMatchNAdapter.NotifyUnbindUser ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
  try{
    AdEngineBAdapter::instance().UnbindUser(userid, zone_id);
  }
  catch(Ice::Exception& e){
    MCE_WARN("AdMatchManagerI::BindUser --> call AdEngineBAdapter.UnbindUser ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("AdMatchManagerI::BindUser --> call AdEngineBAdapter.UnbindUser ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
}

void AdMatchManagerI::PvLog(const set<AdGroupPtr>& zone_selected, const UserProfile& profile, uint64_t user_key, AdInfoSeq& ad_info_seq, const string& referer, const AdZonePtr& zone_ptr, const map<Ice::Long, int>& group_costs, int rotate_index){
  TimeStat ts;
  ostringstream log;
  int abs_pos = 1;
	vector<string> logseq;
  string referer_tmp = referer;
  LogHelper::instance().Process(referer_tmp);
  for(set<AdGroupPtr>::iterator sit = zone_selected.begin(); sit != zone_selected.end(); ++sit, ++abs_pos){
    AdInfo ad_info;
    MyUtil::Date date(time(NULL));
    AdGroupPtr group_ptr = *sit;

    AdEffectStat::instance().IncPv(user_key, group_ptr->group_id());
    //MCE_INFO("AdMatchNManagerI::GetAds --> IncPv user_key = " << user_key << " group_id = " << group_ptr->group_id());

    if(!group_ptr->creative_ids().empty()){
      //ad_info.creativeid = *(group_ptr->creative_ids().begin());
      int next_creative_index = 0;
      if(zone_ptr->GetRotateSize()){
        next_creative_index = rotate_index / zone_ptr->GetRotateSize();
        //MCE_INFO("AdMatchManagerI::PvLog -->  userid = " << userid << " rotate_index = " << rotate_index << " RotateSize = " << zone_ptr->GetRotateSize() << " next_creative_index = " << next_creative_index);
      }
      ad_info.creativeid = group_ptr->GetNextCreative(next_creative_index);
    }
    else{
      ad_info.creativeid = 0;
    }
    ad_info.groupid = group_ptr->group_id();
    ad_info.updateTime = 0;
    LogParas log_para;
    log_para.server_index_ = server_index_;
    map<Ice::Long, int>::const_iterator cost_it = group_costs.find(ad_info.groupid);
    if(cost_it == group_costs.end()){
      log_para.cost_ = 0;
    }
    else{
      log_para.cost_ = cost_it->second;
    }
    log_para.user_profile_ = profile;
    log_para.pv_time_ = Date(time(NULL)).time();
    log_para.creative_id_ = ad_info.creativeid;
    log_para.user_key_ = user_key;
    log_para.bid_unit_key_ = group_ptr->bid_unit_key();
    log_para.max_price_ = group_ptr->max_price();
    log_para.zone_id_ = zone_ptr->id();
    log_para.ad_count_ = zone_ptr->ad_count();
    ad_info.text = LogHelper::instance().MakeText(log_para);
    ad_info_seq.push_back(ad_info);

    //call AdLogAdapter
    Ice::Long zone_id = log_para.zone_id_;
    Ice::Long creativeid = log_para.creative_id_;
    string http = LogHelper::instance().MakeHttp(log_para);
    string ip = LogHelper::instance().MakeIp(profile.ip());
    log << "AdLog@10.3.17.135||" << ip << "||" << date.toDateTime() << "||\"GET " << http << " HTTP/1.0\"||" << "200||"
        << "undefined||" << "undefined||" << "undefined||" << "2||" << "1||" << "none||" << referer_tmp << "||" << zone_id <<
        "^B^B^B^B^B^B^B" <<"||" << creativeid << "^B" << group_ptr->member_id() << "^B" << group_ptr->bid_unit_id()
        << "^B0^B" << group_ptr->max_price() << "^B" << referer_tmp << "^B" << abs_pos << "" << "^B1" << "^B2||4";
		logseq.push_back(log.str());
    /*try{
      ts.reset();
			logseq.push_buck(log.str());
      //AdLogAdapter::instance().Pv(log.str());
      //MCE_INFO("AdMatchManagerI::PvLog --> TIME_TEST write pv_log : " << ts.getTime());
    }
    catch(Ice::Exception& e){
      MCE_WARN("AdMatchManagerI::PvLog --> call AdLogAdapter.Pv ERR  line:"<<__LINE__<<" err:"<<e);
    }
    catch(std::exception& e){
      MCE_WARN("AdMatchManagerI::PvLog --> call AdLogAdapter.Pv ERR  line:"<<__LINE__<<" err:"<<e.what());
    }*/
    log.str("");
  }
	try{
    AdLogAdapter::instance().PvBySeq(logseq);
    //MCE_INFO("AdMatchManagerI::PvLog --> TIME_TEST write pv_log : " << ts.getTime());
  }
  catch(Ice::Exception& e){
    MCE_WARN("AdMatchManagerI::PvLog --> call AdLogAdapter.Pv ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("AdMatchManagerI::PvLog --> call AdLogAdapter.Pv ERR  line:"<<__LINE__<<" err:"<<e.what());
  }

}

void AdMatchManagerI::FilterBindUser(AdZoneDict& zone_dict, AdResultMap& ans, int userid){
  //MCE_INFO("AdMatchManagerI::FilterBindUser --> userid = " << userid);
  IceUtil::RWRecMutex::RLock lock(mutex_);
  for(AdZoneDict::iterator it = zone_dict.begin(); it != zone_dict.end();){
    UserBind ub;
    ub.userid = userid;
    ub.zoneid = it->second->id();
    //MCE_DEBUG("AdMatchManagerI::FilterBindUser --> userid = " << userid << " zone_id = " << ub.zoneid);
    UserBindMap::iterator ubit = user_bind_cache_.find(ub);
    if(ubit != user_bind_cache_.end()){
      //MCE_INFO("AdMatchManagerI::FilterBindUser --> userid = " << userid << " erase zoneid = " << ub.zoneid);
      AdInfoSeq ad_info_seq;
      AdInfo ad_info;
      ad_info.creativeid = ubit->second;
      ad_info.groupid = 0;
      ad_info.updateTime = 0;
      ad_info.text = "";
      ad_info_seq.push_back(ad_info);
      ans[ub.zoneid] = ad_info_seq;
      zone_dict.erase(it++);
    }
    else{
      ++it;
    }
  }
}


AdResultMap AdMatchManagerI::GetAds(const string& userProfile, const string& referer, int rotate_index, const Ice::Current& current){

  MCE_INFO("AdMatchNManagerI::GetAds --> call from " << current.con->toString());
  ostringstream timelog;

  TimeStat ts;
  TimeStat subts;
  AdResultMap ans;
  UserProfile profile;
  ostringstream info;
  set<AdGroupPtr> selected;

  bool hide = false;
  Ice::Context::const_iterator i = current.ctx.find("hide");
  if (i != current.ctx.end() && i->second == "true") {
    hide = true;
  }

  bool get_ad_success = true;
  UserProfileClient::instance().Deserialize(userProfile, &profile);

  timelog << " after Deserialize time_cost = " << ts.getTime();

  if(!init_ok_){
    MCE_INFO("AdMatchNManagerI::GetAds --> initing.......");
    return ans;
  }

  LogHelper::instance().CookieEmptyCheck(profile);

  timelog << " after CookieEmptyCheck time_cost = " << ts.getTime();

  info << "uid = "<<profile.id()<<",age = " << profile.age() << ",gender = "<< profile.gender() <<
      ",stage = " << profile.stage() << ",grade = " << profile.grade() <<
      ",current_area = " << profile.current_area() << ",ip_area = " << profile.ip_area() << ",school =" << profile.school();
  //MCE_INFO("AdMatchNManagerI::GetAds -->~~~~~~~~~~~~~~~~~~~~~rotate_index =  " << rotate_index << "   " << info.str());

  uint64_t user_key = Translator::instance().GetIndex(profile);
  subts.reset();
  AdZoneDict zone_dict = AdZonePool::instance().GetAdZoneOrdered(referer, profile.stage(), hide);
  //MCE_DEBUG("[VIP_DEBUG] uid = " << profile.id() << ", hide = " << hide);

  timelog << " after GetAdZoneOrdered2 time_cost = " << ts.getTime();

  if(zone_dict.empty()){
    //MCE_WARN("AdMatchNManagerI::GetAds --> No Result For GetAdZoneOrdered referer = " << referer);
    return ans;
  }
  //MCE_INFO("AdMatchNManagerI::GetAds --> AdZoneDict->size = " << zone_dict.size());
  //MCE_INFO("AdMatchNManagerI::GetAds --> TIME_TEST GetAdZoneOrdered = " << subts.getTime());

  //////////////////////////////////////////////////////////
  FilterBindUser(zone_dict, ans, profile.id());

  timelog << " after FilterBindUser time_cost = " << ts.getTime();

  //////////////////////////////////////////////////////////
  for(AdZoneDict::iterator it = zone_dict.begin(); it != zone_dict.end(); ++it){
    //MCE_INFO("AdMatchNManagerI::GetAds --> process adzone : " << it->second->id());
    ostringstream zone_debug_log;
    set<AdGroupPtr> zone_selected;
    map<Ice::Long, int> group_costs;
    AdZonePtr zone = it->second;
    Ice::Long zone_id = it->second->id();
    zone_debug_log << "[ZONE_DEBUG] adzone_id = " << zone_id << ", ";
    int ad_count = it->second->ad_count();
    int rotate_size = it->second->GetRotateSize();
    int zone_rotate_index = 0;
    if(rotate_size){
      zone_rotate_index = rotate_index % rotate_size;
    }
    zone_debug_log << "zone_rotate_index = " << zone_rotate_index << ", ";
    //**MCE_INFO("AdMatchNManagerI::GetAds --> @@@@@@@@@@@@@@@@@ zone_id = " << zone_id << " ad_count = " << ad_count << " rotate_size = " << rotate_size << " rotate_index = " << rotate_index << " zone_rotate_index = " << zone_rotate_index);
    subts.reset();
    AdGroupPtr rotate_group_ptr = it->second->GetRotateGroups(zone_rotate_index, user_key);
    //MCE_INFO("AdMatchNManagerI::GetAds --> TIME_TEST GetRotateGroups = " << subts.getTime());

    //MCE_INFO("AdMatchNManagerI::GetAds --> AdZoneDict@ ad_count = " << ad_count << " zone_id = " << zone_id << " rotate_size = " << rotate_size);
    if(rotate_group_ptr){
      set<AdGroupPtr> rotate_set;
      rotate_set.insert(rotate_group_ptr);
      AdRanker::instance().Filter(rotate_set, selected);
      if(!rotate_set.empty()){
        zone_debug_log << "has rotate_group = " << rotate_group_ptr->group_id() << ", ";
        zone_selected.insert(rotate_group_ptr);
        //MCE_INFO("AdMatchNManagerI::GetAds ==> zone_id = " << zone_id << " ad_count = " << ad_count << " GET ROTATE SUCCESS, group_id = " << rotate_group_ptr->group_id() <<" zone_rotate_index = " << zone_rotate_index);
        ad_count--;
      }
      else{
        //**MCE_INFO("AdMatchNManagerI::GetAds ==>Get ROTATE ad from AdZone : " << zone_id << " zone_rotate_index = " << zone_rotate_index << " AFTER RANK FAILED !!!! adgroup_id = " << rotate_group_ptr->group_id() << " bid_unit_id = " << rotate_group_ptr->bid_unit_id() << " bid_unit_key = " << rotate_group_ptr->bid_unit_key() << " userkey = " << user_key);
      }
    } else {
      zone_debug_log << "has no rotate_group, ";
      if ((zone->member_type() == 0) && ((rotate_size - zone_rotate_index) <= zone->self_rotation_number())) {
        MCE_INFO("AdMatchNManagerI::GetAds --> need ad from AdEngine/Tw: uid = " << profile.id() << ", zone_id = " << zone->id() << ", rotate_index = " << zone_rotate_index);
        AdInfoSeq ad_info_seq;
        AdInfo ad_info;
        ad_info.groupid = -1;
        ad_info_seq.push_back(ad_info);
        ans[zone->id()] = ad_info_seq;
        ans[9999963] = AdInfoSeq(); // old code, to delete
        continue;
      }
    }
    timelog << " after GetRotateGroups time_cost = " << ts.getTime();
    //update selected
    selected.insert(zone_selected.begin(), zone_selected.end());
    if(ad_count > 0){
      int get_cpm_count= 0;
      subts.reset();
      set<AdGroupPtr> groups = DimIndexMatcher::instance().Get(it->second->id(), userProfile);
      //MCE_INFO("AdMatchNManagerI::GetAds --> TIME_TEST DimIndexMatcher::Get = " << subts.getTime());
      if(!groups.empty()){
        //MCE_INFO("AdMatchNManagerI::GetAds --> get  Rank  groups.size = " << groups.size() << " userkey = " << user_key << " selected.size() = " << selected.size() << " ad_count = " << ad_count);
        subts.reset();
        RankStructSeq rank_seq = AdRanker::instance().Rank(groups, user_key, selected, ad_count);
        //MCE_INFO("AdMatchNManagerI::GetAds --> TIME_TEST AdRanker::Rank = " << subts.getTime());
        for(RankStructSeq::const_iterator rank_it = rank_seq.begin(); rank_it != rank_seq.end(); ++rank_it){
          AdGroupPtr obj = AdGroupPool::instance().GetAdGroup((*rank_it)->gid);
          if(obj){
            get_cpm_count++;
            zone_selected.insert(obj);
            group_costs[(*rank_it)->gid] = (*rank_it)->cost;
            //MCE_INFO("AdMatchNManagerI::GetAds ==> zone_id = " << zone_id << " ad_count = " << ad_count << " GET CPM SUCCESS, group_id = " << (*rank_it)->gid << " cost = " << (*rank_it)->cost);

          }
        }
        ad_count -= get_cpm_count;
       //**MCE_INFO("AdMatchNManagerI::GetAds ==>Get CMP ad from AdZone : " << zone_id << " zone_rotate_index = " << zone_rotate_index << " success!  size = " << rank_seq.size());
      }
      timelog << " after DimIndexMatcher::Get time_cost = " << ts.getTime();
      if(ad_count > 0){
        //MCE_INFO("AdMatchNManagerI::GetAds --> Get Default Ad");
        //TODO default has only one;
        AdGroupPtr default_group_ptr = it->second->GetDefaultGroups();
        if(default_group_ptr){
          zone_debug_log << ", has default_group = " << default_group_ptr->group_id();
          //MCE_INFO("AdMatchNManagerI::GetAds ==> zone_id = " << zone_id <<" ad_count = " << ad_count << " Get DEFAULT SUCCESS, group_id = "<< default_group_ptr->group_id() << " zone_rotate_index = " << zone_rotate_index);
          zone_selected.insert(default_group_ptr);
        }
        else{
          zone_debug_log << ", has no default_group";
          get_ad_success = false;
          //MCE_INFO( "AdMatchNManagerI::GetAds ==> zone_id = " << zone_id << " NO ROTATE, NO CMP, NO DEFAULT");
        }
      }
    }
    timelog << " after GetDefaultGroups::Get time_cost = " << ts.getTime();
    //update selected
    selected.insert(zone_selected.begin(), zone_selected.end());
    //MCE_INFO("AdMatchNManagerI::GetAds --> zone_selected.size() = " << zone_selected.size() << " selected.size() = " << selected.size());

    AdInfoSeq ad_info_seq;
    PvLog(zone_selected, profile, user_key, ad_info_seq, referer, it->second, group_costs, rotate_index);
    timelog << " after PvLog time_cost = " << ts.getTime();
    ans[zone_id] = ad_info_seq;
  }
  ostringstream res_log;
  //MCE_INFO("AdMatchNManagerI::GetAds --> USE_TIME_LOG : " << timelog.str());
  res_log << "AdMatchNManagerI::GetAds -->~~~~~~~~~~~~~~~~~~~~~userid = " << profile.id() << " ans.size = " << ans.size() <<" TIME_TEST ALL END  time = " << ts.getTime() << " ";
  for(AdResultMap::iterator ansit = ans.begin(); ansit != ans.end(); ++ansit){
    res_log << "FINAL zone_id = " << ansit->first << " result.size() = " << ansit->second.size() << ", ";
  }
  //MCE_INFO(res_log.str());
  return ans;
}

void AdMatchManagerI::Click(int uid, const string& ip, const string& text,const string& referer ,const string& click_url ,const Ice::Current& current){
  ostringstream clicklog, chargelog;
  LogParas para = LogHelper::instance().PaserText(text);

  if(!para.ubc_.empty()){
    string deal_referer = referer;
    LogHelper::instance().Process(deal_referer);
    string deal_click_url = click_url;
    LogHelper::instance().Process(deal_click_url);

    //AdAnalysisAdapter::instance().Click(click_para);
    Ice::Long group_id = para.creative_id_ / 100000;
    AdGroupPtr group_ptr = AdGroupPool::instance().GetAdGroup(group_id);
    if(group_ptr){

      uint64_t user_key = boost::lexical_cast<uint64_t>(para.user_key_);
      AdEffectStat::instance().IncClick(user_key, group_id);
      //MCE_INFO("AdMatchManagerI::Click --> userid = " << uid << " creativeid = " << para.creative_id_ << " text = " << text);
      MyUtil::Date pv_date(boost::lexical_cast<time_t>(para.pv_time_));
      if(para.cost_ > group_ptr->max_price()){
        //MCE_WARN("AdMatchManagerI::Click --> highest_bid  ==> " << para.cost_ << " > " << group_ptr->max_price());
        para.cost_ = group_ptr->max_price();
      }

      chargelog << para.creative_id_ << ',' << group_id << ','
        << group_ptr->campaign_id() << ',' << group_ptr->member_id() << ',' << para.zone_id_ << ','
        << /*group_ptr->site_id*/'0' << ',' << /*group_ptr->site_id*/'0' << ',' << para.zone_id_ << ','
        << para.cost_ << ',' << LogHelper::instance().FormatedClickTime(time(NULL)) << ',' << para.cost_ << ','
        << para.cost_ << ',' << "1" << ',' << "lm" << ','
        << "0," <<  group_ptr->bid_unit_id() << ',' << ip << ',' << uid << ',' << '0';


      clicklog << para.creative_id_ << "," << MyUtil::Date(time(NULL)).toDateTime() << "," << group_ptr->bid_unit_id()<< "," << "0"/*para.cost_*/ << ","
          << ip << "," << para.zone_id_ << ",1000000_" << uid << "," << para.ubc_ << "," << deal_click_url << ","
          << "1,-," << group_ptr->member_id() << "," << "," << group_ptr->max_price() << "," << "2,"<< deal_referer <<",,"
          << pv_date.toDateTime() <<"," << uid << ",2,1,1," << ip << ",,," << group_ptr->max_price() << ",1,lm,0,"
          << group_ptr->bid_unit_id() << ",0,1,4";
      try{
        AdLogAdapter::instance().ClickForBp(clicklog.str());
      }
      catch(Ice::Exception& e){
        MCE_WARN("AdMatchManagerI::Click --> call AdLogAdapter.ClickForBp ERR  line:"<<__LINE__<<" err:"<<e);
      }
      catch(std::exception& e){
        MCE_WARN("AdMatchManagerI::Click --> call AdLogAdapter.ClickForBp ERR  line:"<<__LINE__<<" err:"<<e.what());
      }

      try{
        AdLogAdapter::instance().ClickForCharge(chargelog.str());
      }
      catch(Ice::Exception& e){
        MCE_WARN("AdMatchManagerI::Click --> call AdLogAdapter.ClickForCharge ERR  line:"<<__LINE__<<" err:"<<e);
      }
      catch(std::exception& e){
        MCE_WARN("AdMatchManagerI::Click --> call AdLogAdapter.ClickForCharge ERR  line:"<<__LINE__<<" err:"<<e.what());
      }
      try {
        //MCE_INFO("AdMatchManagerI::Click --> call AdRecommendAdapter.Click: uid =" << uid << ", group_id = " << group_id);
        AdRecommendIAdapter::instance().Click(uid, para.zone_id_, group_id);
      } catch (Ice::Exception& e) {
        MCE_WARN("AdMatchManagerI::Click --> call AdRecommendAdapter.Click ERR  line:"
                << __LINE__ << " err:" << e);
      } catch (std::exception& e) {
        MCE_WARN("AdMatchManagerI::Click --> call AdRecommendAdapter.Click ERR  line:"
                << __LINE__ << " err:" << e.what());
      }
    }
  }
}

void AdMatchManagerI::UserBindCacheInit(){
  UserBindMap user_bind_cache;
  Statement sql;
  sql << "select * from ad_user_bind";
  mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer, "ad_user_bind").store(sql);
  if (res && res.num_rows() > 0) {
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      int userid = (int) row["userid"];
      Ice::Long creativeid = (Ice::Long)row["creative_id"];
      Ice::Long zoneid = (Ice::Long)row["adzone_id"];
      UserBind ul;
      ul.userid = userid;
      ul.zoneid = zoneid;
      user_bind_cache[ul] = creativeid;
    }
  }
  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    user_bind_cache_.swap(user_bind_cache);
  }
}

void AdMatchManagerI::Init(){
  MCE_INFO("AdMatchManagerI::Init --> begin init :" << Date(time(NULL)).toDateTime());
  AdMemberPool::instance().Init();
  MCE_INFO("AdMatchManagerI::Init --> AdMemberPool::instance().Init(); DONE");
  AdCampaignPool::instance().Init();
  MCE_INFO("AdMatchManagerI::Init -->  AdCampaignPool::instance().Init(); DONE");
  AdGroupPool::instance().Init();
  MCE_INFO("AdMatchManagerI::Init -->  AdGroupPool::instance().Init(); DONE");
  AdZonePool::instance().Init();
  MCE_INFO("AdMatchManagerI::Init -->  AdZonePool::instance().Init(); DONE");
  CompeteRef::instance().Init();
  MCE_INFO("AdMatchManagerI::Init -->  CompeteRef::instance().Init(); DONE");
}

void InitTask::handle(){
  SchoolPool::instance().Init();
  MCE_INFO("AdMatchManagerI::Init --> SchoolPool::instance().Init(); DONE");
  AdMatchManagerI::instance().UserBindCacheInit();
  AdMatchManagerI::instance().Init();
  TaskManager::instance().scheduleRepeated(new AdEffectStat::StorageInfoTimer());
  TaskManager::instance().scheduleRepeated(new AdEffectStat::KickTimer());
  TaskManager::instance().scheduleRepeated(new AdEffectTimer());
  AdMatchManagerI::instance().InitOk();
}

void ReloadTimer::handle(){
  AdMatchManagerI::instance().Init();
}


/************************TEST********************************/
AdResultMap AdMatchManagerI::GetAdsTest(::Ice::Int uid, ::Ice::Int stage,
    ::Ice::Int gender, ::Ice::Int age, const ::std::string& currentArea,
    const ::std::string& ipArea, ::Ice::Long school, ::Ice::Int grade,
    int count, const string & referer, int rotate_index, const Ice::Current& current) {
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
  return AdMatchManagerI::GetAds(tmp, tmpref, rotate_index);

}
/***********************************************************/

