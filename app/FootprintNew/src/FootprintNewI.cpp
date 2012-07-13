#include "FootprintNewI.h"
#include "util/cpp/String.h"
#include "BatchUpdaterAdapter.h"
#include "QueryRunner.h"
#include "ActiveTrackAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

#include "util/cpp/TimeCost.h"
#include "util/cpp/InvokeMonitor.h"
#include <Monitor/client/service/ServiceMonitor.h>

using namespace MyUtil;
using namespace xce::fptn;
using namespace com::xiaonei::xce;
using namespace ::xce::log;
using namespace xce::serverstate;

//---------------------------------------------------------------------------
void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	
	service.getAdapter()->add(new FootprintManagerI, service.createIdentity("FMN", ""));
	TaskManager::instance().config(TASK_LEVEL_BUILD,ThreadPoolConfig(0,30));
	TaskManager::instance().config(TASK_LEVEL_ADD,ThreadPoolConfig(0,100));
	TaskManager::instance().config(TASK_LEVEL_REMOVE,ThreadPoolConfig(0,100));
	TaskManager::instance().config(TASK_LEVEL_SAVE,ThreadPoolConfig(0,100));
	ActiveTrack::instance().initialize();

	service.registerObjectCache(FOOTPRINT_SET, "FS", new FootprintFactoryI);//register
	// 将UserVisitCount的cache由User移到FootprintNew  -- 090911 by zhanghan
	service.registerObjectCache(USER_VISIT_COUNT, "UVC", new UserVisitCountFactoryI);

        // 加入向Controller注册
        int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("Footprint.Mod");
        int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Footprint.Interval", 5);
	string ctrEndpoints = service.getCommunicator()->getProperties()
		->getPropertyWithDefault("Footprint.ControllerEndpoint", "ControllerFootprint");
        ServerStateSubscriber::instance().initialize(ctrEndpoints,
		       &FootprintManagerI::instance(), mod, interval);
	//统计log
	ServiceMonitorManager::instance().start();
}

//------------------------FootprintFactoryI---------------------------------------
Ice::ObjectPtr FootprintFactoryI::create(Ice::Int id) {
	MCE_INFO("FootprintFactoryI::create id " << id );
	
	// 改回异步加载  -- 09.09.27 by zhanghan
	/* TaskManager::instance().execute(new FootprintBuildTask(id));
	return new FootprintSet; */

	// 改为同步加载 -- 09.10.28 by zhanghan
	string table = FootprintManagerI::instance().getTableName(id);
	Statement sql;
	sql << "SELECT * FROM " << table << " WHERE " << COLUMN_HOST << "=" << id;
	FootprintSetPtr obj = new FootprintSet;
	try {
		QueryRunner("footprintn", CDbRServer, table).query(sql, FootprintSetHandler(obj));
	} catch (mysqlpp::Exception& e ) {
		MCE_WARN("[FootprintFactoryI::create] create footprint of " << id << " mysqlpp::exception : " << e.what());
		obj->setErrorFlag();
	} catch (...) {
		MCE_WARN("[FootprintFactoryI::create] create footprint of " << id << " unknown exception");
		obj->setErrorFlag();
	}
	ServiceI::instance().addObject(obj, FOOTPRINT_SET, (long)id);
	MCE_INFO("Loaded " << obj->getFootprints().size() << " for " << id);
	return obj;
}

void FootprintBuildTask::handle(){
	FootprintManagerI& manager = FootprintManagerI::instance();string table = manager.getTableName(_id);
	Statement sql;
	sql << "SELECT * FROM " << table << " WHERE " << COLUMN_HOST << "=" << _id;
	/* no limit here. Outdated item will be deleted later*/

	FootprintSetPtr obj = new FootprintSet;
	QueryRunner("footprintn", CDbRServer, table).query(sql, FootprintSetHandler(obj));

	FootprintSetPtr initObj = NULL;
        try {
		initObj = ServiceI::instance().findObject<FootprintSetPtr>(FOOTPRINT_SET, (long)_id);
	} catch (mysqlpp::Exception& e) {
		MCE_WARN("[FootprintBuildTask::handle] load " << _id << " from db mysqlpp::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[FootprintBuildTask::handle] load " << _id << " exception  ");
	}

	if (!initObj) {
		ServiceI::instance().addObject(obj, FOOTPRINT_SET, (long)_id);
		MCE_INFO("Loaded " << obj->getFootprints().size() << " for " << _id);
	} else{
	  	initObj->merge(obj);
		MCE_INFO("Refresh " << obj->getFootprints().size() << " for " << _id);
	}

}

