#ifndef __USER_CACHE_RELOADER__
#define __USER_CACHE_RELOADER__

#include <ServiceI.h>
#include <UserCache.h>
#include <UserBaseAdapter.h>
#include <QueryRunner.h>
#include <CallGuarantee/src/CallGuarantee.h>

namespace xce{
namespace usercache{

const std::string USER_CACHE_RELOADER = "M";

const int TASK_LEVEL_RELOAD = 1;
const int TASK_LEVEL_UPDATE = 2;
const int TASK_LEVEL_INSERT = 3;

class UserCacheReloaderI : public UserCacheReloader,  virtual public MyUtil::Singleton<UserCacheReloaderI> {
public:
        UserCacheReloaderI() {}
	virtual bool isValid(const Ice::Current& = Ice::Current());
	virtual void setValid(bool valid, const Ice::Current& = Ice::Current());
	void reload(const MyUtil::IntSeq& ids, const Ice::Current&);
	void rebuildOnline(const MyUtil::IntSeq& ids, const Ice::Current&);
	void insert(int id, const Ice::Current&);
        void update(const UserCacheUpdateOldDataPtr& data, const Ice::Current&);
	MyUtil::ObjectResultPtr GetObjectResult(const MyUtil::IntSeq& ids);
private:
	bool valid_;
	IceUtil::RWRecMutex validmutex_;

	void GetUserPassport(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr&);
	void GetUserState(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr&);
	void GetUserStage(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr&);
	void GetUserBorn(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr&);
	void GetUserNames(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr&);
	void GetUserUrl(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr&);
	void GetOnlineStat(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr&);
};

class InsertTask: public MyUtil::Task{
public:
	InsertTask(int id):
		MyUtil::Task(TASK_LEVEL_INSERT),id_(id) {}
	virtual void handle();
private:
	int id_;
};

class ReloadTask: public MyUtil::Task{
public:
	ReloadTask(const MyUtil::IntSeq& ids, const Ice::Current& current):
		MyUtil::Task(TASK_LEVEL_RELOAD),ids_(ids), current_(current) {}
	virtual void handle();
private:
	void DistributeIds(std::vector<std::vector<int> >& ids);
	MyUtil::IntSeq ids_;
	Ice::Current current_;
};

class BatchUserPassportResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
        BatchUserPassportResultHandler(const  MyUtil::ObjectResultPtr& result):_result(result){}
protected:
        virtual bool handle(mysqlpp::Row& res) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchUserStageResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
        BatchUserStageResultHandler(const MyUtil::ObjectResultPtr& result):_result(result){}
protected:
        virtual bool handle(mysqlpp::Row& res) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchUserBornResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
        BatchUserBornResultHandler(const MyUtil::ObjectResultPtr& result):_result(result){}
protected:
        virtual bool handle(mysqlpp::Row& res) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchUserUrlResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
        BatchUserUrlResultHandler(const MyUtil::ObjectResultPtr& result):_result(result){}
protected:
        virtual bool handle(mysqlpp::Row& res) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchUserNamesResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
        BatchUserNamesResultHandler(const MyUtil::ObjectResultPtr& result):_result(result){}
protected:
        virtual bool handle(mysqlpp::Row& res) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchUserStateResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
        BatchUserStateResultHandler(const MyUtil::ObjectResultPtr& result):_result(result){}
protected:
        virtual bool handle(mysqlpp::Row& res) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class ReloadCallGuarantee : public callguarantee::CallGuarantee,
        public MyUtil::Singleton<ReloadCallGuarantee> {
public:
        ReloadCallGuarantee() : CallGuarantee(1, 1000, 2, 10000) {}
        virtual bool Do(const std::vector<std::pair<int, Ice::ObjectPtr> >& params);
        virtual bool Redo(const std::vector<int>& ids);
};

class InsertCallGuarantee: public callguarantee::CallGuarantee,
	public MyUtil::Singleton<InsertCallGuarantee> {
public:
	InsertCallGuarantee() : CallGuarantee(1, 1000, 2, 10000) {}
	virtual bool Do(const std::vector<std::pair<int, Ice::ObjectPtr> >& params);
	virtual bool Redo(const std::vector<int>& ids);
private:
	UserCacheData3Ptr Base2Cache(const xce::adapter::userbase::UserBaseFullViewPtr& data);
};

class UpdateCallGuarantee : public callguarantee::CallGuarantee,
	public MyUtil::Singleton<UpdateCallGuarantee> {
public:
	UpdateCallGuarantee() : CallGuarantee(1, 1000, 2, 10000) {}
	virtual bool Do(const std::vector<std::pair<int, Ice::ObjectPtr> >& params);
	virtual bool Redo(const std::vector<int>& ids);
};

class UpdateTask: public MyUtil::Task{
public:
	UpdateTask(const UserCacheUpdateOldDataPtr& data, const Ice::Current& current): MyUtil::
		Task(TASK_LEVEL_UPDATE), data_(data), current_(current) {}
	virtual void handle();
private:
	UserCacheUpdateNewDataPtr GetNewData();
	UserCacheUpdateNewDataPtr Check(const UserCacheUpdateOldDataPtr& data);
	UserCacheUpdateOldDataPtr data_;
	Ice::Current current_;
};


}
}

#endif
