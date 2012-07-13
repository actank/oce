/*
 * SearchCacheData.h
 *
 *  Created on: 2009-4-20
 *      Author: ning.zhao@opi-corp.com
 */

#ifndef _SEARCHCACHEDATA_H_
#define _SEARCHCACHEDATA_H_

#include "SearchCache.h"
#include <QueryRunner.h>
#include <IceUtil/IceUtil.h>

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
const std::string SEARCH_CACHE_SELECTED = "selected";
const std::string SEARCH_CACHE_STATUS = "status";
const std::string SEARCH_CACHE_TINYURL = "tinyurl";
const std::string SEARCH_CACHE_HEADURL = "headurl";
const std::string SEARCH_CACHE_BROWSE_CONFIG = "browse_config";
const std::string SEARCH_CACHE_STATUS_CONFIG = "status_config";
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

struct Network{
	int id;
	int stage;
	std::string name;
};

struct ElementarySchool{
	int id;
	int year;
	string name;
};

struct JuniorSchool{
	int id;
	int year;
	string name;
};

struct HighSchool{
	int id;
	int year;
	string name;
};

struct College{
	int id;
	string name;
	string department;
	int year;
};

struct University{
	int id;
	int year;
	string name;
	string department;
};

struct Workplace{
	int id;
	string name;
};

struct Region{
	int id;
	string city;
	string province;
};

class SearchCacheData: virtual public Ice::Object {
public:
	SearchCacheData();
	SearchCacheResult getProperties();
	void setElementarySchoolProperties(mysqlpp::Row& row, const char* fieldElementarySchoolId,
			const char* fieldElementarySchoolYear, const char* fieldElementarySchoolName);
	void setJuniorSchoolProperties(mysqlpp::Row& row, const char* fieldJuniorSchoolId,
			const char* fieldJuniorSchoolYear, const char* fieldJuniorSchoolName);
	void setHighSchoolProperties(mysqlpp::Row& row, const char* fieldHighSchoolId, 
			const char* fieldHighSchoolYear, const char* fieldHighSchoolName);
	void setCollegeProperties(mysqlpp::Row& row, const char* fieldCollegeId,
			const char* fieldCollegeName, const char* fieldCollegeDepartment,
			const char* fieldCollegeYear);
	void setUniversityProperties(mysqlpp::Row& row, const char* fieldUniversityId,
			const char* fieldUniversityYear, const char* fieldUniversityName,
			const char* fieldUniversityDepartment);
	void setWorkplaceProperties(mysqlpp::Row& row, const char* fieldWorkplaceId, 
			const char* fieldWorkplaceName);
	void setRegionProperties(mysqlpp::Row& row, const char* fieldRegionId, 
			const char* fieldRegionCity, const char* fieldRegionProvince);
	void setNetworkProperties(mysqlpp::Row& row, const char* fieldNetworkId, 
			const char* fieldNetworkName, const char* fieldNetworkStage);
	void setConfigProperties(mysqlpp::Row& row, const char* fieldBrowseConfig,
			const char* fieldStatusConfig);
	void setUserUrlProperties(mysqlpp::Row& row, 
			const char* fieldTinyUrl, const char* fieldHeadUrl);
	void setBasicProperties(mysqlpp::Row& row, const char* fieldName, 
			const char* fieldStar, const char* fieldSelected, 
			const char* fieldStatus, const char* fieldHomeCity,
			const char* fieldHomeProvince);
	void setUsersProperties(mysqlpp::Row& row, const char* fieldSignature);
public:
	int _id;
	std::string _name;
	std::string _signature;
	int _star;
	int _selected;
	int _status;
	std::string _tinyurl;
	std::string _headurl;
	int _browse_config;
	int _status_config;
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
};

typedef IceUtil::Handle<SearchCacheData> SearchCacheDataPtr;
typedef std::vector<SearchCacheDataPtr> SearchCacheDataSeq;

}
}

#endif
