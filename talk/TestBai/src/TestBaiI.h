#ifndef _TESTBAII_H_
#define _TESTBAII_H_

#include <Ice/Ice.h>
#include "TestBai.h"
#include "Singleton.h"
#include "Evictor.h"
#include "TaskManager.h"
#include "QueryRunner.h"

static const std::string DB_INSTANCE = "im_stat";
static const std::string DB_TABLE = "yuanfei";
/*
class ResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	typedef MyUtil::SizedEvictor<int, int> ValueEvictor;

	ResultHandlerI(ValueEvictor& tb):_tb(tb){}
	~ResultHandlerI() {}
	virtual bool handle(mysqlpp::Row& res) const;
private:
	ValueEvictor& _tb;
};
*/
class TestBaiI : public demo::TestBai, public MyUtil::Singleton<TestBaiI> {
public:
	TestBaiI();
	void _load();
	virtual void Insert(Ice::Int uid, Ice::Int count, const Ice::Current& = Ice::Current());
	virtual void Delete(Ice::Int uid, const Ice::Current& = Ice::Current());
	virtual Ice::Int Find(Ice::Int uid, const Ice::Current& = Ice::Current());
	virtual void Update(Ice::Int uid, Ice::Int count, const Ice::Current& = Ice::Current());
private:
	typedef MyUtil::SizedEvictor<int, int> ValueEvictor;
	ValueEvictor _tb;

	IceUtil::Mutex _tb_mutex;
};

class TestBaiTask : public MyUtil::Task{
public:
    virtual void handle();
};

#endif /* _TESTBAII_H_ */
