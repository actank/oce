#ifndef __FOOTPRINT_I_H__
#define __FOOTPRINT_I_H__

#include "Footprint.h"
#include "ServiceI.h"
#include "Date.h"

#include "QueryRunner.h"

#include <IceUtil/Mutex.h>
#include <boost/lexical_cast.hpp>

namespace mop {
namespace hi {
namespace svc {
namespace fpt {
const Ice::Int FOOTPRINT_SET = 0;

//由于代码仅用于blogFootprint，所以定为常量
const std::string TABLE_NAME = "blog_footprint";
const int TABLE_COUNT = 100;
const std::string DB_NAME = "blog_footprint_new";

//const string COLUMN_ID = "ID";
const static char* COLUMN_HOST = "HOST";
const static char* COLUMN_GUEST = "GUEST";
const static char* COLUMN_GUEST_NAME = "GUEST_NAME";
const static char* COLUMN_GUEST_HEAD = "GUEST_HEAD";
const static char* COLUMN_TIME = "TIME";

const static int TASK_LEVEL_BUILD = 1;
const static int TASK_LEVEL_ADD = 2;
const static int TASK_LEVEL_REMOVE = 3;
//const static int TASK_LEVEL_REFRESH = 4;


const Ice::Int REACHLIMIT = 0;
const Ice::Int PASS = 1;
const Ice::Int PASS_RESET = 2;

const static int VIEW_COUNT_LIMIT = 500;

class FootprintFactoryI : public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(long id);
};

class FootprintBuildTask : public MyUtil::Task {
public:
	FootprintBuildTask(long id):MyUtil::Task(TASK_LEVEL_BUILD),_id(id) {};
	virtual void handle();
private:
	long _id;
};

/*
class FootprintRefreshTask : public MyUtil::Task {
public:
	FootprintRefreshTask(int id, FootprintSetPtr& obj):MyUtil::Task(TASK_LEVEL_REFRESH),_id(id),_obj(obj) {};
	virtual void handle();
private:
	int _id;
	FootprintSetPtr _obj;
};
*/
class FootprintSetHandler : public com::xiaonei::xce::ResultHandler{
public:
	FootprintSetHandler(const FootprintSetPtr& fsp):_fsp(fsp){};//这里用const ptr 初始化非const ptr可以编译成功？？  wzc 09.05.07
	virtual bool handle(mysqlpp::Row& row) const;
private:
	FootprintSetPtr _fsp;
};

//---------------------------------------------------------------------------
struct QueueTask {
	FootprintSeq queue;
	time_t time;
};
typedef map<string, QueueTask> QueueTaskMap;

//---------------------------------------------------------------------------
class FootprintManagerI : public FootprintManager,
	public MyUtil::Singleton<FootprintManagerI> {
public:
	FootprintManagerI();
	virtual ~FootprintManagerI();

	virtual void visitHomepage(const FootprintPtr& footprint,
			const Ice::Current& = Ice::Current());

	virtual Ice::Int getSize(Ice::Long uid, const Ice::Current& = Ice::Current());
	virtual FootprintSeq getAll(Ice::Long uid, Ice::Int begin, Ice::Int limit,
			const Ice::Current& = Ice::Current());

	void add(const FootprintPtr& footprint, const Ice::Current& = Ice::Current());
	void remove(const FootprintPtr& footprint);

	FootprintSetPtr getFootprintSet(Ice::Long userId);
	string getTableName(long userId);
	bool validFootprint(const FootprintPtr& footprint);
	void evictFootprint(FootprintSeq& footprints);

private:
	int _footprintCacheSize;
	int _footprintDataSize;

	int _validFootprintInterval;
	int _footprintQueueSizeMax;
	int _footprintQueueInterval;

	bool _visitUserFootprint;

	int _tableCount;
	string _tableName;

	IceUtil::Mutex _mutexAdd;
	QueueTaskMap _queueTaskAdd;
	IceUtil::Mutex _mutexRemove;
	QueueTaskMap _queueTaskRemove;

};
typedef IceUtil::Handle<FootprintManagerI> FootprintManagerIPtr;

//---------------------------------------------------------------------------
//class DummyFootprintSetI : virtual public FootprintSet {
//public:
//	virtual Ice::Int getSize(const Ice::Current& = Ice::Current()){return 0;};
//	virtual FootprintSeq getAll(Ice::Int begin, Ice::Int limit,
//	            const Ice::Current& = Ice::Current()){return FootprintSeq();};
//	virtual bool add(const FootprintPtr& ftprt,
//	            const Ice::Current& = Ice::Current()){return true;};
//};

class FootprintSetI : virtual public FootprintSet,
		virtual public IceUtil::Mutex {
public:
	FootprintSetI()
		:_view_count_limit_time(time(NULL)),
		_view_count_limit_count(0){}
	virtual Ice::Int getSize(const Ice::Current& = Ice::Current());
	virtual FootprintSeq getAll(Ice::Int begin, Ice::Int limit,
			const Ice::Current& = Ice::Current());
	virtual bool add(const FootprintPtr& ftprt,
			const Ice::Current& = Ice::Current());
	virtual void merge(const FootprintSetPtr&,
			const Ice::Current& = Ice::Current());

private:
	time_t _view_count_limit_time;
	int _view_count_limit_count;
	int isReachViewCountLimit();
};

class FootprintEqualComparator : public unary_function<FootprintPtr, bool> {
public:
	FootprintEqualComparator(Ice::Long host, Ice::Int guest) :
		_host(host), _guest(guest) {
	}
	;
	bool operator()(const FootprintPtr& footprint) const {
		return footprint && footprint->host == _host && footprint->guest
				== _guest;
	}
private:
	Ice::Long _host;
	Ice::Int _guest;
};

struct FootprintGreaterComparator : public binary_function<FootprintPtr, FootprintPtr, bool> {
	bool operator()(const FootprintPtr& lhs, const FootprintPtr& rhs) const {
		return lhs->time > rhs->time;
	}
};


//---------------------------------------------------------------------------
class AddTask : public MyUtil::Task {
public:
	AddTask(std::string table, FootprintSeq& queue);
	virtual void handle();
private:
	std::string _table;
	FootprintSeq _queue;
};

class RemoveTask : public MyUtil::Task {
public:
	RemoveTask(std::string table, FootprintSeq& queue);
	virtual void handle();
private:
	std::string _table;
	FootprintSeq _queue;
};


}
;
}
;
}
;
}
;
#endif

