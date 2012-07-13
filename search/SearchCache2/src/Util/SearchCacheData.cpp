/*
 *
 * SearchCacheDataI.h
 *
 *  Created on: 2009-4-20
 *      Author: ning.zhao@opi-corp.com
 */

#include "SearchCacheData.h"
#include <boost/lexical_cast.hpp>
#include <ServiceI.h>
#include "LogWrapper.h"

using namespace std;
using namespace xce::searchcache;
using namespace MyUtil;
using namespace mysqlpp;
using namespace IceUtil;


SearchCacheData::SearchCacheData() {
  _id = 0;
  _star = 0;
  _status = 0;
  _stage = 0;
  _birthday_year = 0;
  _birthday_month = 0;
  _birthday_day = 0;
  _state = 0;
  _browse_config = 0;
  _status_config = 0;
  _basic_config = 0;
  _auth = 0;
  _viewcount = 0;
}

SearchCacheData& SearchCacheData::operator=(const SearchCacheData &other)
{
  if(this == &other)
    return *this;

  this->_id = other._id;
  this->_name = other._name;
  this->_gender = other._gender;
  this->_signature = other._signature;
  this->_star = other._star;
  this->_status = other._status;
  this->_stage = other._stage;
  this->_birthday_year = other._birthday_year;
  this->_birthday_month = other._birthday_month;
  this->_birthday_day = other._birthday_day;
  this->_state = other._state;
  this->_lastlogintime = other._lastlogintime;
  this->_tinyurl = other._tinyurl;
  this->_headurl = other._headurl;
  this->_astrology = other._astrology;
  this->_browse_config = other._browse_config;
  this->_status_config = other._status_config;
  this->_basic_config = other._basic_config;
  this->_auth = other._auth;
  this->_viewcount = other._viewcount;
  this->_home_city = other._home_city;
  this->_home_province = other._home_province;
  this->_interest = other._interest;
  this->_music = other._music;
  this->_movie = other._movie;
  this->_game = other._game;
  this->_comic = other._comic;
  this->_sport = other._sport;
  this->_book = other._book;
  this->_society = other._society;
  this->_profile_privacy = other._profile_privacy;

  RWRecMutex::WLock lock(_dataMutex);
  this->_networks.clear();
  for(vector<Network>::const_iterator iter=other._networks.begin();
      iter != other._networks.end();
      ++iter) {
    this->_networks.push_back(*iter);

  }

  this->_regions.clear();
  for(vector<Region>::const_iterator iter=other._regions.begin();
      iter != other._regions.end();
      ++iter) {
    this->_regions.push_back(*iter);

  }
  this->_workplaces.clear();
  for(vector<Workplace>::const_iterator iter=other._workplaces.begin();
      iter != other._workplaces.end();
      ++iter) {
    this->_workplaces.push_back(*iter);

  }
  this->_universities.clear();
  for(vector<University>::const_iterator iter=other._universities.begin();
      iter != other._universities.end();
      ++iter) {
    this->_universities.push_back(*iter);

  }
  this->_colleges.clear();
  for(vector<College>::const_iterator iter=other._colleges.begin();
      iter != other._colleges.end();
      ++iter) {
    this->_colleges.push_back(*iter);

  }
  this->_highSchools.clear();
  for(vector<HighSchool>::const_iterator iter=other._highSchools.begin();
      iter != other._highSchools.end();
      ++iter) {
    this->_highSchools.push_back(*iter);

  }
  this->_juniorSchools.clear();
  for(vector<JuniorSchool>::const_iterator iter=other._juniorSchools.begin();
      iter != other._juniorSchools.end();
      ++iter) {
    this->_juniorSchools.push_back(*iter);

  }
  this->_elementarySchools.clear();
  for(vector<ElementarySchool>::const_iterator iter=other._elementarySchools.begin();
      iter != other._elementarySchools.end();
      ++iter) {
    this->_elementarySchools.push_back(*iter);

  }

  return *this;
}
SearchCacheResult SearchCacheData::getProperties() {
  SearchCacheResult result;
  return result;
}

void SearchCacheData::setElementarySchoolProperties(Row& row) {
  const char* elementary_school_id ="elementary_school_id"; 
  const char* elementary_school_year ="elementary_school_year"; 
  const char* elementary_school_name ="elementary_school_name"; 

  ElementarySchool school;
  school.id = (int) row[elementary_school_id];
  school.year = (int) row[elementary_school_year];
#ifndef NEWARCH
  school.name = row[elementary_school_name].get_string();
#else
  school.name = row[elementary_school_name].c_str();
#endif
  if(school.name == "")
    return;
  IceUtil::RWRecMutex::WLock lock(_dataMutex);  
  _elementarySchools.push_back(school);
}

void SearchCacheData::setElementarySchoolPropertiesAccordingVector(vector<Row>& rows) {
  const char* elementary_school_id ="elementary_school_id"; 
  const char* elementary_school_year ="elementary_school_year"; 
  const char* elementary_school_name ="elementary_school_name"; 

  RWRecMutex::WLock lock(_dataMutex);
  _elementarySchools.clear();
  for (vector<Row>::const_iterator iter = rows.begin();
      iter!= rows.end();
      ++iter) {
    ElementarySchool school;
    school.id = (int) (*iter)[elementary_school_id];
    school.year = (int) (*iter)[elementary_school_year];
#ifndef NEWARCH
    school.name = (*iter)[elementary_school_name].get_string();
#else
    school.name = (*iter)[elementary_school_name].c_str();
#endif
    if(school.name == "")
      continue;
    _elementarySchools.push_back(school);
  }
}
void SearchCacheData::setJuniorSchoolProperties(Row& row) {
  const char* junior_high_school_id ="junior_high_school_id"; 
  const char* junior_high_school_year ="junior_high_school_year"; 
  const char* junior_high_school_name ="junior_high_school_name"; 

  JuniorSchool school;
  school.id = (int) row[junior_high_school_id];
  school.year = (int) row[junior_high_school_year];
#ifndef NEWARCH
  school.name = row[junior_high_school_name].get_string();
#else
  school.name = row[junior_high_school_name].c_str();
#endif
  if(school.name == "")
    return;
  RWRecMutex::WLock lock(_dataMutex);
  _juniorSchools.push_back(school);
}

