/*
 * test.cpp
 *
 *  Created on: Apr 8, 2010
 *      Author: yejingwei
 */
#include "ExpressionParser.h"
#include <iostream>
#include <sstream>
#include <boost/tuple/tuple.hpp>
#include <exception>
#include <set>
#include <stdexcept>
#include <ostream>
#include <algorithm>
#include <boost/lexical_cast.hpp>

#include <QueryRunner.h>
#include <DbDescriptor.h>
#include "ServiceI.h"
#include "FriendRankCacheAdapter.h"
using namespace std;
using namespace xce::feed;
using namespace xce::socialgraph;
using namespace com::xiaonei::xce;
using namespace MyUtil;

set<int> Parser::CalculateIds() {
  MCE_DEBUG("Parser::CalculateIds()");
  size_t index = 0;
  //  TempResultPtr temp_result = new TempResult();//保存中间结果
  set<int> result;
  vector<FactorPtr>::iterator fit = factors_.begin();
  (*fit)->Plus(result);//对于第一个factor，加上一个空的ids，没有影响
  MCE_DEBUG("Parser::CalculateIds --> After calculating factor :" << index << ",result size = " << result.size());
  ++fit;
  ++index;
  vector<Op>::iterator oit = ops_.begin();
  while (fit != factors_.end()) {

    char op = (*oit).op_type();
    switch (op) {
    case '+':
      (*fit)->Plus(result);
      break;
    case '-':
      if(result.empty()){ //空的话，－结果还是空
        break;
      }
      (*fit)->Minus(result);
      break;
    case '&':
      if(result.empty()){//空的话，&结果还是空
        break;
      }
      (*fit)->And(result);
      break;
    }
    MCE_DEBUG("Parser::CalculateIds --> After calculating factor :" << index << ",result size = " << result.size());

    ++oit;
    ++fit;
    ++index;
  }
  return result;
}

bool Parser::Init(string & expr) {
  MCE_DEBUG("Parser::Init --> expression = " << expr);
  expr_str_ = expr;
  return true;
}

vector<FactorPtr> Parser::Tokenize() {
  MCE_DEBUG("Parser::Tokenize --> expr = " << expr_str_ );

  size_t pos = 0;
  while (pos < expr_str_.size()) {
    char tmpchar = expr_str_[pos];
    if (kOpTypes.find(tmpchar) != string::npos) {//字符是操作符
      Op tmpOp(tmpchar);
      ops_.push_back(tmpOp);
      ++pos;
    } else if (kFactorTypes.find(tmpchar) != string::npos) {//字符是因子类型符号
      factors_.push_back(ScanFactor(pos));
    } else {
      ostringstream oss;
      oss << "Parser::Tokenize() --> wrong charactor : '" << tmpchar
          << "',at position:" << pos << ",expression:" << expr_str_;
      string s = oss.str();
      throw logic_error(s);
    }
  }

  ostringstream foss;
  foss << "Parser::Tokenize --> factors are :";
  for (vector<FactorPtr>::iterator sit = factors_.begin(); sit
      != factors_.end(); ++sit) {
    foss << (*sit)->type() << " " << (*sit)->param_id() << ",";
  }
  MCE_DEBUG(foss.str());
  ostringstream eoss;
  eoss << "Parser::Tokenize --> operators are :";
  if (ops_.size() == 0) {
    eoss << " empty ";
  } else {
    for (vector<Op>::iterator sit = ops_.begin(); sit != ops_.end(); ++sit) {
      eoss << sit->op_type() << ",";
    }
  }
  MCE_DEBUG(eoss.str());
  return factors_;
}

FactorPtr Parser::ScanFactor(size_t & pos) {
  char tmpchar = expr_str_[pos];
  /*  MCE_DEBUG("Parser::ScanFactor() --> tmpchar = "  <<  tmpchar);*/
  switch (tmpchar) {
  case 'f': {
    return new FriendFactor(tmpchar, ScanInteger(pos));
  }
  case 'u': {
    return new UserFactor(tmpchar, ScanInteger(pos));
  }
  case 'a': {
    return new AppFactor(tmpchar, ScanInteger(pos));
  }
  case 'A': {
    return new AppFanFactor(tmpchar, ScanInteger(pos));
  }
  case 'p': {
    return new PageFactor(tmpchar, ScanInteger(pos));
  }
  case 's': {
    return new HighSchoolFactor(tmpchar, ScanInteger(pos));
  }
  case 'g': {
    return new MiniGroupFactor(tmpchar, ScanInteger(pos));
  }
  case 'v': {
    return new FollowFactor(tmpchar, ScanInteger(pos));
  }
  case 'z': {
    return new XiaoZuFactor(tmpchar, ScanInteger(pos));
  }
  case 'x': {
    return new MiniSiteFactor(tmpchar, ScanInteger(pos));
  }
  case 'c': {
    return new CheWenFactor(tmpchar, ScanInteger(pos));
  }
  default:
    throw logic_error("Parser::ScanFactor() --> wrong factor type = " + tmpchar);
    return NULL;
  }
}

