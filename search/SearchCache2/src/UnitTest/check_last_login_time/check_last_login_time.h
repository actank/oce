#ifndef __CHECK_VIEW_COUNT_H_
#define __CHECK_VIEW_COUNT_H_

#include <boost/shared_ptr.hpp> 
#include <set>
#include <string>
#include <IceUtil/IceUtil.h>
#include "QueryRunner.h"
#include "../../Util/IDbCache.h"

using std::string;
using std::set;
using std::map;
namespace xce{
namespace searchcache{


class DbHandler {
 private:
   int cluster_;
   DbHandler(){}
   DbHandler& operator=(const DbHandler& rhs);
   static DbHandler* instance_;
   static IceUtil::Mutex mutex_;
   map<int, boost::shared_ptr<IDbCache> > db_handler_map_;
 public:
   bool Initialize();
   ~DbHandler();
   static DbHandler& instance() {
     if (!instance_) {
       IceUtil::Mutex::Lock lock(mutex_);
       if (!instance_) {
         instance_ = new DbHandler();
       }
     }
     return *instance_;
   }

   boost::shared_ptr<IDbCache> getDbHandler(int user_id);
};
//***************************************************************************
class BasicStatusResultHandler2I: public com::xiaonei::xce::ResultHandler {
public:
	BasicStatusResultHandler2I(Ice::Int& maxId, std::set<int>& valid_ids_set);
	virtual bool handle(mysqlpp::Row& row) const;
private:
	Ice::Int& max_id_;
  std::set<int>& valid_ids_set_;
};


//***************************************************************************
class BatchLastLoginTimeResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	BatchLastLoginTimeResultHandlerI(std::set<int>& valid_ids_set, std::map<int, string>& id_2_last_login_time);
	virtual bool handle(mysqlpp::Row& row) const;
  std::string Int2String(int user_id) const;
private:
  std::set<int>& valid_ids_set_;
  std::map<int,string>& id_2_last_login_time_;
};

}
}
#endif
