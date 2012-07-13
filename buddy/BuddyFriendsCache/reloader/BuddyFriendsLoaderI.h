#ifndef __BUDDY_FRIENDS_LOADER_I_H__
#define __BUDDY_FRIENDS_LOADER_I_H__

#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
#include <ServiceI.h>
#include "ReplicaCluster/CacheLoader/src/CacheLoader.h"
#include "BuddyFriendsCache.h"
#include "buddy/BuddyFriendsCache/share/BuddyFriendsShare.h"

namespace xce {
  namespace buddy {

    const int TASK_LEVEL_BUDDY_FRIENDS_LOADER_LOAD = 1;
    const int TASK_LEVEL_BUDDY_FRIENDS_LOADER_RELOAD = 2;
    const int TASK_LEVEL_BUDDY_FRIENDS_LOADER_ADD = 3;

    class BuddyFriendsLoaderI : virtual public BuddyFriendsLoader,
    virtual public MyUtil::Singleton<BuddyFriendsLoaderI> {
      public:
        virtual void load(::Ice::Int,const MyUtil::IntSeq& buddies, const Ice::Current&);
        virtual void reload(::Ice::Int, const Ice::Current&);
        virtual void add(::Ice::Int, ::Ice::Int, const Ice::Current&);
      private:
        friend class MyUtil::Singleton<BuddyFriendsLoaderI>;
    };

    class SetDataHelper: public MyUtil::Singleton<SetDataHelper> {
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

    class LoadTask : virtual public MyUtil::Task {
      public:
        LoadTask( Ice::Int id,const MyUtil::IntSeq& buddies,
            Ice::Int level=TASK_LEVEL_BUDDY_FRIENDS_LOADER_LOAD ) :
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
        ReloadTask( ::Ice::Int id, ::Ice::Int level=TASK_LEVEL_BUDDY_FRIENDS_LOADER_RELOAD ) :
          Task(level),_id(id){
          }
      protected:
        virtual void handle();
      private:
        ::Ice::Int _id;
    };

    class AddTask : virtual public MyUtil::Task {
      public:
        AddTask( ::Ice::Int hostId, ::Ice::Int guestId, ::Ice::Int level=TASK_LEVEL_BUDDY_FRIENDS_LOADER_ADD ) :
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
