#ifndef _COMPAREINFO_RELOADER_H_
#define _COMPAREINFO_RELOADER_H_

#include <ServiceI.h>
#include <CompareInfo.h>
#include <QueryRunner.h>
#include <CallGuarantee/src/CallGuarantee.h>

namespace xce{
namespace compareinfo{

const std::string COMPAREINFO_RELOADER = "M";

const int TASK_LEVEL_RELOAD = 1;
const int TASK_LEVEL_UPDATE = 2;
const int TASK_LEVEL_INSERT = 3;

class CompareInfoReloaderI : public CompareInfoReloader,  virtual public MyUtil::Singleton<CompareInfoReloaderI> {
public:
        CompareInfoReloaderI() {}
	virtual bool isValid(const Ice::Current& = Ice::Current());
	virtual void setValid(bool valid, const Ice::Current& = Ice::Current());
	void reload(const MyUtil::IntSeq& ids, const Ice::Current&);
        void update(const CompareInfoUpdateOldDataPtr& data, const Ice::Current&);
	MyUtil::ObjectResultPtr GetObjectResult(const MyUtil::IntSeq& ids);
private:
	bool valid_;
	IceUtil::RWRecMutex validmutex_;
};

class ReloadTask: public MyUtil::Task{
public:
	ReloadTask(const MyUtil::IntSeq& ids, const Ice::Current current):
		MyUtil::Task(TASK_LEVEL_RELOAD),ids_(ids) ,current_(current){}
	virtual void handle();
private:
	void DistributeIds(std::vector<std::vector<int> >& ids);
	MyUtil::IntSeq ids_;
	Ice::Current current_;
};


class ReloadCallGuarantee : public callguarantee::CallGuarantee,
        public MyUtil::Singleton<ReloadCallGuarantee> {
public:
        ReloadCallGuarantee() : CallGuarantee(1, 1000, 2, 10000) {}
        virtual bool Do(const std::vector<std::pair<int, Ice::ObjectPtr> >& params);
        virtual bool Redo(const std::vector<int>& ids);
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
	UpdateTask(const CompareInfoUpdateOldDataPtr& data, const Ice::Current& current): MyUtil::
		Task(TASK_LEVEL_UPDATE), data_(data), current_(current) {}
	virtual void handle();
private:
	CompareInfoUpdateNewDataPtr GetNewData();
	//CompareInfoUpdateNewDataPtr Check(const CompareInfoUpdateOldDataPtr& data);
	CompareInfoUpdateOldDataPtr data_;
	Ice::Current current_;
};


}
}

#endif

