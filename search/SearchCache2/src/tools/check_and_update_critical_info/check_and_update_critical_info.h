/// 
/// @file check_and_update_critical_info.h
/// @brief 对于用户名，headurl，network几个关键信息，将TT中的值与数据库中的值相比较，若不一致，则更新相应的TT
/// @author zhigang.wu
/// @date 2010-04-24
/// 
#ifndef __CHECK_VIEW_COUNT_H_
#define __CHECK_VIEW_COUNT_H_

#include <boost/shared_ptr.hpp> 
#include <set>
#include <vector>
#include <map>
#include <string>
#include <IceUtil/IceUtil.h>
#include "QueryRunner.h"
#include "../../Util/IDbCache.h"

using std::string;
using std::set;
using std::map;
using std::vector;
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
  //std::string Int2String(int user_id) const;
private:
  std::set<int>& valid_ids_set_;
  std::map<int,string>& id_2_last_login_time_;
};

class BatchUserNameResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	BatchUserNameResultHandlerI(std::set<int>& valid_ids_set, std::map<int, string>& id_2_name);
	virtual bool handle(mysqlpp::Row& row) const;
  //std::string Int2String(int user_id) const;
private:
  std::set<int>& valid_ids_set_;
  std::map<int,string>& id_2_name_;
};

class BatchHeadUrlResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	BatchHeadUrlResultHandlerI(std::set<int>& valid_ids_set, std::map<int, string>& id_2_head_url);
	virtual bool handle(mysqlpp::Row& row) const;
  //std::string Int2String(int user_id) const;
private:
  std::set<int>& valid_ids_set_;
  std::map<int,string>& id_2_head_url_;
};

//***************************************************************************
class BatchNetworkHistoryResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	BatchNetworkHistoryResultHandlerI(std::set<int>& valid_ids_set, std::map<int, std::vector<Network> >& id_2_network_history);
	virtual bool handle(mysqlpp::Row& row) const;
  //std::string Int2String(int user_id) const;
private:
  std::set<int>& valid_ids_set_;
  std::map<int, std::vector<Network> >& id_2_network_history_;
};



}
}
#endif