void SearchCacheData::setJuniorSchoolPropertiesAccordingVector(vector<Row>& rows) {
  const char* junior_high_school_id ="junior_high_school_id"; 
  const char* junior_high_school_year ="junior_high_school_year"; 
  const char* junior_high_school_name ="junior_high_school_name"; 

  RWRecMutex::WLock lock(_dataMutex);
  _juniorSchools.clear();
  for (vector<Row>::const_iterator iter = rows.begin();
      iter!= rows.end();
      ++iter) {
    JuniorSchool school;
    school.id = (int) (*iter)[junior_high_school_id];
    school.year = (int) (*iter)[junior_high_school_year];
#ifndef NEWARCH
    school.name = (*iter)[junior_high_school_name].get_string();
#else
    school.name = (*iter)[junior_high_school_name].c_str();
#endif
    if(school.name == "")
      continue;
    _juniorSchools.push_back(school);
  }
}


void SearchCacheData::setHighSchoolProperties(Row& row) {
  const char* high_school_id = "high_school_id";
  const char* high_school_year = "enroll_year";
  const char* high_school_name = "high_school_name";
  const char* h_class1 = "h_class1";
  const char* h_class2 = "h_class2";
  const char* h_class3 = "h_class3";

  HighSchool school;
  school.id = (int)row[high_school_id];
  school.year = (int)row[high_school_year];

#ifndef NEWARCH
  school.name =row[high_school_name].get_string();
  school.class1 =row[h_class1].get_string();
  school.class2 =row[h_class2].get_string();
  school.class3 =row[h_class3].get_string();
#else
  school.name =row[high_school_name].c_str();
  school.class1 =row[h_class1].c_str();
  school.class2 =row[h_class2].c_str();
  school.class3 =row[h_class3].c_str();
#endif
  if(school.name == "")
    return;
  RWRecMutex::WLock lock(_dataMutex);
  _highSchools.push_back(school);
}

void SearchCacheData::setHighSchoolPropertiesAccordingVector(vector<Row>& rows) {
  const char* high_school_id = "high_school_id";
  const char* high_school_year = "enroll_year";
  const char* high_school_name = "high_school_name";
  const char* h_class1 = "h_class1";
  const char* h_class2 = "h_class2";
  const char* h_class3 = "h_class3";

  RWRecMutex::WLock lock(_dataMutex);
  _highSchools.clear();
  for (vector<Row>::const_iterator iter = rows.begin();
      iter!= rows.end();
      ++iter) {
    HighSchool school;
    school.id = (int) (*iter)[high_school_id];
    school.year = (int) (*iter)[high_school_year];
#ifndef NEWARCH
    school.name = (*iter)[high_school_name].get_string();
    school.class1 = (*iter)[h_class1].get_string();
    school.class2 = (*iter)[h_class2].get_string();
    school.class3 = (*iter)[h_class3].get_string();
#else
    school.name = (*iter)[high_school_name].c_str();
    school.class1 = (*iter)[h_class1].c_str();
    school.class2 = (*iter)[h_class2].c_str();
    school.class3 = (*iter)[h_class3].c_str();
#endif
    if(school.name == "")
      continue;
    _highSchools.push_back(school);
  }
}

void SearchCacheData::setCollegeProperties(Row& row) {
  const char* college_id = "college_id";
  const char* enroll_year = "enroll_year";
  const char* department = "department";
  const char* college_name = "college_name";

  College school;
  school.id = (int) row[college_id];
  school.year = (int) row[enroll_year];
#ifndef NEWARCH
  school.name = row[college_name].get_string();
  school.department = row[department].get_string();
#else
  school.name = row[college_name].c_str();
  school.department = row[department].c_str();
#endif
  if(school.name == "")
    return;
  RWRecMutex::WLock lock(_dataMutex);
  _colleges.push_back(school);
}
void SearchCacheData::setCollegePropertiesAccordingVector(vector<Row>& rows) {
  const char* college_id = "college_id";
  const char* enroll_year = "enroll_year";
  const char* department = "department";
  const char* college_name = "college_name";

  RWRecMutex::WLock lock(_dataMutex);
  _colleges.clear();
  for (vector<Row>::const_iterator iter = rows.begin();
      iter!= rows.end();
      ++iter) {
    College school;
    school.id = (int) (*iter)[college_id];
    school.year = (int) (*iter)[enroll_year];
#ifndef NEWARCH
    school.name = (*iter)[college_name].get_string();
    school.department = (*iter)[department].get_string();
#else
    school.name = (*iter)[college_name].c_str();
    school.department = (*iter)[department].c_str();
#endif
    if(school.name == "")
      continue;

    _colleges.push_back(school);
  }
}