int Parser::ScanInteger(size_t & pos) {//扫描因子括弧内的数字
  size_t start = expr_str_.find("(", pos);
  size_t end = expr_str_.find(")", pos);
  if (start > end) {
    throw logic_error("Parser::ScanInteger() --> start larger than end");
  }
  pos = end + 1;
  string intstr = expr_str_.substr(start + 1, end - start - 1);
  int value = boost::lexical_cast<int>(intstr);
  /*  MCE_DEBUG("Parser::ScanInteger() --> pos = " << pos << ", integer value = " << value);*/
  return value;
}

void Parser::PrintIds() {
  MCE_DEBUG( "ids_'s size is :" << expr_ids_.size() );
  for (set<int>::iterator itr = expr_ids_.begin(); itr != expr_ids_.end(); ++itr) {
    MCE_DEBUG( *itr << " ");
  }
}

//--------------------------------------------------------------------------
void AppFactor::Plus(set<int> & result) {
  string head = GenerateSQL();
  Statement sql;
  sql << head;
  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(instance_, CDbRServer, table_).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" AppFactor::Plus --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" AppFactor::Plus --> error:" << e.what());
  }
  if (!res) {
    MCE_WARN("StoryQueryResult wrong");
    return;
  }

  for (size_t i = 0; i < res.size(); ++i) {
    result.insert(res.at(i)["user_id"]);
  }
}

void AppFactor::Minus(set<int> & result) {

  string head = GenerateSQL();
  Statement sql;
  sql << head;

  sql << " AND user_id IN (";
  set<int>::iterator itr = result.begin();
  sql << *itr;
  for (++itr; itr != result.end(); ++itr) {
    sql << "," << *itr;
  }
  sql << ")";

  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(instance_, CDbRServer, table_).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" AppFactor::Plus --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" AppFactor::Plus --> error:" << e.what());
  }
  if (!res) {
    MCE_WARN("StoryQueryResult wrong");
    return;
  }
  for (size_t i = 0; i < res.size(); ++i) {
    result.erase(res.at(i)["user_id"]);
  }

}

void AppFactor::And(set<int> & result) {

  string head = GenerateSQL();
  Statement sql;
  sql << head;

  sql << " AND user_id IN (";
  set<int>::iterator itr = result.begin();
  sql << *itr;
  for (++itr; itr != result.end(); ++itr) {
    sql << "," << *itr;
  }
  sql << ")";

  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(instance_, CDbRServer, table_).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" AppFactor::Plus --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" AppFactor::Plus --> error:" << e.what());
  }
  MCE_DEBUG("AppFactor::And");
  result.clear();
  if (!res) {
    MCE_WARN("StoryQueryResult wrong");
    return;
  }
  for (size_t i = 0; i < res.size(); ++i) {
    result.insert(res.at(i)["user_id"]);
  }

}

string AppFactor::GenerateSQL() { //app
  ostringstream tablestream;
  tablestream << "app_user_" << (param_id_ % 100);
  table_ = tablestream.str();
  ostringstream oss;
  oss << "SELECT user_id FROM " << table_ << " WHERE app_id = " << param_id_ << " ";
  string tmpstr = oss.str();
  return tmpstr;

}
//--------------------------------------------------------------------------
void AppFanFactor::Plus(set<int> & result) {
  string head = GenerateSQL();
  Statement sql;
  sql << head;
  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(instance_, CDbRServer, table_).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" AppFanFactor::Plus --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" AppFanFactor::Plus --> error:" << e.what());
  }
  for (size_t i = 0; i < res.size(); ++i) {
    result.insert(res.at(i)["user_id"]);
  }
}

