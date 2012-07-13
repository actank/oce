#include "socialgraph/recommendbyinfo/cpp/dbhelper.h"

#include <boost/algorithm/string.hpp>

#include "socialgraph/socialgraphutil/clogging.h"
#include "util/cpp/MD5Util.h"
#include "util/cpp/IntSeqSerialize.h"

namespace xce {
namespace socialgraph {

void DbHelper::ReadFriendList(int userid, std::vector<int>& friends) {
	using namespace com::xiaonei::xce;
  std::ostringstream pattern;
  pattern << "relation_" << userid % 100;

  Statement sql;
  sql
    << "SELECT host, guest FROM relation_" << userid % 100
    << " WHERE host=" << userid;

  try {
    QueryRunner("relation_friendrank", CDbRServer, pattern.str()).query(sql, FriendReadHandler(friends));
  } catch (Ice::Exception& e) {
		CLOG(DEBUG) << "ReadFriendList Connection Exception:"  << e.what();
  } catch (...) {
		CLOG(DEBUG) << "ReadFriendList DB unknow Exception";
  }
}

void DbHelper::ReadApplyList(int userid, std::vector<int>& accepter_list) {
  using namespace ::com::xiaonei::xce;
  if (userid <= 0) {
    return;
  }

	std::ostringstream pattern;
  pattern << "buddyapply_applicant_" << (userid % 100);		//pattern

	std::ostringstream table;
	table << "buddyapply_applicant_" << (userid % 100);		//table name

  Statement oss;
  oss << "SELECT accepter FROM " << table.str() << " WHERE applicant = " << userid << " LIMIT 100";
  RelationResultHandler rsh(accepter_list, "accepter");
  try {
  	QueryRunner("buddyapply_friendfinder_applicant", CDbRServer, pattern.str()).query(oss, rsh);
  } catch (Ice::Exception& e) {
		CLOG(DEBUG) << "ReadApplyList Connection Exception:"  << e.what();
  } catch (...) {
		CLOG(DEBUG) << "ReadApplyList DB unknow Exception";
  }
}

void DbHelper::GetUnivInfo(int userid, InfoList& univ_info) {
	using namespace com::xiaonei::xce;
  std::string table_name = "university_info";
  
  Statement sql;       // 查数据库
  sql << "SELECT * FROM " << table_name << " where userid=" << userid;

  try {
    QueryRunner("university_info_friendfinder", CDbRServer).query(sql, UnivReadHandler(univ_info));
  } catch (Ice::Exception& e) {
		CLOG(DEBUG) << "GetUnivInfo DB Connection Exception : " << e.what();
  } catch (...) {
		CLOG(DEBUG) << "DB Unknow Exception";
  }
}

void DbHelper::GetHighschoolInfo(int userid, InfoList& highschool_info) {
	using namespace com::xiaonei::xce;
  std::string table_name = "high_school_info";

  Statement sql;
  sql << "SELECT * FROM " << table_name << " where userid=" << userid;
  try {
    QueryRunner("high_school_info_friendfinder", CDbRServer).query(sql, HighschoolReadHandler(highschool_info));
  } catch (Ice::Exception& e) {
		CLOG(DEBUG) << "DB Connection Exception : " << e.what();
  } catch (...) {
		CLOG(DEBUG) << "DB Unknow Exception";
  }
}

void DbHelper::GetJuniorschoolInfo(int userid, InfoList& juniorschool_info) {
	using namespace com::xiaonei::xce;
  std::string table_name = "junior_high_school_info";
  Statement sql;
  sql << "SELECT * FROM " << table_name << " where userid=" << userid;
  try {
    QueryRunner("junior_high_school_info_friendfinder", CDbRServer).query(sql, JuniorschoolReadHandler(juniorschool_info));
  } catch (Ice::Exception& e) {
		CLOG(DEBUG) << "DB Connection Exception : " << e.what();
  } catch (...) {
		CLOG(DEBUG) << "DB Unknow Exception";
  }
}

void DbHelper::GetElementaryschoolInfo(int userid, InfoList& elementaryschool_info) {
	using namespace com::xiaonei::xce;

	std::string table_name = "elementary_school_info";
  Statement sql;
  sql << "SELECT * FROM " << table_name << " where userid=" << userid;
  try {
    QueryRunner("elementary_school_info_friendfinder", CDbRServer).query(sql, ElementaryschoolReadHandler(elementaryschool_info));
  } catch (Ice::Exception& e) {
		CLOG(DEBUG) << "DB Connection Exception : " << e.what();
  } catch (...) {
		CLOG(DEBUG) << "DB Unknow Exception";
  }
}

bool DbHelper::InsertRecommendByInfoDb(int userid, int type, const std::vector<int>& recommends) {
	//CLOG(DEBUG) << "DbHelper::InsertRecommendByInfoDb userid(" << userid 
	//						<< ") type(" << type << ") recommends.size:" << recommends.size();
	using namespace com::xiaonei::xce;

	std::string table_name = "recommend_by_info";

	mysqlpp::DateTime now(time(NULL));
	Statement sql;
	sql << "INSERT INTO " << table_name << " (userid, stage, create_time, recommend) VALUES (" << userid << ", " 
			<< type << ", " << mysqlpp::quote << now << ", " << mysqlpp::quote << MyUtil::IntSeqSerialize::serialize(recommends) 
			<< ") ON DUPLICATE KEY UPDATE create_time=VALUES(create_time), recommend=VALUES(recommend);";
  try {
    QueryRunner("socialgraph_block", CDbRServer).execute(sql);
  } catch (Ice::Exception& e) {
		CLOG(DEBUG) << "DB Connection Exception : " << e.what();
		return false;
  } catch (...) {
		CLOG(DEBUG) << "DB Unknow Exception";
		return false;
  }
	return true;
}

void DbHelper::GetRecommendFromInfoDb(int userid, int type, std::vector<int>& recommends) {
	//CLOG(DEBUG) << "DbHelper::GetRecommendFromInfoDb userid(" << userid 
	//	<< ") type(" << type << ") recommends.size:" << recommends.size();
	using namespace com::xiaonei::xce;

	std::string table_name = "recommend_by_info";
	Statement sql;
	sql << "SELECT recommend FROM " << table_name << " WHERE userid = " << userid << " AND stage = " << type;
  try {
    QueryRunner("socialgraph_block", CDbRServer).query(sql, RecommendByInfoReadHandler(recommends));
  } catch (Ice::Exception& e) {
		CLOG(DEBUG) << "DB Connection Exception : " << e.what();
  } catch (...) {
		CLOG(DEBUG) << "DB Unknow Exception";
  }
}

void DbHelper::GetBatchAgeInfoDb(const std::vector<int>& total_ids, std::map<int, int>& id2age){
	using namespace com::xiaonei::xce;

	std::string table_name = "user_born";

	Statement sql;
	sql 
			<< "SELECT id, birth_year FROM " << table_name << " where id in (";
	for (std::vector<int>::const_iterator iter = total_ids.begin();
			iter != total_ids.end(); ++iter) {
		if (iter != total_ids.end() - 1) {
			sql << *iter << ", ";
		}	else {
			sql << *iter;
		}
	}
	sql << ")";

  try {
    QueryRunner("user_born_friendfinder", CDbRServer).query(sql, UserAgeInfoReadHandler(id2age));
  } catch (Ice::Exception& e) {
		CLOG(WARN) << "GetBatchAgeInfoDb DB Connection Exception : " << e.what();
  } catch (...) {
		CLOG(WARN) << "GetBatchAgeInfoDb DB Unknow Exception";
  }
}

long DbHelper::GetCreateDataTime(int userid) {
	using namespace com::xiaonei::xce;
	long time = 0;

  Statement oss;
  oss << "SELECT UNIX_TIMESTAMP(create_time) FROM recommend_by_info WHERE userid=" << userid;

	try {
    mysqlpp::StoreQueryResult result = QueryRunner("socialgraph_block", CDbRServer).store(oss);
		for (size_t i = 0; i < result.num_rows(); ++i) {
			time = result[i][0];
			break;
		}
	} catch (Ice::Exception& e) {
		CLOG(WARN) << "GetCreateDataTime Connection Exception: " << e.what();
	} catch (std::exception& e) {
		CLOG(WARN) << "GetCreateDataTime std Exception: " << e.what();
	} catch (...) {
		CLOG(WARN) << "Unknow Exception";
	}

	return time;
}

long DbHelper::GetCreateDataTimeFromNewInfoDb(int userid) {
	using namespace com::xiaonei::xce;
	long time = 0;

  Statement oss;
  oss << "SELECT UNIX_TIMESTAMP(create_time) FROM recommend_by_info_new WHERE userid=" << userid;

	try {
    mysqlpp::StoreQueryResult result = QueryRunner("socialgraph_recommend_info", CDbRServer).store(oss);
		for (size_t i = 0; i < result.num_rows(); ++i) {
			time = result[i][0];
			break;
		}
	} catch (Ice::Exception& e) {
		CLOG(WARN) << "GetCreateDataTimeFromNewInfoDb Connection Exception: " << e.what();
	} catch (std::exception& e) {
		CLOG(WARN) << "GetCreateDataTimeFromNewInfoDb std Exception: " << e.what();
	} catch (...) {
		CLOG(WARN) << "Unknow Exception";
	}

	return time;
}

void DbHelper::DeleteRecommendByInfoDb(int userid, int stage) {
	using namespace com::xiaonei::xce;

	std::string table_name = "recommend_by_info_new";

	Statement sql;
	sql 
			<< "DELETE FROM " << table_name << " where userid=" << userid << " AND stage=" << stage;
  try {
    QueryRunner("socialgraph_recommend_info", CDbRServer).execute(sql);
  } catch (Ice::Exception& e) {
		CLOG(WARN) << "DeleteRecommendByInfoDb DB Connection Exception : " << e.what();
  } catch (...) {
		CLOG(WARN) << "DeleteRecommendByInfoDb DB Unknow Exception";
  }
}

bool DbHelper::InsertRecommendByInfoDbByStage(int userid, 
																							int stage, 
																							int stage_index, 
																							const std::string school_name, 
																							const std::vector<int>& recommends)	{
	using namespace com::xiaonei::xce;
	std::string table_name = "recommend_by_info_new";

	mysqlpp::DateTime now(time(NULL));
	Statement sql;
	sql << "INSERT INTO " << table_name << " (userid, stage, stage_index, school_name, create_time, recommend) VALUES (" << userid 
			<< ", " << stage << ", " << stage_index << ", " << mysqlpp::quote << school_name 	<< ", " << mysqlpp::quote << now
			<< ", " << mysqlpp::quote << MyUtil::IntSeqSerialize::serialize(recommends)
			<< ") ON DUPLICATE KEY UPDATE school_name=VALUES(school_name),create_time=VALUES(create_time),recommend=VALUES(recommend);";
		
  try {
    QueryRunner("socialgraph_recommend_info", CDbRServer).execute(sql);
  } catch (Ice::Exception& e) {
		CLOG(DEBUG) << "InsertRecommendByInfoDbByStage DB Connection Exception : " << e.what();
		return false;
  } catch (...) {
		CLOG(DEBUG) << "InsertRecommendByInfoDbByStage DB Unknow Exception";
		return false;
  }
	return true;
}

void DbHelper::GetRecommendFromInfoDbByStage(int userid, int stage, std::vector<RecommendItem>& recommend_items) {
	using namespace com::xiaonei::xce;

	std::string table_name = "recommend_by_info_new";
	Statement sql;
	sql << "SELECT school_name, recommend FROM " << table_name << " WHERE userid = " << userid << " AND stage = " << stage;
  try {
    QueryRunner("socialgraph_recommend_info", CDbRServer).query(sql, RecommendByInfoNewReadHandler(recommend_items));
  } catch (Ice::Exception& e) {
		CLOG(DEBUG) << "GetRecommendFromInfoDbByStage DB Connection Exception : " << e.what();
  } catch (...) {
		CLOG(DEBUG) << "GetRecommendFromInfoDbByStage DB Unknow Exception";
  }
}

//-----------------------------------------------------------------

bool ViewCountHandler::handle(mysqlpp::Row& row) const {
  last_ = last_ > (int)row["id"] ? last_ : (int)row["id"];

  int userid = (int)row["id"];
  int view_count = (int)row["viewcount"];
  view_count_map_.insert(std::make_pair<int, int>(userid, view_count));
  return true;
}

//-----------------------------------------------------------------

bool UnivBatchReadHandler::handle(mysqlpp::Row& row) const {
	last_ = last_ > (int)row["id"] ? last_ : (int)row["id"];

	std::vector<std::string> info;

	char tmpstr[25];
  sprintf(tmpstr, "%d", (int)row["enroll_year"]);
		
	std::string univ_name(row["university_name"]);
	std::string department(row["department"]);
	std::string enroll_year(tmpstr);

	if (700000000 <= (int)row["userid"] &&		//过滤开心用户
			750000000 >= (int)row["userid"]) {
		return true;
	}

	//去空格
	boost::trim(univ_name);
	boost::trim(department);
	boost::trim(enroll_year);

	if(univ_name == "" || department == "" || enroll_year == "") {
		return true;
	}

	CLOG(INFO) << "id(" << (int)row["id"] << ")  university_name(" << univ_name
			<< ")  department(" << department << ")  enroll_year("  << enroll_year << ")";

	info.push_back(univ_name);
	info.push_back(department);
	info.push_back(enroll_year);

	long key = MyUtil::MD5Util::stringSeqToMD5(info);
	result_map_[key].push_back((int)row["userid"]);
	return true;
}

//-----------------------------------------------------------------

bool HighschoolBatchReadHandler::handle(mysqlpp::Row& row) const {
	last_ = last_ > (int)row["id"] ? last_ : (int)row["id"];

	std::vector<std::string> info;

	char tmpstr[25];
  sprintf(tmpstr, "%d", (int)row["enroll_year"]);

	std::string highschool_name(row["high_school_name"]);
	std::string enroll_year(tmpstr);

	if (700000000 <= (int)row["userid"] &&		//过滤开心用户
			750000000 >= (int)row["userid"]) {
		return true;
	}
	
	boost::trim(highschool_name);
	boost::trim(enroll_year);

	if(highschool_name == "" || enroll_year == "") {
		return true;
	}

	CLOG(INFO) << "id(" << (int)row["id"] << ")  high_school_name(" << highschool_name
				<< ")  enroll_year("  << enroll_year << ")";
	
	info.push_back(highschool_name);
	info.push_back(enroll_year);

	long key = MyUtil::MD5Util::stringSeqToMD5(info);
	result_map_[key].push_back((int)row["userid"]);
	return true;
}

//-----------------------------------------------------------------

bool JuniorschoolBatchReadHandler::handle(mysqlpp::Row& row) const {
	last_ = last_ > (int)row["userid"] ? last_ : (int)row["userid"];

	std::vector<std::string> info;

	char tmpstr[25];
  sprintf(tmpstr, "%d", (int)row["junior_high_school_year"]);

	std::string juniorschoolName( row["junior_high_school_name"] );
	std::string enrollYear( tmpstr );

	if (700000000 <= (int)row["userid"] &&		//过滤开心用户
			750000000 >= (int)row["userid"]) {
		return true;
	}
	
	boost::trim(juniorschoolName);
	boost::trim(enrollYear);

	if(juniorschoolName == "" || enrollYear == "") {
		return true;
	}

	MCE_DEBUG( "userid(" << (int)row["userid"] << ")  junior_high_school_name(" << juniorschoolName 
				<< ")  enroll_year("  << enrollYear << ")");

	info.push_back( juniorschoolName );
	info.push_back( enrollYear );

	long key = MyUtil::MD5Util::stringSeqToMD5(info);
	result_map_[key].push_back( (int)row["userid"] );
	return true;
}

//-----------------------------------------------------------------

bool ElementaryschoolBatchReadHandler::handle(mysqlpp::Row& row) const {
	last_ = last_ > (int)row["userid"] ? last_ : (int)row["userid"];

	std::vector<std::string> info;

	char tmpstr[25];
  sprintf(tmpstr, "%d", (int)row["elementary_school_year"]);

	std::string elementaryschool_name(row["elementary_school_name"]);
	std::string enroll_year(tmpstr);

	if (700000000 <= (int)row["userid"] &&		//过滤开心用户
			750000000 >= (int)row["userid"]) {
		return true;
	}
	
	boost::trim(elementaryschool_name);
	boost::trim(enroll_year);

	if(elementaryschool_name == "" || enroll_year == "") {
		return true;
	}

	CLOG(INFO) << "userid(" << (int)row["userid"] << ")  elementary_school_name(" << elementaryschool_name
				<< ")  enroll_year("  << enroll_year << ")";

	info.push_back(elementaryschool_name);
	info.push_back(enroll_year);

	long key = MyUtil::MD5Util::stringSeqToMD5(info);
	result_map_[key].push_back((int)row["userid"]);
	return true;
}

//-----------------------------------------------------------------

bool FriendReadHandler::handle(mysqlpp::Row& row) const {
  if ((int)friends_.size() < 2000) {
    friends_.push_back((int)row["guest"]);
  }
  return true;
}

//-----------------------------------------------------------------

bool UnivReadHandler::handle(mysqlpp::Row& row) const {
  std::vector<std::string> info;

  char tmpstr[25];
  sprintf(tmpstr, "%d", (int)row["enroll_year"]); 
  std::string enroll_year(tmpstr);
  std::string univ_name(row["university_name"]);
  std::string department(row["department"]);

  //去空格
  boost::trim(univ_name);
  boost::trim(department);
  boost::trim(enroll_year);

	if (univ_name == "" || department == "" || enroll_year == "") {
		return true;
	}

  info.push_back(univ_name);
  info.push_back(department);
  info.push_back(enroll_year); 

  univ_info_.push_back(info);
  return true;
}

//-----------------------------------------------------------------

bool HighschoolReadHandler::handle(mysqlpp::Row& row) const {
  std::vector<std::string> info;

  char tmpstr[25];
  sprintf(tmpstr, "%d", (int)row["enroll_year"]);
  std::string enroll_year(tmpstr);
  std::string highschool_name(row["high_school_name"]);

  //去空格
  boost::trim(highschool_name);
  boost::trim(enroll_year);

	if (highschool_name == "" || enroll_year == "") {
		return true;
	}

  info.push_back(highschool_name);
  info.push_back(enroll_year);

  highschool_info_.push_back(info);
  return true;
}

//-----------------------------------------------------------------

bool JuniorschoolReadHandler::handle(mysqlpp::Row& row) const {
  std::vector<std::string> info;

  char tmpstr[25];
  sprintf(tmpstr, "%d", (int)row["junior_high_school_year"]);
  std::string enroll_year(tmpstr);
  std::string juniorschool_name(row["junior_high_school_name"]);

  //去空格
  boost::trim(juniorschool_name);
  boost::trim(enroll_year);

	if (juniorschool_name == "" || enroll_year == "") {
		return true;
	}

  info.push_back(juniorschool_name);
  info.push_back(enroll_year);

  juniorschool_info_.push_back(info);
  return true;
}

//-----------------------------------------------------------------

bool ElementaryschoolReadHandler::handle(mysqlpp::Row& row) const {
  std::vector<std::string> info;

  char tmpstr[25];
  sprintf(tmpstr, "%d", (int)row["elementary_school_year"]);
  std::string enroll_year(tmpstr);

  std::string elementaryschool_name(row["elementary_school_name"]);

  //去空格
  boost::trim(elementaryschool_name);
  boost::trim(enroll_year);

	if (elementaryschool_name == "" || enroll_year == "") {
		return true;
	}

  info.push_back(elementaryschool_name);
  info.push_back(enroll_year);

  elementaryschool_info_.push_back(info);
  return true;
}

//-----------------------------------------------------------------

bool RecommendByInfoReadHandler::handle(mysqlpp::Row& row) const {
	recommends_ = MyUtil::IntSeqSerialize::unserialize(row["recommend"]);
	return true;
}

//-----------------------------------------------------------------

bool RecommendByInfoNewReadHandler::handle(mysqlpp::Row& row) const {
	RecommendItem item;
	item.recommend_list_ = MyUtil::IntSeqSerialize::unserialize(row["recommend"]);
	item.school_name_ = std::string(row["school_name"]);
	
	recommends_.push_back(item);
	return true;
}

//-----------------------------------------------------------------

bool UserAgeInfoReadHandler::handle(mysqlpp::Row& row) const {
	int userid = (int)row["id"];
	int birth_year = (int)row["birth_year"];
	id2age_.insert(std::make_pair<int, int>(userid, birth_year));
	return true;
} 

//-----------------------------------------------------------------

RelationResultHandler::RelationResultHandler(std::vector<int>& results, const std::string& toField) :
  _results(results), _toField(toField) {
}

RelationResultHandler::~RelationResultHandler() {
}

bool RelationResultHandler::handle(mysqlpp::Row& row) const {
  int to = (int)row[_toField.c_str()];
  _results.push_back(to);
  return true;
}

}
}