void SearchCacheData::setUniversityProperties(Row& row) {
  const char* university_id = "university_id"; 
  const char* enroll_year = "enroll_year";
  const char* university_name = "university_name";
  const char* department = "department";
  const char* dorm = "dorm";

  University school;
  school.id = (int) row[university_id];
  school.year = (int) row[enroll_year];
#ifndef NEWARCH
  school.name = row[university_name].get_string();
  school.department = row[department].get_string();
  school.dorm = row[dorm].get_string();
#else
  school.name = row[university_name].c_str();
  school.department = row[department].c_str();
  school.dorm = row[dorm].c_str();
#endif
  if(school.name == "")
    return;

  RWRecMutex::WLock lock(_dataMutex);
  _universities.push_back(school);
}
void SearchCacheData::setUniversityPropertiesAccordingVector(vector<Row>& rows) {
  const char* university_id = "university_id"; 
  const char* enroll_year = "enroll_year";
  const char* university_name = "university_name";
  const char* department = "department";
  const char* dorm = "dorm";

  RWRecMutex::WLock lock(_dataMutex);
  _universities.clear();
  for (vector<Row>::const_iterator iter = rows.begin();
      iter!= rows.end();
      ++iter) {
    University school;
    school.id = (int) (*iter)[university_id];
    school.year = (int) (*iter)[enroll_year];
#ifndef NEWARCH
    school.name = (*iter)[university_name].get_string();
    school.department = (*iter)[department].get_string();
    school.dorm = (*iter)[dorm].get_string();
#else
    school.name = (*iter)[university_name].c_str();
    school.department = (*iter)[department].c_str();
    school.dorm = (*iter)[dorm].c_str();
#endif
    if(school.name == "")
      continue;

    _universities.push_back(school);
  }
}

void SearchCacheData::setWorkplaceProperties(Row& row) {
  const char* workplace_id = "workplace_id";
  const char* workplace_name = "workplace_name";
  Workplace workplace;
  workplace.id = (int) row[workplace_id];
#ifndef NEWARCH
  workplace.name = row[workplace_name].get_string();
#else
  workplace.name = row[workplace_name].c_str();
#endif
  if(workplace.name == "")
    return;

  RWRecMutex::WLock lock(_dataMutex);
  _workplaces.push_back(workplace);
}

void SearchCacheData::setWorkplacePropertiesAccordingVector(vector<Row>& rows) {
  const char* workplace_id = "workplace_id";
  const char* workplace_name = "workplace_name";

  RWRecMutex::WLock lock(_dataMutex);
  _workplaces.clear();
  for (vector<Row>::const_iterator iter = rows.begin();
      iter!= rows.end();
      ++iter) {
    Workplace workplace;
    workplace.id = (int) (*iter)[workplace_id];
#ifndef NEWARCH
    workplace.name = (*iter)[workplace_name].get_string();
#else
    workplace.name = (*iter)[workplace_name].c_str();
#endif
    if(workplace.name == "")
      continue;

    _workplaces.push_back(workplace);
  }
}

void SearchCacheData::setRegionProperties(Row& row) {
  const char* region_id = "region_id";
  const char* city_name = "city_name";
  const char* province_name = "province_name";

  Region region;
  region.id = (int) row[region_id];
#ifndef NEWARCH
  region.city = row[city_name].get_string();
  region.province = row[province_name].get_string();
#else
  region.city = row[city_name].c_str();
  region.province = row[province_name].c_str();
#endif
  if(region.city == "" && region.province == "")
    return;

  RWRecMutex::WLock lock(_dataMutex);
  _regions.push_back(region);
}

void SearchCacheData::setRegionPropertiesAccordingVector(vector<Row>& rows) {
  const char* region_id = "region_id";
  const char* city_name = "city_name";
  const char* province_name = "province_name";

  RWRecMutex::WLock lock(_dataMutex);
  _regions.clear();
  for (vector<Row>::const_iterator iter = rows.begin();
      iter!= rows.end();
      ++iter) {
    Region region;
    region.id = (int) (*iter)[region_id];
#ifndef NEWARCH
    region.city = (*iter)[city_name].get_string();
    region.province = (*iter)[province_name].get_string();
#else
    region.city = (*iter)[city_name].c_str();
    region.province = (*iter)[province_name].c_str();
#endif
    if(region.city == "" && region.province == "")
      continue;

    _regions.push_back(region);
  }
}
void SearchCacheData::setNetworkProperties(Row& row) {
  const char* network_id = "network_id";
  const char* stage = "stage";
  const char* network_name = "network_name";

  Network network;
  network.id = (int) row[network_id];
  network.stage = (int) row[stage];
#ifndef NEWARCH
  network.name = row[network_name].get_string();
#else
  network.name = row[network_name].c_str();
#endif
  if(network.name == "")
    return;

  RWRecMutex::WLock lock(_dataMutex);
  _networks.push_back(network);
}

void SearchCacheData::setNetworkPropertiesAccordingVector(vector<Row>& rows) {
  const char* network_id = "network_id";
  const char* stage = "stage";
  const char* network_name = "network_name";
  RWRecMutex::WLock lock(_dataMutex);
  _networks.clear();
  for (vector<Row>::const_iterator iter = rows.begin();
      iter!= rows.end();
      ++iter) {
    Network network;
    network.id = (int) (*iter)[network_id];
    network.stage = (int) (*iter)[stage];
#ifndef NEWARCH
    network.name = (*iter)[network_name].get_string();
#else
    network.name = (*iter)[network_name].c_str();
#endif
    if(network.name == "")
      continue;

    _networks.push_back(network);
  }
}