void AppFanFactor::Minus(set<int> & result) {

  string head = GenerateSQL();
  Statement sql;
  sql << head;

  sql << " AND user_id IN (";
  set<int>::iterator itr = result.begin();
  sql << *itr;
  for (++itr; itr != result.end(); ++itr) {
    sql << "," << *itr;
  }
  sql << ")";

  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(instance_, CDbRServer, table_).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" AppFanFactor::Plus --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" AppFanFactor::Plus --> error:" << e.what());
  }
  if (!res) {
    MCE_WARN("StoryQueryResult wrong");
    return;
  }
  for (size_t i = 0; i < res.size(); ++i) {
    result.erase(res.at(i)["user_id"]);
  }

}

void AppFanFactor::And(set<int> & result) {
  string head = GenerateSQL();
  Statement sql;
  sql << head;

  sql << " AND user_id IN (";
  set<int>::iterator itr = result.begin();
  sql << *itr;
  for (++itr; itr != result.end(); ++itr) {
    sql << "," << *itr;
  }
  sql << ")";

  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(instance_, CDbRServer, table_).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" AppFanFactor::Plus --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" AppFanFactor::Plus --> error:" << e.what());
  }
  result.clear();
  if (!res) {
    MCE_WARN("StoryQueryResult wrong");
    return;
  }
  for (size_t i = 0; i < res.size(); ++i) {
    result.insert(res.at(i)["user_id"]);
  }

}

string AppFanFactor::GenerateSQL() { //app fans
  ostringstream tab;
  tab << "app_fans" << "_" << (param_id_ % 100);
  table_ = tab.str();
  ostringstream sql;
  sql << "SELECT user_id FROM " << table_ << " WHERE app_id=" << param_id_ << " ";
  string tmpstr = sql.str();
  return tmpstr;
}
//--------------------------------------------------------------------------

void PageFactor::Plus(set<int> & result) {
  string head = GenerateSQL();
  Statement sql;
  sql << head;
  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(instance_, CDbRServer, table_).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" PageFactor::Plus --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" PageFactor::Plus --> error:" << e.what());
  }
  for (size_t i = 0; i < res.size(); ++i) {
    result.insert(res.at(i)["user_id"]);
  }
}

void PageFactor::Minus(set<int> & result) {

  string head = GenerateSQL();
  Statement sql;
  sql << head;

  sql << " AND user_id IN (";
  set<int>::iterator itr = result.begin();
  sql << *itr;
  for (++itr; itr != result.end(); ++itr) {
    sql << "," << *itr;
  }
  sql << ")";

  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(instance_, CDbRServer, table_).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" PageFactor::Plus --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" PageFactor::Plus --> error:" << e.what());
  }
  if (!res) {
    MCE_WARN("StoryQueryResult wrong");
    return;
  }
  for (size_t i = 0; i < res.size(); ++i) {
    result.erase(res.at(i)["user_id"]);
  }

}

void PageFactor::And(set<int> & result) {

  string head = GenerateSQL();
  Statement sql;
  sql << head;

  sql << " AND user_id IN (";
  set<int>::iterator itr = result.begin();
  sql << *itr;
  for (++itr; itr != result.end(); ++itr) {
    sql << "," << *itr;
  }
  sql << ")";

  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(instance_, CDbRServer, table_).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" PageFactor::Plus --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" PageFactor::Plus --> error:" << e.what());
  }
  result.clear();
  if (!res) {
    MCE_WARN("StoryQueryResult wrong");
    return;
  }
  for (size_t i = 0; i < res.size(); ++i) {
    result.insert(res.at(i)["user_id"]);
  }

}

string PageFactor::GenerateSQL() {

  ServiceI &service = ServiceI::instance();
  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
  instance_ = props->getPropertyWithDefault("Service.Database.PageFans",
      "page_fans");
  table_ = props->getPropertyWithDefault("Service.table.PageFans", "page_fans");

  ostringstream pageFansTab;
  pageFansTab << table_ << "_" << (param_id_ % 100);
  table_ = pageFansTab.str();
  ostringstream sql;
  sql << "SELECT user_id FROM " << pageFansTab.str() << " WHERE page_id="
      << param_id_ << " AND state=0";
  string tmpstr = sql.str();
  //MCE_DEBUG("PageFactor::GenerateSQL --> the sql head = " << tmpstr);
  return tmpstr;

}
//--------------------------------------------------------------------------
void FollowFactor::Plus(set<int> & result) {
  string head = GenerateSQL();
  Statement sql;
  sql << head;
  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(instance_, CDbRServer, table_).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" PageFactor::Plus --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" PageFactor::Plus --> error:" << e.what());
  }
  for (size_t i = 0; i < res.size(); ++i) {
    result.insert(res.at(i)["follower_id"]);
  }
}

