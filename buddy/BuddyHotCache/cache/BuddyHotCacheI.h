#ifndef __BUDDY_HOT_CACHE_I_H__
#define __BUDDY_HOT_CACHE_I_H__

#include "ServiceI.h"
#include "BuddyHotCache.h"
#include "../share/CriticalHotData.h"

namespace xce {
  namespace buddy {

    const static int BHC = 0;
    const static int TASK_LEVEL_BUDDY_HOT_CACHE_FILL = 1;
    class BuddyHotCacheManagerI : 
      virtual public MyUtil::Singleton<BuddyHotCacheManagerI>, 
      virtual public BuddyHotCacheManager {
        public:

          virtual void addHotFriend(::Ice::Int host, const HotDataPtr& data, const Ice::Current& = Ice::Current());

          virtual void deleteHotFriend(::Ice::Int host, const HotDataPtr& data, const Ice::Current& = Ice::Current());

          virtual void setHotFriend(::Ice::Int host, const HotDataSeq& data, const Ice::Current& = Ice::Current());

          virtual MyUtil::IntSeq getHotFriendList(int host, const Ice::Current& = Ice::Current());

          virtual HotDataSeq getHotFriendListData(int host, const Ice::Current& = Ice::Current());  

          virtual int getHotFriendCount(int host, const Ice::Current& = Ice::Current());

          virtual void reload(::Ice::Int host, const Ice::Current& = Ice::Current());

          virtual bool isValid(const Ice::Current& = Ice::Current());

          virtual void setValid(bool valid,const Ice::Current& = Ice::Current());

          virtual void setData(const ::MyUtil::ObjectResultPtr& objs, const Ice::Current& = Ice::Current());

          CriticalHotDataPtr locate(int host);
        private:

          friend class MyUtil::Singleton<BuddyHotCacheManagerI>;
          BuddyHotCacheManagerI()	{
          }
          BuddyHotCacheManagerI(const BuddyHotCacheManagerI&) {
          }


        private:
          bool _isValid; 
          IceUtil::RWRecMutex _validMutex;
      };

    class BuddyHotCacheDAO : 
      virtual public MyUtil::Singleton<BuddyHotCacheDAO> {
        public:
          void insertHotFriend(::Ice::Int host, const HotDataPtr& data);
          void deleteHotFriend(::Ice::Int host, const HotDataPtr& data);
          //deleteAll与addAll需成对使用
          void deleteAll(::Ice::Int host);
          void insertAll(::Ice::Int host, const HotDataSeq& data);
          friend class MyUtil::Singleton<BuddyHotCacheDAO>;
          BuddyHotCacheDAO()	{
          }
      };

    class AddHotFriendTask : virtual public MyUtil::Task {
      public:
        AddHotFriendTask(::Ice::Int host, 
            const HotDataPtr& data, 
            const Ice::Current& current) :
          _host(host),
          _data(data),
          _current(current){
          }
        virtual void handle();
      private:
        ::Ice::Int _host;
        HotDataPtr _data;
        Ice::Current _current;
    };

    class DeleteHotFriendTask : virtual public MyUtil::Task {
      public:
        DeleteHotFriendTask(::Ice::Int host,
            const HotDataPtr& data,
            const Ice::Current& current) :
          _host(host),
          _data(data),
          _current(current){
          }
        virtual void handle();
      private:
        ::Ice::Int _host;
        HotDataPtr _data;
        Ice::Current _current;
    };

    class SetHotFriendTask : virtual public MyUtil::Task {
      public:
        SetHotFriendTask(::Ice::Int host,
            const HotDataSeq& data,
            const Ice::Current& current) :
          _host(host),
          _data(data),
          _current(current){
          }
        virtual void handle();
      private:
        ::Ice::Int _host;
        HotDataSeq _data;
        Ice::Current _current;
    };

    class FillTask: virtual public MyUtil::Task {
      public:
        FillTask(const MyUtil::ObjectResultPtr& objs,::Ice::Int level=TASK_LEVEL_BUDDY_HOT_CACHE_FILL):
          MyUtil::Task(level), _objs(objs) {
          }
        virtual void handle();
      private:
        MyUtil::ObjectResultPtr _objs;
    };

    class ServiceSchemaI: public MyUtil::Descriptor {
      public:
        virtual void configure(const Ice::PropertiesPtr& props);
    };
  }
}

#endif
