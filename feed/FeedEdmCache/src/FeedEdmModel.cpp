#include "FeedEdmModel.h"
#include "IceUtil/ThreadException.h"
#include <boost/lexical_cast.hpp>
#include <json/json.h>
#include <sstream>
#include "LogWrapper.h"
#include "TaskManager.h"
namespace xce {
namespace edm {
// tables for direct
const std::string kOrientTableName = "feed_edm_direct";
const std::string kAssignTableName = "feed_edm_assigned";

void MiniteCache::handle() {
  Reset();
}
bool MiniteCache::Insert(Ice::Int edm_id) {
  IceUtil::Mutex::Lock lock(mutex_);
  std::map<Ice::Int,int>::iterator ite = edm_send_info_.find(edm_id);
  if (ite != edm_send_info_.end()) {
    ++ite->second;
    MCE_DEBUG("times of edm:" << edm_id << "=" << edm_send_info_.find(edm_id)->second); 
    return true;
  } else {
    std::pair<std::map<Ice::Int,int>::iterator, bool> result = edm_send_info_.insert(make_pair(edm_id, 1));
    MCE_DEBUG("times of edm:" << edm_id << "=" << edm_send_info_.find(edm_id)->second);
    return result.second;
  }
  return true;
}
int MiniteCache::GetSendTimes(Ice::Int edm_id) {
  IceUtil::Mutex::Lock lock(mutex_);
  std::map<Ice::Int,int>::iterator ite = edm_send_info_.find(edm_id);
  if (ite != edm_send_info_.end()) {
    return ite->second;
  }
  return 0;
}
bool MiniteCache::Reset() {
  IceUtil::Mutex::Lock lock(mutex_);
  edm_send_info_.clear();
  return true;
}
Ice::Int ConfigureModel::id() {
  return id_;
}
void ConfigureModel::set_id(Ice::Int id) {
  id_ = id;
}
 int ConfigureModel::type() {
  return type_;
}
void ConfigureModel::set_type(int type) {
  type_ = type;
}
int ConfigureModel::ad_type() {
  return ad_type_;
}
void ConfigureModel::set_ad_type(int ad_type) {
  ad_type_ = ad_type;
}
int ConfigureModel::limit_type() {
  return limit_type_;
}
void ConfigureModel::set_limit_type(int limit_type) {
  limit_type_ = limit_type;
}
int ConfigureModel::interactive_flag() {
  return interactive_flag_;
}
void ConfigureModel::set_interactive_flag(int interactive_flag) {
  interactive_flag_ = interactive_flag;
}
int ConfigureModel::displayzzs() {
  return displayzzs_;
}
void ConfigureModel::set_displayzzs(int displayzzs) {
  displayzzs_ = displayzzs;
}
int ConfigureModel::demand() {
  return demand_;
}
void ConfigureModel::set_demand(int demand) {
  demand_ = demand;
}
int ConfigureModel::max_per_hour() {
  return max_per_hour_;
}
void ConfigureModel::set_max_per_hour(int max_per_hour) {
  max_per_hour_ = max_per_hour;
}
std::string ConfigureModel::name() {
  return name_;
}
void ConfigureModel::set_name(std::string name) {
  name_ = name;
}
int ConfigureModel::status() {
  return status_;
}
void ConfigureModel::set_status(int status) {
  status_ = status;
}
time_t ConfigureModel::begin_time() {
  return begin_time_;
}
void ConfigureModel::set_begin_time(time_t begin_time) {
  begin_time_ = begin_time;
}
time_t ConfigureModel::create_ime() {
  return create_ime_;
}
void ConfigureModel::set_create_ime(time_t create_ime) {
  create_ime_ = create_ime;
}
time_t ConfigureModel::update_ime() {
  return update_ime_;
}
void ConfigureModel::set_update_ime(time_t update_ime) {
  update_ime_ = update_ime;
}
std::string ConfigureModel::title() {
  return title_;
}
void ConfigureModel::set_title(std::string title) {
  title_ = title;
}
std::string ConfigureModel::title_link() {
  return title_link_;
}
void ConfigureModel::set_title_link(std::string title_link) {
  title_link_ = title_link;
}
std::string ConfigureModel::body() {
  return body_;
}
void ConfigureModel::set_body(std::string body) {
  body_ = body;
}
std::string ConfigureModel::body_link() {
  return body_link_;
}
void ConfigureModel::set_body_link(std::string body_link) {
  body_link_ = body_link;
}
std::string ConfigureModel::logo() {
  return logo_;
}
void ConfigureModel::set_logo(std::string logo) {
  logo_ = logo;
}
std::string ConfigureModel::media() {
  return media_;
}
void ConfigureModel::set_media(std::string media) {
  media_ = media;
}
int ConfigureModel::customer_id() {
  return customer_id_;
}
void ConfigureModel::set_customer_id(int customer_id) {
  customer_id_ = customer_id;
}
int ConfigureModel::consume() {
  return consume_;
}
void ConfigureModel::set_consume(int consume) {
  consume_ = consume;
}
int ConfigureModel::click() {
  return click_;
}
void ConfigureModel::set_click(int click) {
  click_ = click;
}
int ConfigureModel::expose() {
  return expose_;
}
void ConfigureModel::set_expose(int expose) {
  expose_ = expose;
}
int ConfigureModel::ilike() {
  return ilike_;
}
void ConfigureModel::set_ilike(int ilike) {
  ilike_ = ilike;
}
int ConfigureModel::share() {
  return share_;
}
void ConfigureModel::set_share(int share) {
  share_ = share;
}
int ConfigureModel::pub_type() {
  return pub_type_;
}
void ConfigureModel::set_pub_type(int pub_type) {
  pub_type_ = pub_type;
}
std::string ConfigureModel::time_interval() {
  return time_interval_;
}
void ConfigureModel::set_time_interval(std::string time_interval) {
  time_interval_ = time_interval;
}
std::string ConfigureModel::feed_clazz() {
  return feed_clazz_;
}
void ConfigureModel::set_feed_clazz(std::string feed_clazz) {
  feed_clazz_ = feed_clazz;
}
int ConfigureModel::reply_count() {
  return reply_count_;
}
void ConfigureModel::set_reply_count(int reply_count) {
  reply_count_ = reply_count;
}
string ConfigureModel::social_title() {
  return social_title_;
}
void ConfigureModel::set_social_title(string social_title) {
  social_title_ = social_title;
}
int ConfigureModel::app_id() {
  return app_id_;
}
void ConfigureModel::set_app_id(int app_id) {
  app_id_ = app_id;
}

int ConfigureModel::display_friend_type() {
  return display_friend_type_;
}

void ConfigureModel::set_display_friend_type(int display_friend_type) {
  display_friend_type_ = display_friend_type;
}
string ConfigureModel::like_page_link() {
  return like_page_link_;
}
void ConfigureModel::set_like_page_link(string like_page_link) {
  like_page_link_ = like_page_link;
}
string ConfigureModel::like_name() {
  return like_name_;
}
void ConfigureModel::set_like_name(string like_name) {
  like_name_ = like_name;
}
std::string ConfigureModel::ToString() {
  std::ostringstream out;
  out << "edm_id:" << id_ << ",type:" << type_<< ",ad_type:" << ad_type_<< ",limit_type:" << limit_type_<< ",interactive_flag:" << interactive_flag_<< ",displayzzs:" << displayzzs_<< ",demand:" << demand_<< ",max_per_hour:" << max_per_hour_<< ",name:" << name_<< ",status:" << status_<< ",begin_time:" << begin_time_<< ",create_ime:" << create_ime_<< ",update_ime:" << update_ime_<< ",title:" << title_<< ",title_link:" << title_link_<< ",body:" << body_<< ",body_link:" << body_link_<< ",logo:" << logo_<< ",media:" << media_<< ",customer_id:" << customer_id_<< ",consume:" << consume_<< ",click:" << click_<< ",expose:" << expose_<< ",ilike:" << ilike_<< ",share:" << share_<< ",pub_type:" << pub_type_<< ",time_interval:" << time_interval_<< ",feed_clazz:" << feed_clazz_<< ",reply_count:" << reply_count_ << ",social_title:" << social_title_ << ",app_id:" << app_id_ << ",display_friend_type:" << display_friend_type_;
  return out.str();
}