bool FootprintSetHandler::handle(mysqlpp::Row& row) const {

	// footprint->id is not the original meanings, now it is used to mark
	// persistenting status in db, 1 for persistent, 0 for not
	//footprint->id = 1;//boost::lexical_cast<Ice::Long>(ent->getProperty(COLUMN_ID));
	//footprint->host = (int)row[COLUMN_HOST];
	_fsp->getFootprints() = IntSeqSerialize::unserialize(row[COLUMN_GUESTS]);
//	for (size_t i = 0; i < _fsp->footprints.size(); ++i) 
//	{
//		MCE_INFO("_fsp->footprints(" << i << ") :" << _fsp->footprints.at(i));
//	}
	return true;
}

//------------------- FootprintManagerI 主类 ----------------------------------------------
FootprintManagerI::FootprintManagerI() {
	ServiceI& service = ServiceI::instance();
	Ice::PropertiesPtr props = service.getCommunicator()->getProperties();

	_footprintCacheSize = props->getPropertyAsIntWithDefault(
			"Service." + service.getName() + ".FootprintSet.CacheSize", 20);
	_footprintDataSize = props->getPropertyAsIntWithDefault("Service." + service.getName() + ".FootprintSet.DataSize",
			6);

	_validFootprintInterval = props->getPropertyAsIntWithDefault("Service." + service.getName()
			+ ".FootprintSet.ValidFootprintInterval", 20);
	_tableCount = props->getPropertyAsIntWithDefault("Service." + service.getName() + ".TableCount", 100);
	_tableName = props->getPropertyWithDefault("Service." + service.getName() + ".TableName", "footprintn");
	_serverName = service.getName();
	_visitUserFootprint = _serverName.compare(0, strlen("Footprint"), "Footprint") == 0;
	_refreshDbInterval = props->getPropertyAsIntWithDefault("Service." + service.getName() + ".RefreshDbInterval", 3);
	int saveBatchSizeInt = props->getPropertyAsIntWithDefault("Service." + service.getName() + ".SaveBatchSize", 200);
	_saveBatchSize = (saveBatchSizeInt > 0)?(size_t)saveBatchSizeInt:200;
	string slaveStr = props->getPropertyWithDefault("Service." + service.getName() + ".IsSlave", "No");
	_isSlave = (slaveStr=="Yes");

	MCE_INFO("[FootprintManagerI::FootprintManagerI] serverName:" << _serverName << " tableName:" << _tableName 
			<< " tableCount:" << _tableCount << " visitUserFootprint:" << _visitUserFootprint
			<< " saveBatchSize:" << _saveBatchSize << " refreshDbInterval:" << _refreshDbInterval
			<< " isSlave?" << _isSlave);
}

FootprintManagerI::~FootprintManagerI() {

}
//locate the FootprintSet use userId
FootprintSetPtr FootprintManagerI::getFootprintSet(Ice::Int userId, bool ignoreError) {
	FootprintSetPtr fptn = ServiceI::instance().locateObject<FootprintSetPtr>(FOOTPRINT_SET, userId);
	if ( fptn ) {
		if ( fptn->isError() ) {
			if ( ! ignoreError ) {
				// 如果locate有误,则在ObjectCache里将此次locate的cache删掉,下次重新读DB
				// 防止积累错误而导致脚印清空	-- by zhanghan 09.12.11
				MCE_INFO("[FootprintManagerI::getFootprintSet] invalid footprints of " << userId << ", remove cache");
				ServiceI::instance().removeObject(FOOTPRINT_SET, userId);
				return NULL;
			} else {
				// 如果选择了ignoreError, 则不删除cache, 只移除错误标志
				fptn->clear();
			}
		}
	}
	return fptn;
}

