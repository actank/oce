#include "AdUserCacheLoaderI.h"
#include <boost/lexical_cast.hpp>
#include "ServiceI.h"

using namespace std;
using namespace IceUtil;
using namespace xce::ad;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::feed;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&AdUserCacheLoaderI::instance(),
      service.createIdentity("M", ""));
  int reset_interval_ =
      service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault(
          "Service." + ServiceI::instance().getName()
              + ".USERAREADATA_RESET_TIME_INTERVAL", 3600*24*1000);
  TaskManager::instance().scheduleRepeated(new UserAreaDataResetTimer(
      reset_interval_));
}

void UserDataLoader::query(){
  for (size_t i = 0; i < taskseq_.size(); ++i) {
    MyUtil::TaskManager::instance().execute(taskseq_.at(i));
  }
  taskseq_.clear();
  IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
  if (!timedWait(IceUtil::Time::milliSeconds(LOAD_TIMEOUT))) {
    MCE_WARN("UserDataLoader::query -> count: " << count_ << " missing");
  } else {
    MCE_INFO("UserDataLoader::query -> success");
  }
}

void UserDataLoader::addQuery(const LoaderHandlerPtr& handler) {
  SqlExecuteTask* t = new SqlExecuteTask(this, handler);
  taskseq_.push_back(t);
  ++count_;
}


void UserDataLoader::SqlExecuteTask::handle() {
  handler_->handle();
  loader_->finish();
}

void UserBornLoaderHandlerI::handle(){
  mysqlpp::StoreQueryResult res = QueryRunner(DB_USER_BORN, CDbRServer).store(sql_);
  if (res && res.num_rows() > 0) {
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      int id = (int) row["id"];
      UserProfileWrapperMap::const_iterator it = result_.find(id);
      string gender = row["gender"].data();
      string home_city = row["home_city"].data();
      int birth_year = (int)row["birth_year"];
      int birth_month = (int)row["birth_month"];
      int birth_day = (int)row["birth_day"];
      MCE_INFO("UserBornLoaderHandlerI::handle --> userid = " << id << " gender = " << gender << " home_city = " << home_city << " birth_year = " << birth_year << " birth_month = " << birth_month << " birth_day = " << birth_day);
      (it->second)->SetUserBornProperties(gender, birth_year, birth_month, birth_day, home_city);
    }
  }
  MCE_DEBUG("UserBornLoaderHandlerI::handle --> UserProfileWrapperPtr -> SetUserBornProperties");
}

