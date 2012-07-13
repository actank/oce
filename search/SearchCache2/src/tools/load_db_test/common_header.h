#ifndef _COMMON_HEADER_H__
#define _COMMON_HEADER_H__


#include <map>
#include <vector>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <IceUtil/IceUtil.h>
#include <queue>


#include "QueryRunner.h"
#include "ConnectionHolder.h"
#include "UtilCxx/src/LogWrapper.h"

using namespace std;
using namespace com::xiaonei::xce;
using namespace MyUtil;
using namespace xce;


const string DB_SEARCH2_USERBASIC = "search3_userbasic";
const string DB_SEARCH2_USERBORN = "search3_userborn";
const string DB_SEARCH2_USERFOND = "search3_userfond";

const string TABLE_USER_NETWORK_HISTORY = "network_history";

//const string DB_SEARCH2_USERBASIC_STATUS = "search_user_passport";

const string DB_SEARCH2_USERCONFIG = "search3_userconfig";
const string TABLE_USER_CONFIG = "user_config";
const string DB_SEARCH2_USERURL = "search3_user_url";

const string DB_SEARCH2_USERTIME = "search3_usertime";

const string DB_SEARCH2_USERNAME = "search3_user_names";
const string TABLE_USERS = "user_names";

const string DB_SEARCH2_USERSTATE = "search3_user";
const string TABLE_STATE = "user_state";

const string DB_SEARCH2_USERSTAGE = "search3_userstage";

const string DB_VIEW_COUNT = "search3_user_view_count";

const string DB_USER_RIGHT = "search3_user_right";

const string TABLE_RIGHT = "user_right";

const int TASK_LEVEL_FILL = 1;
const int TASK_LEVEL_FILL_BASIC = 2;
const int TASK_LEVEL_FILL_CONFIG = 3;
const int TASK_LEVEL_FILL_USERS = 8;

const int TASK_LEVEL_UPDATE_DBCACHE = 9;

const size_t FILL_BATCH_SIZE = 3000;
const int UPDATE_VIEW_COUNT_BATCH_SIZE = 300;
const string USER_STATUS_FIELD = " id, status ";
const string USER_STAGE_FIELD = " id, stage ";
const string USER_BORN_FIELD = " id, birth_day,birth_month,birth_year,astrology,gender,home_province,home_city ";

//const string USER_URL_FIELD = " id, tinyurl, headurl ";
const string USER_URL_FIELD = " id, headurl ";
const string USER_CONFIG_FIELD = " id, status_config, profile_privacy ";
//const string USER_CONFIG_FIELD = " id, browse_config, status_config, basic_config, profile_privacy ";
const string USER_STATE_FIELD = " id, state, star ";
const string NETWORK_HISTORY_FIELD = " userid, network_name";
//const string NETWORK_HISTORY_FIELD = " userid, network_id, network_name, stage ";
const string ELEMENTARY_SCHOOL_INFO_FIELD = " userid, elementary_school_name";
//const string ELEMENTARY_SCHOOL_INFO_FIELD = " userid, elementary_school_id, elementary_school_name,elementary_school_year ";
const string JUNIOR_HIGH_SCHOOL_INFO_FIELD = " userid, junior_high_school_name";
//const string JUNIOR_HIGH_SCHOOL_INFO_FIELD = " userid, junior_high_school_id, junior_high_school_name, junior_high_school_year ";
const string HIGH_SCHOOL_INFO_FIELD = " userid, high_school_name";
//const string HIGH_SCHOOL_INFO_FIELD = " userid, high_school_id, high_school_name, enroll_year, h_class1, h_class2, h_class3 ";
const string COLLEGE_INFO_FIELD = " userid, college_name";
//const string COLLEGE_INFO_FIELD = " userid, college_id, college_name, department,enroll_year ";
const string UNIVERSITY_INFO_FIELD  = " userid, university_name";
//const string UNIVERSITY_INFO_FIELD  = " userid, university_id, university_name, department, enroll_year, dorm ";
const string WORKPLACE_INFO_FIELD = " userid, workplace_name ";
const string REGION_INFO_FIELD = " userid, city_name ";
//const string REGION_INFO_FIELD = " userid, region_id, province_name, city_name ";
const string USER_FOND_FIELD = " user_id, type, content ";
const string USER_TIME_FIELD = " id, lastlogintime ";
//const string USER_NAME_FIELD = " id, nickname, name ";
const string USER_NAME_FIELD = " id, name ";
const string USER_VIEWCOUNT_FIELD = " id, viewcount ";
const string USER_AUTH_FIELD = " id, auth ";

