#ifndef __FRIENDFINDER_BYCOMMON_I_H__
#define __FRIENDFINDER_BYCOMMON_I_H__

#include <FriendFinderByCommon.h>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>

namespace xce {
namespace friendfinder {

const int FRIENDFINDER_BYCOMMON_DATA = 0;

const int FRIENDFINDER_BYCOMMON_DATA_LIMIT = 300;
const int FRIENDFINDER_BYCOMMON_DATA_SHARE_LIMIT = 0;

const int TASK_LEVEL_FILL = 1;
const int TASK_LEVEL_UPDATE = 2;

////***************************************************************************

class FriendFinderByCommonData: virtual public Ice::Object {
public:
	void set( ::Ice::Int id,com::xiaonei::service::CommonFriendSeq& cfs );
	CommonFriendItemSeq getItemsByShared(int limit);
	CommonFriendItemSeq getItemsById(int limit);
	MyUtil::IntSeq getIdsByShared(int limit);
	MyUtil::IntSeq getIdsAsc(int limit);
	MyUtil::IntSeq getIdsWithIn(::MyUtil::IntSeq& friends_asc,int limit);

	int size();
	bool remove(::Ice::Int friendId);
	std::string serialize();

	CommonFriendItemSeq _data;
private:
	IceUtil::RWRecMutex _mutex;
};
typedef IceUtil::Handle<FriendFinderByCommonData> FriendFinderByCommonDataPtr;

class FriendFinderByCommonDataFactory: virtual public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(::Ice::Int id);
};

class FriendFinderByCommonDataResultHandler : virtual public com::xiaonei::xce::ResultHandler
{
	public:
		FriendFinderByCommonDataResultHandler(const FriendFinderByCommonDataPtr& result):_result(result) {}
	protected:
		virtual bool handle(mysqlpp::Row& row) const;
	private:
		FriendFinderByCommonDataPtr _result;
};

class FriendFinderByCommonManagerI: virtual public FriendFinderByCommonManager,
                virtual public MyUtil::Singleton<FriendFinderByCommonManagerI>{
public:
	virtual void setCommonFriendSeq(::Ice::Int userId,const com::xiaonei::service::CommonFriendSeq& cfs,
			const ::Ice::Current&);
	virtual CommonFriendItemSeq getCommonFriendItemSeq(::Ice::Int userId, ::Ice::Int limit,
			const ::Ice::Current&);
	virtual CommonFriendItemSeq getCommonFriendItemSeqById(::Ice::Int userId, ::Ice::Int limit,
			const ::Ice::Current&);
	virtual ::Ice::Int getCount(::Ice::Int userId,
			const ::Ice::Current&);
	virtual void removeCommonFriendItem(::Ice::Int userId, ::Ice::Int friendId,
			const ::Ice::Current&);
	virtual void reload(::Ice::Int userId,
			const ::Ice::Current&);

	virtual MyUtil::IntSeq getFriendsByCommon(::Ice::Int guestId,::Ice::Int hostId,::Ice::Int limit,
			const ::Ice::Current&);
private:
	FriendFinderByCommonDataPtr locateFriendFinderByCommonData(::Ice::Int);
};

class FillTask: virtual public MyUtil::Task {
public:
        FillTask(::Ice::Int hostId,const com::xiaonei::service::CommonFriendSeq& cfs,::Ice::Int level=TASK_LEVEL_FILL ):
                MyUtil::Task(level), _hostId(hostId), _cfs(cfs) {
        }
        virtual void handle();
private:
	::Ice::Int _hostId;
        com::xiaonei::service::CommonFriendSeq _cfs;
};

class UpdateToDBTask : virtual public MyUtil::Task {
public:
	UpdateToDBTask( ::Ice::Int hostId, const FriendFinderByCommonDataPtr& obj, ::Ice::Int level=TASK_LEVEL_UPDATE ) :
		Task(level),_hostId(hostId),_obj(obj) {
	}
protected:
	virtual void handle();
private:
	::Ice::Int _hostId;
	FriendFinderByCommonDataPtr _obj;
};

class ServiceSchemaI: public MyUtil::Descriptor {
public:
        virtual void configure(const Ice::PropertiesPtr& props);
};

}

}

#endif
