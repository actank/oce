#include <boost/lexical_cast.hpp>
#include "TripodCacheData.h"

using namespace xce::search::tripodcachedata;
using namespace oce::tripodsearchcache;
using namespace IceUtil;
using namespace mysqlpp;

void TripodCacheData::setUserPassportProperties(mysqlpp::Row& row)  {
  const char* status = "status";
  const char* safe_status = "safe_status";
  RWRecMutex::WLock lock(_dataMutex);
  int status_tmp, safe_status_tmp;
  status_tmp = (int) row[status];
  safe_status_tmp = (int) row[safe_status];
  set_status(status_tmp);
  set_safestatus(safe_status_tmp);
}

void TripodCacheData::setConfigProperties(mysqlpp::Row& row) {
  const char* browse_config = "browse_config";
  const char* status_config = "status_config";
  const char* basic_config = "basic_config";
  const char* profile_privacy = "profile_privacy";
  RWRecMutex::WLock lock(_dataMutex);
  int browse_config_tmp, status_config_tmp, basic_config_tmp;
  std::string profile_privacy_tmp;
  browse_config_tmp = (int) row[browse_config];
  status_config_tmp = (int) row[status_config];
  basic_config_tmp = (int) row[basic_config];
  profile_privacy_tmp = splitProfilePrivacy((std::string) row[profile_privacy]);
  set_browseconfig(browse_config_tmp);
  set_statusconfig(status_config_tmp);
  set_basicconfig(basic_config_tmp);
  set_profileprivacy(profile_privacy_tmp);
}

void TripodCacheData::setUserUrlProperties(mysqlpp::Row& row) {
  RWRecMutex::WLock lock(_dataMutex);
  const char* headurl = "headurl";
  const char* tinyurl = "tinyurl";
  std::string headurl_tmp, tinyurl_tmp;
#ifndef NEWARCH
  headurl_tmp = row[headurl].get_string();
  tinyurl_tmp = row[tinyurl].get_string();
#else
  headurl_tmp = row[headurl].c_str();
  tinyurl_tmp = row[tinyurl].c_str();
#endif
  set_headurl(headurl_tmp);
  set_tinyurl(tinyurl_tmp);
}

void TripodCacheData::setUserStateProperties(mysqlpp::Row& row){
  const char* state = "state";
  int state_tmp;
  RWRecMutex::WLock lock(_dataMutex);
  state_tmp = (int) row[state];
  set_state(state_tmp);
}

void TripodCacheData::setUserStageProperties(mysqlpp::Row& row){
  const char* stage= "stage";
  int stage_tmp;
  RWRecMutex::WLock lock(_dataMutex);
  stage_tmp = (int) row[stage];
  set_stage(stage_tmp);
}

void TripodCacheData::setUserNameProperties(mysqlpp::Row& row){
  const char* name = "name";
  const char* nickname = "nickname";
  RWRecMutex::WLock lock(_dataMutex);
  std::string  name_tmp, signature_tmp;
#ifndef NEWARCH
  name_tmp = row[name].get_string();
  signature_tmp = row[nickname].get_string();
#else
  name_tmp = row[name].c_str();
  signature_tmp = row[nickname].c_str();
#endif
  set_name(name_tmp);
  set_signature(signature_tmp);
}

void TripodCacheData::setUserBornProperties(mysqlpp::Row& row) {
  const char* gender = "gender";
  const char* home_city = "home_city";
  const char* home_province = "home_province";
  const char* astrology = "astrology";
  const char* birth_year = "birth_year";
  const char* birth_month = "birth_month";
  const char* birth_day = "birth_day";
  std::string gender_tmp;
  std::string home_city_tmp;
  std::string home_province_tmp;
  std::string astrology_tmp;
  int birth_year_tmp;
  int birth_month_tmp;
  int birth_day_tmp;
  RWRecMutex::WLock lock(_dataMutex);
#ifndef NEWARCH
  gender_tmp = row[gender].get_string();
  home_city_tmp = row[home_city].get_string();
  home_province_tmp = row[home_province].get_string();
  astrology_tmp = row[astrology].get_string();
#else
  gender_tmp = row[gender].c_str();
  home_city_tmp = row[home_city].c_str();
  home_province_tmp = row[home_province].c_str();
  astrology_tmp = row[astrology].c_str();
#endif
  try  {
    birth_year_tmp = (int)row[birth_year];
  }  catch (mysqlpp::Exception& e)  {
    birth_year_tmp = 0;
  }
  try  {
    birth_month_tmp = (int)row[birth_month];
  }  catch(mysqlpp::Exception& e)  {
    birth_month_tmp = 0;
  }
  try  {
    birth_day_tmp = (int)row[birth_day];
  }  catch(mysqlpp::Exception& e)  {
    birth_day_tmp = 0;
  }
  set_gender(gender_tmp);
  set_homecity(home_city_tmp);
  set_homeprovince(home_province_tmp);
  set_astrology(astrology_tmp);
  set_birthyear(birth_year_tmp);
  set_birthmonth(birth_month_tmp);
  set_birthday(birth_day_tmp);
}

