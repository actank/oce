#ifndef __USER_API_CACHE_RELOADER__
#define __USER_API_CACHE_RELOADER__

#include <ServiceI.h>
#include <UserApiCache.h>
#include <QueryRunner.h>
#include <CallGuarantee/src/CallGuarantee.h>

namespace xce{
namespace userapicache{

const std::string USER_API_CACHE_RELOADER = "M";

const int TASK_LEVEL_RELOAD = 1;
const int TASK_LEVEL_UPDATE = 2;

class UserApiCacheReloaderI : public UserApiCacheReloader,  virtual public MyUtil::Singleton<UserApiCacheReloaderI> {
public:
        UserApiCacheReloaderI() {}
        virtual ~UserApiCacheReloaderI() {}
	void reload(const MyUtil::IntSeq& ids, const Ice::Current&);
        void update(const UserApiCacheUpdateOldDataPtr& data, const Ice::Current&);
	virtual bool isValid(const Ice::Current& = Ice::Current());
	virtual void setValid(bool valid, const Ice::Current& = Ice::Current());

	MyUtil::ObjectResultPtr GetObjectResult(const MyUtil::IntSeq& ids);
private:
	bool valid_;
        IceUtil::RWRecMutex validmutex_;

	void GetUserPassport(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr&);
	void GetUserState(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr&);
	void GetUserRight(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr&);
	void GetUserConfig(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr&);
	void GetUserBorn(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr&);
	void GetUserStage(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr&);
	void GetUserNames(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr&);
	void GetUserUrl(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr&);
};

class UpdateTask: public MyUtil::Task{
public:
	UpdateTask(const UserApiCacheUpdateOldDataPtr& data): MyUtil::
		Task(TASK_LEVEL_UPDATE), data_(data) {}
	virtual void handle();
private:
	UserApiCacheUpdateNewDataPtr GetNewData();
	UserApiCacheUpdateNewDataPtr Check(const UserApiCacheUpdateOldDataPtr& data);
	UserApiCacheUpdateOldDataPtr data_;
};

class ReloadTask: public MyUtil::Task{
public:
	ReloadTask(const MyUtil::IntSeq& ids):
		MyUtil::Task(TASK_LEVEL_RELOAD),ids_(ids) {}
	virtual ~ReloadTask(){}
	virtual void handle();
private:
	void DistributeIds(std::vector<std::vector<int> >& ids);
	void SendData(int mod,const MyUtil::ObjectResultPtr&);
	MyUtil::IntSeq ids_;
};

class BatchUserPassportResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
        BatchUserPassportResultHandler(const  MyUtil::ObjectResultPtr& result):_result(result){}
        virtual ~BatchUserPassportResultHandler(){}
protected:
        virtual bool handle(mysqlpp::Row& res) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchUserBornResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
        BatchUserBornResultHandler(const MyUtil::ObjectResultPtr& result):_result(result){}
        virtual ~BatchUserBornResultHandler(){}
protected:
        virtual bool handle(mysqlpp::Row& res) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchUserStageResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
        BatchUserStageResultHandler(const MyUtil::ObjectResultPtr& result):_result(result){}
        virtual ~BatchUserStageResultHandler(){}
protected:
        virtual bool handle(mysqlpp::Row& res) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchUserUrlResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
        BatchUserUrlResultHandler(const MyUtil::ObjectResultPtr& result):_result(result){}
        virtual ~BatchUserUrlResultHandler(){}
protected:
        virtual bool handle(mysqlpp::Row& res) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchUserNamesResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
        BatchUserNamesResultHandler(const MyUtil::ObjectResultPtr& result):_result(result){}
        virtual ~BatchUserNamesResultHandler(){}
protected:
        virtual bool handle(mysqlpp::Row& res) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchUserConfigResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
        BatchUserConfigResultHandler(const MyUtil::ObjectResultPtr& result):_result(result){}
        virtual ~BatchUserConfigResultHandler(){}
protected:
        virtual bool handle(mysqlpp::Row& res) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchUserRightResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
        BatchUserRightResultHandler(const MyUtil::ObjectResultPtr& result):_result(result){}
        virtual ~BatchUserRightResultHandler(){}
protected:
        virtual bool handle(mysqlpp::Row& res) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchUserStateResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
        BatchUserStateResultHandler(const MyUtil::ObjectResultPtr& result):_result(result){}
        virtual ~BatchUserStateResultHandler(){}
protected:
        virtual bool handle(mysqlpp::Row& res) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class UpdateCallGuarantee : public callguarantee::CallGuarantee,
	public MyUtil::Singleton<UpdateCallGuarantee> {
public:
	UpdateCallGuarantee() : CallGuarantee(1, 1000, 2, 10000) {}
	virtual bool Do(const std::vector<std::pair<int, Ice::ObjectPtr> >& params);
	virtual bool Redo(const std::vector<int>& ids);
};

class ReloadCallGuarantee : public callguarantee::CallGuarantee,
	public MyUtil::Singleton<ReloadCallGuarantee> {
public:
	ReloadCallGuarantee() : CallGuarantee(1, 1000, 2, 10000) {}
	virtual bool Do(const std::vector<std::pair<int, Ice::ObjectPtr> >& params);
	virtual bool Redo(const std::vector<int>& ids);
};

}
}

#endif
