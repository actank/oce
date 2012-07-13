#ifndef __DIST_USER_CACHE_RELOADERI_H__
#define __DIST_USER_CACHE_RELOADERI_H__

#include <ServiceI.h>
#include <UserCache.h>
#include <UserBaseAdapter.h>
#include <QueryRunner.h>
#include <CallGuarantee/src/CallGuarantee.h>
#include "DistUserCacheAdapter.h"

namespace xce {
namespace distcache {
namespace usercache {

using ::xce::distcache::usercacheadapter::UserCacheMemDataPtr;
using ::xce::distcache::usercacheadapter::UserCacheMemData;
const int BATCH_PRODUCE_SIZE = 3000;
const std::string DIST_USER_CACHE_RELOADER = "M";

const int TASK_LEVEL_RELOAD = 1;
const int TASK_LEVEL_UPDATE = 2;
const int TASK_LEVEL_INSERT = 3;

const int TASK_LEVEL_UPDATEONLINEUSERSTATE = 4;
const int TASK_LEVEL_MAINTAINONLINEUSERSTATE = 5;

const int FILTER_CACHE = 0;
using namespace xce::usercache;

const int WEBPAGER_ONLINE = 2;
const int TALK_ONLINE = 4;

class UpdateOnlineTask : virtual public MyUtil::Task {
public:
	UpdateOnlineTask(OnlineTypeSeq onlineSeq);
	virtual void handle();
private:
	OnlineTypeSeq onlineSeq_;
};

class UpdateOnlineThread : virtual public IceUtil::Thread {
public:
	void add(OnlineTypeSeq data);
	void add(OnlineTypePtr data);
protected:
	virtual void run();
private:
	OnlineTypeSeq onlineSeq_;
	IceUtil::Monitor<IceUtil::Mutex> mutex_;
};

typedef IceUtil::Handle<UpdateOnlineThread>  UpdateOnlineThreadPtr;

class MaintainOnlineThread : virtual public IceUtil::Thread {
public:
	void add(int userId, int online);
	void add(const MyUtil::Int2IntMap& onlineMap);
	void fresh(const MyUtil::Int2IntMap& onlineMap);
	void getOnlineInfo(const MyUtil::IntSeq& userIds, MyUtil::Int2IntMap& onelineMap, 
							std::map<long, xce::distcache::usercacheadapter::UserCacheMemDataPtr>& onlineUserData, 
							std::map<long, xce::distcache::usercacheadapter::UserCacheMemDataPtr>& offlineUserData);
protected:
	virtual void run();
	void init();
	void add(const std::vector<int>& userIds);
private:
	MyUtil::Int2IntMap onlineMap_;
	IceUtil::Monitor<IceUtil::Mutex> mutex_;
};

typedef IceUtil::Handle<MaintainOnlineThread>  MaintainOnlineThreadPtr;

class DistUserCacheReloaderI : public xce::usercache::DistUserCacheReloader, virtual public MyUtil::Singleton<DistUserCacheReloaderI>
{
public:
	DistUserCacheReloaderI();
	void startTask();
	virtual bool isValid(const Ice::Current& = Ice::Current());
	virtual void setValid(bool valid, const Ice::Current& = Ice::Current());
	void reload(const MyUtil::IntSeq& ids, const Ice::Current&);
	void insert(int id, const Ice::Current&);
	void update(const xce::usercache::UserCacheUpdateOldDataPtr& data, const Ice::Current&);
	MyUtil::ObjectResultPtr getObjectResult(const MyUtil::IntSeq& ids);
	void setOnline(int id, int online , const Ice::Current&);
	void setOnlineBatch(const OnlineTypeSeq& data , const Ice::Current&);
	void getUserPassport(const MyUtil::IntSeq& ids, const MyUtil::ObjectResultPtr&);
private:
	bool valid_;
	IceUtil::RWRecMutex validmutex_;
	MaintainOnlineThreadPtr maintainOnlineThread_;
	UpdateOnlineThreadPtr updateOnlineThread_;