string FootprintManagerI::getTableName(int userId) {
	ostringstream result;
	result << _tableName << "_" << (userId % _tableCount);
	return result.str();
}


bool FootprintManagerI::validFootprint(const Ice::Int& f) {
  return true;
}


void FootprintManagerI::evictFootprint(FootprintSeq& footprints) {
	// we keep footprintDataSize footprints in memory and
	// keep n less than footprintDataSize in database
	// Remove outdated footprints from memory and database
	/*
        for (int i = footprints.size() - 1; i >= _footprintDataSize; --i) {
		FootprintPtr& f = footprints.at(i);
		if (f->id == 1) {
			MCE_DEBUG("remove one footprint in db " << f->host << " " << f->guest);
			remove(f);
			f->id = 0;
		}
	}*/
	while (footprints.size() > (size_t)_footprintCacheSize) {
		footprints.pop_back();
	}
}

void FootprintManagerI::visitHomepageOneway(const FootprintPtr& footprint, const Ice::Current& current) {
	ostringstream oss;
	oss << " host: " << footprint->host << " visited: " << footprint->tinyinfo->guest;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	visitHome(footprint, current.ctx);
}

FootprintInfoPtr FootprintManagerI::visitHomepage(const FootprintPtr& footprint, Ice::Int begin, Ice::Int limit,
		const Ice::Current& current) {
	ostringstream oss;
	oss << " host: " << footprint->host << " visited: " << footprint->tinyinfo->guest;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	// 踩脚印
	visitHome(footprint, current.ctx);

	// 取得当前脚印
	FootprintInfoPtr result = getAll(footprint->host, begin, limit, current);
	
	return result;
}

Ice::Int FootprintManagerI::getSize(const Ice::Int uid, const Ice::Current& current) {
	ostringstream oss;
	oss << " uid: " << uid;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	FootprintSetPtr p = getFootprintSet(uid);
	if ( p ) {
		int size = p->getSize();
		if ( size < 0 ) {
			return 0;
		} else {
			return size;
		}
	} else {
		return 0;
	}
}

FootprintInfoPtr FootprintManagerI::getAll(Ice::Int uid, Ice::Int begin, Ice::Int limit, const Ice::Current& current) {

	ostringstream oss;
	oss << " uid: " << uid;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	FootprintInfoPtr result = new FootprintInfo;
	ostringstream inMsg;
	inMsg << "FootprintManagerI::getAll [host=" << uid;
        if ( current.con ) {
		inMsg << " ip=" << current.con->toString() << "]";
	}
	TimeCost tc = TimeCost::create(inMsg.str());
	
	FootprintSetPtr p = getFootprintSet(uid);
	tc.step(" getFootprintSet");

	if ( p ) {
		result->fpts = p->getAll(begin, limit);
		tc.step(" getFootprintSet()->getAll");
	}
	if ( _visitUserFootprint ) {
		result->visitcount = getUserVisitCount(uid);
		tc.step(" getFootprintSet()->getUserVisitCount");
	} else {
		result->visitcount = 0;
	}

	if ( result->visitcount < 0 ) {
		result->visitcount = 0;
	}
	return result;
}

