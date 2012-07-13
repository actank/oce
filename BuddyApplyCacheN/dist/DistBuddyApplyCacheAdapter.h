#ifndef __DIST_BUDDYAPPLYCACHE_ADAPTER_H__
#define __DIST_BUDDYAPPLYCACHE_ADAPTER_H__

#include "util/cpp/TimeCost.h"
#include "ServiceI.h"
#include "XceDistCacheAdapter.h"
#include "BuddyApplyCacheData.pb.h"
#include <sys/time.h>
#include <set>
#include <map>
#include <vector> 
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <stdlib.h>
#include <stdio.h>
#include "util/cpp/TimeCost.h"
#include <QueryRunner.h>

namespace xce
{
  namespace distcache
  {
    namespace buddyapplycache
    {

      class BuddyApplyCacheData : 
        public xce::distcache::buddyapplycache::ApplyDataSeq, 
        public Ice::Object 
      {
        public:
          BuddyApplyCacheData(){}
      };

      typedef IceUtil::Handle<BuddyApplyCacheData> BuddyApplyCacheDataPtr;
      
      class BuddyApplyCacheResultHandlerI: public com::xiaonei::xce::ResultHandler {
        public:
          BuddyApplyCacheResultHandlerI(
              const BuddyApplyCacheDataPtr& result) :
            _result(result) {
            }
          virtual bool handle(mysqlpp::Row& row) const {
            ApplyData* data = _result->add_item();
            data->set_applicant((int) row["applicant"]);
            data->set_time((int) row["unix_time"]);
            data->set_why((string) row["why"]);
            data->set_statfrom((string) row["stat_from"]);
            data->set_groupname((string) row["group_name"]);
            return true;
          }
        private:
          xce::distcache::buddyapplycache::BuddyApplyCacheDataPtr _result;
      };

      class BuddyApplyFactory : 
        virtual public xce::distcache::MemFactory,
        virtual public MyUtil::Singleton<BuddyApplyFactory> {
          Ice::ObjectPtr create(::Ice::Int id) {
            MCE_DEBUG("BuddyApplyFactory::create(" << id << ")");
            BuddyApplyCacheDataPtr result = new BuddyApplyCacheData();
            com::xiaonei::xce::Statement sql;
            sql
              << "SELECT applicant,UNIX_TIMESTAMP(time) AS unix_time,why,stat_from,group_name"
              << " FROM buddyapply_accepter_" << abs(id % 100) << " WHERE accepter = '" << id << "'"
              << " ORDER BY time DESC"
              << " LIMIT 100";
            BuddyApplyCacheResultHandlerI handler(result);
            ostringstream pattern;
            pattern << "buddyapply_accepter_" << id % 100;
            com::xiaonei::xce::QueryRunner("buddycore_accepter", com::xiaonei::xce::CDbRServer,pattern.str()).query(sql, handler);
            return result;
          }
        };

      class DistBuddyApplyCacheAdapter : 
        public xce::distcache::XceDistCacheRWAdapter<BuddyApplyCacheData, BuddyApplyCacheDataPtr>, 
        public MyUtil::Singleton<DistBuddyApplyCacheAdapter> 
      {
        public:
          void initialize() 
          {
            std::string name("BuddyApplyCache");
            init(name);
          }
      };
    }
  }
}
#endif
