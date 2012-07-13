#include "SqlResultHandlerI.h"

using namespace MyUtil;
using namespace IceUtil;
using namespace com::xiaonei::xce;
using namespace xce::searchcache;

SingleResultHandlerI::SingleResultHandlerI(SearchCacheDataPtr& result, void (SearchCacheData::*p)(mysqlpp::Row& row)) :
	_result(result), _pFunc(p) {
}

bool SingleResultHandlerI::handle(mysqlpp::Row& row) const {
	((_result.get())->*_pFunc)(row);
	return false;
}
//***************************************************************************
BatchResultHandlerI::BatchResultHandlerI(std::string id, void (SearchCacheData::*p)(mysqlpp::Row& row), int cacheid):
    _id(id), _pFunc(p), _cacheid(cacheid) {
}

bool BatchResultHandlerI::handle(mysqlpp::Row& row) const {
	int id = (int) row[_id.c_str()];
	SearchCacheDataPtr data=ServiceI::instance().findObject<SearchCacheDataPtr> (_cacheid,id);
	if (data == 0) {
		// MCE_WARN("IMPOSSIABLE, user_config has id=" << id << " but user_basic not. "
		// 		<< __FILE__ << " " << __LINE__);
	} else {
		((data.get())->*_pFunc)(row);
	}
	return true;
}

BatchBasicResultHandlerI::BatchBasicResultHandlerI(MyUtil::ObjectResultPtr& result, Ice::Int& maxId, int cacheid) :
	_result(result), _maxId(maxId), _cacheid(cacheid) {
        /*
        _initDbSusscess = true;
        _db_cache = DatabaseCacheFactory::instance().getDbCache();
        if (_db_cache->Initialize() == false) {
            MCE_WARN("BatchBasicResultHandlerI: Db initialize failed");
            _initDbSusscess = false;
        }
        */
}

bool BatchBasicResultHandlerI::handle(mysqlpp::Row& row) const {
	int id = (int) row["id"];
	_maxId = id > _maxId ? id : _maxId;
	int status = (int)row["status"];
	if (status>=3){
		MCE_WARN("the user "<<id<<" status is "<<status);
		return true;
	}
	MyUtil::ObjectMap::iterator it = _result->data.find(id);
	if (it == _result->data.end()) {
		SearchCacheDataPtr obj = new SearchCacheData;
        /*
        if (_initDbSusscess) {
            _db_cache->Query(id, obj.get());
        }
        */
		obj->_id = id;
		obj->setBasicProperties(row);
		_result->data.insert(make_pair(id, obj));
	} else {
		MCE_WARN("unreachable code segment " << __FILE__ << " " << __LINE__);
	}
	return true;
}

//***************************************************************************
BatchTask::BatchTask(std::string id, std::string db, void (SearchCacheData::*p)(mysqlpp::Row& row), 
    const Statement& sql,int level):Task(level),_sql(sql), _db(db), _id(id), _pFunc(p){
}

void BatchTask::handle(){
        BatchResultHandlerI handler(_id, _pFunc);
        QueryRunner(_db, CDbRServer).query(_sql, handler);
}

//***************************************************************************
