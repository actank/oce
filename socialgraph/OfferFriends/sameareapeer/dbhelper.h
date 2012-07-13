#ifndef DBHELPER_H_
#define DBHELPER_H_

#include <string>

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "DbCxxPool/src/QueryRunner.h"

namespace xce {
namespace socialgraph {

static const int MAN = 1;
static const int WOMAN = 0;

//-----------------------------------------------------------------------
/*学校信息*/
struct SchoolInfo {
	SchoolInfo() {
		userid_ = 0;
		school_id_ = 0;
		school_name_ = "";
		enroll_year_ = 0;
	}
	int userid_;
	int school_id_;
	std::string school_name_;
	int enroll_year_;
};

//-----------------------------------------------------------------------
/*学校编码信息*/
struct SchoolCode {
	SchoolCode() {
		id_ = 0;
		code_pro_ = "";
		code_city_ = "";
		code_county_ = "";
	}
	int id_;
	std::string code_pro_;
	std::string code_city_;
	std::string code_county_;
};

//-----------------------------------------------------------------------

class DbHelper {
public:
	/*@brief 获取编码对应的名字*/
	static void GetChinaArea(const std::vector<std::string>& area_list,
													 std::map<std::string, std::string>& index_area_map);

	/*@brief 获取用户的性别信息*/
	static void GetUserGender(const std::vector<int>& uid_list,
														std::map<int, int>& id_gender);

	/*@brief 获取用户的阶段信息*/
	static void GetUserStage(const std::vector<int>& uid_list, 
													 std::map<int, int>& user_stage);
};

//-----------------------------------------------------------------------

/*@brief high_school_info db helper */
class HighSchoolInfoHandler : public com::xiaonei::xce::ResultHandler {
public:
	HighSchoolInfoHandler(int& last, std::map<int, SchoolInfo>& id_highschool_map) :
		last_(last), id_highschool_map_(id_highschool_map) { }

	virtual bool handle(mysqlpp::Row& row) const;

private:
	int& last_;
	std::map<int, SchoolInfo>& id_highschool_map_;
};

//-----------------------------------------------------------------------

/*@brief junior_high_school_info db helper */
class JuniorHighSchoolInfoHandler : public com::xiaonei::xce::ResultHandler {
public:
	JuniorHighSchoolInfoHandler(int& last, std::map<int, SchoolInfo>& id_juniorhighschool_map) :
		last_(last), id_juniorhighschool_map_(id_juniorhighschool_map) { }
	virtual bool handle(mysqlpp::Row& row) const;

private:
	int& last_;
	std::map<int, SchoolInfo>& id_juniorhighschool_map_;
};

//-----------------------------------------------------------------------

/*@brief special_school_info db helper */
class SpecialSchoolInfoHandler : public com::xiaonei::xce::ResultHandler {
public:
	SpecialSchoolInfoHandler(int& last, std::map<int, SchoolInfo>& id_specialschool_map) :
		last_(last), id_specialschool_map_(id_specialschool_map) { }
	virtual bool handle(mysqlpp::Row& row) const;

private:
	int& last_;
	std::map<int, SchoolInfo>& id_specialschool_map_;
};

//-----------------------------------------------------------------------

/*@brief 学校编码 db helper */
class SchoolCodeHandler : public com::xiaonei::xce::ResultHandler {
public:
	SchoolCodeHandler(std::map<int, SchoolCode>& school_code_map):
		school_code_map_(school_code_map) { }

	virtual bool handle(mysqlpp::Row& row) const;

private:
	std::map<int, SchoolCode>& school_code_map_;
};

//-----------------------------------------------------------------------

/*@brief 编码对应的地区名 db helper */
class ChinaAreaHandler : public com::xiaonei::xce::ResultHandler {
public:
	ChinaAreaHandler(std::map<std::string, std::string>& index_area_map):
		index_area_map_(index_area_map) {}
	virtual bool handle(mysqlpp::Row& row) const;

private:
	std::map<std::string, std::string>& index_area_map_; 
};

//-----------------------------------------------------------------------

/*@brief 用户性别 db helper */
class UserGenderHandler : public com::xiaonei::xce::ResultHandler {
public:
	UserGenderHandler(std::map<int, int>& user_gender):
		user_gender_(user_gender) { }
	virtual bool handle(mysqlpp::Row& row) const;

private:
	std::map<int, int>& user_gender_;
};

//-----------------------------------------------------------------------
/*@brief 用户阶段 db helper */
class UserStageHandler : public com::xiaonei::xce::ResultHandler {
public:
	UserStageHandler(std::map<int, int>& user_stage):
		user_stage_(user_stage) { }
	virtual bool handle(mysqlpp::Row& row) const;

private:
	std::map<int, int>& user_stage_;
};
}
}

#endif