void UserStageLoaderHandlerI::handle(){
  mysqlpp::StoreQueryResult res = QueryRunner(DB_USER_STAGE, CDbRServer).store(sql_);
  if (res && res.num_rows() > 0) {
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      int id = (int) row["id"];
      UserProfileWrapperMap::const_iterator it = result_.find(id);
      int stage = (int)row["stage"];
      MCE_INFO("UserStageLoaderHandlerI::handle --> userid = " << id << " stage = " << stage);
      (it->second)->SetUserStageProperties(stage);
    }
  }
  MCE_DEBUG("UserStageLoaderHandlerI::handle --> UserProfileWrapperPtr -> SetUserStageProperties");
}
void UniversityLoaderHandlerI::handle(){
  mysqlpp::StoreQueryResult res = QueryRunner(DB_UNIVERSITY_INFO, CDbRServer).store(sql_);
  if (res && res.num_rows() > 0) {
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      int id = (int) row["userid"];
      UserProfileWrapperMap::const_iterator it = result_.find(id);
      Ice::Long univ_id = (Ice::Long)row["university_id"];
      string type_of_course = row["type_of_course"].data();
      int enroll_year = (int)row["enroll_year"];
      MCE_INFO("UniversityLoaderHandlerI::handle --> userid = " << id << " univ_id = " << univ_id << " enroll_year = " << enroll_year << " type_of_course = " << type_of_course);
      (it->second)->SetUniversityProperties(univ_id, type_of_course,enroll_year);
    }
  }
  MCE_DEBUG("UniversityLoaderHandlerI::handle --> UserProfileWrapperPtr -> SetUniversityProperties");
}
void WorkplaceLoaderHandlerI::handle(){
  mysqlpp::StoreQueryResult res = QueryRunner(DB_WORKPLACE_INFO, CDbRServer).store(sql_);
  if (res && res.num_rows() > 0) {
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      int id = (int) row["userid"];
      UserProfileWrapperMap::const_iterator it = result_.find(id);
      string city_id = row["city_id"].data();
      MCE_INFO("WorkplaceLoaderHandlerI::handle --> userid = " << id << " city_id = " << city_id);
      (it->second)->SetWorkPlaceProperties(city_id);
    }
  }
  MCE_DEBUG("WorkplaceLoaderHandlerI::handle --> UserProfileWrapperPtr -> SetWorkPlaceProperties");
}
void JuniorLoaderHandlerI::handle(){
  mysqlpp::StoreQueryResult res = QueryRunner(DB_JUNIOR_HIGH_SCHOOL_INFO, CDbRServer).store(sql_);
  if (res && res.num_rows() > 0) {
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      int id = (int) row["userid"];
      UserProfileWrapperMap::const_iterator it = result_.find(id);
      Ice::Long junior_high_school_id = (Ice::Long)row["junior_high_school_id"];
      int junior_high_school_year = (int)row["junior_high_school_year"];
      MCE_INFO("JuniorLoaderHandlerI::handle --> userid = " << id << " junior_high_school_id = " << junior_high_school_id << " junior_high_school_year = " << junior_high_school_year);
      (it->second)->SetJuniorProperties(junior_high_school_id, junior_high_school_year);
    }
  }
  MCE_DEBUG("JuniorLoaderHandlerI::handle --> UserProfileWrapperPtr -> SetJuniorProperties");
}
void HighSchoolLoadertHandlerI::handle(){
  mysqlpp::StoreQueryResult res = QueryRunner(DB_HIGH_SCHOOL_INFO, CDbRServer).store(sql_);
  if (res && res.num_rows() > 0) {
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      int id = (int) row["userid"];
      UserProfileWrapperMap::const_iterator it = result_.find(id);
      Ice::Long high_school_id = (Ice::Long)row["high_school_id"];
      int enroll_year = (int)row["enroll_year"];
      MCE_INFO("JuniorLoaderHandlerI::handle --> userid = " << id << " high_school_id = " << high_school_id << " high_school_enroll_year = " << enroll_year);
      (it->second)->SetHighSchoolProperties(high_school_id, enroll_year);
    }
  }
  MCE_DEBUG("HighSchoolLoadertHandlerI::handle --> UserProfileWrapperPtr -> SetHighSchoolProperties");
}


AdUserCacheLoaderI::AdUserCacheLoaderI() {
  MCE_DEBUG("AdUserCacheLoaderI::AdUserCacheLoaderI --> CALL InitUserAreaData()");
  InitUserAreaData();
}

void AdUserCacheLoaderI::LoadByIdSeq(const MyUtil::IntSeq& ids,
    const Ice::Current& current) {
  MCE_INFO("AdUserCacheLoaderI::LoadByIdSeq --> ids.size() = " << ids.size());
  if(!ids.empty()){
    TaskManager::instance().execute(new LoadTask(ids));
  }
}

void AdUserCacheLoaderI::LoadById(Ice::Int userId, const Ice::Current& current){
  MCE_INFO("AdUserCacheLoaderI::LoadByUserId : userId = " << userId);
  MyUtil::IntSeq ids;
  ids.push_back(userId);
  if(!ids.empty()){
    MCE_DEBUG("AdUserCacheLoaderI::LoadByUserId --> userId = " << userId <<" missids run LoadTask ");
    TaskManager::instance().execute(new LoadTask(ids));
  }
}

string AdUserCacheLoaderI::GetById(Ice::Int userId, const Ice::Current& current){
  MCE_INFO("AdUserCacheLoaderI::GetById --> userId = " << userId);
  MyUtil::IntSeq ids;
  ids.push_back(userId);
  LoadTask load(ids);
  load.handle();
  vector<string> obj = load.GetSerialize();
  return *(obj.begin());
}