bool FootprintManagerI::add(const FootprintPtr& footprint, const string& ref, 
		bool incCount, bool ignorelimitstatus, bool isRetry, bool ignoreError) {
	// mark this footprint as new one for persist it in db
	FootprintSetPtr p = getFootprintSet(footprint->host, ignoreError);
	if ( !p ) {
		/* if ( ! isRetry ) {
			// 如果是第一次添加且出错, 则加入一个Task进行一组重试操作
			TaskManager::instance().execute(new AddFootprintRetryTask(footprint, ref, incCount, ignorelimitstatus));
		} */
		return false;
	}
	try {
		bool validFootprint = p->add(footprint, ignorelimitstatus);
		// 决策&写入DB
		if ( !_isSlave ) {
			TaskManager::instance().execute(new AddTask(footprint, *this));
		}

		if (! (validFootprint && _visitUserFootprint) ) {
			return true;
		}

		// 是否向数据库中增加计数
		if ( incCount ) {
			// MCE_DEBUG("Increase Visit Count for " << footprint->host);
			if (!_isSlave ) {
				try {
					mop::hi::svc::adapter::BatchUpdaterAdapter::instance().incUserViewCount(footprint->host);
				} catch (Ice::Exception& e) {
					MCE_WARN("[FootprintManagerI::add]BatchUpdaterAdapter Ice::Exception :" << e.what());
				} catch (...) {
					MCE_WARN("[FootprintManagerI::add]BatchUpdaterAdapter UNKNOWN Exception");
				}			
			}
			// 改写内存中的人气值
			incUserVisitCount(footprint->host);
		}
		// 加入远程统计log 	--  2009.4.13 by zhanghan
		ostringstream object;
		object << footprint->tinyinfo->guest;
		ActiveTrack::instance().action(footprint->host, ref, "ADD_FOOTPRINT", object.str());	
	} catch (Ice::Exception& e) {
		MCE_WARN("FootprintManagerI::add exception." << e);
	} catch (...) {
		MCE_WARN("Some exception. In FootprintManagerI::add");
	}

	return true;
}

void FootprintManagerI::visitHome(const FootprintPtr& footprint,const map<string, string>& params) {
	
	// 获取来源
	string ref = "UNKNOWN";
	map<string, string>::const_iterator itRef = params.find("REF");
	if ( itRef != params.end() ) {
		ref = (*itRef).second;
	}	
	// 判断是否加人气
	bool incCount = true;
	map<string, string>::const_iterator itNocount = params.find("NOCOUNT");
	if ( itNocount != params.end() ) {
		if ( (*itNocount).second.find("YES") != string::npos ) {
			incCount = false;
		}				
	}
	// 是否无视人气上限
	bool ignorelimitstatus = true;
        map<string, string>::const_iterator iterator=params.find("ignorelimitstatus");
	if (iterator == params.end() || iterator->second=="no") {
		ignorelimitstatus = false;
	}

	// 添加脚印
	footprint->tinyinfo->timestamp = time(NULL);
	add(footprint, ref, incCount, ignorelimitstatus);

}

void FootprintManagerI::addToSaveBucket(const FootprintPtr& footprint) {

	IceUtil::Mutex::Lock lock(_mutexAdd);

	string tableName = getTableName(footprint->host);
	FptnBucket& qt = _saveBucketAdd[tableName];

	// 当新发生修改的host出现的时候, 改为用已有cache初始化, 从而解决在写入数据库时旧脚印丢失的bug --090820 by zhanghan
        if (qt.fptnMap.empty()){
	  	qt.time = time(NULL);
	}
	if (qt.fptnMap.find(footprint->host) == qt.fptnMap.end() ) {
		qt.fptnMap.insert(pair<int, FootprintSeq>(footprint->host, getAll(footprint->host, 0, GUESTS_SIZE)->fpts)); 
		reverse(qt.fptnMap[footprint->host].begin(), qt.fptnMap[footprint->host].end());
	}
	FootprintSeq& fptQueue = qt.fptnMap[footprint->host];
	FootprintSeq::iterator pos = find_if(fptQueue.begin(), fptQueue.end(), FootprintEqualComparator(footprint->tinyinfo));
	if (pos != fptQueue.end()) {
	 	fptQueue.erase(pos);
	}	
	fptQueue.push_back(footprint->tinyinfo);

	time_t now = time(NULL);	
	if ( (qt.fptnMap.size() < _saveBatchSize) && (abs(now - qt.time) < _refreshDbInterval) ) {
		return;
	}

	// 写入db
	qt.time = now;
	TaskManager::instance().execute(new SaveTask(tableName, qt.fptnMap));
}