void TripodCacheData::setViewCountProperties(mysqlpp::Row& row) {
  const char* view_count= "viewcount";
  int view_count_tmp;
  RWRecMutex::WLock lock(_dataMutex);
  view_count_tmp = (int) row[view_count];
  set_viewcount(view_count_tmp);
}

void TripodCacheData::setLastlogintimeProperties(mysqlpp::Row& row) {
  const char* last_login_time = "lastlogintime";
  std::string last_login_time_tmp;
  RWRecMutex::WLock lock(_dataMutex);
#ifndef NEWARCH
  last_login_time_tmp = row[last_login_time].get_string();
#else
  last_login_time_tmp = row[last_login_time].c_str();
#endif
  set_lastlogintime(last_login_time_tmp);
}

void TripodCacheData::setUserFondProperties(mysqlpp::Row& row) {
  const char* content= "content";
  const char* type = "type";

  RWRecMutex::WLock lock(_dataMutex);
  int type_tmp = (int) row[type];
#ifndef NEWARCH
  switch(type_tmp)
  {
    case 2:
      set_interest(row[content].get_string());
      break;
    case 3:
      set_music(row[content].get_string());
      break;
    case 4:
      set_book(row[content].get_string());
      break;
    case 5:
      set_movie(row[content].get_string());
      break;
    case 6:
      set_game(row[content].get_string());
      break;
    case 7:
      set_comic(row[content].get_string());
      break;
    case 8:
      set_sport(row[content].get_string());
      break;
    case 9:
      set_society(row[content].get_string());
      break;
    default:
      break;
  }
#else
  switch(type_tmp)
  {
    case 2://interest
      set_interest(row[content].c_str());
      break;
    case 3://music
      set_music(row[content].c_str());
      break;
    case 4://book
      set_book(row[content].c_str());
      break;
    case 5://movie
      set_movie(row[content].c_str());
      break;
    case 6://game
      set_game(row[content].c_str());
      break;
    case 7://comic
      set_comic(row[content].c_str());
      break;
    case 8://sport
      set_sport(row[content].c_str());
      break;
    case 9://society
      set_society(row[content].c_str());
      break;
    default:
      break;
  }
#endif
}

void TripodCacheData::setCollegePropertiesAccordingVector(vector<mysqlpp::Row>& rows) {
  const char* college_id = "college_id";
  const char* enroll_year = "enroll_year";
  const char* department = "department";
  const char* college_name = "college_name";

  RWRecMutex::WLock lock(_dataMutex);
  clear_collegeschools();
  for (vector<Row>::const_iterator iter = rows.begin();
      iter!= rows.end();
      ++iter) {
    int id, year;
    string name, department_tmp;
    id = (int) (*iter)[college_id];
    year = (int) (*iter)[enroll_year];
#ifndef NEWARCH
    name = (*iter)[college_name].get_string();
    department_tmp = (*iter)[department].get_string();
#else
    name = (*iter)[college_name].c_str();
    department_tmp = (*iter)[department].c_str();
#endif
    if(name == "")
      continue;
    CollegeInfo * tmp = add_collegeschools();
    tmp->set_id(id);
    tmp->set_year(year);
    tmp->set_name(name);
    tmp->set_department(department_tmp);
  }
}

