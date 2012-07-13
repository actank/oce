#ifndef _COMMON_HEADER_H__
#define _COMMON_HEADER_H__


#include <map>
#include <vector>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <IceUtil/IceUtil.h>
#include <queue>
#include <semaphore.h>
#include <pthread.h>


#include "QueryRunner.h"
#include "ConnectionHolder.h"
#include "UtilCxx/src/LogWrapper.h"
#include "search/SearchCache2/src/Util/SearchCacheData.h"
#include "search/SearchCache2/src/Util/IDbCache.h"
#include "search/SearchCache2/src/Util/DatabaseCacheFactory.h"
#include "search/SearchCache2/src/Util/SearchCacheUtil.h"

using namespace std;
using namespace xce::searchcache; 
using namespace com::xiaonei::xce;
using namespace MyUtil;
using namespace xce;


const string DB_SEARCH2_USERBASIC = "search2_userbasic";
const string DB_SEARCH2_USERBORN = "search2_userborn";
const string DB_SEARCH2_USERFOND = "search2_userfond";

const string TABLE_USER_NETWORK_HISTORY = "network_history";

const string DB_SEARCH2_USERBASIC_STATUS = "search_user_passport";

const string DB_SEARCH2_USERCONFIG = "search2_userconfig";
const string TABLE_USER_CONFIG = "user_config";
const string DB_SEARCH2_USERURL = "user_url";

const string DB_SEARCH2_USERTIME = "search2_usertime";

const string DB_SEARCH2_USERNAME = "user_names";
const string TABLE_USERS = "user_names";

const string DB_SEARCH2_USERSTATE = "search2_user";
const string TABLE_STATE = "user_state";

const string DB_SEARCH2_USERSTAGE = "search2_userstage";

const string DB_VIEW_COUNT = "user_view_count";

const string DB_USER_RIGHT = "user_right_preloader";

const string TABLE_RIGHT = "user_right";

const int TASK_LEVEL_FILL = 1;
const int TASK_LEVEL_FILL_BASIC = 2;
const int TASK_LEVEL_FILL_CONFIG = 3;
const int TASK_LEVEL_FILL_USERS = 8;

const int TASK_LEVEL_UPDATE_DBCACHE = 9;

const int FILL_BATCH_SIZE = 3000;
const int UPDATE_VIEW_COUNT_BATCH_SIZE = 300;
const string USER_STATUS_FIELD = " id, status ";
const string USER_STAGE_FIELD = " id, stage ";
const string USER_BORN_FIELD = " id, birth_day,birth_month,birth_year,astrology,gender,home_province,home_city ";

const string USER_URL_FIELD = " id, tinyurl, headurl ";
const string USER_CONFIG_FIELD = " id, browse_config, status_config, basic_config, profile_privacy ";
const string USER_STATE_FIELD = " id, state, star ";
const string NETWORK_HISTORY_FIELD = " userid, network_id, network_name, stage ";
const string ELEMENTARY_SCHOOL_INFO_FIELD = " userid, elementary_school_id, elementary_school_name,elementary_school_year ";
const string JUNIOR_HIGH_SCHOOL_INFO_FIELD = " userid, junior_high_school_id, junior_high_school_name, junior_high_school_year ";
const string HIGH_SCHOOL_INFO_FIELD = " userid, high_school_id, high_school_name, enroll_year, h_class1, h_class2, h_class3 ";
const string COLLEGE_INFO_FIELD = " userid, college_id, college_name, department,enroll_year ";
const string UNIVERSITY_INFO_FIELD  = " userid, university_id, university_name, department, enroll_year, dorm ";
const string WORKPLACE_INFO_FIELD = " userid, workplace_id, workplace_name ";
const string REGION_INFO_FIELD = " userid, region_id, province_name, city_name ";
const string USER_FOND_FIELD = " user_id, type, content ";
const string USER_TIME_FIELD = " id, lastlogintime ";
const string USER_NAME_FIELD = " id, nickname, name ";
const string USER_VIEWCOUNT_FIELD = " id, viewcount ";
const string USER_AUTH_FIELD = " id, auth ";

#endif