void FootprintManagerI::remove(const FootprintPtr& footprint) {

	//delete form FootprintSeq first	
	FootprintSeq& footprints = getFootprintSet(footprint->host)->getFootprints();
	FootprintSeq::iterator pos = find_if(footprints.begin(), footprints.end(), FootprintEqualComparator(footprint->tinyinfo));
	
	footprints.erase(pos);

	if (_isSlave) {
		return;
	}

	IceUtil::Mutex::Lock lock(_mutexRemove);
	string tableName = getTableName(footprint->host);
	FptnBucket& qt = _saveBucketRemove[tableName];
	
	time_t now = time(NULL);
	if (qt.fptnMap.size() < _saveBatchSize && abs(now - qt.time) < _refreshDbInterval) {
		return;
	}

	for(size_t i = 0; i < footprints.size(); i++){
		if(i >= 6 ) break;
		qt.fptnMap[footprint->host].push_back(footprints[i]);
	}
	TaskManager::instance().execute(new SaveTask(tableName, qt.fptnMap));
}

UserVisitCountPtr FootprintManagerI::locateUserVisitCount(int userId) {

	UserVisitCountPtr countPtr = ServiceI::instance().locateObject<UserVisitCountPtr> (USER_VISIT_COUNT, userId);
	if ( countPtr ) {
		// 如果locate有误,则在ObjectCache里将此次locate的cache删掉,下次重新读DB
		// 防止内存和DB的人气值不一致导致的清0 bug -- by zhanghan 091123
		// MCE_DEBUG("[FootprintManagerI::locateUserVisitCount] visitcout of " << userId << " : " << countPtr->getCount());

		if ( countPtr->getCount() < 0 ) {
			MCE_INFO("[FootprintManager] invalid VisitCount of " << userId << ", remove cache");
			ServiceI::instance().getObjectCache()->removeObject(USER_VISIT_COUNT, userId);
			return NULL;
		}
	}
	return countPtr;
}

UserVisitCountPtr FootprintManagerI::findUserVisitCount(int userId) {
	return ServiceI::instance().findObject<UserVisitCountPtr> (
			USER_VISIT_COUNT, userId);
}

Ice::Int FootprintManagerI::getUserVisitCount(Ice::Int userId, const Ice::Current& current) {
	ostringstream oss;
	oss << " uid: " << userId;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	if ( !_visitUserFootprint ) {
		MCE_WARN("[FootprintManagerI::setUserVisitCount]" << _serverName << " shouldn't use this function");
		return 0;
	}
	UserVisitCountPtr p = locateUserVisitCount(userId);
	try {
		if ( p ) {
			int count = p->getCount();
			if ( count < 0 ) {
				return 0;
			} else {
				MCE_DEBUG("[FootprintManagerI::getUserVisitCount] get visitcount of " << userId << " as " << count);
				return count;
			}
		} else {
			return 0;
		}
	
	} catch (...) {
		MCE_WARN("[FootprintManagerI::getUserVisitCount] getCount Exception ");
	}
	return 0;
}

void FootprintManagerI::incUserVisitCount(Ice::Int userId, const Ice::Current& current) {
	ostringstream oss;
	oss << " uid: " << userId;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserVisitCountPtr p = findUserVisitCount(userId);
	if ( p ) {
		if ( p >= 0 ) {
			p->incCount(1);
		}
	} else {
		// 如果内存中没有则不操作。因为之前已有数据库操作，保证DB正确
		;
	}
}

void FootprintManagerI::incUserVisitCountBatch(const MyUtil::IntSeq& userIds,
		Ice::Int count, const Ice::Current& current) {
	ostringstream oss;
	oss << " userIds.size: " << userIds.size();
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	for (size_t i = 0; i < userIds.size(); ++i) {
		UserVisitCountPtr p = findUserVisitCount(userIds[i]);
		if ( p ) {
			p->incCount(count);
		} else {
			// 如果内存中没有则不操作。因为之前已有数据库操作，保证DB正确
		}
	}
}