void FollowFactor::Minus(set<int> & result) {
  string head = GenerateSQL();
  Statement sql;
  sql << head;

  sql << " AND follower_id IN (";
  set<int>::iterator itr = result.begin();
  sql << *itr;
  for (++itr; itr != result.end(); ++itr) {
    sql << "," << *itr;
  }
  sql << ")";

  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(instance_, CDbRServer, table_).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" PageFactor::Plus --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" PageFactor::Plus --> error:" << e.what());
  }
  if (!res) {
    MCE_WARN("StoryQueryResult wrong");
    return;
  }
  for (size_t i = 0; i < res.size(); ++i) {
    result.erase(res.at(i)["follower_id"]);
  }

}

void FollowFactor::And(set<int> & result) {

  string head = GenerateSQL();
  Statement sql;
  sql << head;

  sql << " AND follower_id IN (";
  set<int>::iterator itr = result.begin();
  sql << *itr;
  for (++itr; itr != result.end(); ++itr) {
    sql << "," << *itr;
  }
  sql << ")";

  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(instance_, CDbRServer, table_).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" PageFactor::Plus --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" PageFactor::Plus --> error:" << e.what());
  }
  result.clear();
  if (!res) {
    MCE_WARN("StoryQueryResult wrong");
    return;
  }
  for (size_t i = 0; i < res.size(); ++i) {
    result.insert(res.at(i)["follower_id"]);
  }

}

string FollowFactor::GenerateSQL() {

  ServiceI &service = ServiceI::instance();
  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
  instance_ = props->getPropertyWithDefault("Service.Database.VFollows",
      "page");
  table_ = props->getPropertyWithDefault("Service.table.VFollows", "v_follow_list");

  //ostringstream pageFansTab;
  //pageFansTab << table_ << "_" << (param_id_ % 100);
  //table_ = pageFansTab.str();
  ostringstream sql;
  sql << "SELECT follower_id FROM " << table_ << " WHERE v_id="
      << param_id_;
  string tmpstr = sql.str();
  //MCE_DEBUG("PageFactor::GenerateSQL --> the sql head = " << tmpstr);
  return tmpstr;

}
//--------------------------------------------------------------------------


void FriendFactor::Plus(set<int> & result) {
  string head = GenerateSQL();
  Statement sql;
  sql << head;

  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(instance_, CDbRServer, table_).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" FriendFactor::Plus --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" FriendFactor::Plus --> error:" << e.what());
  }

  for (size_t i = 0; i < res.size(); ++i) {
    result.insert(res.at(i)["guest"]);
  }
}

void FriendFactor::Minus(set<int> & result) {

  string head = GenerateSQL();
  Statement sql;
  sql << head;

  sql << " AND guest IN (";
  set<int>::iterator itr = result.begin();
  sql << *itr;
  for (++itr; itr != result.end(); ++itr) {
    sql << "," << *itr;
  }
  sql << ")";

  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(instance_, CDbRServer, table_).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" FriendFactor::Plus --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" FriendFactor::Plus --> error:" << e.what());
  }
  if (!res) {
    MCE_WARN("StoryQueryResult wrong");
    return;
  }
  for (size_t i = 0; i < res.size(); ++i) {
    result.erase(res.at(i)["guest"]);
  }

}

void FriendFactor::And(set<int> & result) {
  string head = GenerateSQL();
  Statement sql;
  sql << head;

  sql << " AND guest IN (";
  set<int>::iterator itr = result.begin();
  sql << *itr;
  for (++itr; itr != result.end(); ++itr) {
    sql << "," << *itr;
  }
  sql << ")";

  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(instance_, CDbRServer, table_).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" FriendFactor::Plus --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" FriendFactor::Plus --> error:" << e.what());
  }
  result.clear();
  if (!res) {
    MCE_WARN("StoryQueryResult wrong");
    return;
  }
  for (size_t i = 0; i < res.size(); ++i) {
    result.insert(res.at(i)["guest"]);
  }

}

