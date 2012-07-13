#include "DbHelper.h"
#include "QueryRunner.h"
#include "IceLogger.h"
#include "FeedCache.h"
#include "FeedInteractionI.h"

using namespace xce::feed;
using namespace com::xiaonei::xce;
using namespace MyUtil;

bool ParallelLoader::query(ObjectResultPtr & res) {
	int count = _count;
  for (size_t i = 0; i < _taskSeq.size(); ++i) {
		MyUtil::TaskManager::instance().execute(_taskSeq.at(i));
	}
	_taskSeq.clear();

	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
	if (!timedWait(IceUtil::Time::milliSeconds(LOAD_TIMEOUT))) {
		MCE_WARN("ParallelLoader::query -> count: " << count << " missing:" << _count);
	}
	res = _result;
	return _count == 0;
}
void ParallelLoader::addQuery(const string& dbInstance, const string& sql,
		const string& pattern, bool isSelect) {
	SqlExecuteTask* t = new SqlExecuteTask(this, isSelect);
	t->init(dbInstance, sql, pattern);
	_taskSeq.push_back(t);
	++_count;
}

void ParallelLoader::SqlExecuteTask::handle() {
//	MCE_DEBUG("ParallelLoader --> SqlExecuteTask  seq:"<<_sql);
	try{
  if (_isSelect) {
		MyUtil::ObjectResultPtr result = new MyUtil::ObjectResult;
		ResultHandlerI handler(result);
		QueryRunner(_dbInstance, CDbRServer, _pattern).query(_sql, handler);
		_loader->finish(result);
	} else {
		QueryRunner(_dbInstance, CDbWServer, _pattern).store(_sql);
		_loader->finish();
	}
  }catch(std::exception& e){
    MCE_WARN("SqlExecuteTask::handle --> err, " << e.what());
  }
}

