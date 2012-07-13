#ifndef __CountUpdater_h__
#define __CountUpdater_h__

#include <Ice/Ice.h>
#include <IceUtil/AbstractMutex.h>
#include <IceUtil/Mutex.h>
#include "BasicUpdater.h"
#include "Singleton.h"
#include "TaskManager.h"

namespace mop {

namespace hi {

namespace svc {

namespace model {

const uint WAIT_TIME = 5;
const uint VECTOR_SIZE = 100;
const uint TASK_UPDATE_LEVEL = 2;

struct DescDatabase {
	DescDatabase(const std::string& db, const std::string& pp, int tc) :
		database(db), patternPrefix(pp), tableCluster(tc) {
	}
	string database;
	string patternPrefix;
	int tableCluster;
};
struct DescSqlbuilder {
	DescSqlbuilder(const std::string& t, const std::string& vc,
			const StrSeq& idc, bool useInsDup = false) :
		useInsertDuplicate(useInsDup), tablePrefix(t), valueColumn(vc),
				idColumns(idc) {
	}
	bool useInsertDuplicate;
	string tablePrefix;
	string valueColumn;
	StrSeq idColumns;
};

/*class CountUpdater: virtual public BasicUpdater<vector<Ice::Long> , Ice::Int> {
public:
	typedef vector<Ice::Long> IdType;

public:
	CountUpdater(string database, string table, string valueColumn,
			StrSeq& idColumns, int timeSlice = 1, int tabCluster = 0,
			bool useInsertDuplicate = false);
	CountUpdater(const DescDatabase& db, const DescSqlbuilder& sqlbuilder,
			time_t timeSlice);

protected:
	virtual Ice::Int calc(const Ice::Int& orig, const Ice::Int& inc);
	virtual void fixSave(IdSet& ids, const Ice::Int& count);
	virtual void trim(IdSetMap& idSets);

	void save(IdSet& ids, Ice::Int count, const string& table,
			const string& pattern);
private:
	void trim(IdSetMap& idSets, int sign, int loopInterval);

	int _timeSlice;
	int _tabCluster;
	string _patternPrefix;
	bool _useInsertDuplicate;
	BuildSqlVectorPtr _buildSqlThread;
};*/

struct SqlNode : virtual public Ice::Object {
	std::string database;
	std::string pattern;
	std::string sql;
};

typedef IceUtil::Handle<SqlNode> SqlNodePtr;

class BuildSqlVector : virtual public Ice::Object,
			virtual public IceUtil::Thread {
public:
	BuildSqlVector(){};
	void build(std::string sqlStr,std::string database, std::string pattern);
	virtual void run();
private:

	std::vector<SqlNodePtr> _sqlVector;
	IceUtil::Monitor<IceUtil::Mutex> _mutex;

};

typedef IceUtil::Handle<BuildSqlVector> BuildSqlVectorPtr;

class UpdaterTask : public MyUtil::Task {
public:
	UpdaterTask(std::vector<SqlNodePtr> sqlVector):MyUtil::Task(TASK_UPDATE_LEVEL),_sqlVector(sqlVector){};
	virtual void handle();
private:
	std::vector<SqlNodePtr> _sqlVector;
};

class CountUpdater: virtual public BasicUpdater<vector<Ice::Long> , Ice::Int> {
public:
	typedef vector<Ice::Long> IdType;

public:
	/* @brief 	构造函数
	 * @param	database	db的decripter名
	 * @param	table		表名的pattern
	 * @param	valueColumn	要修改的值列
	 * @param	idColumns	作为key的列组
	 * @param	timeSlice	every looping time
	 * @param	tabCluster	表散的份数
	 * @param	useInsertDuplicate	是否使用"ON DUPLICATE KEY UPDATE"(存在更新, 否则添加)
	 * @param	beginIdColumnsInSql	在idColumns中, 从第几个开始才用在sql中(之前的只用于散表操作)
	 */
	CountUpdater(string database, string table, string valueColumn,
			StrSeq& idColumns, int timeSlice = 1, int tabCluster = 0,
			bool useInsertDuplicate = false, int beginIdColumnsInSql=0);

	/* @brief	构造函数
	 * @param	db		db描述信息(在这里只用到了db的descripter)
	 * @param	sqlbuilder	构造sql的各项信息的结构体
	 * @param	timeSlice	every looping time
	 * @param	beginIdColumnsInSql     在idColumns中, 从第几个开始才用在sql中(之前的只用于散表操作)
	 */ 
	CountUpdater(const DescDatabase& db, const DescSqlbuilder& sqlbuilder,
			time_t timeSlice, int beginIdColumnsInSql=0);

protected:
	virtual Ice::Int calc(const Ice::Int& orig, const Ice::Int& inc);
	virtual void fixSave(IdSet& ids, const Ice::Int& count);
	virtual void trim(IdSetMap& idSets);

	void save(IdSet& ids, Ice::Int count, const string& table,
			const string& pattern);
private:
	void trim(IdSetMap& idSets, int sign, int loopInterval);

	int _timeSlice;
	int _tabCluster;
	string _patternPrefix;
	bool _useInsertDuplicate;
	int _beginIdColumnsInSql;
	BuildSqlVectorPtr _buildSqlThread;
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