string FriendFactor::GenerateSQL() {
  ServiceI &service = ServiceI::instance();
  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
  instance_ = props->getPropertyWithDefault("Service.Database.Relation",
      "xfeed_relation");
  table_ = props->getPropertyWithDefault("Service.table.Relation", "relation_");
  ostringstream tablename;
  tablename << table_ << (param_id_ % 100); //表名
  ostringstream sql;
  sql << "SELECT guest FROM " << tablename.str() << " WHERE host=" << param_id_
      << " ";
  table_ = tablename.str();
  return sql.str();
}

//--------------------------------------------------------------------------


void HighSchoolFactor::Plus(set<int> & result) {
  string head = GenerateSQL();
  Statement sql;
  sql << head;
  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(instance_, CDbRServer, table_).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" HighSchoolFactor::Plus --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" HighSchoolFactor::Plus --> error:" << e.what());
  }

  ostringstream oss;
  for (size_t i = 0; i < res.size(); ++i) {
    result.insert(res.at(i)["uid"]);
    oss << res.at(i)["uid"] << ",";
  }
  //MCE_DEBUG("FriendFactor::Plus --> result ids --> " << oss.str());
}

void HighSchoolFactor::Minus(set<int> & result) {

  string head = GenerateSQL();
  Statement sql;
  sql << head;

  sql << " AND uid IN (";
  set<int>::iterator itr = result.begin();
  sql << *itr;
  for (++itr; itr != result.end(); ++itr) {
    sql << "," << *itr;
  }
  sql << ")";

  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(instance_, CDbRServer, table_).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" HighSchoolFactor::Plus --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" HighSchoolFactor::Plus --> error:" << e.what());
  }
  if (!res) {
    MCE_WARN("StoryQueryResult wrong");
    return;
  }
  for (size_t i = 0; i < res.size(); ++i) {
    result.erase(res.at(i)["uid"]);
  }
}

void HighSchoolFactor::And(set<int> & result) {

  string head = GenerateSQL();
  Statement sql;
  sql << head;

  sql << " AND uid IN (";
  set<int>::iterator itr = result.begin();
  sql << *itr;
  for (++itr; itr != result.end(); ++itr) {
    sql << "," << *itr;
  }
  sql << ")";

  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(instance_, CDbRServer, table_).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" HighSchoolFactor::Plus --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" HighSchoolFactor::Plus --> error:" << e.what());
  }
  result.clear();
  if (!res) {
    MCE_WARN("StoryQueryResult wrong");
    return;
  }
  for (size_t i = 0; i < res.size(); ++i) {
    result.insert(res.at(i)["uid"]);
  }

}

string HighSchoolFactor::GenerateSQL() {
  ServiceI &service = ServiceI::instance();
  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
  instance_ = props->getPropertyWithDefault("Service.Database.HIGHSCHOOL",
      "high_school_page");
  table_ = props->getPropertyWithDefault("Service.Table.ASP_MEMBER",
      "asp_member");
  ostringstream sql;
  sql << "SELECT uid FROM " << table_ << " WHERE pid=" << param_id_
      << "  AND status=0 AND stage <= 30 ";
  return sql.str();
}


//--------------------------------------------------------------------------


void MiniGroupFactor::Plus(set<int> & result) {
  string head = GenerateSQL();
  Statement sql;
  sql << head ;
  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(instance_, CDbRServer, table_).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" MiniGroupFactor::Plus --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" MiniGroupFactor::Plus --> error:" << e.what());
  }

  ostringstream oss;
  for (size_t i = 0; i < res.size(); ++i) {
    result.insert(res.at(i)["member_id"]);
//    oss << res.at(i)["uid"] << ",";
  }
  //MCE_DEBUG("FriendFactor::Plus --> result ids --> " << oss.str());
}

void MiniGroupFactor::Minus(set<int> & result) {

  string head = GenerateSQL();
  Statement sql;
  sql << head;

  sql << " AND member_id IN (";
  set<int>::iterator itr = result.begin();
  sql << *itr;
  for (++itr; itr != result.end(); ++itr) {
    sql << "," << *itr;
  }
  sql << ")";

  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(instance_, CDbRServer, table_).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" MiniGroupFactor::Plus --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" MiniGroupFactor::Plus --> error:" << e.what());
  }
  if (!res) {
    MCE_WARN("StoryQueryResult wrong");
    return;
  }
  for (size_t i = 0; i < res.size(); ++i) {
    result.erase(res.at(i)["member_id"]);
  }
}

