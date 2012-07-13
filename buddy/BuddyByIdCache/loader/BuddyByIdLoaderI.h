#ifndef __BUDDY_BYID_LOADER_I_H__
#define __BUDDY_BYID_LOADER_I_H__

#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
#include <ServiceI.h>
#include "ReplicaCluster/CacheLoader/src/CacheLoader.h"
#include "BuddyByIdCache.h"
#include "Checker/share/OperaData.h"
#include "../util/StatusCache.h"

namespace xce {
  namespace buddy {

    const int TASK_LEVEL_BUDDY_BYID_LOADER_LOAD = 1;
    const int TASK_LEVEL_BUDDY_BYID_LOADER_RELOAD = 2;
    const int TASK_LEVEL_BUDDY_BYID_LOADER_ADD = 3;
    const int TASK_LEVEL_BUDDY_BYID_LOADER_STATUS_UPDATE = 4;
    const int TASK_LEVEL_BUDDY_BYID_LOADER_FACADE = 5;
    const int TASK_LEVEL_BUDDY_BYID_LOADER_COUNT_RECORD_FCC = 6;
    const int TASK_LEVEL_BUDDY_BYID_LOADER_FACADENONREALTIME = 7;
    const int TASK_LEVEL_CHECK_ADD_BUDDY = 8;
    const int TASK_LEVEL_CHECK_BUDDY_DATA = 9;

    const unsigned int MAX_DATA_SIZE = 100;
    const int RELOAD_INTERNAL = 10000;

    class BuddyByIdStatusCache : virtual public StatusCache {
      public:
        virtual void reload(int id);
    };
    
    class StatusCacheInitThread: public IceUtil::Thread {
      public:
        virtual void run(){
          bool isReady = false;
          int tryLimit = 10;
          while( !isReady && (--tryLimit)>0 ){
            MCE_INFO("[StatusCacheInitThread::run()] trying to get status from StatusCache:" << tryLimit );
            try{
              isReady = xce::statuscache::StatusCacheAdapter::instance().isValid();
            } catch( Ice::Exception e ){
              MCE_WARN("[StatusCacheAdapter::isValid()] found Ice::Exception : " << e.what() );
              continue;
            } catch( ... ){
              MCE_WARN("[StatusCacheAdapter::isValid()] found unkown exception : " );
              continue;
            }
            sleep(1);
          }
          if( isReady ){
            BuddyByIdStatusCache::instance().initializeFromSvc();
          } else{
            BuddyByIdStatusCache::instance().initializeFromDB();
          }

        }
    };
    typedef IceUtil::Handle<StatusCacheInitThread> StatusCacheInitThreadPtr;

    class BuddyByIdLoaderFactory: virtual public xce::cacheloader::IdListFactory, virtual public MyUtil::Singleton<BuddyByIdLoaderFactory>  {
      protected:
        friend class MyUtil::Singleton<BuddyByIdLoaderFactory>;
      public:
        Ice::ObjectPtr create(Ice::Int id);
        virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);

        Ice::ObjectPtr createAllFriends(Ice::Int id);
    };

    class BuddyByIdLoaderI : virtual public BuddyByIdLoader,
    virtual public MyUtil::Singleton<BuddyByIdLoaderI> {
      public:
        virtual void load(::Ice::Int,const MyUtil::IntSeq& buddies, const Ice::Current&);
        virtual void reload(::Ice::Int, const Ice::Current& = Ice::Current());
        virtual void add(::Ice::Int, ::Ice::Int, const Ice::Current&);
        virtual void statusUpdate(::Ice::Int, ::Ice::Int, const Ice::Current&);
        virtual int filter(::Ice::Int,const MyUtil::IntSeq& buddies, const Ice::Current&);
        virtual MyUtil::ByteSeq getStatusMinMax(::Ice::Int minId, ::Ice::Int maxId, const ::Ice::Current& current = Ice::Current());
      
        void initialize();

        class ReloadTimer : public MyUtil::Timer, virtual public MyUtil::Singleton<ReloadTimer> {
          public:
            virtual void handle();
            
            void swapReload(vector<int>& todo){
              IceUtil::Mutex::Lock lock(_reloadMutex);
              _reloadQueue.swap(todo);
            }

            void addReload(int id){
              IceUtil::Mutex::Lock lock(_reloadMutex);
              vector<int>::iterator it = lower_bound(_reloadQueue.begin(), _reloadQueue.end(), id);
              if(it != _reloadQueue.end() && *it == id){
                //find it, do nothing!
              } else {
                _reloadQueue.insert(it,id);
              }
            }
          private:
            ReloadTimer():
              Timer(RELOAD_INTERNAL){
              }
            friend class MyUtil::Singleton<ReloadTimer>; 
            vector<int> _reloadQueue;
            IceUtil::Mutex _reloadMutex;
        };  
      private:
        friend class MyUtil::Singleton<BuddyByIdLoaderI>;
    };
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

    class AllOrderByIdAscHandler : virtual public com::xiaonei::xce::ResultHandler{
      public:
        AllOrderByIdAscHandler(MyUtil::IntSeq& data,
            const std::string& keyField ) :
          _data(data), _keyField(keyField){
          }
      protected:
        virtual bool handle(mysqlpp::Row& res) const;
      private:
        MyUtil::IntSeq& _data;
        std::string _keyField;
    };

    class SetDataHelper: public MyUtil::Singleton<SetDataHelper> {
      public:
        void initialize();
        void invokeDelay( Ice::Int, const Ice::ObjectPtr& );
        void invokeNow( Ice::Int, const Ice::ObjectPtr& );
        void swap(std::vector< pair<long,Ice::ObjectPtr> >& empty_vector);
        void work(std::vector< pair<long,Ice::ObjectPtr> >& todo);
        void notify(std::vector< pair<long,Ice::ObjectPtr> >& todo);
      private:
        IceUtil::Monitor<IceUtil::Mutex> mutex_;
        std::vector< pair<long,Ice::ObjectPtr> > datas_;

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