void AdUserCacheLoaderI::ReloadUserInfoByIdSeq(const MyUtil::IntSeq& ids, const Ice::Current& current){
  MCE_INFO("AdUserCacheLoaderI::ReloadUserInfoByIdSeq : size = " << ids.size());
  if(!ids.empty()){
    TaskManager::instance().execute(new LoadTask(ids, true));
  }
}

void AdUserCacheLoaderI::ReloadUserInfoById(Ice::Int userId, const Ice::Current& current){
  MCE_INFO("AdUserCacheLoaderI::ReloadUserInfoById  userid = " << userId);
  MyUtil::IntSeq ids;
  ids.push_back(userId);
  TaskManager::instance().execute(new LoadTask(ids, true));
}


UserProfileWrapperMap AdUserCacheLoaderI::CreateUserInfo(
    const MyUtil::IntSeq& ids) {
  MCE_INFO("AdUserCacheLoaderI::CreateUserInfo --> size : " << ids.size());
  UserProfileWrapperMap results;
  ostringstream oss;
  oss << "(";
  for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
    if (it != ids.begin()) {
      oss << ", ";
    }
    oss << *it;
    UserProfileWrapperPtr obj = new UserProfileWrapper;
    obj->SetUserId(*it);
    results[*it] = obj;
  }
  oss << ")";
  MCE_DEBUG("start load -->  time = " << MyUtil::Date(time(NULL)).minute() << " : " << MyUtil::Date(time(NULL)).second());
  UserDataLoaderPtr loader = new UserDataLoader();
  LoaderHandlerPtr born_handler = new UserBornLoaderHandlerI(oss.str(), results);
  LoaderHandlerPtr stage_handler = new UserStageLoaderHandlerI(oss.str(), results);
  LoaderHandlerPtr univ_handler = new UniversityLoaderHandlerI(oss.str(), results);
  LoaderHandlerPtr workplace_handler = new WorkplaceLoaderHandlerI(oss.str(), results);
  LoaderHandlerPtr junior_handler = new JuniorLoaderHandlerI(oss.str(), results);
  LoaderHandlerPtr highschool_handler = new HighSchoolLoadertHandlerI(oss.str(), results);
  loader->addQuery(born_handler);
  loader->addQuery(stage_handler);
  loader->addQuery(univ_handler);
  loader->addQuery(workplace_handler);
  loader->addQuery(junior_handler);
  loader->addQuery(highschool_handler);
  loader->query();
  MCE_DEBUG("end load -->  time = " << MyUtil::Date(time(NULL)).minute() << " : " << MyUtil::Date(time(NULL)).second());
  return results;
}


void AdUserCacheLoaderI::InitUserAreaData() {
  TaskManager::instance().execute(new InitUserAreaDataTask());
}

