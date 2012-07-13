#ifndef __BUDDY_BYONLINETIME_LOADER_I_H__
#define __BUDDY_BYONLINETIME_LOADER_I_H__

#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
#include <ServiceI.h>
#include "ReplicaCluster/CacheLoader/src/CacheLoader.h"
#include "BuddyByOnlineTimeCache.h"

namespace xce {
namespace buddy {

const int TASK_LEVEL_BUDDY_BYONLINETIME_LOADER_LOAD = 1;
const int TASK_LEVEL_BUDDY_BYONLINETIME_LOADER_RELOAD = 2;
const int TASK_LEVEL_BUDDY_BYONLINETIME_LOADER_ONLINE = 3;
const int TASK_LEVEL_BUDDY_BYONLINETIME_LOADER_NOTIFYONLINE = 4;

const size_t GET_FRIENDS_LIMIT = 10000;
const size_t NOTIFY_ONLINE_LIMIT = 600000;

class BuddyByOnlineTimeLoaderFactory: virtual public xce::cacheloader::IdListFactory, virtual public MyUtil::Singleton<BuddyByOnlineTimeLoaderFactory>  {
protected:
        friend class MyUtil::Singleton<BuddyByOnlineTimeLoaderFactory>;
public:
        Ice::ObjectPtr create(Ice::Int id);
        virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
};

class BuddyByOnlineTimeLoaderI : virtual public BuddyByOnlineTimeLoader,
        virtual public MyUtil::Singleton<BuddyByOnlineTimeLoaderI> {
public:
	virtual void reload(::Ice::Int,const Ice::Current&);
        virtual void load(const MyUtil::Int2IntSeqMap&, const Ice::Current&);
        virtual void online(::Ice::Int,const Ice::Current& = Ice::Current() );
private:
        friend class MyUtil::Singleton<BuddyByOnlineTimeLoaderI>;
};

class OnlineEventHelper: public MyUtil::Singleton<OnlineEventHelper> {
public:
        void initialize();
        void invokeDelay( Ice::Int );
        void invokeNow( Ice::Int );
        void swap( MyUtil::IntSeq& empty_vector );
private:
        IceUtil::Monitor<IceUtil::Mutex> mutex_;
	MyUtil::IntSeq datas_;

        class OnlineEventThread: virtual public IceUtil::Thread {
        public:
                OnlineEventThread(OnlineEventHelper& helper) :
                        helper_(helper) {
                        }
                virtual void run();
        private:
                OnlineEventHelper& helper_;
        };
        typedef IceUtil::Handle<OnlineEventThread> OnlineEventThreadPtr;

        OnlineEventThreadPtr invoker_;
};

class OnlineNotifyHelper: public MyUtil::Singleton<OnlineNotifyHelper> {
public:
        void initialize();
	void invokeDelay(int userId,int onlineId);
	void invokeNow(int userId,int onlineId);
        void swap( OnlineNoteSeq& empty_vector );
private:
        IceUtil::Monitor<IceUtil::Mutex> mutex_;
	OnlineNoteSeq datas_;

        class OnlineNotifyThread: virtual public IceUtil::Thread {
        public:
                OnlineNotifyThread(OnlineNotifyHelper& helper) :
                        helper_(helper) {
                        }
                virtual void run();
        private:
                OnlineNotifyHelper& helper_;
        };
        typedef IceUtil::Handle<OnlineNotifyThread> OnlineNotifyThreadPtr;

        OnlineNotifyThreadPtr invoker_;
};

class OnlineTimeHelper : virtual public MyUtil::Singleton<OnlineTimeHelper>{
public:
        MyUtil::IntSeq sort(int userId,const MyUtil::IntSeq& buddies);
private:
        friend class MyUtil::Singleton<OnlineTimeHelper>;
};
class OnlineTimeHandler : virtual public com::xiaonei::xce::ResultHandler{
public:
        OnlineTimeHandler(std::map<int, int>& times ) :
                _times(times) {
        }
protected:
        virtual bool handle(mysqlpp::Row& res) const;
private:
        std::map<int, int>& _times;
};

class SetDataHelper: public MyUtil::Singleton<SetDataHelper> {
public:
        void initialize();
        void invokeDelay( Ice::Int, const Ice::ObjectPtr& );
        void invokeNow();
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

class ReloadTask : virtual public MyUtil::Task {
public:
	ReloadTask( Ice::Int id,
		Ice::Int level=TASK_LEVEL_BUDDY_BYONLINETIME_LOADER_RELOAD ) :
			Task(level),_id(id){
	}
protected:
        virtual void handle();
private:
	Ice::Int _id;
};

class LoadTask : virtual public MyUtil::Task {
public:
        LoadTask( const MyUtil::Int2IntSeqMap& lists,
                Ice::Int level=TASK_LEVEL_BUDDY_BYONLINETIME_LOADER_LOAD ) :
                        Task(level),_lists(lists){
        }
protected:
        virtual void handle();
private:
        MyUtil::Int2IntSeqMap _lists;
};

class OnlineTask: virtual public MyUtil::Task {
public:
	OnlineTask( const MyUtil::IntSeq& ids,
		Ice::Int level=TASK_LEVEL_BUDDY_BYONLINETIME_LOADER_ONLINE ) :
			Task(level),_ids(ids){
	}
protected:
        virtual void handle();
private:
        MyUtil::IntSeq _ids;
};

class NotifyOnlineTask: virtual public MyUtil::Task {
public:
	NotifyOnlineTask( const OnlineNoteSeq& notes,
		Ice::Int level=TASK_LEVEL_BUDDY_BYONLINETIME_LOADER_NOTIFYONLINE ) :
			Task(level),_notes(notes){
	}
protected:
        virtual void handle();
private:
	OnlineNoteSeq _notes;
};

//***************************************************************************

class ServiceSchemaI: public MyUtil::Descriptor {
public:
        virtual void configure(const Ice::PropertiesPtr& props);
};

}
}

#endif
