/*
 * SearchMemCacheData.h
 *
 *  Created on: 2009-4-20
 *      Author: ning.zhao@opi-corp.com
 */

#ifndef _SEARCHMEMCACHEDATA_H_
#define _SEARCHMEMCACHEDATA_H_

#include <QueryRunner.h>
#include <IceUtil/IceUtil.h>
#include <ServiceI.h>

#include <SearchCache.h>
#include "BinString.h"
#include "SearchCacheDataSerialize.h"
#include "SearchCacheData.h"

namespace xce {
namespace searchcache {

const std::string SEARCH_CACHE_TYPE_NETWORK = "network";
const std::string SEARCH_CACHE_TYPE_JUNIOR_SCHOOL = "junior_school";
const std::string SEARCH_CACHE_TYPE_HIGH_SCHOOL = "high_school";
const std::string SEARCH_CACHE_TYPE_ELEMENTARY_SCHOOL = "elementary_school";
const std::string SEARCH_CACHE_TYPE_COLLEGE = "college";
const std::string SEARCH_CACHE_TYPE_UNIVERSITY = "university";
const std::string SEARCH_CACHE_TYPE_WORKPLACE = "workplace";
const std::string SEARCH_CACHE_TYPE_REGION = "region";

const std::string SEARCH_CACHE_ID = "user_basic.id";
const std::string SEARCH_CACHE_NAME = "name";
const std::string SEARCH_CACHE_SIGNATURE = "signature";
const std::string SEARCH_CACHE_STAR = "star";
const std::string SEARCH_CACHE_STATE= "state";
const std::string SEARCH_CACHE_STAGE= "stage";
const std::string SEARCH_CACHE_SELECTED= "selected";
const std::string SEARCH_CACHE_GENDER = "gender";
const std::string SEARCH_CACHE_BIRTHDAY_YEAR = "birth_year";
const std::string SEARCH_CACHE_STATUS = "status";
const std::string SEARCH_CACHE_TINYURL = "tinyurl";
const std::string SEARCH_CACHE_HEADURL = "headurl";
const std::string SEARCH_CACHE_BROWSE_CONFIG = "browse_config";
const std::string SEARCH_CACHE_STATUS_CONFIG = "status_config";
const std::string SEARCH_CACHE_BASIC_CONFIG  = "basic_config";
const std::string SEARCH_CACHE_AUTH = "auth";
const std::string SEARCH_CACHE_PROFILE_PRIVACY ="profile_privacy";
const std::string SEARCH_CACHE_VIEW_COUNT  = "viewcount";
const std::string SEARCH_CACHE_HOME_CITY = "home_city";
const std::string SEARCH_CACHE_HOME_PROVINCE = "home_province";
const std::string SEARCH_CACHE_NETWORK_ID = "network_id";
const std::string SEARCH_CACHE_NETWORK_NAME = "network_name";
const std::string SEARCH_CACHE_NETWORK_STAGE = "network_history.stage";
const std::string SEARCH_CACHE_ELEMENTARY_SCHOOL_ID = "elementary_school_info.elementary_school_id";
const std::string SEARCH_CACHE_ELEMENTARY_SCHOOL_YEAR = "elementary_school_info.elementary_school_year";
const std::string SEARCH_CACHE_ELEMENTARY_SCHOOL_NAME = "elementary_school_info.elementary_school_name";
const std::string SEARCH_CACHE_JUNIOR_SCHOOL_ID = "junior_high_school_info.junior_high_school_id";
const std::string SEARCH_CACHE_JUNIOR_SCHOOL_NAME = "junior_high_school_info.junior_high_school_name";
const std::string SEARCH_CACHE_JUNIOR_SCHOOL_YEAR = "junior_high_school_info.junior_high_school_year";
const std::string SEARCH_CACHE_HIGH_SCHOOL_YEAR = "enroll_year";
const std::string SEARCH_CACHE_HIGH_SCHOOL_NAME = "high_school_name";
const std::string SEARCH_CACHE_HIGH_SCHOOL_ID = "high_school_id";
const std::string SEARCH_CACHE_COLLEGE_ID = "college_info.college_id";
const std::string SEARCH_CACHE_COLLEGE_NAME ="college_info.college_name";
const std::string SEARCH_CACHE_COLLEGE_DEPARTMENT = "college_info.department";
const std::string SEARCH_CACHE_COLLEGE_YEAR = "college_info.enroll_year";
const std::string SEARCH_CACHE_UNIVERSITY_ID = "university_id";
const std::string SEARCH_CACHE_UNIVERSITY_NAME = "university_name";
const std::string SEARCH_CACHE_UNIVERSITY_YEAR = "enroll_year";
const std::string SEARCH_CACHE_UNIVERSITY_DEPARTMENT = "department";
const std::string SEARCH_CACHE_WORKPLACE_ID = "workplace_id";
const std::string SEARCH_CACHE_WORKPLACE_NAME = "workplace_name";
const std::string SEARCH_CACHE_REGION_ID = "region_id";
const std::string SEARCH_CACHE_REGION_PROVINCE = "province_name";
const std::string SEARCH_CACHE_REGION_CITY = "city_name";

struct Network;
struct ElementarySchool;
struct JuniorSchool;
struct HighSchool;
struct College;
struct University;
struct Workplace;
struct Region;
struct ElementarySchool;

class SearchMemCacheData: virtual public Ice::Object {
public:
	SearchMemCacheData();
	SearchCacheResult getProperties();
public:
    //基本信息
	int _id;
	std::string _name;
	std::string _gender;
	std::string _signature;
	int _star;
	int _status;
	int _stage;
    int _birthday_year;
    //int _birthday_month;
    //int _birthday_day;
    int _state;
    //std::string _lastlogintime;
	std::string _tinyurl;
	std::string _headurl;
	//std::string _astrology;
	int _browse_config;
	int _status_config;
	//int _basic_config;
	//int _viewcount;
  int _auth;
  std::string _profile_privacy;
	std::string _home_city;
	std::string _home_province;
  
	std::vector<Network> _networks;
	std::vector<Region> _regions;
	std::vector<Workplace> _workplaces;
	std::vector<University> _universities;
	std::vector<College> _colleges;
	std::vector<HighSchool> _highSchools;
	std::vector<JuniorSchool> _juniorSchools;
	std::vector<ElementarySchool> _elementarySchools;
	IceUtil::RWRecMutex _dataMutex;

    BinStringPtr Serialize() const;
    bool Unserialize(const char*buf, unsigned int lens);
};

typedef IceUtil::Handle<SearchMemCacheData> SearchMemCacheDataPtr;
typedef std::vector<SearchMemCacheDataPtr> SearchMemCacheDataSeq;

}
}

#endif