void AdUserCacheLoaderI::SetUserAreaData(){
    MCE_DEBUG("AdUserCacheLoaderI::SetUserAreaData : ");
    com::xiaonei::xce::Statement sql, sql2;
    ::std::set<int> region_id_set;
    ::std::map<int, vector<Ice::Long> > region2univmap;
    ::std::map<int, string> region2areamap;

    AdUserAreaCacheMap user_area_cache_;
    ::std::map<string, string> user_city_cache_;
    ::std::map<Ice::Long, string> user_highschool_cache_;
    ::std::map<Ice::Long, string> user_juniorschool_cache_;

    sql << "select id, region_id from university;";
    mysqlpp::StoreQueryResult res = QueryRunner(DB_UNIVERSITY, CDbRServer).store(
        sql);
    if (res && res.num_rows() > 0) {
      ostringstream oss;
      bool first = true;
      oss << "(";
      for (size_t i = 0; i < res.num_rows(); ++i) {
        AdUserAreaCachePtr obj = new AdUserAreaCache;
        mysqlpp::Row row = res.at(i);
        Ice::Long universityid = (Ice::Long) row["id"];
        int regionid = (int) row["region_id"];
        ::std::map<int, vector<Ice::Long> >::iterator r2uit = region2univmap.find(regionid);
        if(r2uit == region2univmap.end()){
          MyUtil::LongSeq unividseq;
          region2univmap[regionid] = unividseq;
        }
        region2univmap[regionid].push_back(universityid);
        obj->region_id_ = regionid;
        obj->univ_id_ = universityid;
        user_area_cache_[universityid] = obj;
        //MCE_DEBUG("universityid = " << universityid << "region_id = " << regionid);
        ::std::set<int>::iterator it = region_id_set.find(regionid);
        if (it == region_id_set.end()) {
          if (first) {
            first = false;
          } else {
            oss << ",";
          }
          region_id_set.insert(regionid);
          oss << regionid;
        }
      }
      oss << ");";
      sql2
          << "select region_id, area_code from ad_china_area where region_id in "
          << oss.str();
      res = QueryRunner(DB_AD_CHINA_AREA, CDbRServer).store(sql2);
      if (res && res.num_rows() > 0) {
        for (size_t i = 0; i < res.num_rows(); ++i) {
          mysqlpp::Row row = res.at(i);
          string areacode = row["area_code"].data();
          int regionid = (int) row["region_id"];
          region2areamap[regionid] = areacode;
          ::std::map<int, vector<Ice::Long> >::iterator rit = region2univmap.find(
              regionid);
          MCE_DEBUG("region_id = " << regionid << "  :  ------------->");
          if (rit != region2univmap.end()) {
            for (vector<Ice::Long>::const_iterator tit = rit->second.begin(); tit
                != rit->second.end(); ++tit) {
              //MCE_DEBUG("university_id = " << *it << "  area_code = " << areacode);
              user_area_cache_[*tit]->area_code_ = areacode;
            }
          }
        }
      }
    }

    //load region table for city_name2region_id map;
    sql.clear();
    sql <<"select id, name from region;";
    res = QueryRunner(DB_UNIVERSITY, CDbRServer).store(
          sql);
    if (res && res.num_rows() > 0) {
      for (size_t i = 0; i < res.num_rows(); ++i){
        mysqlpp::Row row = res.at(i);
        string city_name = row["name"].data();
        int region_id = (int)row["id"];
        ::std::map<int, string> :: iterator r2ait = region2areamap.find(region_id);
        if(r2ait != region2areamap.end()){
          user_city_cache_[city_name] = r2ait->second;
          //MCE_DEBUG("user_city_cache_ :  city_name = " << city_name << "   area_code = " << r2ait->second);
        }
      }
    }

    //load user_high_data_ for highschoolid2areacode map;
    sql.clear();
    sql <<"select id, code from high_school where code != '';";
    res = QueryRunner(DB_UNIVERSITY, CDbRServer).store(
          sql);
    if (res && res.num_rows() > 0) {
      for (size_t i = 0; i < res.num_rows(); ++i){
        mysqlpp::Row row = res.at(i);
        Ice::Long high_school_id = (Ice::Long)row["id"];
        string area_code = row["code"].data();
        user_highschool_cache_[high_school_id] = area_code;
        //MCE_DEBUG("user_highschool_cache_ :  high_school_id = " << high_school_id << "   area_code = " << area_code);
      }
    }
    //load user_junior_data_ for juniorschoolid2areacode map;
    sql.clear();
    sql <<"select id, code from junior_school where code != '';";
    res = QueryRunner(DB_UNIVERSITY, CDbRServer).store(
          sql);
    if (res && res.num_rows() > 0) {
      for (size_t i = 0; i < res.num_rows(); ++i){
        mysqlpp::Row row = res.at(i);
        Ice::Long junior_school_id = (Ice::Long)row["id"];
        string area_code = row["code"].data();
        user_juniorschool_cache_[junior_school_id] = area_code;
        //MCE_DEBUG("user_juniorschool_cache_ :  junior_school_id = " << junior_school_id << "   area_code = " << area_code);
      }
    }
    // load user_college_data_ for collegeschoolid2areacode map;
    {
      IceUtil::RWRecMutex::WLock lock(mutex_);
      user_area_data_.swap(user_area_cache_);
      user_city_data_.swap(user_city_cache_);
      user_highschool_data_.swap(user_highschool_cache_);
      user_juniorschool_data_.swap(user_juniorschool_cache_);
    }
}

