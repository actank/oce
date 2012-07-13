#ifndef __FriendFinderByMSNI_h__
#define __FriendFinderByMSNI_h__

#include "FriendFinderByMSN.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>

namespace xce {
namespace friendfinder {

const int FRIENDFINDER_BYMSN_DATA = 0;

const int TASK_LEVEL_ADD = 1;
const int TASK_LEVEL_REMOVE = 2;
const int TASK_LEVEL_AUTO_APPLY = 3;

////***************************************************************************

class FriendFinderByMSNData: virtual public Ice::Object {
public:
	MSNdata getMSNdata();

	MSNmap getPUAmap();
	MyUtil::IntSeq getPUAids();
	MyUtil::IntSeq getPUAidsWithMask(const MyUtil::IntSeq&);

	MSNmap getPUKmap();
	MyUtil::IntSeq getPUKids();
	MyUtil::IntSeq getPUKidsRandom(int limit);
	MyUtil::IntSeq getPUKidsWithMask(const MyUtil::IntSeq&);

	void add(int hostId,int guestId,int type);

	void removePUA(int hostId,int guestId);
	void removePUK(int hostId,int guestId);

	void set(int guestId,int type);
private:
	IceUtil::Mutex mutex_;
	MSNdata msndata_;
};
typedef IceUtil::Handle<FriendFinderByMSNData> FriendFinderByMSNDataPtr;

////***************************************************************************

class FriendFinderByMSNDataFactory: virtual public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(::Ice::Int id);
};

class FriendFinderByMSNDataResultHandler : virtual public com::xiaonei::xce::ResultHandler
{
public:
	FriendFinderByMSNDataResultHandler(::Ice::Int hostId, const FriendFinderByMSNDataPtr& result):
		_hostId(hostId),
		_result(result) {}
protected:
	virtual bool handle(mysqlpp::Row& row) const;
private:
	::Ice::Int _hostId;
	FriendFinderByMSNDataPtr _result;
};

class FriendFinderByMSNApplyResultHandler : virtual public com::xiaonei::xce::ResultHandler
{
public:
	FriendFinderByMSNApplyResultHandler(MyUtil::IntSeq& apply):_apply(apply) {}
protected:
	virtual bool handle(mysqlpp::Row& row) const;
private:
	MyUtil::IntSeq& _apply;
};

class FriendResultHandler : virtual public com::xiaonei::xce::ResultHandler
{
public:
	FriendResultHandler(Ice::Int id, const FriendFinderByMSNDataPtr& obj, MyUtil::IntSeq& apply):
		_id(id), _obj(obj), _apply(apply){
	}
protected:
	virtual bool handle(mysqlpp::Row& row) const;
private:
	Ice::Int _id;
	FriendFinderByMSNDataPtr _obj;
	MyUtil::IntSeq& _apply;
};

////***************************************************************************

class FriendFinderByMSNManagerI: virtual public FriendFinderByMSNManager,
		virtual public MyUtil::Singleton<FriendFinderByMSNManagerI>{
public:

	virtual MSNdata getMSNdata(::Ice::Int,
			const Ice::Current&);

	virtual MSNmap getPUAmap(::Ice::Int,
			const Ice::Current&);
	virtual MyUtil::IntSeq getPUAids(::Ice::Int,
			const Ice::Current&);
	virtual MyUtil::IntSeq getPUAidsWithMask(::Ice::Int,const MyUtil::IntSeq&,
			const Ice::Current&);

	virtual MSNmap getPUKmap(::Ice::Int,
			const Ice::Current&);
	virtual MyUtil::IntSeq getPUKids(::Ice::Int,
			const Ice::Current&);
	virtual MyUtil::IntSeq getPUKidsRandom(::Ice::Int,::Ice::Int,
			const Ice::Current&);
	virtual MyUtil::IntSeq getPUKidsWithMask(::Ice::Int,const MyUtil::IntSeq&,
			const Ice::Current&);


	virtual void add(::Ice::Int,::Ice::Int,::Ice::Int,
			const Ice::Current&);

	virtual void remove(::Ice::Int,::Ice::Int,
			const Ice::Current&);
	virtual void removePUA(::Ice::Int,::Ice::Int,
			const Ice::Current&);
	virtual void removePUK(::Ice::Int,::Ice::Int,
			const Ice::Current&);

	virtual void reload(::Ice::Int,
			const Ice::Current&);

private:
	FriendFinderByMSNDataPtr locateFriendFinderByMSNData(::Ice::Int);
	FriendFinderByMSNDataPtr findFriendFinderByMSNData(::Ice::Int);
};

////***************************************************************************

class AddToDBTask: virtual public MyUtil::Task {
public:
        AddToDBTask(::Ice::Int hostId, ::Ice::Int guestId, ::Ice::Int type, ::Ice::Int level=TASK_LEVEL_ADD):
                MyUtil::Task(level), _hostId(hostId), _guestId(guestId), _type(type) {
        }
        virtual void handle();
private:
	Ice::Int _hostId;
	Ice::Int _guestId;
	Ice::Int _type;
};

class RemoveFromDBTask: virtual public MyUtil::Task {
public:
        RemoveFromDBTask(::Ice::Int hostId, ::Ice::Int guestId, ::Ice::Int level=TASK_LEVEL_REMOVE):
                MyUtil::Task(level), _hostId(hostId), _guestId(guestId) {
        }
        virtual void handle();
private:
	Ice::Int _hostId;
	Ice::Int _guestId;
};

class AutoApplyTask: virtual public MyUtil::Task {
public:
        AutoApplyTask(::Ice::Int hostId, const FriendFinderByMSNDataPtr& obj, const MyUtil::IntSeq& apply, ::Ice::Int level=TASK_LEVEL_AUTO_APPLY):
                MyUtil::Task(level), _hostId(hostId), _obj(obj), _apply(apply) {
        }
        virtual void handle();
private:
	Ice::Int _hostId;
	FriendFinderByMSNDataPtr _obj;
	MyUtil::IntSeq _apply;
};

////***************************************************************************

class ServiceSchemaI: public MyUtil::Descriptor {
public:
        virtual void configure(const Ice::PropertiesPtr& props);
};

}

}

#endif