void FootprintManagerI::setUserVisitCount(Ice::Int userId, Ice::Int count,
		const Ice::Current& current) {
	ostringstream oss;
	oss << " uid: " << userId << " count: " << count;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	if (count < 0) {
		return;
	}
	if ( !_visitUserFootprint ) {
		MCE_WARN("[FootprintManagerI::setUserVisitCount]" << _serverName << " shouldn't use this function");
		return;
	}

	Str2StrMap filter;
	filter[COLUMN_ID] = boost::lexical_cast<string>(userId);
	Str2StrMap properties;
	properties[COLUMN_VIEWCOUNT] = boost::lexical_cast<string>(count);
	
	// 因为在调用这个方法前并未单独写数据库，所以在这里要写入db
	if ( ! _isSlave ) {
		QueryRunner("user_view_count", CDbWServer).execute(Statement::buildUpdate(
				TABLE_VIEW_COUNT, filter, properties));
	}
	
	UserVisitCountPtr p = findUserVisitCount(userId);
	if ( p ) {
		p->setCount(count);
	}
}
// --------- AddTask ------------------
// AddTask::AddTask(const FootprintPtr& footprint, const string& tableName, FptnBucket& qt):
AddTask::AddTask(const FootprintPtr& footprint, FootprintManagerI& manager):
			Task(TASK_LEVEL_ADD),
			_footprint(footprint),
			_manager(manager) {
}

void AddTask::handle() {
	_manager.addToSaveBucket(_footprint);
}

// --------- SaveTask ----------------
SaveTask::SaveTask(string table, FootprintMap& fptnMap) :
	Task(TASK_LEVEL_SAVE), _table(table) {
	fptnMap.swap(_fptnMap);
}

void SaveTask::handle() {
	if (_fptnMap.empty()) {
		return;
	}
	Statement sql;
	MCE_INFO("AddTask::handle table:" << _table);	
	// sql << "REPLACE INTO " << _table << " (host, guests) values";
	sql << "INSERT INTO " << _table << " (host, guests) values";
	for (FootprintMap::iterator it = _fptnMap.begin(); it != _fptnMap.end(); ++ it){

		if ( it != _fptnMap.begin() ) {
			sql << ", ";
		}

		// 因为越新的脚印越后加入, 所以需要对这个vector进行翻转 -- 090820 by zhanghan
		FootprintSeq ftQueue = (*it).second;
		reverse(ftQueue.begin(), ftQueue.end());
		sql << "(" << (*it).first << ", "  << mysqlpp::quote << IntSeqSerialize::serialize(ftQueue) << ")";
	}

	sql << " ON DUPLICATE KEY UPDATE " << "guests = values(guests)"; 

	QueryRunner("footprintn", CDbWServer, _table).execute(sql);
}

// --------------- AddFootprintRetryTask -------------------------
/* AddFootprintRetryTask::AddFootprintRetryTask(const FootprintPtr& footprint, 
		const string& ref, bool incCount, bool ignorelimitstatus):
	footprint_(footprint),
	ref_(ref),
	incCount_(incCount),
	ignorelimitstatus_(ignorelimitstatus) {
}

void AddFootprintRetryTask::handle() {
	// 第1次重试, 不ignore从DB加载的异常
	MCE_INFO("[AddFootprintRetryTask::handle] retry add " << footprint_->tinyinfo->guest << "->" << footprint_->host << "1st times");
	if ( FootprintManagerI::instance().add(footprint_, ref_, incCount_, ignorelimitstatus_, true, false) ) {
		MCE_INFO("[AddFootprintRetryTask::handle] retry add " << footprint_->tinyinfo->guest << "->" << footprint_->host << "1st times SUCCESS")
	} else {
		// 第2次重试，ignore之		
		MCE_INFO("[AddFootprintRetryTask::handle] retry add " << footprint_->tinyinfo->guest << "->" << footprint_->host << "1st times failed, try 2nd");
		if ( FootprintManagerI::instance().add(footprint_, ref_, incCount_, ignorelimitstatus_, true, true) ) {
			MCE_INFO("[AddFootprintRetryTask::handle] retry add " << footprint_->tinyinfo->guest << "->" << footprint_->host << "2nd times SUCCESS")
		} else {
			 MCE_INFO("[AddFootprintRetryTask::handle] retry add " << footprint_->tinyinfo->guest << "->" << footprint_->host << "2nd times FAILED")
		}
	}
} */

//------------------------FootprintSet-----------------------------------------
Ice::Int FootprintSet::getSize() {
	IceUtil::Mutex::Lock lock(*this);
	return footprints_.size();
}

