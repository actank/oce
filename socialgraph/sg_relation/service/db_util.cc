#include "db_util.h"
#include <sstream>
#include "util/cpp/TimeCost.h"
#include "permission.h"

namespace xce {
namespace socialgraph {

std::string DbUtil::GetSgRelationTableName(int user_id) {
  static std::string prefix("sg_relation_");
  std::ostringstream oss;
  int mod = user_id % kSgRelationTableScalar;
  oss << prefix << mod;
  return oss.str();
}

bool FirstLevelResultHandler::handle(mysqlpp::Row& row) const {
  if (!row.empty()) {
    FirstLevelRow flr;
    flr.index_value_ = (int) row["index_value"];
    flr.desc_ = (std::string) row["description"];
    seq_.push_back(flr);
  }
  return true;
}

bool EmotionStateResultHandler::handle(mysqlpp::Row& row) const {
  if (!row.empty()) {
    es_row_.user_id_ = (int) row["user_id"];
    es_row_.first_level_ = (int) row["first_level"];
    es_row_.second_level_ = (int) row["second_level"];
  }
  else {
    es_row_.user_id_ = -1;
    es_row_.first_level_ = 0;
    es_row_.second_level_ = 0;
  }
  return true;
}

bool SgRelationResultHandler::handle(mysqlpp::Row& row) const {
  if (!row.empty()) {
    SgRelationRow sg_relation_row;
    sg_relation_row.host_ = (int) row["host_id"];
    sg_relation_row.guest_ = (int) row["guest_id"];
    sg_relation_row.first_level_ = (int) row["first_level"];
    sg_relation_row.second_level_ = (int) row["second_level"];
    seq_.push_back(sg_relation_row);
  }
  return true;
}

bool SecondLevelResultHandler::handle(mysqlpp::Row& row) const {
  if (!row.empty()) {
    SecondLevelRow slr;
    slr.index_value_ = (int) row["index_value"];
    slr.desc_ = (std::string) row["description"];
    slr.parent_level_ = (int) row["parent_level"];
    seq_.push_back(slr);
  }
  return true;
}

FirstLevelRowSeq DbUtil::GetFullFirstLevel() {
  using namespace MyUtil;
  using namespace com::xiaonei::xce;
  TimeCost tc = TimeCost::create("DbUitl::GetFullFirstLevel", TimeCost::LOG_LEVEL_INFO);
  FirstLevelRowSeq full;
  FirstLevelRowSeq seq;
  int start = 0;
  static int limit = 100;
  static std::string kTableName = "first_level_dict";
  static std::string kDbSourceName = "sg_relation_other";
  do {
    seq.clear();
    Statement sql; 
    sql << "SELECT index_value, description from " << kTableName;
    sql << " LIMIT " << start << ", " << limit; 
    try {
    QueryRunner(kDbSourceName, CDbRServer).query(sql, FirstLevelResultHandler(seq));
    } catch (std::exception& e) {
      std::cerr << e.what();
      MCE_WARN("DbUtil::GetFullFirstLevel " << e.what());
    } catch (...) {
      MCE_WARN("DbUtil::GetFullFirstLevel ... exception");
    }
    
    if (!seq.empty()) {
      full.insert(full.end(), seq.begin(), seq.end());
      start += seq.size();
    }
  } while (!seq.empty());
  return full;
}

SecondLevelRowSeq DbUtil::GetFullSecondLevel() {
  using namespace MyUtil;
  using namespace com::xiaonei::xce;
  TimeCost tc = TimeCost::create("DbUitl::GetFullSecondLevel", TimeCost::LOG_LEVEL_INFO);
  SecondLevelRowSeq full;
  SecondLevelRowSeq seq;
  int start = 0;
  static int limit = 100;
  static std::string kTableName = "second_level_dict";
  static std::string kDbSourceName = "sg_relation_other";
  do {
   seq.clear();
   Statement sql; 
   sql << "SELECT index_value, description, parent_level from " << kTableName;
   sql << " LIMIT " << start << ", " << limit; 
   try {
     QueryRunner(kDbSourceName, CDbRServer).query(sql, SecondLevelResultHandler(seq));
   } catch (std::exception& e) {
     std::cerr << e.what();
     MCE_WARN("DbUtil::GetFullSecondLevel " << e.what());
   } catch (...) {
     MCE_WARN("DbUtil::GetFullSecondLevel ... exception");
   }
   
   if (!seq.empty()) {
     full.insert(full.end(), seq.begin(), seq.end());
     start += seq.size();
   }
  } while (!seq.empty());
  return full;
} 

void DbUtil::UpdateEmotionState(int user_id, const LevelTuple& lt) {
  using namespace MyUtil;
  using namespace com::xiaonei::xce;

  bool valid = LevelHolder::instance().Valid(lt);
  if (!valid) {
    return;
  }

  std::ostringstream otem;
  otem << "DbUitl::UpdateEmotionState"
       << " user_id:" << user_id
       << " LevelTuple(" << lt.first
       << ", " << lt.second << ")";
  TimeCost tc = TimeCost::create(otem.str(), TimeCost::LOG_LEVEL_INFO);
  static std::string kTableName = "emotion_state";
  static std::string kDbSourceName = "sg_relation_other";
  Statement sql;
  sql << "INSERT INTO " << kTableName 
      << "(user_id, first_level, second_level) VALUES(" 
      << user_id << "," << lt.first 
      << "," << lt.second << ")"
      << " ON DUPLICATE KEY UPDATE first_level = VALUES(first_level)"
      << ", second_level = VALUES(second_level)";
  try {
     mysqlpp::SimpleResult result = QueryRunner(kDbSourceName, CDbWServer).execute(sql);
    // TODO: delete std::cout 
    std::cout << "DbUtil::UpdateEmotionState  result.rows():" << result.rows() << std::endl;
  } catch (std::exception& e) {
    std::cerr << e.what();
    MCE_WARN("DbUtil::UpdateEmotionState " << e.what());
  } catch (...) {
    MCE_WARN("DbUtil::UpdateEmotionState ... exception");
  }
}

void DbUtil::UpdateSgRelation(int host, int guest, const LevelTuple& lt) {
  using namespace MyUtil;
  using namespace com::xiaonei::xce;

  bool valid = LevelHolder::instance().Valid(lt);
  if (!valid) {
    return;
  }

  std::ostringstream otem;
  otem << "DbUitl::UpdateSgRelation"
       << " host:" << host << " guest:" << guest
       << " LevelTuple(" << lt.first
       << ", " << lt.second << ")";
  TimeCost tc = TimeCost::create(otem.str(), TimeCost::LOG_LEVEL_INFO);
  static std::string kDbSourceName = "sg_relation";
  std::string table_name = GetSgRelationTableName(host);

  Statement sql;
  sql << " INSERT INTO " << table_name
      << " (host_id, guest_id, first_level, second_level)"
      << " VALUES (" << host << ", " << guest 
      << ", " << lt.first << ", " << lt.second << ")";
  try {
     mysqlpp::SimpleResult result = QueryRunner(kDbSourceName, CDbWServer).execute(sql);
    // TODO: delete std::cout 
    std::cout << "DbUtil::UpdateSgRelation  result.rows():" << result.rows() << std::endl;
  } catch (std::exception& e) {
    std::cerr << e.what();
    MCE_WARN("DbUtil::UpdateSgRelation " << e.what());
  } catch (...) {
    MCE_WARN("DbUtil::UpdateSgRelation ... exception");
  }

}


void DbUtil::DeleteSgRelation(int host, int guest, const LevelTuple& lt) {
  using namespace MyUtil;
  using namespace com::xiaonei::xce;

  bool valid = LevelHolder::instance().Valid(lt);
  if (!valid) {
    return;
  }

  std::ostringstream otem;
  otem << "DbUitl::DeleteSgRelation"
       << " host:" << host << " guest:" << guest
       << " LevelTuple(" << lt.first
       << ", " << lt.second << ")";
  TimeCost tc = TimeCost::create(otem.str(), TimeCost::LOG_LEVEL_INFO);
  static std::string kDbSourceName = "sg_relation";
  std::string table_name = GetSgRelationTableName(host);

  Statement sql;
  sql << " DELETE FROM " << table_name
      << " WHERE host_id = " << host 
      << " AND first_level = " << lt.first
      << " AND guest_id = " << guest 
      << " AND second_level = " << lt.second;
  try {
     mysqlpp::SimpleResult result = QueryRunner(kDbSourceName, CDbWServer).execute(sql);
    // TODO: delete std::cout 
    std::cout << "DbUtil::DeleteSgRelation  result.rows():" << result.rows() << std::endl;
  } catch (std::exception& e) {
    std::cerr << e.what();
    MCE_WARN("DbUtil::DeleteSgRelation " << e.what());
  } catch (...) {
    MCE_WARN("DbUtil::DeleteSgRelation ... exception");
  }
}

EmotionStateRow DbUtil::GetEmotionState(int user_id) {
  using namespace MyUtil;
  using namespace com::xiaonei::xce;
  std::ostringstream oss;
  oss << "DbUitl::GetEmotionState(" << user_id << ")";
  TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);
  static std::string kTableName = "emotion_state";
  static std::string kDbSourceName = "sg_relation_other";
  Statement sql;
  sql << "SELECT user_id, first_level, second_level FROM " 
      << kTableName 
      << " WHERE user_id = " << user_id;
  