void TripodCacheData::setUniversityPropertiesAccordingVector(vector<mysqlpp::Row>& rows) {
  const char* university_id = "university_id";
  const char* enroll_year = "enroll_year";
  const char* university_name = "university_name";
  const char* department = "department";
  const char* dorm = "dorm";
  RWRecMutex::WLock lock(_dataMutex);
  clear_universityschools();
  for (vector<Row>::const_iterator iter = rows.begin();
      iter!= rows.end();
      ++iter) {
    int id, year;
    string name, department_tmp, dorm_tmp;
    id = (int) (*iter)[university_id];
    year = (int) (*iter)[enroll_year];
#ifndef NEWARCH
    name = (*iter)[university_name].get_string();
    department_tmp = (*iter)[department].get_string();
    dorm_tmp = (*iter)[dorm].get_string();
#else
    name = (*iter)[university_name].c_str();
    department_tmp = (*iter)[department].c_str();
    dorm_tmp = (*iter)[dorm].c_str();
#endif
    if(name == "")
      continue;
    UniversityInfo * tmp = add_universityschools();
    tmp->set_id(id);
    tmp->set_name(name);
    tmp->set_year(year);
    tmp->set_department(department_tmp);
    tmp->set_dorm(dorm_tmp);
  }
}

void TripodCacheData::setWorkplacePropertiesAccordingVector(vector<mysqlpp::Row>& rows) {
  const char* workplace_id = "workplace_id";
  const char* workplace_name = "workplace_name";
  RWRecMutex::WLock lock(_dataMutex);
  clear_workplaces();
  for (vector<Row>::const_iterator iter = rows.begin();
      iter!= rows.end();
      ++iter) {
    int id;
    string name;
    id = (int) (*iter)[workplace_id];
#ifndef NEWARCH
    name = (*iter)[workplace_name].get_string();
#else
    name = (*iter)[workplace_name].c_str();
#endif
    if(name == "")
      continue;
    WorkplaceInfo * tmp = add_workplaces();
    tmp->set_id(id);
    tmp->set_name(name);
  }
}

void TripodCacheData::setRegionPropertiesAccordingVector(vector<mysqlpp::Row>& rows) {
  const char* region_id = "region_id";
  const char* city_name = "city_name";
  const char* province_name = "province_name";

  RWRecMutex::WLock lock(_dataMutex);
  clear_regions();
  for (vector<Row>::const_iterator iter = rows.begin();
      iter!= rows.end();
      ++iter) {
    int id;
    string city, province;
    id = (int) (*iter)[region_id];
#ifndef NEWARCH
    city = (*iter)[city_name].get_string();
    province = (*iter)[province_name].get_string();
#else
    city = (*iter)[city_name].c_str();
    province = (*iter)[province_name].c_str();
#endif
    if(city == "" && province == "")
      continue;
    RegionInfo * tmp = add_regions();
    tmp->set_id(id);
    tmp->set_city(city);
    tmp->set_province(province);
  }
}

void TripodCacheData::setNetworkPropertiesAccordingVector(vector<mysqlpp::Row>& rows) {
  const char* network_id = "network_id";
  const char* stage = "stage";
  const char* network_name = "network_name";
  RWRecMutex::WLock lock(_dataMutex);
  clear_networks();
  for (vector<Row>::const_iterator iter = rows.begin();
      iter!= rows.end();
      ++iter) {
    int id, stage_tmp;
    string name;
    id = (int) (*iter)[network_id];
    stage_tmp = (int) (*iter)[stage];
#ifndef NEWARCH
    name = (*iter)[network_name].get_string();
#else
    name = (*iter)[network_name].c_str();
#endif
    if(name == "")
      continue;
    NetWorkInfo * tmp = add_networks();
    tmp->set_id(id);
    tmp->set_stage(stage_tmp);
    tmp->set_name(name);
  }
}