const char* stage = "stage";
const char* name = "name";
const char* url = "headurl";
const char* status_config = "status_config";
const char* profile_privacy = "profile_privacy";
const char* network_name = "network_name";
const char* elementary_school_name = "elementary_school_name";
const char* junior_high_school_name = "junior_high_school_name";
const char* high_school_name = "high_school_name";
const char* college_name = "college_name";
const char* university_name = "university_name";
const char* workplace_name = "workplace_name";
const char* region_city_name = "city_name";
struct SearchCacheData {
	std::string stage_;
	std::string name_;
	std::string url_;
	std::string status_config_;
	std::string profile_privacy_;
	std::vector<std::string> networks_;
	std::vector<std::string> elementary_schools_; 
	std::vector<std::string> junior_schools_;
	std::vector<std::string> high_schools_;
	std::vector<std::string> colleges_;
	std::vector<std::string> universitys_;
	std::vector<std::string> workplaces_;
	std::vector<std::string> region_citys_;

	void setStatus(const vector<string>& row){
	}
	void setStage(const vector<string>& row){
		stage_ = row[1].c_str();
	}
	void setName(const vector<string>& row){
		name_ = row[1].c_str();
	}
	void setUrl(const vector<string>& row){
		url_ = row[1].c_str();
	}
    void setConfig(const vector<string>& row){
        status_config_ = row[1].c_str();
        profile_privacy_ = row[2].c_str();
    }
	void setNetworks(const vector<string>& row){
		networks_.push_back(row[1].c_str());
	}
    void setElementarySchools(const vector<string>& row){
        elementary_schools_.push_back(row[1].c_str());
    }
	void setJuniorSchools(const vector<string>& row){
		junior_schools_.push_back(row[1].c_str());	
	}
	void setHighSchools(const vector<string>& row){
		high_schools_.push_back(row[1].c_str());
	}
	void setColleges(const vector<string>& row){
		colleges_.push_back(row[1].c_str());
	}
	void setUniversitys(const vector<string>& row){
		universitys_.push_back(row[1].c_str());
	}
	void setWorkplaces(const vector<string>& row){
		workplaces_.push_back(row[1].c_str());
	}
	void setRegionCitys(const vector<string>& row){
		region_citys_.push_back(row[1].c_str());
	}

/*

	void setStatus(const mysqlpp::Row& row){
	}
	void setStage(const mysqlpp::Row& row){
		stage_ = row[stage].c_str();
	}
	void setName(const mysqlpp::Row& row){
		name_ = row[name].c_str();
	}
	void setUrl(const mysqlpp::Row& row){
		url_ = row[url].c_str();
	}
    void setConfig(const mysqlpp::Row& row){
        status_config_ = row[status_config].c_str();
        profile_privacy_ = row[profile_privacy].c_str();
    }
	void setNetworks(const mysqlpp::Row& row){
		networks_.push_back(row[network_name].c_str());
	}
    void setElementarySchools(const mysqlpp::Row& row){
        elementary_schools_.push_back(row[elementary_school_name].c_str());
    }
	void setJuniorSchools(const mysqlpp::Row& row){
		junior_schools_.push_back(row[junior_high_school_name].c_str());	
	}
	void setHighSchools(const mysqlpp::Row& row){
		high_schools_.push_back(row[high_school_name].c_str());
	}
	void setColleges(const mysqlpp::Row& row){
		colleges_.push_back(row[college_name].c_str());
	}
	void setUniversitys(const mysqlpp::Row& row){
		universitys_.push_back(row[university_name].c_str());
	}
	void setWorkplaces(const mysqlpp::Row& row){
		workplaces_.push_back(row[workplace_name].c_str());
	}
	void setRegionCitys(const mysqlpp::Row& row){
		region_citys_.push_back(row[region_city_name].c_str());
	}
*/
};
typedef boost::shared_ptr<SearchCacheData> SearchCacheDataPtr;
#endif