/*分解Jason串profile_privacy中只含有b,e,w开头的隐私 eg:{a:1,b:2,e:4,w:-1,...} */
std::string SearchCacheData::splitProfilePrivacy(std::string profile_privacy)  {
  std::string bew_privacy = "";//仅判断basic,education和workplace三种隐私
  std::string::size_type indexB = profile_privacy.find("b");
  std::string::size_type indexE = profile_privacy.find("e");
  std::string::size_type indexW= profile_privacy.find("w");
  std::string::size_type indexWait;
  bew_privacy += "{";
  if(indexB != std::string::npos)  {
    indexWait = profile_privacy.find(",",indexB);
    if(indexWait == std::string::npos)  {
      indexWait =profile_privacy.find("}",indexB);
    }  
    bew_privacy += profile_privacy.substr(indexB, indexWait - indexB); 
    bew_privacy += ",";
  }
  if(indexE != std::string::npos)  {
    indexWait = profile_privacy.find(",", indexE);
    if(indexWait == std::string::npos)  {
      indexWait =profile_privacy.find("}",indexB);
    }  
    bew_privacy += profile_privacy.substr(indexE, indexWait - indexE);
    bew_privacy += ",";
  }
  if(indexW != std::string::npos)  {
    indexWait = profile_privacy.find(",", indexW);
    if(indexWait == std::string::npos)  {
      indexWait =profile_privacy.find("}",indexB);
    }  
    bew_privacy += profile_privacy.substr(indexW, indexWait - indexW);
  }
  bew_privacy += "}";
  return bew_privacy;
}

void SearchCacheData::setConfigProperties(Row& row) {
  const char* browse_config = "browse_config";
  const char* status_config = "status_config";
  const char* basic_config = "basic_config";
  const char* profile_privacy = "profile_privacy";
  RWRecMutex::WLock lock(_dataMutex);
  _browse_config = (int) row[browse_config];
  _status_config = (int) row[status_config];
  _basic_config = (int) row[basic_config];
  _profile_privacy = splitProfilePrivacy((std::string) row[profile_privacy]);

}

void SearchCacheData::setUserAuthProperties(Row& row)  {
  const char * auth = "auth";
  RWRecMutex::WLock lock(_dataMutex);
  _auth = (int) row[auth];
}

void SearchCacheData::setViewCountProperties(Row& row) {
  const char* viewcount = "viewcount";

  RWRecMutex::WLock lock(_dataMutex);
  _viewcount = (int) row[viewcount];
}

void SearchCacheData::setUserNameProperties(Row& row){
  const char* signature = "nickname";
  const char* name = "name";

  RWRecMutex::WLock lock(_dataMutex);
#ifndef NEWARCH
  _signature = row[signature].get_string();
  _name = row[name].get_string();
#else
  _signature = row[signature].c_str();
  _name = row[name].c_str();
#endif
  //MCE_DEBUG("[DEBUG]setUserNameProperties: _signature is "<<_signature<<" _name is "<<_name);
}

void SearchCacheData::setUserStageProperties(Row& row){
  const char* stage= "stage";

  RWRecMutex::WLock lock(_dataMutex);
  _stage = (int) row[stage];
  //MCE_DEBUG("[DEBUG]setUserStageProperties: _stage is "<<_stage);
}



void SearchCacheData::setInterestProperties(Row& row){
  const char* interest = "interest";

  RWRecMutex::WLock lock(_dataMutex);
#ifndef NEWARCH
  _interest = row[interest].get_string();
#else
  _interest = row[interest].c_str();
#endif
}

void SearchCacheData::setMusicProperties(Row& row){
  const char* music = "music";

  RWRecMutex::WLock lock(_dataMutex);
#ifndef NEWARCH
  _music = row[music].get_string();
#else
  _music = row[music].c_str();
#endif
}

void SearchCacheData::setMovieProperties(Row& row){
  const char* movie = "movie";

  RWRecMutex::WLock lock(_dataMutex);
#ifndef NEWARCH
  _movie = row[movie].get_string();
#else
  _movie = row[movie].c_str();
#endif
}

void SearchCacheData::setGameProperties(Row& row){
  const char* game = "game";

  RWRecMutex::WLock lock(_dataMutex);
#ifndef NEWARCH
  _game = row[game].get_string();
#else
  _game = row[game].c_str();
#endif
}

void SearchCacheData::setComicProperties(Row& row){
  const char* comic = "comic";

  RWRecMutex::WLock lock(_dataMutex);
#ifndef NEWARCH
  _comic = row[comic].get_string();
#else
  _comic = row[comic].c_str();
#endif
}

void SearchCacheData::setSportProperties(Row& row){
  const char* sport = "sport";

  RWRecMutex::WLock lock(_dataMutex);
#ifndef NEWARCH
  _sport = row[sport].get_string();
#else
  _sport = row[sport].c_str();
#endif
}

void SearchCacheData::setBookProperties(Row& row){
  const char* book = "book";

  RWRecMutex::WLock lock(_dataMutex);
#ifndef NEWARCH
  _book = row[book].get_string();
#else
  _book = row[book].c_str();
#endif
}

void SearchCacheData::setSocietyProperties(Row& row){
  const char* society = "society";

  RWRecMutex::WLock lock(_dataMutex);
#ifndef NEWARCH
  _society = row[society].get_string();
#else
  _society = row[society].c_str();
#endif
}

void SearchCacheData::setLastlogintimeProperties(Row& row){
  const char* lastlogintime = "lastlogintime";

  RWRecMutex::WLock lock(_dataMutex);
#ifndef NEWARCH
  _lastlogintime = row[lastlogintime].get_string();
#else
  _lastlogintime = row[lastlogintime].c_str();
#endif
}

void SearchCacheData::setUserStateProperties(Row& row){
  const char* state = "state";
  const char* star = "star";

  RWRecMutex::WLock lock(_dataMutex);
  _state = (int) row[state];
  _star = (int) row[star];

  //MCE_DEBUG("[DEBUG]setUserStateProperties: _state is "<<_state<<", _star is "<<_star);
}

