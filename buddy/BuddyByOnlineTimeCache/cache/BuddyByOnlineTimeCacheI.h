#ifndef __BUDDY_BYONLINETIME_CACHE_I_H__
#define __BUDDY_BYONLINETIME_CACHE_I_H__

#include "BuddyByOnlineTimeCache.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <string>
#include "UtilCxx/src/Singleton.h"
#include "IceExt/src/ObjectCacheI.h"
#include "IceExt/src/ServiceI.h"

namespace xce {
namespace buddy {

const int BUDDY_BYONLINETIME_CACHE_DATA_I = 0;

const int TASK_LEVEL_BUDDY_BYONLINETIME_CACHE_FILL = 1;
const int TASK_LEVEL_BUDDY_BYONLINETIME_CACHE_LOAD = 2;
const int TASK_LEVEL_BUDDY_BYONLINETIME_CACHE_RELOAD = 3;
const int TASK_LEVEL_BUDDY_BYONLINETIME_CACHE_REMOVE = 4;
const int TASK_LEVEL_BUDDY_BYONLINETIME_CACHE_NOTIFYONLINE = 5;
const int TASK_LEVEL_BUDDY_BYONLINETIME_CACHE_NOTIFYONLINEBATCH = 6;

/*****************************************************************************/

class BuddyByOnlineTimeDataI: virtual public Ice::Object {
public:
	BuddyByOnlineTimeDataI();
	BuddyByOnlineTimeDataI( xce::buddy::BuddyByOnlineTimeDataPtr );
	Ice::Int get_count();
	MyUtil::IntSeq get_most_recent(int begin, int limit);
	void notify_somebody_online(int the_online_one);
	void remove(int userid);

	IceUtil::Mutex mutex;
	MyUtil::IntSeq friends;
};
typedef IceUtil::Handle<BuddyByOnlineTimeDataI> BuddyByOnlineTimeDataIPtr;

class BuddyByOnlineTimeCacheManagerI: virtual public BuddyByOnlineTimeCacheManager, virtual public MyUtil::Singleton<BuddyByOnlineTimeCacheManagerI> {
public:
	void bootstrap(const string& endpoint, int mod);
	virtual ::Ice::Int getFriendCount(::Ice::Int, const Ice::Current&);
	virtual ::MyUtil::Int2IntMap getFriendCounts(const MyUtil::IntSeq&, const Ice::Current&);
	virtual ::MyUtil::IntSeq getFriendList(::Ice::Int, ::Ice::Int, const Ice::Current&);
	virtual ::MyUtil::IntSeq getFriendListN(::Ice::Int, ::Ice::Int, ::Ice::Int, const Ice::Current&);
	virtual ::MyUtil::Int2IntSeqMap getFriendLists(const MyUtil::IntSeq&, const Ice::Current&);
        virtual void load( ::Ice::Int, const ::Ice::ObjectPtr&, const Ice::Current&);
        virtual void remove( ::Ice::Int, ::Ice::Int, const Ice::Current&);
	virtual void notifySomebodyOnline( ::Ice::Int, ::Ice::Int, const Ice::Current&);
	virtual void notifyOnlineBatch(const OnlineNoteSeq&, const Ice::Current&);

	virtual bool isValid(const Ice::Current& = Ice::Current());
	virtual void setValid(bool valid,const Ice::Current& = Ice::Current());
        virtual void setData(const ::MyUtil::ObjectResultPtr& BuddyData, const Ice::Current&);

private:
	string _endpoint;
	int _mod;
	bool _isValid;
	IceUtil::RWRecMutex _validMutex;

	friend class MyUtil::Singleton<BuddyByOnlineTimeCacheManagerI>;
	BuddyByOnlineTimeCacheManagerI(){
	}
	BuddyByOnlineTimeDataIPtr getBuddyByOnlineTimeDataI(Ice::Int userId);
	BuddyByOnlineTimeDataIPtr locateBuddyByOnlineTimeDataI(Ice::Int userId);
};

/*****************************************************************************/

class BuddyByOnlineTimeCacheFactory: virtual public MyUtil::ServantFactory {
public:
        virtual Ice::ObjectPtr create(Ice::Int id);
};

/*****************************************************************************/

class FillTask: virtual public MyUtil::Task {
public:
        FillTask(const MyUtil::ObjectResultPtr& buddyData,::Ice::Int level=TASK_LEVEL_BUDDY_BYONLINETIME_CACHE_FILL):
                MyUtil::Task(level), _buddyData(buddyData) {
        }
        virtual void handle();
private:
        MyUtil::ObjectResultPtr _buddyData;
};

class LoadTask: virtual public MyUtil::Task {
public:
        LoadTask(Ice::Int id, const Ice::ObjectPtr& obj, Ice::Int level=TASK_LEVEL_BUDDY_BYONLINETIME_CACHE_LOAD):
		MyUtil::Task(level), _id(id), _obj(obj)  {
        }
        virtual void handle();
private:
        Ice::Int _id;
        Ice::ObjectPtr _obj;

};

class ReloadTask: virtual public MyUtil::Task {
public:
        ReloadTask(Ice::Int id, Ice::Int level=TASK_LEVEL_BUDDY_BYONLINETIME_CACHE_RELOAD):
		MyUtil::Task(level), _id(id) {
        }
        virtual void handle();
private:
        Ice::Int _id;

};

class RemoveTask : virtual public MyUtil::Task {
public:
        RemoveTask( ::Ice::Int hostId, ::Ice::Int guestId, ::Ice::Int level=TASK_LEVEL_BUDDY_BYONLINETIME_CACHE_REMOVE ) :
                Task(level),_hostId(hostId),_guestId(guestId){
        }
protected:
        virtual void handle();
private:
        ::Ice::Int _hostId;
        ::Ice::Int _guestId;
};

class NotifyOnlineTask : virtual public MyUtil::Task {
public:
        NotifyOnlineTask( ::Ice::Int hostId, ::Ice::Int onlineId, ::Ice::Int level=TASK_LEVEL_BUDDY_BYONLINETIME_CACHE_NOTIFYONLINE ) :
                Task(level),_hostId(hostId),_onlineId(onlineId){
        }
protected:
        virtual void handle();
private:
        ::Ice::Int _hostId;
        ::Ice::Int _onlineId;
};

class NotifyOnlineBatchTask : virtual public MyUtil::Task {
public:
        NotifyOnlineBatchTask( const OnlineNoteSeq& notes, ::Ice::Int level=TASK_LEVEL_BUDDY_BYONLINETIME_CACHE_NOTIFYONLINEBATCH ) :
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

