#ifndef __DIST_BUDDYBYIDCACHE_ADAPTER_H__
#define __DIST_BUDDYBYIDCACHE_ADAPTER_H__

#include "util/cpp/TimeCost.h"
#include "ServiceI.h"
#include "XceDistCacheAdapter.h"
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
#include "BuddyCacheData.h"

namespace xce
{
  namespace distcache
  {
    namespace buddybyidcache
    {
      class BuddyByIdCacheData : 
        virtual public xce::distcache::buddycache::BuddyCacheData
      {
        public:
          BuddyByIdCacheData(){}
      };

      typedef IceUtil::Handle<BuddyByIdCacheData> BuddyByIdCacheDataPtr;

      class BuddyByIdCacheResultHandlerI: public com::xiaonei::xce::ResultHandler {
        public:
          BuddyByIdCacheResultHandlerI(
              MyUtil::IntSeq& result) :
            _result(result) {
            }
          virtual bool handle(mysqlpp::Row& row) const {
            _result.push_back((int) row["guest"]);
            return true;
          }
        private:
          MyUtil::IntSeq& _result;
      };

      class BuddyByIdFactory : 
        virtual public xce::distcache::MemFactory{
          Ice::ObjectPtr create(::Ice::Int id) {
            BuddyByIdCacheDataPtr result = new BuddyByIdCacheData;
            MyUtil::IntSeq friends;
            MyUtil::IntSeq filteredfriends;
            com::xiaonei::xce::Statement sql;
            sql     << "SELECT guest"
              << " FROM relation_" << (id % 100)
              << " WHERE host = " << id;
            ostringstream pattern;
            pattern << "relation_" << (id % 100);
            com::xiaonei::xce::QueryRunner("relation_buddycache", com::xiaonei::xce::CDbRServer, pattern.str()).query(
                sql, BuddyByIdCacheResultHandlerI( friends));
            filter(id,friends).swap( filteredfriends );
            for(MyUtil::IntSeq::iterator it = filteredfriends.begin(); it!=filteredfriends.end(); ++it){
              result->add_friend_(*it);
            }
            result->set_size(result->friend__size());
            return result;
          }

          virtual MyUtil::IntSeq filter(int id,const MyUtil::IntSeq& buddies){
            MyUtil::IntSeq result(buddies);
            return result;
          }
        };

      class DistBuddyByIdCacheAdapter : 
        public xce::distcache::XceDistCacheRWAdapter<BuddyByIdCacheData, BuddyByIdCacheDataPtr>, 
        public MyUtil::Singleton<DistBuddyByIdCacheAdapter> 
      {
        public:
          void initialize() 
          {
            std::string name("BuddyByIdCache");
            init(name);
          }
      };

    }
  }
}
#endif
