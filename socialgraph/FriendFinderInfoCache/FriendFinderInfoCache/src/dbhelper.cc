#include "socialgraph/FriendFinderInfoCache/FriendFinderInfoCache/src/dbhelper.h"

#include <boost/algorithm/string.hpp>
#include "base/logging.h"
#include "DbCxxPool/src/QueryRunner.h"

namespace xce {
namespace socialgraph {

void DbHelper::GetChinaArea(const std::vector<std::string>& area_list,
													  std::map<std::string, std::string>& index_area_map) 
{
	using namespace com::xiaonei::xce;

	if (area_list.empty()) {
		return;
	}
	Statement area_sql;

	area_sql << "SELECT * FROM china_area WHERE code in (";
	
	for (std::vector<std::string>::const_iterator iter = area_list.begin();
			iter != area_list.end(); ++iter) {
		area_sql << *iter;
		if (iter != area_list.end() - 1) {
			area_sql << ", ";
		}
	}
	area_sql << ")";
		
	try {
		QueryRunner("high_school_info_friendfinder", CDbRServer).query(area_sql, ChinaAreaHandler(index_area_map));
	} catch (Ice::Exception& e) {
		LOG(INFO) << "[DbHelper] GetChinaArea run Ice Exception " << e.what();
	} catch (...) {
		LOG(INFO) << "[DbHelper] GetChinaArea unknow Exception ";
	}
}

void DbHelper::GetUserGender(const std::vector<int> uid_list,
														 std::map<int, int>& id_gender) 
{
	using namespace com::xiaonei::xce;
	Statement gender_sql;
	gender_sql << "SELECT id, gender FROM user_born WHERE id in (";

	for (std::vector<int>::const_iterator iter = uid_list.begin();
			iter != uid_list.end(); ++iter) {
		gender_sql << *iter;
		if (iter != uid_list.end() - 1) {
			gender_sql << ", ";
		}
	}
	gender_sql << ")";

	try {
		QueryRunner("user_born_friendrank", CDbRServer).query(gender_sql, UserGenderHandler(id_gender));
	} catch (Ice::Exception& e) {
		LOG(INFO) << "[DbHelper] GetUserGender Ice Exception " << e.what();
	} catch (...) {
		LOG(INFO) << "[DbHelper] GetUserGender unknown Exception "; 
	}
}

//-----------------------------------------------------------------------

bool HighSchoolInfoHandler::handle(mysqlpp::Row& row) const {
	last_ = (last_ > (int)row["id"] ? last_ : (int)row["id"]);
	std::string high_school_name(row["high_school_name"]);

	SchoolInfo info;
	info.userid_ = (int)row["userid"];
	info.school_id_ = (int)row["high_school_id"];
	info.school_name_ = high_school_name;
	boost::trim(info.school_name_);
	info.enroll_year_ = (int)row["enroll_year"];

	id_highschool_map_.insert(std::make_pair<int, SchoolInfo>(info.userid_, info));
	return true;
}

//-----------------------------------------------------------------------

bool JuniorHighSchoolInfoHandler::handle(mysqlpp::Row& row) const {
	last_ = (last_ > (int)row["id"] ? last_ : (int)row["id"]);
	std::string junior_high_school_name(row["junior_high_school_name"]);

	SchoolInfo info;
	info.userid_ = (int)row["userid"];
	info.school_id_ = (int)row["junior_high_school_id"];
	info.school_name_ = junior_high_school_name;
	boost::trim(info.school_name_);
	info.enroll_year_ = (int)row["junior_high_school_year"];

	id_juniorhighschool_map_.insert(std::make_pair<int, SchoolInfo>(info.userid_, info));
	return true;
}

//-----------------------------------------------------------------------

bool SpecialSchoolInfoHandler::handle(mysqlpp::Row& row) const {
	last_ = (last_ > (int)row["id"] ? last_ : (int)row["id"]);
	std::string junior_high_school_name(row["college_name"]);

	SchoolInfo info;
	info.userid_ = (int)row["userid"];
	info.school_id_ = (int)row["college_id"];
	info.school_name_ = junior_high_school_name;
	boost::trim(info.school_name_);
	info.enroll_year_ = (int)row["enroll_year"];

	id_specialschool_map_.insert(std::make_pair<int, SchoolInfo>(info.userid_, info));
	return true;
}

//-----------------------------------------------------------------------

bool SchoolCodeHandler::handle(mysqlpp::Row& row) const {
	SchoolCode item;
	item.id_ = row["id"];
	item.code_pro_ = std::string(row["code_pro"]);
	item.code_city_ = std::string(row["code_city"]);
	item.code_county_ = std::string(row["code_county"]);

	school_code_map_.insert(std::make_pair<int, SchoolCode>(item.id_, item));
	return true;
}

//-----------------------------------------------------------------------

bool ChinaAreaHandler::handle(mysqlpp::Row& row) const {
	std::string index(row["code"]);
	std::string name(row["name"]);
	
	index_area_map_.insert(std::make_pair<std::string, std::string>(index, name));
	return true;
}

//-----------------------------------------------------------------------

bool UserGenderHandler::handle(mysqlpp::Row& row) const {
	std::string gender(row["gender"]);
	boost::trim(gender);

	int userid = (int)row["id"];

	if ("男生" == gender) {
		user_gender_.insert(std::make_pair<int, int>(userid, MAN));
	} else {
		user_gender_.insert(std::make_pair<int, int>(userid, WOMAN));
	}
	
	return true;
}

}
}