string AdUserCacheLoaderI::GetAreacodeFromUserAreaCache(Ice::Long universityid) {
  MCE_INFO("AdUserCacheLoaderI::GetAreacodeFromUserAreaCache --> universityid = " << universityid);
  {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    AdUserAreaCacheMap::iterator it = user_area_data_.find(universityid);
    if (it != user_area_data_.end())
      return it->second->area_code_;
    else {
      MCE_WARN("AdUserCacheLoaderI::GetAreacodeIdFromUserAreaCache --> miss universityid = " << universityid);
    }
  }
  return "";
}

string AdUserCacheLoaderI::GetAreacodeFromUserCityCache(const string& city_name){
  MCE_INFO("AdUserCacheLoaderI::GetRegionIdFromUserCityCache --> city_name = " << city_name);
  {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    ::std::map<string, string>::iterator it = user_city_data_.find(city_name);
    if (it != user_city_data_.end())
      return it->second;
    else {
      MCE_WARN("AdUserCacheLoaderI::GetRegionIdFromUserCityCache --> miss city_name = " << city_name);
    }
  }
  return "";
}

string AdUserCacheLoaderI::GetAreacodeFromUserJuniorCache(Ice::Long juniorschool_id){
  MCE_INFO("AdUserCacheLoaderI::GetAreacodeFromUserJuniorCache --> juniorschool_id = " << juniorschool_id);
  {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    ::std::map<Ice::Long, string>::iterator it = user_juniorschool_data_.find(juniorschool_id);
    if (it != user_juniorschool_data_.end())
      return it->second;
    else {
      MCE_WARN("AdUserCacheLoaderI::GetAreacodeFromUserJuniorCache --> miss juniorschool_id = " << juniorschool_id);
    }
  }
  return "";
}
string AdUserCacheLoaderI::GetAreacodeFromUserHighSchoolCache(Ice::Long highschool_id){
  MCE_INFO("AdUserCacheLoaderI::GetAreacodeFromUserHighSchoolCache --> highschool_id = " << highschool_id);
  {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    ::std::map<Ice::Long, string>::iterator it = user_highschool_data_.find(highschool_id);
    if (it != user_highschool_data_.end())
      return it->second;
    else {
      MCE_WARN("AdUserCacheLoaderI::GetAreacodeFromUserHighSchoolCache --> miss highschool_id = " << highschool_id);
    }
  }
  return "";
}

void InitUserAreaDataTask::handle() {
  AdUserCacheLoaderI::instance().SetUserAreaData();
}

LoadTask::LoadTask(const MyUtil::IntSeq& userIds, bool is_reload) :
  Task(TASK_LEVEL_RELOAD),user_ids_(userIds), is_reload_(is_reload){
}

void LoadTask::LoadFromAdUserInfo(){
  MCE_DEBUG("LoadTask::LoadFromAdUserInfo --> size = " << user_ids_.size());
  ::std::set<int> ids_set;
  com::xiaonei::xce::Statement sql;
  sql
      << "select userid, stage, gender, age, current_area, home_area, schoolid, grade from ad_user_info where userid in";
  ostringstream oss;
  oss << "(";
  for (MyUtil::IntSeq::const_iterator it = user_ids_.begin(); it != user_ids_.end(); ++it) {
    ids_set.insert(*it);
    if (it != user_ids_.begin()) {
      oss << ",";
    }
    oss << (*it);
  }
  oss << ");";
  sql << oss.str();
  mysqlpp::StoreQueryResult res =
      QueryRunner(DB_AD_USER_INFO, CDbRServer).store(sql);
  if (res && res.num_rows() > 0) {
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      UserProfileWrapperPtr obj = new UserProfileWrapper;
      int userid = (int) row["userid"];
      int stage = (int)row["stage"];
      int gender = (int)row["gender"];
      int age = (int)row["age"];
      string current_area = row["current_area"].data();
      string home_area = row["home_area"].data();
      Ice::Long school = (Ice::Long)row["schoolid"];
      int grade = (int)row["grade"];
      UserProfile upf = obj->SetAllProperties(userid, stage, gender, age, current_area, home_area,
          school, grade);
      MCE_INFO("LoadTask::LoadFromAdUserInfo --> RESULT : userid = " << userid << " gender = " << gender << " stage = " << stage << " age = " << age <<  " current_area = " << current_area << " home_area = " << home_area << " school = " << school << " grade = " << grade);
      try{
        UserProfileClient::instance().Set(upf);
      }
      catch(...){
        MCE_WARN("LoadTask::LoadFromAdUserInfo --> UserProfileClient::instance().Set(pro)  exception");
      }
      try{
        string upfstr;
        UserProfileClient::instance().Serialize(upf, &upfstr);
        UpfSerialize.push_back(upfstr);
      }
      catch(...){
        MCE_WARN("LoadTask::LoadFromAdUserInfo --> UpfSerialize exception");
      }
      ids_set.erase(userid);
    }
  }
  for (::std::set<int>::iterator it = ids_set.begin(); it != ids_set.end(); ++it) {
    miss_ids_.push_back(*it);
  }
}

