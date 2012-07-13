#ifndef _SOCIALGRAPH_DBUTIL_H__
#define _SOCIALGRAPH_DBUTIL_H__

#include <vector>
#include <string>

//#include "dbpool/queryrunner.h"
#include "DbCxxPool/src/QueryRunner.h"

namespace xce {
namespace socialgraph {

class DbUtil {
 public:
  static DbUtil& GetInstance();
  DbUtil();
  virtual ~DbUtil();
  std::vector<int> GetSgBlockListByBiz(int host, const std::string& biz = "NON");
  std::vector<int> GetSgBlockList(int host);
};


class SgBlockResultHandler : public com::xiaonei::xce::ResultHandler {
 public:
  SgBlockResultHandler(std::vector<int>& seq) : seq_(seq) {}
  virtual bool handle(mysqlpp::Row& row) const;
 private:
  std::vector<int>& seq_;
};

}}

#endif

