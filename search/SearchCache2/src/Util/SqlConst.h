#ifndef __SQLCONST_H__
#define __SQLCONST_H__
const string DB_SEARCH2_USERBASIC = "search2_userbasic";
const string TABLE_USER_BASIC = "user_basic";
const string TABLE_USER_NETWORK_HISTORY = "network_history";

const string DB_SEARCH2_USERCONFIG = "search2_userconfig";
const string TABLE_USER_CONFIG = "user_config";

const string DB_SEARCH2_USERURL = "user_url";

const string DB_SEARCH2_USERTIME = "search2_usertime";

const string DB_SEARCH2_USER = "user_names";
const string TABLE_USERS = "user_names";

const string DB_SEARCH2_USERRIGHT = "user_right_preloader";
const string TABLE_USER_RIGHT = "user_right";

//定义每个表检索的字段
const string USER_BASIC_FIELD = " id, status, star, name, tinyurl, headurl,home_city,home_province, gender, stage, birth_day, birth_month, birth_year, astrology ";
const string USER_URL_FIELD = " id, tinyurl, headurl ";
const string USER_CONFIG_FIELD = " id, browse_config, status_config, profile_privacy ";
const string NETWORK_HISTORY_FIELD = " userid, network_id, network_name, stage "; 
const string ELEMENTARY_SCHOOL_INFO_FIELD = " userid, elementary_school_id, elementary_school_name, elementary_school_year ";
const string JUNIOR_HIGH_SCHOOL_INFO_FIELD = " userid, junior_high_school_id, junior_high_school_name, junior_high_school_year ";
const string HIGH_SCHOOL_INFO_FIELD = " userid, high_school_id, high_school_name, enroll_year, h_class1, h_class2, h_class3 ";
const string COLLEGE_INFO_FIELD = " userid, college_id, college_name, department,enroll_year ";
const string UNIVERSITY_INFO_FIELD  = " userid, university_id, university_name, department, enroll_year, dorm ";
const string WORKPLACE_INFO_FIELD = " userid, workplace_id, workplace_name ";
const string REGION_INFO_FIELD = " userid, region_id, province_name, city_name ";
const string INTEREST_FIELD = " users, interest ";
const string MUSIC_FIELD = " users, music ";
const string MOVIE_FIELD = " users, movie ";
const string GAME_FIELD = " users, game ";
const string COMIC_FIELD = " users, comic ";
const string SPORT_FIELD = " users, sport ";
const string BOOK_FIELD = " users, book ";
const string SOCIETY_FIELD = " users, society ";
const string USER_TIME_FIELD = " id, lastlogintime ";
const string USERS_FIELD = " id, nickname ";
const string USER_AUTH_FIELD = " id, auth ";

#endif

