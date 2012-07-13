#include "socialgraph/OfferFriends/sameareapeer/dbhelper.h"

#include <boost/algorithm/string.hpp>

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
	std::ostringstream oss;

	oss << "SELECT * FROM china_area WHERE code in (";
	
	for (std::vector<std::string>::const_iterator iter = area_list.begin();
			iter != area_list.end(); ++iter) {
		if (!iter->empty()) {
			oss << *iter << ", ";
		}
	}
	std::string tmp = oss.str();
	if (boost::ends_with(tmp, ", ")) {
		boost::erase_last(tmp, ", ");
	} else if (boost::ends_with(tmp, "(")) {
		return;
	}

	tmp += ")";
	area_sql << tmp;
		
	try {
		QueryRunner("high_school_info_friendfinder", CDbRServer).query(area_sql, ChinaAreaHandler(index_area_map));
	} catch (Ice::Exception& e) {
		LOG(INFO) << "[DbHelper] GetChinaArea run Ice Exception " << e.what() << " " << tmp;
	} catch (...) {
		LOG(INFO) << "[DbHelper] GetChinaArea unknow Exception " << tmp;
	}
}

void DbHelper::GetUserGender(const std::vector<int>& uid_list, std::map<int, int>& id_gender) {
	using namespace com::xiaonei::xce;
	Statement gender_sql;

	std::ostringstream oss;
	oss << "SELECT id, gender FROM user_born WHERE id in (";

	for (std::vector<int>::const_iterator iter = uid_list.begin();
			iter != uid_list.end(); ++iter) {
		if (0 != *iter) {
			oss << *iter << ", ";
		}
	}
	std::string tmp = oss.str();
	if (boost::ends_with(tmp, ", ")) {
		boost::erase_last(tmp, ", ");
	} else if (boost::ends_with(tmp, "(")) {
		return;
	}
	
	tmp += ")";
	gender_sql << tmp;

	try {
		QueryRunner("user_born_friendrank", CDbRServer).query(gender_sql, UserGenderHandler(id_gender));
	} catch (Ice::Exception& e) {
		LOG(INFO) << "[DbHelper] GetUserGender Ice Exception " << e.what() << " " << tmp;
	} catch (...) {
		LOG(INFO) << "[DbHelper] GetUserGender unknown Exception " << tmp; 
	}
}

void DbHelper::GetUserStage(const std::vector<int>& uid_list, std::map<int, int>& user_stage) {
	using namespace com::xiaonei::xce;
	Statement stage_sql;

	std::ostringstream oss;
	oss << "SELECT id, stage FROM user_stage WHERE id in (";

	for (std::vector<int>::const_iterator iter = uid_list.begin();
			iter != uid_list.end(); ++iter) {
		if (0 != *iter) {
			oss << *iter << ", ";
		}
	}
	std::string tmp = oss.str();
	if (boost::ends_with(tmp, ", ")) {
		boost::erase_last(tmp, ", ");
	} else if (boost::ends_with(tmp, "(")) {
		return;
	}
	
	tmp += ")";
	stage_sql << tmp;
	//LOG(INFO) << "tmp : " << tmp;

	try {
		QueryRunner("user_born_friendrank", CDbRServer).query(stage_sql, UserStageHandler(user_stage));
	} catch (Ice::Exception& e) {
		LOG(INFO) << "[DbHelper] GetUserStage Ice Exception " << e.what() << " " << tmp;
	} catch (...) {
		LOG(INFO) << "[DbHelper] GetUserStage unknown Exception " << tmp; 
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

//-----------------------------------------------------------------------

bool UserStageHandler::handle(mysqlpp::Row& row) const {
	int id = (int)row["id"];
	int stage = (int)row["stage"];
	//LOG(INFO) << id << " -- " << stage;
	user_stage_.insert(std::make_pair<int, int>(id, stage));
	return true;
}

}
}
