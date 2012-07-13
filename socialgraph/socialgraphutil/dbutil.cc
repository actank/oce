#include "socialgraph/socialgraphutil/dbutil.h"

#include "base/once.h"


namespace xce {
namespace socialgraph {

const static std::string kDbSourceName = "socialgraph_block";
const static std::string kSgBlockTableName = "sg_block";

static DbUtil* dbutil_instance = NULL;

static void InitDbUtil() {
  dbutil_instance = new DbUtil();
}

DbUtil& DbUtil::GetInstance() {
  using namespace xce;
  static XCE_DECLARE_ONCE(once_guard_);
  XceOnceInit(&once_guard_, &InitDbUtil);
  return *dbutil_instance;
}

DbUtil::DbUtil() {
  //using namespace MyUtil;
  using namespace com::xiaonei::xce;
  ConnectionPoolManager::instance().initialize();
}

DbUtil::~DbUtil() {

}

std::vector<int> DbUtil::GetSgBlockListByBiz(int host, const std::string& biz) {
//  using namespace MyUtil;
  using namespace com::xiaonei::xce;
  std::vector<int> block_list;
  Statement sql;
  sql << "SELECT block_id FROM "
      << kSgBlockTableName << " WHERE source_id = " << host << " and business = '"
      << biz << "'";
  try {
    QueryRunner(kDbSourceName, CDbRServer).query(sql, SgBlockResultHandler(block_list));
  } catch (...) {
    //TODO: log...
  }
  return block_list;
}

std::vector<int> DbUtil::GetSgBlockList(int host) {
  using namespace MyUtil;
  using namespace com::xiaonei::xce;
  std::vector<int> block_list;
  Statement sql;
  sql << "SELECT block_id FROM "
      << kSgBlockTableName << " WHERE source_id = " << host;
  try {
    QueryRunner(kDbSourceName, CDbRServer).query(sql, SgBlockResultHandler(block_list));
  } catch (...) {
    //TODO: log...
  }
  return block_list;

}


bool SgBlockResultHandler::handle(mysqlpp::Row& row) const {
  if (!row.empty()) {
    int block_id = (int) row["block_id"];
    seq_.push_back(block_id);
  }
  return true;
}

}}
