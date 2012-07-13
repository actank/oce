#ifndef __BUDDY_HOT_FACTORY_H__
#define __BUDDY_HOT_FACTORY_H__

#include "ServiceI.h"
#include <QueryRunner.h>
#include "BuddyHotCache.h"
#include "CriticalHotData.h"
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"

namespace xce{
  namespace buddy{

    const static string DB_BUDDY_HOT_FRIEND = "relation_info";
    const static string DB_HOT_FRIEND_TABLE = "hotfriend";

    class BuddyHotResultHandler : virtual public com::xiaonei::xce::ResultHandler{
      public:
        BuddyHotResultHandler(HotDataSeq& data) :
          _data(data){
          }
      protected:
        virtual bool handle(mysqlpp::Row& res) const{
          HotDataPtr da = new HotData();
          da->hotfriend = res["GUEST"];
          da->timestamp = res["UNIXTIME"];
          da->memo = (string)res["MEMO"];
          da->position = res["POSITION"];
          _data.push_back(da);
          return true;
        }
      private:
        HotDataSeq& _data;
    };

    class BuddyHotCacheFactory : 
      virtual public MyUtil::ServantFactory{
      public:
        virtual Ice::ObjectPtr create(Ice::Int id){
          com::xiaonei::xce::Statement sql;
          sql << "SELECT GUEST,UNIX_TIMESTAMP(TIME) AS UNIXTIME,MEMO,POSITION FROM " 
            << DB_HOT_FRIEND_TABLE<<" WHERE HOST = '" << id << "' ORDER BY POSITION";
          HotDataSeq data;
          com::xiaonei::xce::QueryRunner(DB_BUDDY_HOT_FRIEND, com::xiaonei::xce::CDbRServer).query(sql, BuddyHotResultHandler(data));
          CriticalHotDataPtr result = new CriticalHotData(data);
          return result;
        }

        virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids) {
          MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult();
          for( MyUtil::IntSeq::const_iterator idIt = ids.begin(); idIt != ids.end(); ++idIt ){
            CriticalHotDataPtr obj = CriticalHotDataPtr::dynamicCast(create(*idIt));
            res->data[*idIt] = obj;
          }
          return res;
        }

      };

    class BuddyHotLoaderFactory :
      virtual public xce::generalpreloader::IdListFactory {
      public:
        virtual Ice::ObjectPtr create(Ice::Int id){
          com::xiaonei::xce::Statement sql;
          sql << "SELECT GUEST,UNIX_TIMESTAMP(TIME) AS UNIXTIME,MEMO,POSITION FROM "
            << DB_HOT_FRIEND_TABLE<<" WHERE HOST = '" << id << "' ORDER BY POSITION";
          HotDataSeq data;
          com::xiaonei::xce::QueryRunner(DB_BUDDY_HOT_FRIEND, com::xiaonei::xce::CDbRServer).query(sql, BuddyHotResultHandler(data));
          BuddyHotDataPtr result = new BuddyHotData();
          data.swap(result->hotdatas);
          return result;
        }

        virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids) {
          MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult();
          for( MyUtil::IntSeq::const_iterator idIt = ids.begin(); idIt != ids.end(); ++idIt ){
            BuddyHotDataPtr obj = BuddyHotDataPtr::dynamicCast(create(*idIt));
            res->data[*idIt] = obj;
          }
          return res;
        }

      };

  }
}
#endif
