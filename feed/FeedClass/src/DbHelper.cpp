/*
 * UserDb.cpp
 *
 *  Created on: Mar 10, 2011
 *      Author: yejingwei
 */

#include "DbHelper.h"
#include "QueryRunner.h"
using namespace MyUtil;
using namespace xce::feed;
using namespace IceUtil;
using namespace com::xiaonei::xce;

const int kDBClusterSize = 100;

string GroupDb::GetMinigroupTableName(int gid) {
  std::ostringstream os;
  os << "minigroup_member_" << gid % kDBClusterSize;
  return os.str();
}

bool GroupDb::GetMembers(int gid, vector<int>& mids) {
  if(gid == 1){//TODO 测试
    mids.push_back(2);
    return true;
  }
  string mini_table_name = GetMinigroupTableName(gid);
  Statement sql;
  sql << "SELECT member_id FROM " << mini_table_name << " WHERE minigroup_id="
      << gid;
  mysqlpp::StoreQueryResult res;
  try{
  res = QueryRunner("minigroup", CDbRServer,
      mini_table_name).store(sql);
  } catch (std::exception& e) {
    MCE_WARN("GroupDb::GetMembers:" << gid << " error:" << e.what());
    return false;
  }
  if (!res) {
    MCE_INFO("GroupDb::GetMembers. err. gid:" << gid);
    return false;
  }
  for (size_t i = 0; i < res.num_rows(); ++i) {
    mysqlpp::Row row = res.at(i);
    mids.push_back(row["member_id"]);
  }
  MCE_INFO("GroupDb::GetMembers. gid:" << gid << " result size:" << mids.size());
  return true;

}