void LoadTask::handle() {
  ostringstream oss;
  UserProfileWrapperMap obj;
  MCE_INFO("LoadTask::handle -- > is_reload_ = " << is_reload_);
  if(!is_reload_){
    LoadFromAdUserInfo();
    if(!miss_ids_.empty()){
      obj = AdUserCacheLoaderI::instance().CreateUserInfo(miss_ids_);
    }
    else{
      MCE_INFO("LoadTask::handle --> UpfSerialize.size() = " << UpfSerialize.size());
    }
  }
  else{
    obj = AdUserCacheLoaderI::instance().CreateUserInfo(user_ids_);
  }
  if(!obj.empty()){
    for (UserProfileWrapperMap::const_iterator itr = obj.begin(); itr != obj.end(); ++itr) {
      UserProfile upf = itr->second->GetUserProfile();
      int userid = itr->first;
      int gender = upf.gender();
      int stage = upf.stage();
      string current_area = upf.current_area();
      string home_area = upf.home_area();
      Ice::Long school = upf.school();
      int grade = upf.grade();
      int age = upf.age();
      //oss << "LoadTask::handle -- > RESULT : userid = " << userid << " gender = " << gender << " stage = " << stage << " age = " << age << " home_area = " << home_area << " current_area = " << current_area << " school = " << school << " grade = " << grade;
      MCE_INFO("LoadTask::handle -- > RESULT : userid = " << userid << " gender = " << gender << " stage = " << stage << " age = " << age << " home_area = " << home_area << " current_area = " << current_area << " school = " << school << " grade = " << grade);
      com::xiaonei::xce::Statement sql;
      sql << "insert into ad_user_info (userid, stage, gender, age, current_area, home_area, schoolid, grade) values"
      << "(" << userid<< ", " << stage << ", " << gender << ", " << age << ", " << mysqlpp::quote << current_area << ", " << mysqlpp::quote << home_area << ", " << school << ", " << grade << ")"
      << " on duplicate key update stage = values(stage), gender = values(gender), age = values(age), current_area = values(current_area), home_area = values(home_area), schoolid = values(schoolid), grade = values(grade)";
      try{
        UserProfileClient::instance().Set(upf);
      }
      catch(...){
        MCE_WARN("LoadTask::handle --> UserProfileClient::instance().Set(pro)  exception");
      }
      try{
        string upfstr;
        UserProfileClient::instance().Serialize(upf, &upfstr);
        UpfSerialize.push_back(upfstr);
      }
      catch(...){
        MCE_WARN("LoadTask::handle --> UpfSerialize exception");
      }
      try{
        QueryRunner(DB_AD_USER_INFO, CDbRServer).store(sql);
      }
      catch(std::exception& e){
        MCE_WARN("LoadTask::handle --> insert into ad_user_info exception : " << e.what());
      }
      MCE_INFO("LoadTask::handle --> UpfSerialize.size() = " << UpfSerialize.size());
    }
  }
}

void UserAreaDataResetTimer::handle() {
  MCE_INFO("UserAreaDataRestTimer::handle --> RESET ALL CACHES");
  AdUserCacheLoaderI::instance().SetUserAreaData();
}