void SearchCacheData::setUserUrlProperties(Row& row) {
  RWRecMutex::WLock lock(_dataMutex);
  const char* headurl = "headurl";
  const char* tinyurl = "tinyurl";
#ifndef NEWARCH
  _headurl = row[headurl].get_string();
  _tinyurl = row[tinyurl].get_string();
#else
  _headurl = row[headurl].c_str();
  _tinyurl = row[tinyurl].c_str();
#endif
  //MCE_DEBUG("[DEBUG]setUserUrlProperties: _headurl is "<<_headurl<<", _tinyurl is "<<_tinyurl);
}

void SearchCacheData::setUserBornProperties(Row& row) {
  const char* birth_year = "birth_year";
  const char* birth_month = "birth_month";
  const char* birth_day = "birth_day";
  const char* gender = "gender";
  const char* home_city = "home_city";
  const char* home_province = "home_province";
  const char* astrology = "astrology";

  RWRecMutex::WLock lock(_dataMutex);
#ifndef NEWARCH
  _gender = row[gender].get_string();
  _home_city= row[home_city].get_string();
  _home_province =row[home_province].get_string();
  _astrology =row[astrology].get_string();
#else
  _gender = row[gender].c_str();
  _home_city = row[home_city].c_str();
  _home_province = row[home_province].c_str();
  _astrology = row[astrology].c_str();
#endif
  //以下三个域偶尔会出现值为NULL的情况，此时会
  //抛出异常 "Bad type conversion: "NULL" incompatible with "i" type
  try {
    _birthday_year = (int) row[birth_year];
  } catch (mysqlpp::Exception& e) {
    _birthday_year = 0;
  }


  try {
    _birthday_month = (int) row[birth_month];
  } catch (mysqlpp::Exception& e) {
    _birthday_month = 0;
  }

  try {
    _birthday_day = (int) row[birth_day];
  } catch (mysqlpp::Exception& e) {
    _birthday_day = 0;
  }

}


void SearchCacheData::setUserStatusProperties(Row& row) {
  const char* status = "status";

  RWRecMutex::WLock lock(_dataMutex);
  _status = (int)row[status];

  //MCE_DEBUG("[DEBUG]setUserStatusProperties: _status is "<<_status);
}

void SearchCacheData::setUserFondProperties(Row& row) {
  //MCE_DEBUG("[DEBUG]entering setUserFondProperties");
  const char* content= "content";
  const char* type = "type";

  RWRecMutex::WLock lock(_dataMutex);
  int temp_type = (int)row[type];

#ifndef NEWARCH
  switch(temp_type)
  {
    case 2:
      _interest = row[content].get_string();
      break;
    case 3:
      _music = row[content].get_string();
      break;
    case 4:
      _book = row[content].get_string();
      break;
    case 5:
      _movie = row[content].get_string();
      break;
    case 6:
      _game = row[content].get_string();
      break;
    case 7:
      _comic = row[content].get_string();
      break;
    case 8:
      _sport = row[content].get_string();
      break;
    case 9:
      _society = row[content].get_string();
      break;
    default:
      break;
  }
#else
  switch(temp_type)
  {
    case 2:
      _interest = row[content].c_str();
      break;
    case 3:
      _music = row[content].c_str();
      break;
    case 4:
      _book = row[content].c_str();
      break;
    case 5:
      _movie = row[content].c_str();
      break;
    case 6:
      _game = row[content].c_str();
      break;
    case 7:
      _comic = row[content].c_str();
      break;
    case 8:
      _sport = row[content].c_str();
      break;
    case 9:
      _society = row[content].c_str();
      break;
    default:
      break;
  }

#endif
  //MCE_DEBUG("[DEBUG]setUserFondProperties: type is "<<temp_type);
}


void SearchCacheData::setBasicProperties(Row& row) {
  const char* status = "status";
  const char* star = "star";
  const char* stage = "stage";
  const char* birth_year = "birth_year";
  const char* birth_month = "birth_month";
  const char* birth_day = "birth_day";
  const char* name = "name";
  const char* gender = "gender";
  const char* home_city = "home_city";
  const char* home_province = "home_province";
  const char* astrology = "astrology";

  RWRecMutex::WLock lock(_dataMutex);
  _status = (int) row[status];
  _star = (int) row[star];
  _stage = (int) row[stage];
  _birthday_year = (int) row[birth_year];
  _birthday_month = (int) row[birth_month];
  _birthday_day = (int) row[birth_day];
#ifndef NEWARCH
  _name = row[name].get_string();
  _gender = row[gender].get_string();
  _home_city= row[home_city].get_string();
  _home_province =row[home_province].get_string();
  _astrology =row[astrology].get_string();
#else
  _name = row[name].c_str();
  _gender = row[gender].c_str();
  _home_city = row[home_city].c_str();
  _home_province = row[home_province].c_str();
  _astrology = row[astrology].c_str();
#endif
}

