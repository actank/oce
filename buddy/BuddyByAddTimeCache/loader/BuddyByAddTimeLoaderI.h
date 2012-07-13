#ifndef __BUDDY_BYADDTIME_LOADER_I_H__
#define __BUDDY_BYADDTIME_LOADER_I_H__

#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
#include <ServiceI.h>
#include "ReplicaCluster/CacheLoader/src/CacheLoader.h"
#include "BuddyByAddTimeCache.h"

namespace xce {
  namespace buddy {

    const int TASK_LEVEL_BUDDY_BYADDTIME_LOADER_LOAD = 1;
    const int TASK_LEVEL_BUDDY_BYADDTIME_LOADER_RELOAD = 2;

    class BuddyByAddTimeLoaderI : virtual public BuddyByAddTimeLoader,
    virtual public MyUtil::Singleton<BuddyByAddTimeLoaderI> {
      public:
        virtual void reload(::Ice::Int, const Ice::Current&);
        virtual void load(const MyUtil::Int2IntSeqMap&, const Ice::Current&);
      private:
        friend class MyUtil::Singleton<BuddyByAddTimeLoaderI>;
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

    class SetDataHelper: public MyUtil::Singleton<SetDataHelper> {
      public:
        void initialize();
        void invokeDelay( Ice::Int, const Ice::ObjectPtr&, const std::string& );
        void invokeNow();
        void invokeNow( Ice::Int, const Ice::ObjectPtr&, const std::string& );
        void swap(std::vector< pair<long,Ice::ObjectPtr> >& empty_datas,std::vector< pair<long,std::string> >& empty_times);
      private:
        IceUtil::Monitor<IceUtil::Mutex> mutex_;
        std::vector< pair<long,Ice::ObjectPtr> > datas_;
        std::vector< pair<long,std::string> > times_;

        class SetDataThread: virtual public IceUtil::Thread {
          public:
            SetDataThread(SetDataHelper& helper) :
              helper_(helper) {
              }
            virtual void run();
          private:
            SetDataHelper& helper_;
        };
        typedef IceUtil::Handle<SetDataThread> SetDataThreadPtr;

        SetDataThreadPtr setter_;
    };

    class ReloadTask : virtual public MyUtil::Task {
      public:
        ReloadTask( ::Ice::Int id, ::Ice::Int level=TASK_LEVEL_BUDDY_BYADDTIME_LOADER_RELOAD ) :
          Task(level),_id(id){
          }
      protected:
        virtual void handle();
      private:
        ::Ice::Int _id;
    };

    class LoadTask : virtual public MyUtil::Task {
      public:
        LoadTask( const MyUtil::Int2IntSeqMap& lists,
            Ice::Int level=TASK_LEVEL_BUDDY_BYADDTIME_LOADER_LOAD ) :
          Task(level),_lists(lists){
          }
      protected:
        virtual void handle();
      private:
        MyUtil::Int2IntSeqMap _lists;
    };

    //***************************************************************************

    class ServiceSchemaI: public MyUtil::Descriptor {
      public:
        virtual void configure(const Ice::PropertiesPtr& props);
    };

  }
}

#endif
