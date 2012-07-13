#ifndef INFO_RESULT_COLLECTOR_H_
#define INFO_RESULT_COLLECTOR_H_

#include "QueryRunner.h"
#include "ObjectCacheI.h"
#include "MD5Util.h"
#include <FriendFinderInfoCacheReplicaAdapter.h>
#include "FriendFinderInfoWorkerI.h"

namespace xce {
namespace friendfinder {
static const int TASK_WORKPLACE = 0;
static const int TASK_UNIV = 1;
static const int TASK_HIGHSCHOOL = 2;
static const int TASK_JUNIORSCHOOL = 3;
static const int TASK_ELEMENTARYSCHOOL = 4;
static const int TASK_BIRTHDAYDATA = 5;

static const int begin = 0;
static const int end = 10;

class InfoResultCollector {
public:
	static void createInfoResultBatch(int userId);

	static void getInfoResultFromWorkplace(bool empty=false);
	static void getInfoResultFromUniv(bool empty=false);
	static void getInfoResultFromHighschool(bool empty=false);
	static void getInfoResultFromJuniorschool(bool empty=false);
	static void getInfoResultFromElementschool(bool empty=false);
	static void getInfoResultFromBirthday(bool empty=false);
};

//**************************************************************************************************************

class UnivBatchReadHandler : public com::xiaonei::xce::ResultHandler {
public:
	UnivBatchReadHandler(int& last, std::map<long, std::vector<int> >& resultMap): _last(last),_resultMap(resultMap) {}
    virtual bool handle(mysqlpp::Row& row) const;
    
private:
	int& _last;
    std::map<long, std::vector<int> >& _resultMap;
};

//**************************************************************************************************************

class HighschoolBatchReadHandler: public com::xiaonei::xce::ResultHandler {
public:
	HighschoolBatchReadHandler(int& last, std::map<long, std::vector<int> >& resultMap): _last(last),_resultMap(resultMap) {}
    virtual bool handle(mysqlpp::Row& row) const;
    
private:
	int& _last;
    std::map<long, std::vector<int> >& _resultMap;
};

//**************************************************************************************************************

class JuniorschoolBatchReadHandler: public com::xiaonei::xce::ResultHandler {
public:
	JuniorschoolBatchReadHandler(int& last, std::map<long, std::vector<int> >& resultMap): _last(last),_resultMap(resultMap) {}
    virtual bool handle(mysqlpp::Row& row) const;
    
private:
	int& _last;
    std::map<long, std::vector<int> >& _resultMap;
};

//**************************************************************************************************************

class ElementaryschoolBatchReadHandler: public com::xiaonei::xce::ResultHandler {
public:
	ElementaryschoolBatchReadHandler(int& last, std::map<long, std::vector<int> >& resultMap): _last(last),_resultMap(resultMap) {}
    virtual bool handle(mysqlpp::Row& row) const;
    
private:
	int& _last;
    std::map<long, std::vector<int> >& _resultMap;
};

//**************************************************************************************************************

class WorkplaceBatchReadHandler: public com::xiaonei::xce::ResultHandler {
public:
	WorkplaceBatchReadHandler(int& last, std::map<long, std::vector<int> >& resultMap): _last(last),_resultMap(resultMap) {}
    virtual bool handle(mysqlpp::Row& row) const;
    
private:
	int& _last;
    std::map<long, std::vector<int> >& _resultMap;
};

//**************************************************************************************************************

class BirthdayBatchReadHandler: public com::xiaonei::xce::ResultHandler {
public:
	BirthdayBatchReadHandler(int& last, std::map<int, std::vector<int> >& resultMap): _last(last),_resultMap(resultMap) {}
    virtual bool handle(mysqlpp::Row& row) const;
    
private:
	int& _last;
	std::map< int, std::vector<int> >& _resultMap;
};

//**************************************************************************************************************

class LoadUnivTask: virtual public MyUtil::Task {          //单个调用，并行搜索
public:
    LoadUnivTask(Ice::Int userId): 
		Task(TASK_UNIV), _userId(userId) { };
    virtual void handle();
private:
    Ice::Int _userId;
};

//**************************************************************************************************************

class LoadHighschoolTask: virtual public MyUtil::Task {          //单个调用，并行搜索
public:
    LoadHighschoolTask(Ice::Int userId): 
		Task(TASK_HIGHSCHOOL), _userId(userId) { };
    virtual void handle();
private:
    Ice::Int _userId;
};

//**************************************************************************************************************

class LoadJuniorschollTask: virtual public MyUtil::Task {          //单个调用，并行搜索
public:
    LoadJuniorschollTask(Ice::Int userId): 
		Task(TASK_JUNIORSCHOOL), _userId(userId) { };
    virtual void handle();
private:
    Ice::Int _userId;
};

//**************************************************************************************************************

class LoadElementschoolTask: virtual public MyUtil::Task {          //单个调用，并行搜索
public:
    LoadElementschoolTask(Ice::Int userId): 
		Task(TASK_ELEMENTARYSCHOOL), _userId(userId) { };
    virtual void handle();
private:
    Ice::Int _userId;
};

//**************************************************************************************************************

class LoadWorkplaceTask: virtual public MyUtil::Task {          //单个调用，并行搜索
public:
    LoadWorkplaceTask(Ice::Int userId):
		Task(TASK_WORKPLACE), _userId(userId) { };
    virtual void handle();

private:
    Ice::Int _userId;
};

//**************************************************************************************************************

class LoadBirthdayTask: virtual public MyUtil::Task {          //单个调用，并行搜索
public:
    LoadBirthdayTask(Ice::Int userId): 
		Task(TASK_BIRTHDAYDATA),_userId(userId) { };
    virtual void handle();

private:
    Ice::Int _userId;
};

}
}

#endif