FootprintSeq FootprintSet::getAll(Ice::Int begin, Ice::Int limit) {
	IceUtil::Mutex::Lock lock(*this);
	FootprintSeq result;
	if ( isError() ) {
		return result;
	}
	begin = begin < 0 ? 0 : begin;
	limit = limit < 0 ? footprints_.size() : limit;
	for (size_t i = begin; i < footprints_.size(); ++i) {
		if (result.size() >= (size_t)limit)
			break;
		/* if ( footprints_.at(i).timestamp < MIN_LIMIT ) {
			footprints_.at(i).timestamp *= 60;
		} */
		result.push_back(footprints_.at(i));
	}
	return result;
}

bool FootprintSet::add(const FootprintPtr& footprint, bool ignorelimitstatus) {
	MCE_INFO("FootprintSet::add : " << footprint->tinyinfo->guest << "->" << footprint->host );
	
	IceUtil::Mutex::Lock lock(*this);
	FootprintManagerI& manager = FootprintManagerI::instance();
	bool validFootprint = true;	

	// 如果本来就读取有错, 则不处理  -- by zhanghan 09.11.12
	if ( isError() ) {
		return false;
	}
	FootprintSeq::iterator pos = find_if(footprints_.begin(), footprints_.end(), FootprintEqualComparator(footprint->tinyinfo));
	if (pos != footprints_.end()) {
	  	// Ice::Int f = *pos;
		FootprintTinyPtr f = *pos;

	  	if (pos - footprints_.begin() >= 6 && manager.validFootprint(f)) {
	    		validFootprint = true;
	  	} else {
	    		validFootprint = false;
	  	}
	  	footprints_.erase(pos);
	}
	footprints_.insert(footprints_.begin(), footprint->tinyinfo);

	// we keep footprintDataSize footprints_ in memory and
	// keep n less than footprintDataSize in database
	// Remove outdated footprints_ from memory and database
	manager.evictFootprint(footprints_);

	// 只有在validFootprint为true，即加人气的时候，才进行以下判断 -- 09.09.22 by zhanghan
	if ( !validFootprint ) {
		return validFootprint;
	}

        int limit_test_result;
	if ( ! ignorelimitstatus) {
		limit_test_result = isReachViewCountLimit(footprint->host);
	}else{
		MCE_INFO("ignorelimitstatus, host: " << footprint->host);
		viewCountLimtCount_++;
		limit_test_result = PASS;
	}
        switch (limit_test_result) {
	case REACHLIMIT :
                MCE_INFO("isReachViewCountLimit: " << footprint->tinyinfo->guest << "->" << footprint->host << " last viewCountLimitTime : " << viewCountLimitTime_); 
               	validFootprint = false;
                break;
        case PASS_RESET :
                validFootprint = true;
                break;
        case PASS :
                validFootprint = true;
                break;
        }
	
	return validFootprint;
}

void FootprintSet::merge(const FootprintSetPtr& ftprtst) {
	IceUtil::Mutex::Lock lock(*this);
	for(FootprintSeq::const_iterator it=ftprtst->footprints_.begin();
		it != ftprtst->footprints_.end(); ++it){
		FootprintSeq::iterator pos = find_if(footprints_.begin(), footprints_.end(),
				FootprintEqualComparator(*it));
		if (pos == footprints_.end()) {
			// MCE_DEBUG("Merge: not exist " << (*it) );
			footprints_.push_back(*it);
		}
	}
}

int FootprintSet::isReachViewCountLimit(int hostId){
        if ( viewCountLimtCount_++ < VIEW_COUNT_LIMIT ) {
                return PASS;
        }
        if ( time(NULL) - viewCountLimitTime_ > (24*60*60) ) {
		int viewCountLimitTimeLast = viewCountLimitTime_;
                viewCountLimitTime_ = (time(NULL)/(24*60*60)) * 24*60*60;
                viewCountLimtCount_ = 0;
                MCE_INFO("time reset for: " << hostId << " new viewCountLimitTime " << viewCountLimitTime_ << " old viewCountLimitTime " << viewCountLimitTimeLast);
                return PASS_RESET;
        }
        return REACHLIMIT;
}

