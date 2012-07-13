#ifndef __FRIEND_COUNT_CACHE_I_H__
#define __FRIEND_COUNT_CACHE_I_H__

#include "FriendCountCache.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
#include <string>
#include "UtilCxx/src/Singleton.h"
#include "IceExt/src/ObjectCacheI.h"
#include "IceExt/src/ServiceI.h"

namespace xce {
namespace buddy {

const int FRIEND_COUNT_CACHE_DATA = 0;

const int TASK_LEVEL_FRIEND_COUNT_CACHE_SET = 1;
const int TASK_LEVEL_FRIEND_COUNT_CACHE_SET_BATCH = 2;
const int TASK_LEVEL_FRIEND_COUNT_CACHE_RECORD = 3;
const int TASK_LEVEL_FRIEND_COUNT_CACHE_RECORD_BATCH = 4;
const int TASK_LEVEL_FRIEND_COUNT_CACHE_RECORD_DEC = 5;
const int TASK_LEVEL_FRIEND_COUNT_CACHE_LOAD = 6;

/*****************************************************************************/

class FriendCountCacheManagerI: virtual public FriendCountCacheManager, virtual public MyUtil::Singleton<FriendCountCacheManagerI> {
public:
	virtual ::Ice::Int getFriendCount(::Ice::Int, const Ice::Current&);
	virtual ::MyUtil::Int2IntMap getFriendCounts(const MyUtil::IntSeq&, const Ice::Current&);
	virtual void setFriendCount( ::Ice::Int, ::Ice::Int, const Ice::Current&);
	virtual void setFriendCounts( const ::MyUtil::Int2IntMap&, const Ice::Current&);
	virtual void dec( ::Ice::Int, const Ice::Current&);
	virtual void reload( ::Ice::Int, const Ice::Current&);
private:
	friend class MyUtil::Singleton<FriendCountCacheManagerI>;
	FriendCountCacheManagerI(){
	}
	FriendCountCacheDataPtr locateFriendCountCacheData(Ice::Int);
	FriendCountCacheDataPtr findFriendCountCacheData(Ice::Int);
};

/*****************************************************************************/

class FriendCountCacheFactory : virtual public MyUtil::ServantFactory {
public:
        virtual Ice::ObjectPtr create(Ice::Int id);
};

/*****************************************************************************/

class LoadFriendCountHelper: public MyUtil::Singleton<LoadFriendCountHelper> {
public:
	void initialize();
	void invokeDelay( Ice::Int );
	void invokeNow( Ice::Int );
	void swap(MyUtil::IntSeq& empty_vector);
private:
	IceUtil::Monitor<IceUtil::Mutex> mutex_;
        MyUtil::IntSeq datas_;

	class LoadFriendCountThread: virtual public IceUtil::Thread {
	public:
		LoadFriendCountThread(LoadFriendCountHelper& helper) :
			helper_(helper) {
		}
		virtual void run();
	private:
		LoadFriendCountHelper& helper_;
	};
	typedef IceUtil::Handle<LoadFriendCountThread> LoadFriendCountThreadPtr;

	LoadFriendCountThreadPtr loader_;
};

/*****************************************************************************/

class SetTask: virtual public MyUtil::Task {
public:
        SetTask(::Ice::Int id, ::Ice::Int count, Ice::Int level=TASK_LEVEL_FRIEND_COUNT_CACHE_SET ):
                MyUtil::Task(level), _id(id), _count(count) {
        }
        virtual void handle();
private:
        ::Ice::Int _id;
        ::Ice::Int _count;
};

class SetBatchTask: virtual public MyUtil::Task {
public:
        SetBatchTask(const ::MyUtil::Int2IntMap& counts, Ice::Int level=TASK_LEVEL_FRIEND_COUNT_CACHE_SET_BATCH ):
                MyUtil::Task(level), _counts(counts) {
        }
        virtual void handle();
private:
        ::MyUtil::Int2IntMap _counts;
};

class CountRecordTask: virtual public MyUtil::Task {
public:
        CountRecordTask( Ice::Int id, Ice::Int count,
                Ice::Int level=TASK_LEVEL_FRIEND_COUNT_CACHE_RECORD ) :
                        Task(level), _id(id), _count(count){
        }
protected:
        virtual void handle();
private:
        Ice::Int _id;
        Ice::Int _count;
};

class CountRecordBatchTask: virtual public MyUtil::Task {
public:
        CountRecordBatchTask( const MyUtil::Int2IntMap& counts,
                Ice::Int level=TASK_LEVEL_FRIEND_COUNT_CACHE_RECORD_BATCH ) :
                        Task(level), _counts(counts){
        }
protected:
        virtual void handle();
private:
        MyUtil::Int2IntMap _counts;
};

class CountDecTask: virtual public MyUtil::Task {
public:
        CountDecTask( Ice::Int id,
                Ice::Int level=TASK_LEVEL_FRIEND_COUNT_CACHE_RECORD_DEC ) :
                        Task(level), _id(id){
        }
protected:
        virtual void handle();
private:
        Ice::Int _id;
};

class LoadTask: virtual public MyUtil::Task {
public:
        LoadTask(const MyUtil::IntSeq& ids, Ice::Int level=TASK_LEVEL_FRIEND_COUNT_CACHE_LOAD ):
                MyUtil::Task(level), _ids(ids) {
        }
        virtual void handle();
private:
        MyUtil::IntSeq _ids;
};

//***************************************************************************

class ServiceSchemaI: public MyUtil::Descriptor {
public:
	virtual void configure(const Ice::PropertiesPtr& props);
};

}
}

#endif