BinStringPtr SearchCacheData::Serialize() const {
  try {
    BinStringPtr binString(new BinString());
    //SearchCacheData需要序列化的总的字段数
    int totalField = 0;
    binString->add(totalField);

    //序列化userid
    SearchCacheDataSerialize::SerializeToBinString(binString, USERID, _id, totalField);

    //序列化用户名
    SearchCacheDataSerialize::SerializeToBinString(binString, USERNAME, _name, totalField);

    //序列化性别
    SearchCacheDataSerialize::SerializeToBinString(binString, GENDER, _gender, totalField);

    //序列化signature
    SearchCacheDataSerialize::SerializeToBinString(binString, SIGNATURE, _signature, totalField);

    //序列化star
    SearchCacheDataSerialize::SerializeToBinString(binString, STAR, _star, totalField);

    //序列化status
    SearchCacheDataSerialize::SerializeToBinString(binString, STATUS, _status, totalField);

    //序列化state
    SearchCacheDataSerialize::SerializeToBinString(binString, STATE, _state, totalField);

    //序列化stage
    SearchCacheDataSerialize::SerializeToBinString(binString, STAGE, _stage, totalField);

    //序列化birthday_year
    SearchCacheDataSerialize::SerializeToBinString(binString, BIRTHYEAR, _birthday_year, totalField);

    //序列化birthday_month
    SearchCacheDataSerialize::SerializeToBinString(binString, BIRTHMONTH, _birthday_month, totalField);

    //序列化birthday_day
    SearchCacheDataSerialize::SerializeToBinString(binString, BIRTHDAY, _birthday_day, totalField);

    //序列化lastlogintime
    SearchCacheDataSerialize::SerializeToBinString(binString, LASTLOGINTIME, _lastlogintime, totalField);

    //序列化tinyurl
    SearchCacheDataSerialize::SerializeToBinString(binString, TINYURL, _tinyurl, totalField);

    //序列化headurl
    SearchCacheDataSerialize::SerializeToBinString(binString, HEADURL, _headurl, totalField);

    //序列化astrology
    SearchCacheDataSerialize::SerializeToBinString(binString, ASTROLOGY, _astrology, totalField);

    //序列化browse_config
    SearchCacheDataSerialize::SerializeToBinString(binString, BROWSECONFIG, _browse_config, totalField);

    //序列化_status_config
    SearchCacheDataSerialize::SerializeToBinString(binString, STATUSCONFIG, _status_config, totalField);

    //序列化basic_config
    SearchCacheDataSerialize::SerializeToBinString(binString, BASICCONFIG, _basic_config, totalField);

    //序列化auth
    SearchCacheDataSerialize::SerializeToBinString(binString, AUTH, _auth, totalField);

    //序列化viewcount
    SearchCacheDataSerialize::SerializeToBinString(binString, VIEWCOUNT, _viewcount, totalField);

    //序列化home_city
    SearchCacheDataSerialize::SerializeToBinString(binString, HOMECITY, _home_city, totalField);

    //序列化home_province
    SearchCacheDataSerialize::SerializeToBinString(binString, HOMEPROVINCE, _home_province, totalField);

    //序列化_interest
    SearchCacheDataSerialize::SerializeToBinString(binString, INTEREST, _interest, totalField);

    //序列化_music
    SearchCacheDataSerialize::SerializeToBinString(binString, MUSIC, _music, totalField);

    //序列化_movie
    SearchCacheDataSerialize::SerializeToBinString(binString, MOVIE, _movie, totalField);

    //序列化_game
    SearchCacheDataSerialize::SerializeToBinString(binString, GAME, _game, totalField);

    //序列化_comic
    SearchCacheDataSerialize::SerializeToBinString(binString, COMIC, _comic, totalField);

    //序列化_sport
    SearchCacheDataSerialize::SerializeToBinString(binString, SPORT, _sport, totalField);

    //序列化_book
    SearchCacheDataSerialize::SerializeToBinString(binString, BOOK, _book, totalField);

    //序列化_society
    SearchCacheDataSerialize::SerializeToBinString(binString, SOCIETY, _society, totalField);

    //序列化_profile_privacy
    SearchCacheDataSerialize::SerializeToBinString(binString, PROFILEPRIVACY, _profile_privacy, totalField);

    int total_size = 0;
    //序列化network vector
    if (_networks.size() > 0) {
      int vec_size = 0;
      //获得neworks所占用的总的字节数
      total_size = sizeof((int)_networks.size());
      BinStringPtr bstr(new BinString());
      for(vector<Network>::const_iterator it = _networks.begin();
          it != _networks.end(); ++it) {
        if ((*it).name.empty()) continue;
        vec_size++;
        total_size += sizeof((*it).id);
        total_size += sizeof((*it).stage);
        total_size += sizeof(int) + (*it).name.length();
        bstr->add((*it).id);
        bstr->add((*it).stage);
        bstr->add((int)(*it).name.length());
        bstr->add((*it).name);
      }

      SearchCacheDataSerialize::SerializeToBinString(binString, NETWORKS, total_size, vec_size, 
          *bstr.get(), totalField); 
    }

    //序列化region vector
    if (_regions.size() > 0) {
      int vec_size = 0;
      //获得regions所占用的总的字节数
      total_size = sizeof((int)_regions.size());
      BinStringPtr bstr(new BinString());
      for(vector<Region>::const_iterator it = _regions.begin();
          it != _regions.end(); ++it) {
        if ((*it).city.empty() && (*it).province.empty()) continue;
        vec_size++;
        total_size += sizeof((*it).id);
        total_size += sizeof(int) + (*it).city.length();
        total_size += sizeof(int) + (*it).province.length();
        bstr->add((*it).id);
        bstr->add((int)(*it).city.length());
        bstr->add((*it).city);
        bstr->add((int)(*it).province.length());
        bstr->add((*it).province);
      }
      SearchCacheDataSerialize::SerializeToBinString(binString, REGIONS, total_size, vec_size, 
          *bstr.get(), totalField); 
    }

    //序列化workplace vector
    if (_workplaces.size() > 0) {
      int vec_size = 0;
      //获得workplaces所占用的总的字节数
      total_size = sizeof((int)_workplaces.size());
      BinStringPtr bstr(new BinString());
      for(vector<Workplace>::const_iterator it = _workplaces.begin();
          it != _workplaces.end(); ++it) {
        if ((*it).name.empty()) continue;
        vec_size++;
        total_size += sizeof((*it).id);
        total_size += sizeof(int) + (*it).name.length();
        bstr->add((*it).id);
        bstr->add((int)(*it).name.length());
        bstr->add((*it).name);
      }
      SearchCacheDataSerialize::SerializeToBinString(binString, WORKPLACES, total_size, vec_size, 
          *bstr.get(), totalField); 
    }

    //序列化university vector
    if (_universities.size() > 0) {
      int vec_size = 0;
      //获得universities所占用的总的字节数
      total_size = sizeof((int)_universities.size());
      BinStringPtr bstr(new BinString());
      for(vector<University>::const_iterator it = _universities.begin();
          it != _universities.end(); ++it) {
        if ((*it).name.empty()) continue;
        vec_size++;
        total_size += sizeof((*it).id);
        total_size += sizeof((*it).year);
        total_size += sizeof(int) + (*it).name.length();
        total_size += sizeof(int) + (*it).department.length();
        total_size += sizeof(int) + (*it).dorm.length();
        bstr->add((*it).id);
        bstr->add((*it).year);
        bstr->add((int)(*it).name.length());
        bstr->add((*it).name);
        bstr->add((int)(*it).department.length());
        bstr->add((*it).department);
        bstr->add((int)(*it).dorm.length());
        bstr->add((*it).dorm);
      }
      SearchCacheDataSerialize::SerializeToBinString(binString, UNIVERSITIES, total_size, vec_size, 
          *bstr.get(), totalField); 
    }

    //序列化college vector
    if (_colleges.size() > 0 ) {
      int vec_size = 0;
      //获得colleges所占用的总的字节数
      total_size = sizeof((int)_colleges.size());
      BinStringPtr bstr(new BinString());
      for(vector<College>::const_iterator it = _colleges.begin();
          it != _colleges.end(); ++it) {
        if ((*it).name.empty()) continue;
        vec_size++;
        total_size += sizeof((*it).id);
        total_size += sizeof((*it).year);
        total_size += sizeof(int) + (*it).name.length();
        total_size += sizeof(int) + (*it).department.length();
        bstr->add((*it).id);
        bstr->add((*it).year);
        bstr->add((int)(*it).name.length());
        bstr->add((*it).name);
        bstr->add((int)(*it).department.length());
        bstr->add((*it).department);
      }
      SearchCacheDataSerialize::SerializeToBinString(binString, COLLEGES, total_size, vec_size, 
          *bstr.get(), totalField); 
    }

    //序列化high school vector
    if (_highSchools.size() > 0) {
      int vec_size = 0;
      //获得highSchool所占用的总的字节数
      total_size = sizeof((int)_highSchools.size());
      BinStringPtr bstr(new BinString());
      for(vector<HighSchool>::const_iterator it = _highSchools.begin();
          it != _highSchools.end(); ++it) {
        if ((*it).name.empty()) continue;
        vec_size++;
        total_size += sizeof((*it).id);
        total_size += sizeof((*it).year);
        total_size += sizeof(int) + (*it).name.length();
        total_size += sizeof(int) + (*it).class1.length();
        total_size += sizeof(int) + (*it).class2.length();
        total_size += sizeof(int) + (*it).class3.length();
        bstr->add((*it).id);
        bstr->add((*it).year);
        bstr->add((int)(*it).name.length());
        bstr->add((*it).name);
        bstr->add((int)(*it).class1.length());
        bstr->add((*it).class1);
        bstr->add((int)(*it).class2.length());
        bstr->add((*it).class2);
        bstr->add((int)(*it).class3.length());
        bstr->add((*it).class3);
      }
      SearchCacheDataSerialize::SerializeToBinString(binString, HIGHSCHOOLS, total_size, vec_size, 
          *bstr.get(), totalField); 
    }

    //序列化junior school vector
    if (_juniorSchools.size() > 0) {
      int vec_size = 0;
      //获得juniorSchool所占用的总的字节数
      total_size = sizeof((int)_juniorSchools.size());
      BinStringPtr bstr(new BinString());
      for(vector<JuniorSchool>::const_iterator it = _juniorSchools.begin();
          it != _juniorSchools.end(); ++it) {
        if ((*it).name.empty()) continue;
        vec_size++;
        total_size += sizeof((*it).id);
        total_size += sizeof((*it).year);
        total_size += sizeof(int) + (*it).name.length();
        bstr->add((*it).id);
        bstr->add((*it).year);
        bstr->add((int)(*it).name.length());
        bstr->add((*it).name);
      }
      SearchCacheDataSerialize::SerializeToBinString(binString, JUNIORSCHOOLS, total_size, vec_size, 
          *bstr.get(), totalField); 
    }

    //序列化elementary school vector
    if (_elementarySchools.size() > 0) {
      int vec_size = 0;
      //获得elementarySchool所占用的总的字节数
      total_size = sizeof((int)_elementarySchools.size());
      BinStringPtr bstr(new BinString());
      for(vector<ElementarySchool>::const_iterator it = _elementarySchools.begin();
          it != _elementarySchools.end(); ++it) {
        if ((*it).name.empty()) continue;
        vec_size++;
        total_size += sizeof((*it).id);
        total_size += sizeof((*it).year);
        total_size += sizeof(int) + (*it).name.length();
        bstr->add((*it).id);
        bstr->add((*it).year);
        bstr->add((int)(*it).name.length());
        bstr->add((*it).name);
      }
      SearchCacheDataSerialize::SerializeToBinString(binString, ELEMENTARYSCHOOLS, total_size, vec_size, 
          *bstr.get(), totalField); 
    }

    //添加总有效字节数
    binString->add(binString->getBufferLen());
    //回写总字段数
    char*p;
    int len;
    binString->getBuf(&p, &len);
    if (len > 0) {
      int *pBuf = (int*)p;
      *pBuf = totalField;
    }
    return binString;
  } catch (...) {
    BinStringPtr binString((BinString*)0);
    MCE_WARN("serialize id =" << _id << " failed! " );
    return binString;
  }
}