	void getUserState(const MyUtil::IntSeq& ids, const MyUtil::ObjectResultPtr&);
	void getUserStage(const MyUtil::IntSeq& ids, const MyUtil::ObjectResultPtr&);
	void getUserBorn(const MyUtil::IntSeq& ids, const MyUtil::ObjectResultPtr&);
	void getUserNames(const MyUtil::IntSeq& ids, const MyUtil::ObjectResultPtr&);
	void getUserUrl(const MyUtil::IntSeq& ids, const MyUtil::ObjectResultPtr&);
	void getOnlineInfo(const MyUtil::IntSeq& ids, const MyUtil::ObjectResultPtr&);
	
	static com::xiaonei::xce::Statement generateSql(const MyUtil::IntSeq&, const string&, const string&);
};

class BatchUserPassportResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
	BatchUserPassportResultHandler(const  MyUtil::ObjectResultPtr&);
protected:
	virtual bool handle(mysqlpp::Row& res) const;
private:
	MyUtil::ObjectResultPtr result_;
};

class BatchUserStageResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
	BatchUserStageResultHandler(const MyUtil::ObjectResultPtr&);
protected:
	virtual bool handle(mysqlpp::Row& res) const;
private:
	MyUtil::ObjectResultPtr result_;
};

class BatchUserBornResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
	BatchUserBornResultHandler(const MyUtil::ObjectResultPtr&);
protected:
	virtual bool handle(mysqlpp::Row& res) const;
private:
	MyUtil::ObjectResultPtr result_;
};

class BatchUserUrlResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
	BatchUserUrlResultHandler(const MyUtil::ObjectResultPtr&);
protected:
	virtual bool handle(mysqlpp::Row& res) const;
private:
	MyUtil::ObjectResultPtr result_;
};

class BatchUserNamesResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
	BatchUserNamesResultHandler(const MyUtil::ObjectResultPtr&);
protected:
	virtual bool handle(mysqlpp::Row& res) const;
private:
	MyUtil::ObjectResultPtr result_;
};

class BatchUserStateResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
	BatchUserStateResultHandler(const MyUtil::ObjectResultPtr&);
protected:
	virtual bool handle(mysqlpp::Row& res) const;
private:
	MyUtil::ObjectResultPtr result_;
};

class InsertTask: public MyUtil::Task {
public:
	InsertTask(int id);
	virtual void handle();
private:
	int id_;
};

class ReloadTask : public MyUtil::Task {
public:
	ReloadTask(const MyUtil::IntSeq&, const Ice::Current&);
	virtual void handle();
private:
	MyUtil::IntSeq ids_;
	Ice::Current current_;
};

class ReloadCallGuarantee : public callguarantee::CallGuarantee, public MyUtil::Singleton<ReloadCallGuarantee> {
public:
	ReloadCallGuarantee();
	virtual bool Do(const std::vector<std::pair<int, Ice::ObjectPtr> >& params);
	virtual bool Redo(const std::vector<int>& ids);
};

class InsertCallGuarantee: public callguarantee::CallGuarantee, public MyUtil::Singleton<InsertCallGuarantee> {
public:
	InsertCallGuarantee();
	virtual bool Do(const std::vector<std::pair<int, Ice::ObjectPtr> >& params);
	virtual bool Redo(const std::vector<int>& ids);
	static xce::distcache::usercacheadapter::UserCacheMemDataPtr Base2Cache(const xce::adapter::userbase::UserBaseFullViewPtr& data);
};

class UpdateCallGuarantee : public callguarantee::CallGuarantee, public MyUtil::Singleton<UpdateCallGuarantee> {
public:
	UpdateCallGuarantee();
	virtual bool Do(const vector<pair<int, Ice::ObjectPtr> >& params);
	virtual bool Redo(const vector<int>& ids);
};

class UpdateTask: public MyUtil::Task {
public:
	UpdateTask(const xce::usercache::UserCacheUpdateOldDataPtr& data, const Ice::Current& current);
	virtual void handle();
private:
	xce::distcache::usercacheadapter::UserCacheMemDataPtr getNewData();
	xce::usercache::UserCacheUpdateOldDataPtr data_;
	Ice::Current current_;
};

}
}
}

#endif
