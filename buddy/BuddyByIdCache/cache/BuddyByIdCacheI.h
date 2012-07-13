#ifndef __BUDDY_BYID_CACHE_I_H__
#define __BUDDY_BYID_CACHE_I_H__

#include "BuddyByIdCache.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
#include <string>
#include "UtilCxx/src/Singleton.h"
#include "IceExt/src/ObjectCacheI.h"
#include "IceExt/src/ServiceI.h"

namespace xce {
  namespace buddy {

    const int BUDDY_BYID_CACHE_DATA = 0;

    const int TASK_LEVEL_BUDDY_BYID_CACHE_FILL = 1;
    const int TASK_LEVEL_BUDDY_BYID_CACHE_LOAD = 2;
    const int TASK_LEVEL_BUDDY_BYID_CACHE_RELOAD = 3;
    const int TASK_LEVEL_BUDDY_BYID_CACHE_REMOVE = 4;
    /*****************************************************************************/

    class BuddyByIdDataI: virtual public Ice::Object {
      public:
        BuddyByIdDataI();
        BuddyByIdDataI(BuddyByIdDataPtr);
        BuddyByIdDataI(::MyUtil::IntSeq&);
        Ice::Int getCount();
        ::MyUtil::IntSeq getList(::Ice::Int,::Ice::Int);
        void remove(::Ice::Int);
      private:
        ::MyUtil::IntSeq _friends;
        IceUtil::Mutex _mutex;
    };
    typedef IceUtil::Handle<BuddyByIdDataI> BuddyByIdDataIPtr;

    class BuddyByIdCacheManagerI: virtual public BuddyByIdCacheManager, virtual public MyUtil::Singleton<BuddyByIdCacheManagerI> {
      public:
        void bootstrap(const string& endpoint, int mod);
        virtual ::Ice::Int getFriendCount(::Ice::Int, const Ice::Current&);
        virtual ::MyUtil::Int2IntMap getFriendCounts(const MyUtil::IntSeq&, const Ice::Current&);
        virtual ::MyUtil::IntSeq getFriendList(::Ice::Int, ::Ice::Int, const Ice::Current&);
        virtual ::MyUtil::IntSeq getFriendListN(::Ice::Int, ::Ice::Int, ::Ice::Int, const Ice::Current&);
        virtual ::MyUtil::Int2IntSeqMap getFriendLists(const MyUtil::IntSeq&, const Ice::Current&);
        virtual void load( ::Ice::Int, const ::Ice::ObjectPtr&, const Ice::Current&);
        virtual void remove( ::Ice::Int, ::Ice::Int, const Ice::Current&);

        virtual bool isValid(const Ice::Current&);
        virtual void setValid(bool valid,const Ice::Current& = Ice::Current());
        virtual void setData(const ::MyUtil::ObjectResultPtr& BuddyData, const Ice::Current&);

      private:
        string _endpoint;
        int _mod;
        bool _isValid;

        IceUtil::RWRecMutex _validMutex;
        friend class MyUtil::Singleton<BuddyByIdCacheManagerI>;
        BuddyByIdCacheManagerI(){
        }
        BuddyByIdDataIPtr getBuddyByIdDataI(Ice::Int userId);
    };

    /*****************************************************************************/

    class BuddyByIdCacheFactory : virtual public MyUtil::ServantFactory {
      public:
        virtual Ice::ObjectPtr create(Ice::Int id);
    };

    /*****************************************************************************/

    class OrderByIdAscHandler : virtual public com::xiaonei::xce::ResultHandler{
      public:
        OrderByIdAscHandler(std::map<int, int>& data,
            const std::string& keyField, const std::string& valueField ) :
          _data(data), _keyField(keyField), _valueField(valueField) {
          }
      protected:
        virtual bool handle(mysqlpp::Row& res) const;
      private:
        std::map<int, int>& _data;
        std::string _keyField;
        std::string _valueField;
    };

    class StatusFilterHandler : virtual public com::xiaonei::xce::ResultHandler{
      public:
        StatusFilterHandler(std::map<int, int>& orderFriend, ostringstream& filter_str, bool& filted, const std::string& idField, const std::string& statusField="") :
          _orderFriend(orderFriend), _filter_str(filter_str), _filted(filted), _idField(idField), _statusField(statusField) {
          }
      protected:
        virtual bool handle(mysqlpp::Row& res) const;
      private:
        std::map<int, int>& _orderFriend;
        ostringstream& _filter_str;
        bool& _filted;
        std::string _idField;
        std::string _statusField;
    };

    /*****************************************************************************/

    class FillTask: virtual public MyUtil::Task {
      public:
        FillTask(const MyUtil::ObjectResultPtr& buddyData,::Ice::Int level=TASK_LEVEL_BUDDY_BYID_CACHE_FILL):
          MyUtil::Task(level), _buddyData(buddyData) {
          }
        virtual void handle();
      private:
        MyUtil::ObjectResultPtr _buddyData;
    };

    class LoadTask: virtual public MyUtil::Task {
      public:
        LoadTask(Ice::Int id, const Ice::ObjectPtr& obj, Ice::Int level=TASK_LEVEL_BUDDY_BYID_CACHE_LOAD):
          MyUtil::Task(level), _id(id), _obj(obj)  {
          }
        virtual void handle();
      private:
        Ice::Int _id;
        Ice::ObjectPtr _obj;

    };

    class ReloadTask: virtual public MyUtil::Task {
      public:
        ReloadTask(Ice::Int id, Ice::Int level=TASK_LEVEL_BUDDY_BYID_CACHE_RELOAD):
          MyUtil::Task(level), _id(id) {
          }
        virtual void handle();
      private:
        Ice::Int _id;

    };

    class RemoveTask : virtual public MyUtil::Task {
      public:
        RemoveTask( ::Ice::Int hostId, ::Ice::Int guestId, ::Ice::Int level=TASK_LEVEL_BUDDY_BYID_CACHE_REMOVE ) :
          Task(level),_hostId(hostId),_guestId(guestId){
          }
      protected:
        virtual void handle();
      private:
        ::Ice::Int _hostId;
        ::Ice::Int _guestId;
    };

    //***************************************************************************

    class ServiceSchemaI: public MyUtil::Descriptor {
      public:
        virtual void configure(const Ice::PropertiesPtr& props);
    };

  }
}

#endif