bool SearchCacheData::Unserialize(const char *buf, unsigned int lens) {
  const char* p = buf;
  int total = 0; 

  if (buf == NULL || lens <= 0) return false;

  //获得总的有效字节数
  total  = *(int*)(p+lens-sizeof(int));

  if (lens-sizeof(int) != total) {
    MCE_WARN("total lens is error! total=" << total << "lens = " << lens);
    return false;
  }

  if (total <= 0) return false;

  int count = 0;
  int vector_size = 0;

  if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, count))
    return false;

  for(int i=0; i<count; ++i) {
    //获得标签
    char lable = 0;
    if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, lable))
      return false;

    //获得长度
    int lens = 0;
    if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, lens))
      return false;

    if (lens < 0)
      continue;

    if (lens > total)
      return false;

    switch(lable) {
      case USERID:
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _id))
          return false;
        break;
      case USERNAME: 
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _name))
          return false;
        break;
      case GENDER:
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _gender))
          return false;
        break;
      case SIGNATURE: 
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _signature))
          return false;
        break;
      case STAR: 
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _star))
          return false;
        break;
      case STATUS:
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _status))
          return false;
        break;
      case STATE:
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _state))
          return false;
        break;
      case STAGE:
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _stage))
          return false;
        break;
      case BIRTHYEAR:
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _birthday_year))
          return false;
        break;
      case BIRTHMONTH:
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _birthday_month))
          return false;
        break;
      case BIRTHDAY:
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _birthday_day))
          return false;
        break;
      case LASTLOGINTIME:
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _lastlogintime))
          return false;
        break;
      case TINYURL: 
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _tinyurl))
          return false;
        break;
      case HEADURL: 
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _headurl))
          return false;
        break;
      case ASTROLOGY: 
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _astrology))
          return false;
        break;
      case BROWSECONFIG: 
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _browse_config))
          return false;
        break;
      case AUTH: 
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _auth))
          return false;
        break;
      case STATUSCONFIG: 
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _status_config))
          return false;
        break;
      case BASICCONFIG: 
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _basic_config))
          return false;
        break;
      case VIEWCOUNT: 
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _viewcount))
          return false;
        break;
      case HOMECITY: 
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _home_city))
          return false;
        break;
      case HOMEPROVINCE: 
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _home_province))
          return false;
        break;
      case INTEREST: 
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _interest))
          return false;
        break;
      case MUSIC: 
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _music))
          return false;
        break;
      case MOVIE: 
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _movie))
          return false;
        break;
      case GAME: 
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _game))
          return false;
        break;
      case COMIC: 
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _comic))
          return false;
        break;
      case SPORT: 
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _sport))
          return false;
        break;
      case BOOK: 
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _book))
          return false;
        break;
      case SOCIETY: 
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _society))
          return false;
        break;
      case PROFILEPRIVACY: 
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, _profile_privacy))
          return false;
        break;

      case NETWORKS: 
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, vector_size))
          return false;
        for(int i = 0; i<vector_size; ++i) {
          Network network;
          if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, network.id))
            return false;
          if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, network.stage))
            return false;
          if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, network.name))
            return false;
          _networks.push_back(network);
        }
        break;
      case REGIONS:
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, vector_size))
          return false;
        for(int i = 0; i<vector_size; ++i) {
          Region region;
          if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, region.id))
            return false;
          if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, region.city))
            return false;
          if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, region.province))
            return false;
          _regions.push_back(region);
        }
        break;
      case WORKPLACES: 
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, vector_size))
          return false;
        for(int i = 0; i<vector_size; ++i) {
          Workplace workplace;
          if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, workplace.id))
            return false;
          if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, workplace.name))
            return false;
          _workplaces.push_back(workplace);
        }
        break;
      case UNIVERSITIES: 
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, vector_size))
          return false;
        for(int i = 0; i<vector_size; ++i) {
          University university;
          if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, university.id))
            return false;
          if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, university.year))
            return false;
          if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, university.name))
            return false;
          if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, university.department))
            return false;
          if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, university.dorm))
            return false;
          _universities.push_back(university);
        }
        break;
      case COLLEGES: 
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, vector_size))
          return false;
        for(int i = 0; i<vector_size; ++i) {
          College college;
          if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, college.id))
            return false;
          if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, college.year))
            return false;
          if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, college.name))
            return false;
          if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, college.department))
            return false;
          _colleges.push_back(college);
        }
        break;
      case HIGHSCHOOLS:
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, vector_size))
          return false;
        for(int i = 0; i<vector_size; ++i) {
          HighSchool highSchool;
          if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, highSchool.id))
            return false;
          if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, highSchool.year))
            return false;
          if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, highSchool.name))
            return false;
          if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, highSchool.class1))
            return false;
          if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, highSchool.class2))
            return false;
          if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, highSchool.class3))
            return false;
          _highSchools.push_back(highSchool);
        }
        break;
      case JUNIORSCHOOLS:
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, vector_size))
          return false;
        for(int i = 0; i<vector_size; ++i) {
          JuniorSchool juniorSchool;
          if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, juniorSchool.id))
            return false;
          if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, juniorSchool.year))
            return false;
          if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, juniorSchool.name))
            return false;
          _juniorSchools.push_back(juniorSchool);
        }
        break;
      case ELEMENTARYSCHOOLS:
        if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, vector_size))
          return false;
        for(int i = 0; i<vector_size; ++i) {
          ElementarySchool elementarySchool;
          if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, elementarySchool.id))
            return false;
          if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, elementarySchool.year))
            return false;
          if (!SearchCacheDataSerialize::UnserializeFromBuffer(&p, total, elementarySchool.name))
            return false;
          _elementarySchools.push_back(elementarySchool);
        }
        break;
      default: //跳过这个指段
        p += lens;
    }
  }
  return true;
}