  EmotionStateRow row;
  try {
    QueryRunner(kDbSourceName, CDbRServer).query(sql, EmotionStateResultHandler(row));
  } catch (std::exception& e) {
     std::cerr << e.what();
     MCE_WARN("DbUtil::GetEmotionState" << e.what());
  } catch (...) {
     MCE_WARN("DbUtil::GetEmotionState ... exception");
  }
  return row;
}

SgRelationRowSeq DbUtil::GetSgRelation(int user_id) {
  using namespace MyUtil;
  using namespace com::xiaonei::xce;
  std::ostringstream oss;
  oss << "DbUitl::GetSgRelation(" << user_id << ")";
  TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);
  static std::string kDbSourceName = "sg_relation";
  std::string table_name = GetSgRelationTableName(user_id);
  SgRelationRowSeq full;
  SgRelationRowSeq seq;
  int start = 0;
  const int limit = 1;
  do {
    seq.clear();
    Statement sql;
    sql << "SELECT host_id, guest_id, first_level, second_level"
        << " FROM " << table_name
        << " WHERE host_id = " << user_id
        << " LIMIT " << start << ", " << limit;
    try {
      QueryRunner(kDbSourceName, CDbRServer).query(sql, SgRelationResultHandler(seq));
    } catch (std::exception& e) {
       std::cerr << e.what();
       MCE_WARN("DbUtil::GetSgRelation" << e.what() << "  user_id:" << user_id);
    } catch (...) {
       MCE_WARN("DbUtil::GetSgRelation ... exception " << "user_id:" << user_id);
    }
    if (!seq.empty()) {
      full.insert(full.end(), seq.begin(), seq.end());
      start += seq.size();
    }
  } while (!seq.empty());  
  return full;
}

}}