 Ice::Int OrientMarryInfo::id() { 
  return id_;
}
 void OrientMarryInfo::set_id(Ice::Int id) { 
  id_ = id;
}
 Ice::Int OrientMarryInfo::edm_id() { 
  return edm_id_;
}
 void OrientMarryInfo::set_edm_id(Ice::Int edm_id) { 
  edm_id_ = edm_id;
}
 int OrientMarryInfo::gender() { 
  return gender_;
}
 void OrientMarryInfo::set_gender(int gender) { 
  gender_ = gender;
}
 int OrientMarryInfo::age() { 
  return age_;
}
 void OrientMarryInfo::set_age(int age) { 
  age_ = age;
}
 int OrientMarryInfo::age_start() { 
  return age_start_;
}
 void OrientMarryInfo::set_age_start(int age_start) { 
  age_start_ = age_start;
}
 int OrientMarryInfo::age_end() { 
  return age_end_;
}
 void OrientMarryInfo::set_age_end(int age_end) { 
  age_end_ = age_end;
}
 int OrientMarryInfo::stage() {
  return stage_;
}
 void OrientMarryInfo::set_stage(int stage) { 
  stage_ = stage;
}
 int OrientMarryInfo::area() {
  return area_;
}
 void OrientMarryInfo::set_area(int area) {
  area_ = area;
}
 int OrientMarryInfo::special() {
  return special_;
}
 void OrientMarryInfo::set_special(int special) {
  special_ = special;
}
 std::string OrientMarryInfo::city() {
  return city_;
}
 void OrientMarryInfo::set_city(std::string city) {
  city_ = city;
}
 std::string OrientMarryInfo::school() {
  return school_;
}
 void OrientMarryInfo::set_school(std::string school) {
  school_ = school;
}
 std::vector<std::string> OrientMarryInfo::province_area() {
  return province_area_;
}
 void OrientMarryInfo::set_province_area(std::vector<std::string>& province_area) {
  province_area_ = province_area;
}
 std::vector<std::string> OrientMarryInfo::province_city() {
  return province_city_;
}
 void OrientMarryInfo::set_province_city(std::vector<std::string>& province_city) {
  province_city_ = province_city;
}
 std::vector<School> OrientMarryInfo::univ() {
  return univ_;
}
 void OrientMarryInfo::set_univ(std::vector<School>& univ) {
  univ_ = univ;
}
 std::vector<School> OrientMarryInfo::hight() {
  return hight_;
}
 void OrientMarryInfo::set_hight(std::vector<School>& hight) {
  hight_ = hight;
}

std::string  OrientMarryInfo::ToString() {
  std::ostringstream out;
  out << "edm_id:" << edm_id_ << ",gender:" << gender_ << ",age:" << age_ << ",age_start:" << age_start_ << ",age_end:" << age_end_  << \
      ",stage:" << stage_ << ",area:" << area_ << ",special:" << special_ << ",city:" << city_ << ",school:" << school_;
  return out.str();
}

bool AssignMarryInfo::Lookup(Ice::Int user_id) {
  std::set<Ice::Int>::iterator ite = assign_user_.find(user_id);
  return  ite != assign_user_.end();
}
bool AssignMarryInfo::Insert(Ice::Int user_id) {
  std::pair<std::set<Ice::Int>::iterator, bool> result = assign_user_.insert(user_id);
  return result.second;
}
bool AssignMarryInfo::Remove(Ice::Int user_id) {
  return assign_user_.erase(user_id);
}
bool AssignMarryInfo::Swap(std::set<Ice::Int>& other_set) {
  size_t  num = other_set.size();
  assign_user_.swap(other_set);
  return num == assign_user_.size();
}

FeedEdmModel::FeedEdmModel(ConfigureModelPtr configure_model_ptr)
    : configure_model_ptr_(configure_model_ptr){
}
void FeedEdmModel::AfterSender(Ice::Int user_id) {
  MiniteCache::instance().Insert(configure_model_ptr_->id());
  configure_model_ptr_->set_consume(configure_model_ptr_->consume()+1);
}
bool FeedEdmModel::_BasicValidate(const FeedUserInfoPtr& user) {
  //MCE_INFO("model limit_type:" << configure_model_ptr_->limit_type() << ",user limit_type:" << user->limitCheck);
  if (configure_model_ptr_->limit_type() != user->limitCheck) {
		//MCE_INFO("FeedEdmModel::_BasicValidate --> userid = " << user->userId << " user->limitCheck = " << user->limitCheck << " configure->id = " << configure_model_ptr_->id() << " configure->type = " << configure_model_ptr_->limit_type());
    return false;
  }
  int send_times_in_minite = MiniteCache::instance().GetSendTimes(configure_model_ptr_->id());
  MCE_DEBUG("send time of edm:" << configure_model_ptr_->id() << "=" << send_times_in_minite);
  MCE_DEBUG("max_per_hour of edm:" << configure_model_ptr_->id() << "=" << configure_model_ptr_->max_per_hour());
  if (send_times_in_minite > configure_model_ptr_->max_per_hour()/60) {
    return false;
  } 
  if (configure_model_ptr_->consume() >= configure_model_ptr_->demand()) {
    return false;
  }
  if(configure_model_ptr_->time_interval().length() < 24) {
    return false;
  }
  time_t now = time(NULL);
  MCE_DEBUG("now:" << now << ",begin_time:" << configure_model_ptr_->begin_time());
  if (configure_model_ptr_->begin_time() > now) {
    return false;
  }
  tm now_tm;
  localtime_r(&now,&now_tm);
  char flag = configure_model_ptr_->time_interval().at(now_tm.tm_hour);
  if (flag != '1') {
    return false;
  }
  return true;
}
FeedNullTypeEdmModel::FeedNullTypeEdmModel(ConfigureModelPtr configure_model_ptr, NullTypeMarryInfoPtr marry_info_ptr)
    : FeedEdmModel(configure_model_ptr), marry_info_ptr_(marry_info_ptr) {
}

bool FeedNullTypeEdmModel::LoadMarryInfo() {
  MCE_INFO("load null type edm:" << this->id());
  return true;
}
FeedEdmModelPtr FeedNullTypeEdmModel::ValidateFunc(const FeedUserInfoPtr& user) {
  if (!_BasicValidate(user)) {
    MCE_DEBUG("null type validate failed");
    return 0;
  }
  return this;
}
std::string FeedNullTypeEdmModel::ToString() {
  return configure_model_ptr_->ToString();
}
FeedOrientEdmModel::FeedOrientEdmModel(ConfigureModelPtr configure_model_ptr, OrientMarryInfoPtr marry_info_ptr) 
    : FeedEdmModel(configure_model_ptr), marry_info_ptr_(marry_info_ptr) {
}
bool FeedOrientEdmModel::LoadMarryInfo() {
  com::xiaonei::xce::Statement sql;
  mysqlpp::StoreQueryResult res;
  sql << "select * from " << kOrientTableName <<" where edm_id =" << configure_model_ptr_->id();
  try {
    res =  com::xiaonei::xce::QueryRunner("feed_edm", com::xiaonei::xce::CDbRServer,kOrientTableName).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("get marry info of edm:" <<configure_model_ptr_->id() << " from " << kOrientTableName << " failed, err->" << e);
    return false;
  }
  if (res.num_rows() < 1) {
    MCE_WARN("couldn't find orient model of edm << " << configure_model_ptr_->id());
    return false;
  }
  mysqlpp::Row row = res.at(0);
  bool result = _FormatOrientMarryInfo(row);
  if (!result) {
    MCE_WARN("format orient marry info failed,edm_id:"<< configure_model_ptr_->id());
    marry_info_ptr_ = 0;
    return false;
  }
  MCE_INFO("load orient edm:" << configure_model_ptr_->id() << " marry info;"); 
  return true;
}
std::string FeedOrientEdmModel::ToString() {
  return configure_model_ptr_->ToString() + marry_info_ptr_->ToString();
}
std::string FeedOrientEdmModel::_NormalizeProviceName(const std::string& province) {
  if (province == "北京" || province == "上海" || province == "天津" || province == "重庆") {
    return province + "市";
  } else if (province == "香港" || province == "澳门") {
    return province + "特别行政区";
  } else if (province == "内蒙古" || province == "西藏") {
    return province + "自治区";
  } else if (province == "广西") {
    return province + "壮族自治区";
  } else if (province == "新疆") {
    return province + "维吾尔自治区";
  } else if (province == "宁夏") {
    return province + "回族自治区";
  } else if (province == "美国" || province == "加拿大") {
    return province;
  } else {
    return province + "省";
  }
  return "其他地区";
}
bool FeedOrientEdmModel::_FormatOrientMarryInfo(mysqlpp::Row& row) {
  OrientMarryInfoPtr marry_info_ptr = new OrientMarryInfo; 
  if (marry_info_ptr == 0) {
    return false;
  }
  Json::Reader reader;
  Json::Value json_value_city;
  Json::Value json_value_school;
  marry_info_ptr->set_id(row["id"]);
  marry_info_ptr->set_edm_id(row["edm_id"]);
  marry_info_ptr->set_gender(row["gender"]);
  marry_info_ptr->set_age(row["age"]);
  marry_info_ptr->set_age_start(row["age_start"]);
  marry_info_ptr->set_age_end(row["age_end"]);
  marry_info_ptr->set_stage(row["stage"]);
  marry_info_ptr->set_area(row["area"]);
  marry_info_ptr->set_special(row["special"]);
  marry_info_ptr->set_city((std::string)row["city"]);
  if (marry_info_ptr->city() == "NULL") {
    marry_info_ptr->set_city("");
  }
  marry_info_ptr->set_school((std::string)row["school"]);
  if (marry_info_ptr->school() == "NULL") {
    marry_info_ptr->set_school("");
  }
  // parse area info
  if (marry_info_ptr->city() != "") {
    bool parsingSuccessful = reader.parse(marry_info_ptr->city(), json_value_city);
    if ( !parsingSuccessful )
    {
      MCE_WARN("Parse city failed,edm_id:" << configure_model_ptr_->id() << ",city_str: " << marry_info_ptr->city());
      marry_info_ptr->set_city("");
    } else {
      Json::Value province = json_value_city["provice"];
      std::vector<std::string> province_area;
      for ( size_t index = 0; index < province.size(); ++index ) {
       province_area.push_back(province[index].asString());
      }
      marry_info_ptr->set_province_area(province_area);
      std::vector<std::string> province_city;
      Json::Value city = json_value_city["city"];
      for ( size_t index = 0; index < city.size(); ++index ) {
        if (city[index].asString() != "" && city[index].asString() != "NULL"){
          province_city.push_back(city[index].asString());
        } else {
          if (index >= province_area.size()) {
            MCE_WARN("city num > province num,edm id:" << marry_info_ptr->id());
          } else if (province_area.at(index) == "") {
            MCE_WARN("province name is null!edm id:" << marry_info_ptr->id());
          } else {
            province_city.push_back(_NormalizeProviceName(province_area.at(index)));
          }
        }
      }
      marry_info_ptr->set_province_city(province_city);
    } 
  }
  // parse school info
  if ( marry_info_ptr->school() != "") {
    bool parsingSuccessful = reader.parse(marry_info_ptr->school(), json_value_school);
    if ( !parsingSuccessful )
    {
      MCE_WARN("Parse school failed,edm_id:" << configure_model_ptr_->id() << ",school_str:" << marry_info_ptr->school());
      marry_info_ptr->set_school("");
    } else {
      Json::Value univ = json_value_school["univ"];
      std::vector<School> school_info_seq;
      for (size_t index = 0; index < univ.size(); ++index ) {
        if (univ[index] != "") {
          School school_info;
          school_info.id_ = univ[index]["id"].asString();
          school_info.name_ = univ[index]["name"].asString();
          school_info.year_ = univ[index]["year"].asString();
          MCE_DEBUG("Load school info,id:" << school_info.id_ << ",year:" << school_info.year_);
          school_info_seq.push_back(school_info);
        }
      }
      marry_info_ptr->set_univ(school_info_seq);
      school_info_seq.clear();
      Json::Value hight = json_value_school["hight"];
      for ( size_t index = 0; index < hight.size(); ++index ) {
        if (hight[index] != "") {
          School school_info;
          school_info.id_ = hight[index]["id"].asString();
          school_info.name_ = hight[index]["name"].asString();
          school_info.year_ = hight[index]["year"].asString();
          MCE_DEBUG("Load hight info,id:" << school_info.id_ << ",year:" << school_info.year_);
          school_info_seq.push_back(school_info);
        }
      } 
      marry_info_ptr->set_hight(school_info_seq);
    }
  }
  marry_info_ptr_ = marry_info_ptr; 
  return true;
}
FeedEdmModelPtr FeedOrientEdmModel::ValidateFunc(const FeedUserInfoPtr& user) {
  if (!_BasicValidate(user)) {
    MCE_DEBUG("didn't pass basic validate ,edm_id:" << this->id());
    return 0;
  }
  if (marry_info_ptr_ == 0) {
    return 0;
  }
  if (!_InfoValidate(user)) {
    return 0;
  }
  return this; 
}
bool FeedOrientEdmModel::_GenderValidate(const FeedUserInfoPtr& user){
  if (marry_info_ptr_->gender() == -1) {
    return true;
  }
  MCE_DEBUG("user gender:" << user->gender << ",model gender:" << marry_info_ptr_->gender());
  if ((user->gender !=-1)&&(user->gender != marry_info_ptr_->gender())) {
    return false;
  }
  return true;  

}
bool FeedOrientEdmModel::_AgeValidate(const FeedUserInfoPtr& user){
  if (marry_info_ptr_->age() == -1) {
    return true;
  }
  time_t now = time(NULL);
  tm now_tm;
  localtime_r(&now,&now_tm);
  MCE_DEBUG("user age:" << user->age << ",model age start from:" <<  marry_info_ptr_->age_start() << ",to:" << marry_info_ptr_->age_end());
  int age_start = marry_info_ptr_->age_start();
  int age_end = marry_info_ptr_->age_end();
  if (age_start == age_end && age_end == 0) {
    return true;
  }
  if (age_start < 0 || age_end < 0 || age_start > age_end || user->age < age_start || user->age > age_end) {
    return false;
  }
  return true;
}
bool FeedOrientEdmModel::_StageValidate(const FeedUserInfoPtr& user) {
 if (marry_info_ptr_->stage() == -1) {
    return true;
  }
  MCE_DEBUG("user stage:" << user->stage << ",model stage:" << marry_info_ptr_->stage());
  return user->stage == marry_info_ptr_->stage();
}
bool FeedOrientEdmModel::_SchoolValidate(const FeedUserInfoPtr& user){
  if (marry_info_ptr_->special() == -1) {
    return true;
  }
  if (marry_info_ptr_->univ().size() == 0 && marry_info_ptr_->hight().size() == 0) {
    return true;
  }
  for (std::vector<xce::edm::FeedSchoolInfo>::const_iterator ite = user->universitySeq.begin(); ite != user->universitySeq.end(); ++ite) {
    std::vector<School> univ = marry_info_ptr_->univ();
    for (std::vector<School>::iterator ite_orient = univ.begin(); ite_orient != univ.end(); ++ite_orient) {
      if ((boost::lexical_cast<int>(ite_orient->id_) == 0||boost::lexical_cast<int>(ite_orient->id_) == ite->id) && (boost::lexical_cast<int>(ite_orient->year_) == 0 || 
              boost::lexical_cast<int>(ite_orient->year_) == ite->enrollYear)) {
        return true;
      }
    }
  }
  for (std::vector<xce::edm::FeedSchoolInfo>::const_iterator ite = user->highschoolSeq.begin(); ite != user->highschoolSeq.end(); ++ite) {
    std::vector<School> hight = marry_info_ptr_->hight();
    for (std::vector<School>::iterator ite_orient = hight.begin(); ite_orient != hight.end(); ++ite_orient) {
      if ((boost::lexical_cast<int>(ite_orient->id_) == 0 || boost::lexical_cast<int>(ite_orient->id_) == ite->id) && (boost::lexical_cast<int>(ite_orient->year_) == 0 || 
              boost::lexical_cast<int>(ite_orient->year_) == ite->enrollYear)) {
        return true;
      }
    }
  }
  return false;
}
bool FeedOrientEdmModel::_CityByIPValidate(const FeedUserInfoPtr& user){
  if (marry_info_ptr_->area() == -1) {
    return true;
  }
  string city = user->city;
  string province = user->province;
  std::vector<std::string> orient_city_seq = marry_info_ptr_->province_city();
  for (std::vector<std::string>::iterator ite= orient_city_seq.begin(); ite != orient_city_seq.end(); ++ite) {
    if (user->userId == 987915272) {
      MCE_INFO("user city:" << city << ",province:" << province << ",orient city:" << *ite );
    }
    if ((*ite == city) || (*ite == province)) {
      return true;
    }  
  }
  return false;

}
bool FeedOrientEdmModel::_InfoValidate(const FeedUserInfoPtr& user){
  bool validate_result = _GenderValidate(user);
  if (!validate_result) {
    return false;
  }
  MCE_DEBUG("gender pass");
  validate_result = _AgeValidate(user);
  if (!validate_result) {
    return false;
  }
  MCE_DEBUG("age pass");
  validate_result = _StageValidate(user);
  if (!validate_result) {
    return false;
  }
  MCE_DEBUG("stage pass");
  validate_result = _SchoolValidate(user);
  if (!validate_result) {
    return false;
  }
  MCE_DEBUG("school pass");
  validate_result = _CityByIPValidate(user);
  if (!validate_result) {
    return false;
  }
  MCE_DEBUG("city pass");
  return true;

}


FeedAssignEdmModel::FeedAssignEdmModel(ConfigureModelPtr configure_model_ptr, AssignMarryInfoPtr marry_info_ptr) 
    : FeedEdmModel(configure_model_ptr), marry_info_ptr_(marry_info_ptr) { 
}
bool FeedAssignEdmModel::LoadMarryInfo() {
  com::xiaonei::xce::Statement sql;
  mysqlpp::StoreQueryResult res;
  sql << "select userid from " << kAssignTableName <<" where config_id =" << configure_model_ptr_->id() << " and sended_flag = 0";
  try {
    res =  com::xiaonei::xce::QueryRunner("feed_edm", com::xiaonei::xce::CDbRServer,kAssignTableName).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("get marry  info of edm:" << configure_model_ptr_->id() << " from " << kAssignTableName << " failed, err->" << e);
    return false;
  }
  if (res.num_rows() < 1) {
    MCE_WARN("couldn't find marry info model of edm:" << configure_model_ptr_->id());
    return false;
  }
  marry_info_ptr_ = new AssignMarryInfo();
  for(unsigned i = 0; i < res.num_rows(); ++i) {
    mysqlpp::Row row = res.at(i);
    marry_info_ptr_->Insert(row["userid"]);
  }
  MCE_INFO("load assign edm:" << configure_model_ptr_->id() << " marry info");
  return true;
}
std::string FeedAssignEdmModel::ToString() {
  return configure_model_ptr_->ToString();
}
void FeedAssignEdmModel::AfterSender(Ice::Int user_id) {
  MiniteCache::instance().Insert(configure_model_ptr_->id());
  if (marry_info_ptr_ != 0) {
    marry_info_ptr_->Remove(user_id);
  }
  DbMarryInfoOperatorPtr marry_info_op_ptr = new DbMarryInfoOperator(configure_model_ptr_->id(), user_id);
  MyUtil::TaskManager::instance().execute(marry_info_op_ptr);
  configure_model_ptr_->set_consume(configure_model_ptr_->consume() +1);
}
void FeedAssignEdmModel::DbMarryInfoOperator::handle() {
  com::xiaonei::xce::Statement sql;
  sql << "delete from " << kAssignTableName <<" where config_id = " << edm_id_ << " and userid = " << user_id_;
  try {
    com::xiaonei::xce::QueryRunner("feed_edm", com::xiaonei::xce::CDbWServer,kAssignTableName).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("update assign info of edm:" << edm_id_ << " from " << kAssignTableName << " failed, err->" << e);
  }
}
FeedEdmModelPtr FeedAssignEdmModel::ValidateFunc(const FeedUserInfoPtr& user) {
  if (!_BasicValidate(user)) {
    return 0;
  }
  if (marry_info_ptr_ == 0) {
    return 0;
  }
  if (!marry_info_ptr_->Lookup(user->userId)) {
    return 0;
  }
  MCE_DEBUG("Validate pass,user id:" << user->userId << ",edm id:" << this->id());
  return this;
}

}
}