    class SetDataHelperNonRealTime: public MyUtil::Singleton<SetDataHelperNonRealTime> {
      public:
        void initialize();
        void invokeDelay( Ice::Int, const Ice::ObjectPtr& );
        void invokeNow( Ice::Int, const Ice::ObjectPtr& );
        void swap(std::vector< pair<long,Ice::ObjectPtr> >& empty_vector);
      private:
        IceUtil::Monitor<IceUtil::Mutex> mutex_;
        std::vector< pair<long,Ice::ObjectPtr> > datas_;

        class SetDataThread: virtual public IceUtil::Thread {
          public:
            SetDataThread(SetDataHelperNonRealTime& helper) :
              helper_(helper) {
                expired_.tv_sec = 0;
                expired_.tv_nsec = 200000000;
              }
            virtual void run();
          private:
            SetDataHelperNonRealTime& helper_;
            struct timespec expired_;
        };
        typedef IceUtil::Handle<SetDataThread> SetDataThreadPtr;

        SetDataThreadPtr setter_;
    };

    class BuddyFilter : virtual public MyUtil::Singleton<BuddyFilter>{
      public:
        MyUtil::IntSeq filter(int userId,const MyUtil::IntSeq& buddies);
        MyUtil::IntSeq filter(int userId,std::map<int, int>& orderFriend);
      private:
        friend class MyUtil::Singleton<BuddyFilter>;
    };

    /*
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
     */

    class LoadTask : virtual public MyUtil::Task {
      public:
        LoadTask( Ice::Int id,const MyUtil::IntSeq& buddies,
            Ice::Int level=TASK_LEVEL_BUDDY_BYID_LOADER_LOAD ) :
          Task(level),_id(id),_buddies(buddies){
          }
      protected:
        virtual void handle();
      private:
        Ice::Int _id;
        MyUtil::IntSeq _buddies;
    };

    class ReloadTask : virtual public MyUtil::Task {
      public:
        ReloadTask( ::Ice::Int id, ::Ice::Int level=TASK_LEVEL_BUDDY_BYID_LOADER_RELOAD ) :
          Task(level),_id(id){
          }
      protected:
        virtual void handle();
      private:
        ::Ice::Int _id;
    };

    class AddTask : virtual public MyUtil::Task {
      public:
        AddTask( ::Ice::Int hostId, ::Ice::Int guestId, ::Ice::Int level=TASK_LEVEL_BUDDY_BYID_LOADER_ADD ) :
          Task(level),_hostId(hostId),_guestId(guestId){
          }
      protected:
        virtual void handle();
      private:
        ::Ice::Int _hostId;
        ::Ice::Int _guestId;
    };

    class StatusUpdateTask : virtual public MyUtil::Task {
      public:
        StatusUpdateTask( ::Ice::Int id, ::Ice::Int status, ::Ice::Int level=TASK_LEVEL_BUDDY_BYID_LOADER_STATUS_UPDATE ) :
          Task(level),_id(id),_status(status){
          }
      protected:
        virtual void handle();
      private:
        ::Ice::Int _id;
        ::Ice::Int _status;
    };

    //***************************************************************************

    class FacadeTask : virtual public MyUtil::Task {
      public:
        FacadeTask( const MyUtil::Int2IntSeqMap& lists,
            Ice::Int level=TASK_LEVEL_BUDDY_BYID_LOADER_FACADE ) :
          Task(level),_lists(lists){
          }
      protected:
        virtual void handle();
      private:
        MyUtil::Int2IntSeqMap _lists;
    };

    class FacadeNonRealTimeTask : virtual public MyUtil::Task {
      public:
        FacadeNonRealTimeTask( const MyUtil::Int2IntSeqMap& lists,
            Ice::Int level=TASK_LEVEL_BUDDY_BYID_LOADER_FACADENONREALTIME ) :
          Task(level),_lists(lists){
          }
      protected:
        virtual void handle();
      private:
        MyUtil::Int2IntSeqMap _lists;
    };
    //***************************************************************************

    class CountRecordFCCTask : virtual public MyUtil::Task {
      public:
        CountRecordFCCTask( const MyUtil::Int2IntMap& counts,
            Ice::Int level=TASK_LEVEL_BUDDY_BYID_LOADER_COUNT_RECORD_FCC ) :
          Task(level), _counts(counts){
          }
      protected:
        virtual void handle();
      private:
        MyUtil::Int2IntMap _counts;
    };

    //***************************************************************************

    class ServiceSchemaI: public MyUtil::Descriptor {
      public:
        virtual void configure(const Ice::PropertiesPtr& props);
    };


    class CheckAddBuddyTask : public MyUtil::Task {
      public:
        CheckAddBuddyTask(xce::check::OperaDataPtr& opera,const std::string& checksum, ::Ice::Int level=TASK_LEVEL_CHECK_ADD_BUDDY ):
          MyUtil::Task(level),
          _opera(opera),
          _checksum(checksum) {
          }
        virtual void handle();
      private:
        xce::check::OperaDataPtr _opera;
        std::string _checksum;
    };

    class CheckBuddyDataTask : public MyUtil::Task {
      public:
        CheckBuddyDataTask(xce::check::OperaDataPtr& opera,const std::string& checksum, ::Ice::Int level=TASK_LEVEL_CHECK_BUDDY_DATA ):
          MyUtil::Task(level),
          _opera(opera),
          _checksum(checksum) {
          }
        virtual void handle();
      private:
        xce::check::OperaDataPtr _opera;
        std::string _checksum;
    };

  }
}

#endif