void FootprintSet::setErrorFlag() {
	// IceUtil::Mutex::Lock lock(*this);
	FootprintTinyPtr tiny = new FootprintTiny;
	tiny->guest = -1;
	tiny->timestamp = 0;
	footprints_.clear();
	footprints_.push_back(tiny);
}

bool FootprintSet::isError() {
	// IceUtil::Mutex::Lock lock(*this);
	if ( footprints_.size() == 1 ) {
		if ( footprints_[0]->guest < 0 ) {
			return true;
		}
	}
	return false;
}

void FootprintSet::clear() {
	IceUtil::Mutex::Lock lock(*this);
	footprints_.clear();
}


// ------------ UserVisitCountFactoryI -------------------
Ice::ObjectPtr UserVisitCountFactoryI::create(Ice::Int id) {
	MCE_INFO("Loading VisitCount " << id);
	Statement sql;
	sql << "SELECT " << COLUMN_VIEWCOUNT << " FROM " << TABLE_VIEW_COUNT << " WHERE id = '" << id << "'";
	UserVisitCountPtr result = new UserVisitCount;
	result->setCount(0);
	try {
		UserVisitCountResultHandlerI handler(result);
		size_t row_count = QueryRunner("user_view_count", CDbRServer).query(sql,
				handler);

		MCE_INFO("Loading VisitCount " << id << " done." << row_count);
		if (row_count == 1) {
			MCE_INFO("Loading VisitCount " << id << " done.");
		} else if ( row_count <= 0 ) {
			Str2StrMap filter;
			filter[COLUMN_ID] = boost::lexical_cast<string>(id);
			QueryRunner("user_view_count", CDbWServer).schedule(
					Statement::buildInsert(TABLE_VIEW_COUNT, filter));
		} else if ( row_count > 1 ) {
			// 如果出现大于1条的错误情况,则将所有一并删除,重新插入作为修正 -- by zhanghan 091123
			Str2StrMap filter;
			filter[COLUMN_ID] = boost::lexical_cast<string>(id);
			QueryRunner("user_view_count", CDbWServer).execute(
					Statement::buildDelete(TABLE_VIEW_COUNT, filter));
			Str2StrMap filterInsert;
			filterInsert[COLUMN_ID] = boost::lexical_cast<string>(id);
			filterInsert[COLUMN_VIEWCOUNT] = boost::lexical_cast<string>(result->getCount());
			QueryRunner("user_view_count",  CDbWServer).schedule(
					Statement::buildInsert(TABLE_VIEW_COUNT, filter));
		}
	} catch (mysqlpp::Exception& e) {
		// 出现错误时, 以count为-1作为标志, 方便外面处理 -- by zhanghan 091123
		result->setCount(-1);	
		MCE_WARN("[UserVisitCountFactoryI::create] load VisitCount of " << id << " mysqlpp::Exception " << e.what());
	} catch (...) {
		result->setCount(-1);
		MCE_WARN("[UserVisitCountFactoryI::create] load VisitCount of " << id << " exception");
	}
	MCE_INFO("Loading with initialize VisitCount " << id << " as " << result->getCount());

	return result;
}

// ----------- UserVisitCountResultHandlerI ----------
UserVisitCountResultHandlerI::UserVisitCountResultHandlerI(
		UserVisitCountPtr& result) :
	_result(result) {
}

bool UserVisitCountResultHandlerI::handle(mysqlpp::Row& row) const {
	int count = (int) row[COLUMN_VIEWCOUNT.c_str()];
	_result->setCount(count);
	return true;
}

// ---------- UserVisitCount --------------------
Ice::Int UserVisitCount::getCount() {
	IceUtil::Mutex::Lock lock(*this);
	return _count;
}

void UserVisitCount::setCount(int count) {
	IceUtil::Mutex::Lock lock(*this);
	_count = count;	
}

void UserVisitCount::incCount(int step) {
	IceUtil::Mutex::Lock lock(*this);
	_count += step;
}
