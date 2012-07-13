#ifndef __DIST_BUDDYBYID_CACHE_RELOADERI_H__
#define __DIST_BUDDYBYID_CACHE_RELOADERI_H__

#include <ServiceI.h>
#include <UserCache.h>
#include <UserBaseAdapter.h>
#include <CallGuarantee/src/CallGuarantee.h>
#include "BuddyByIdCache.h"
#include "DistBuddyByIdCacheAdapter.h"
#include "../util/StatusCache.h"

namespace xce {
  namespace buddy {

    const std::string DIST_BUDDYBYID_CACHE_RELOADER = "M";
    const int TASK_LEVEL_DIST_BUDDY_BYID_CACHE_RELOADER_RELOAD = 1;
    const int TASK_LEVEL_DIST_BUDDY_BYID_CACHE_RELOADER_STATUS_UPDATE = 2;
    const int TASK_LEVEL_DIST_BUDDY_BYID_CACHE_RELOADER_ADD = 3;
    const int TASK_LEVEL_DIST_BUDDY_BYID_CACHE_RELOADER_REMOVE = 4;

    const int DIST_BUDDY_BYID_CACHE_RELOADER_CHUNCK_SIZE = 100;

    class DistBuddyByIdStatusCache : virtual public StatusCache {
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
            DistBuddyByIdStatusCache::instance().initializeFromSvc();
          } else{
            DistBuddyByIdStatusCache::instance().initializeFromDB();
          }
        }
    };
    typedef IceUtil::Handle<StatusCacheInitThread> StatusCacheInitThreadPtr;

    class DistBuddyByIdCacheReloaderI : 
      virtual public xce::buddy::DistBuddyByIdCacheReloader, 
      virtual public MyUtil::Singleton<DistBuddyByIdCacheReloaderI>
    {
      public:
        virtual bool isValid(const Ice::Current& = Ice::Current());
        virtual void setValid(bool valid, const Ice::Current& = Ice::Current());

        virtual void reload(const MyUtil::IntSeq& ids, const Ice::Current& = Ice::Current());
        virtual void statusUpdate(::Ice::Int, ::Ice::Int, const Ice::Current&);
        virtual void add(::Ice::Int, ::Ice::Int, const Ice::Current&);
        virtual void remove(::Ice::Int, ::Ice::Int, const Ice::Current&);
      private:
        DistBuddyByIdCacheReloaderI(){
        }
        friend class MyUtil::Singleton<DistBuddyByIdCacheReloaderI>;

        bool valid_;
        IceUtil::RWRecMutex validmutex_;
    };

    class ReloadTask : public MyUtil::Task {
      public:
        ReloadTask(MyUtil::IntSeq& ids, const Ice::Current& current, const int level=TASK_LEVEL_DIST_BUDDY_BYID_CACHE_RELOADER_RELOAD):
          MyUtil::Task(level),ids_(ids),current_(current) {
        }
        virtual void handle();
      private:
        MyUtil::IntSeq ids_;
        Ice::Current current_;
    };
   
    class AddTask : public MyUtil::Task {
      public:
        AddTask(::Ice::Int host, ::Ice::Int guest, const Ice::Current& current,const int level=TASK_LEVEL_DIST_BUDDY_BYID_CACHE_RELOADER_ADD):
          MyUtil::Task(level),host_(host),guest_(guest),current_(current) {
        }
        virtual void handle();
      private:
        ::Ice::Int host_;
        ::Ice::Int guest_;
        Ice::Current current_;
    };

    class RemoveTask : public MyUtil::Task {
      public:
        RemoveTask(::Ice::Int host, ::Ice::Int guest, const Ice::Current& current,const int level=TASK_LEVEL_DIST_BUDDY_BYID_CACHE_RELOADER_REMOVE):
          MyUtil::Task(level),host_(host),guest_(guest),current_(current) {
        }
        virtual void handle();
      private:
        ::Ice::Int host_;
        ::Ice::Int guest_;
        Ice::Current current_;
    };
 
    class BuddyFilter : virtual public MyUtil::Singleton<BuddyFilter>{
        public:
          MyUtil::IntSeq filter(int userId,const MyUtil::IntSeq& buddies){
            std::map<int, int> orderFriend;
            for (MyUtil::IntSeq::const_iterator it = buddies.begin(); it
                != buddies.end(); ++it ){
              orderFriend.insert( make_pair(*it,*it) );
            }
            return filter(userId,orderFriend);
          }
          MyUtil::IntSeq filter(int userId,std::map<int, int>& orderFriend){
            return StatusCache::instance().filter( userId, orderFriend );
          }
        private:
          friend class MyUtil::Singleton<BuddyFilter>;
      };

      class DistBuddyByIdFactory : 
        virtual public xce::distcache::buddybyidcache::BuddyByIdFactory,
        virtual public MyUtil::Singleton<DistBuddyByIdFactory> {
        public:
          virtual MyUtil::IntSeq filter(int id,const MyUtil::IntSeq& buddies){
            return BuddyFilter::instance().filter(id,buddies);
          }
          
          Ice::ObjectPtr createAllFriends(::Ice::Int id); 
        };

      class StatusUpdateTask : virtual public MyUtil::Task {
        public:
          StatusUpdateTask( ::Ice::Int id, ::Ice::Int status,const Ice::Current& current, const ::Ice::Int level=TASK_LEVEL_DIST_BUDDY_BYID_CACHE_RELOADER_STATUS_UPDATE) :
            Task(level),_id(id),_status(status),current_(current){
            }
        protected:
          virtual void handle();
        private:
          ::Ice::Int _id;
          ::Ice::Int _status;
          Ice::Current current_;
      };
  }
}
#endif