void TripodCacheData::setHighSchoolPropertiesAccordingVector(vector<mysqlpp::Row>& rows) {
  const char* high_id = "high_school_id";
  const char* high_year = "enroll_year";
  const char* high_name = "high_school_name";
  const char* h_class1 = "h_class1";
  const char* h_class2 = "h_class2";
  const char* h_class3 = "h_class3";

  RWRecMutex::WLock lock(_dataMutex);
  clear_highschools();

  for (vector<Row>::const_iterator iter = rows.begin();
      iter!= rows.end();
      ++iter) {
    int id, year;
    string name, class1, class2, class3;
    id = (int) (*iter)[high_id];
    year = (int) (*iter)[high_year];
#ifndef NEWARCH
    name = (*iter)[high_name].get_string();
    class1 = (*iter)[h_class1].get_string();
    class2 = (*iter)[h_class2].get_string();
    class3 = (*iter)[h_class3].get_string();
#else
    name = (*iter)[high_name].c_str();
    class1 = (*iter)[h_class1].c_str();
    class2 = (*iter)[h_class2].c_str();
    class3 = (*iter)[h_class3].c_str();
#endif
    if(name == "")
      continue;
    HighSchoolInfo * tmp = add_highschools();
    tmp->set_id(id);
    tmp->set_name(name);
    tmp->set_year(year);
    tmp->set_class1(class1);
    tmp->set_class2(class2);
    tmp->set_class3(class3);
  }
}

void TripodCacheData::setJuniorSchoolPropertiesAccordingVector(vector<mysqlpp::Row>& rows) {
  const char* junior_high_id ="junior_high_school_id";
  const char* junior_high_year ="junior_high_school_year";
  const char* junior_high_name ="junior_high_school_name";

  RWRecMutex::WLock lock(_dataMutex);
  clear_juniorschools();
  for (vector<Row>::const_iterator iter = rows.begin();
      iter!= rows.end();
      ++iter) {
    int id, year;
    string name;
    id = (int) (*iter)[junior_high_id];
    year = (int) (*iter)[junior_high_year];
#ifndef NEWARCH 
    name = (*iter)[junior_high_name].get_string();
#else
    name = (*iter)[junior_high_name].c_str();
#endif
    if(name == "")
      continue;
    JuniorSchoolInfo * tmp = add_juniorschools();
    tmp->set_id(id);
    tmp->set_year(year);
    tmp->set_name(name);
  }
}

void TripodCacheData::setElementarySchoolPropertiesAccordingVector(vector<mysqlpp::Row>& rows) {
  const char* elementary_id ="elementary_school_id";
  const char* elementary_year ="elementary_school_year";
  const char* elementary_name ="elementary_school_name";

  RWRecMutex::WLock lock(_dataMutex);
  clear_elementaryschools();
  for (vector<Row>::const_iterator iter = rows.begin();
      iter!= rows.end();
      ++iter) {
    int id, year;
    string name;
    id = (int) (*iter)[elementary_id];
    year = (int) (*iter)[elementary_year];
#ifndef NEWARCH
    name = (*iter)[elementary_name].get_string();
#else
    name = (*iter)[elementary_name].c_str();
#endif
    if(name == "")
      continue;
    ElementarySchoolInfo * tmp = add_elementaryschools();
    tmp->set_id(id);
    tmp->set_year(year);
    tmp->set_name(name);

  }
}

std::string TripodCacheData::splitProfilePrivacy(std::string profilePrivacy) {
  string bewPrivacy = "";//仅判断basic,education和workplace三种隐私
  string::size_type indexB = profilePrivacy.find("b");
  string::size_type indexE = profilePrivacy.find("e");
  string::size_type indexW= profilePrivacy.find("w");
  string::size_type indexWait;
  bewPrivacy += "{";
  if(indexB != string::npos)  {
    indexWait = profilePrivacy.find(",",indexB);
    if(indexWait == string::npos)  {
      indexWait = profilePrivacy.find("}",indexB);
    }
    bewPrivacy += profilePrivacy.substr(indexB, indexWait - indexB);
    bewPrivacy += ",";
  }
  if(indexE != string::npos)  {
    indexWait = profilePrivacy.find(",", indexE);
    if(indexWait == string::npos)  {
      indexWait = profilePrivacy.find("}",indexB);
    }
    bewPrivacy += profilePrivacy.substr(indexE, indexWait - indexE);
    bewPrivacy += ",";
  }
  if(indexW != string::npos)  {
    indexWait = profilePrivacy.find(",", indexW);
    if(indexWait == string::npos)  {
      indexWait = profilePrivacy.find("}",indexB);
    }
    bewPrivacy += profilePrivacy.substr(indexW, indexWait - indexW);
  }
  bewPrivacy += "}";
  return bewPrivacy;
}