void MiniGroupFactor::And(set<int> & result) {

  string head = GenerateSQL();
  Statement sql;
  sql << head;

  sql << " AND member_id IN (";
  set<int>::iterator itr = result.begin();
  sql << *itr;
  for (++itr; itr != result.end(); ++itr) {
    sql << "," << *itr;
  }
  sql << ")";

  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(instance_, CDbRServer, table_).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" MiniGroupFactor::Plus --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" MiniGroupFactor::Plus --> error:" << e.what());
  }
  result.clear();
  if (!res) {
    MCE_WARN("StoryQueryResult wrong");
    return;
  }
  for (size_t i = 0; i < res.size(); ++i) {
    result.insert(res.at(i)["member_id"]);
  }

}

string MiniGroupFactor::GenerateSQL() {
  instance_ = "minigroup";//TODO
  ostringstream tmp;
  tmp << "minigroup_member_" <<  param_id_ % 100;
  table_ = tmp.str();
  ostringstream sql;
  sql << "SELECT member_id FROM " << table_ << " WHERE minigroup_id = " << param_id_ << " and access_id in (1,2,3) " ;
  return sql.str();

}
//--------------------------------------------------------------------------


void UserFactor::Plus(set<int> & result) {
  result.insert(param_id_);
}

void UserFactor::Minus(set<int> & result) {
  result.erase(param_id_);
}

void UserFactor::And(set<int> & result) {
  if (result.find(param_id_) != result.end()) {
    result.clear();
    result.insert(param_id_);
  } else {
    result.clear();
  }
}

//-------------------------------------------------------------------------
void XiaoZuFactor::Plus(set<int> & result) {
  string head = GenerateSQL();
  Statement sql;
  sql << head ;
  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(instance_, CDbRServer, table_).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" MiniGroupFactor::Plus --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" MiniGroupFactor::Plus --> error:" << e.what());
  }

  ostringstream oss;
  for (size_t i = 0; i < res.size(); ++i) {
    result.insert(res.at(i)["member_id"]);
//    oss << res.at(i)["uid"] << ",";
  }
  //MCE_DEBUG("FriendFactor::Plus --> result ids --> " << oss.str());
}

void XiaoZuFactor::Minus(set<int> & result) {

  string head = GenerateSQL();
  Statement sql;
  sql << head;

  sql << " AND member_id IN (";
  set<int>::iterator itr = result.begin();
  sql << *itr;
  for (++itr; itr != result.end(); ++itr) {
    sql << "," << *itr;
  }
  sql << ")";

  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(instance_, CDbRServer, table_).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" MiniGroupFactor::Plus --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" MiniGroupFactor::Plus --> error:" << e.what());
  }
  if (!res) {
    MCE_WARN("StoryQueryResult wrong");
    return;
  }
  for (size_t i = 0; i < res.size(); ++i) {
    result.erase(res.at(i)["member_id"]);
  }
}

void XiaoZuFactor::And(set<int> & result) {

  string head = GenerateSQL();
  Statement sql;
  sql << head;

  sql << " AND member_id IN (";
  set<int>::iterator itr = result.begin();
  sql << *itr;
  for (++itr; itr != result.end(); ++itr) {
    sql << "," << *itr;
  }
  sql << ")";

  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(instance_, CDbRServer, table_).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" MiniGroupFactor::Plus --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" MiniGroupFactor::Plus --> error:" << e.what());
  }
  result.clear();
  if (!res) {
    MCE_WARN("StoryQueryResult wrong");
    return;
  }
  for (size_t i = 0; i < res.size(); ++i) {
    result.insert(res.at(i)["member_id"]);
  }

}

string XiaoZuFactor::GenerateSQL() {
  instance_ = "forum";
  ostringstream tmp;
  tmp << "forum_member_" << param_id_%100;
  table_ = tmp.str();
  ostringstream sql;
  sql << "SELECT member_id FROM " << table_ << " WHERE forum_id=" << param_id_ << " and access_id in (1,2,3) " ;
  return sql.str();

}
//---------------------------------------------------------------
void MiniSiteFactor::Plus(set<int> & result) {
  string head = GenerateSQL();
  Statement sql;
  sql << head ;
  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(instance_, CDbRServer, table_).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" MiniSiteFactor::Plus --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" MiniSiteFactor::Plus --> error:" << e.what());
  }

  ostringstream oss;
  for (size_t i = 0; i < res.size(); ++i) {
    result.insert(res.at(i)["follower_id"]);
//    oss << res.at(i)["uid"] << ",";
  }
  //MCE_DEBUG("FriendFactor::Plus --> result ids --> " << oss.str());
}

