#ifndef __BUDDY_BYADDTIME_PRELOADER_I_H__
#define __BUDDY_BYADDTIME_PRELOADER_I_H__

#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
#include "ReplicaCluster/CacheLoader/src/CacheLoader.h"
#include "BuddyByAddTimeCache.h"
#include "ServiceI.h"

namespace xce {
  namespace buddy {

    class BuddyByAddTimePreloaderI : virtual public MyUtil::Singleton<BuddyByAddTimePreloaderI> {
      private:
        friend class MyUtil::Singleton<BuddyByAddTimePreloaderI>;
    };

    class BuddyByAddTimePreloaderFactory: virtual public xce::cacheloader::IdListFactory, virtual public MyUtil::Singleton<BuddyByAddTimePreloaderFactory>  {
      protected:
        friend class MyUtil::Singleton<BuddyByAddTimePreloaderFactory>;
      public:
        virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
    };

    class OrderedListHandler : virtual public com::xiaonei::xce::ResultHandler{
      public:
        OrderedListHandler(MyUtil::IntSeq& buddies,std::multimap<int, int>& data,
            const std::string& keyField, const std::string& valueField ) :
          _buddies(buddies), _data(data), _keyField(keyField), _valueField(valueField) {
          }
      protected:
        virtual bool handle(mysqlpp::Row& res) const;
      private:
        MyUtil::IntSeq& _buddies;
        std::multimap<int, int>& _data;
        std::string _keyField;
        std::string _valueField;
    };

    class ServiceSchemaI: public MyUtil::Descriptor {
      public:
        virtual void configure(const Ice::PropertiesPtr& props);
    };

  }
}

#endif