void MiniSiteFactor::Minus(set<int> & result) {

  string head = GenerateSQL();
  Statement sql;
  sql << head;

  sql << " AND follower_id IN (";
  set<int>::iterator itr = result.begin();
  sql << *itr;
  for (++itr; itr != result.end(); ++itr) {
    sql << "," << *itr;
  }
  sql << ")";

  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(instance_, CDbRServer, table_).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" MiniSiteFactor::Plus --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" MiniSiteFactor::Plus --> error:" << e.what());
  }
  if (!res) {
    MCE_WARN("StoryQueryResult wrong");
    return;
  }
  for (size_t i = 0; i < res.size(); ++i) {
    result.erase(res.at(i)["follower_id"]);
  }
}

void MiniSiteFactor::And(set<int> & result) {

  string head = GenerateSQL();
  Statement sql;
  sql << head;

  sql << " AND follower_id IN (";
  set<int>::iterator itr = result.begin();
  sql << *itr;
  for (++itr; itr != result.end(); ++itr) {
    sql << "," << *itr;
  }
  sql << ")";

  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(instance_, CDbRServer, table_).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" MiniSiteFactor::Plus --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" MiniSiteFactor::Plus --> error:" << e.what());
  }
  result.clear();
  if (!res) {
    MCE_WARN("StoryQueryResult wrong");
    return;
  }
  for (size_t i = 0; i < res.size(); ++i) {
    result.insert(res.at(i)["follower_id"]);
  }

}

string MiniSiteFactor::GenerateSQL() {
  instance_ = "minisite";
  ostringstream tmp;
  table_ = "minisite_follower";
  ostringstream sql;
  sql << "SELECT follower_id FROM " << table_ << " WHERE site_id=" << param_id_  ;
  return sql.str();

}
//---------------------------------------------------------------

void CheWenFactor::Plus(set<int> & result) {
  string head = GenerateSQL();
  Statement sql;
  sql << head ;
  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(instance_, CDbRServer, table_).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" MiniSiteFactor::Plus --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" MiniSiteFactor::Plus --> error:" << e.what());
  }

  ostringstream oss;
  for (size_t i = 0; i < res.size(); ++i) {
    result.insert(res.at(i)["renren_id"]);
//    oss << res.at(i)["uid"] << ",";
  }
  //MCE_DEBUG("FriendFactor::Plus --> result ids --> " << oss.str());
}

void CheWenFactor::Minus(set<int> & result) {

  string head = GenerateSQL();
  Statement sql;
  sql << head;

  sql << " AND renren_id IN (";
  set<int>::iterator itr = result.begin();
  sql << *itr;
  for (++itr; itr != result.end(); ++itr) {
    sql << "," << *itr;
  }
  sql << ")";

  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(instance_, CDbRServer, table_).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" MiniSiteFactor::Plus --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" MiniSiteFactor::Plus --> error:" << e.what());
  }
  if (!res) {
    MCE_WARN("StoryQueryResult wrong");
    return;
  }
  for (size_t i = 0; i < res.size(); ++i) {
    result.erase(res.at(i)["renren_id"]);
  }
}

void CheWenFactor::And(set<int> & result) {

  string head = GenerateSQL();
  Statement sql;
  sql << head;

  sql << " AND renren_id IN (";
  set<int>::iterator itr = result.begin();
  sql << *itr;
  for (++itr; itr != result.end(); ++itr) {
    sql << "," << *itr;
  }
  sql << ")";

  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(instance_, CDbRServer, table_).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" MiniSiteFactor::Plus --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" MiniSiteFactor::Plus --> error:" << e.what());
  }
  result.clear();
  if (!res) {
    MCE_WARN("StoryQueryResult wrong");
    return;
  }
  for (size_t i = 0; i < res.size(); ++i) {
    result.insert(res.at(i)["renren_id"]);
  }
}

string CheWenFactor::GenerateSQL() {
  instance_ = "chewen_user";
  table_ = "cw_followed_";
  ostringstream tablename;
  tablename << table_ << (param_id_ % 10); //表名
  ostringstream sql;
  sql << "SELECT renren_id FROM " << tablename.str() << " WHERE map_renren_id=" << param_id_;
  return sql.str();
}
