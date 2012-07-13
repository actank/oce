#include "FootprintI.h"
#include "util/cpp/String.h"
#include "BatchUpdaterAdapter.h"
#include "QueryRunner.h"
#include "ActiveTrackAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

#include "util/cpp/InvokeMonitor.h"
#include <Monitor/client/service/ServiceMonitor.h>

using namespace MyUtil;
using namespace mop::hi::svc::fpt;
using namespace mop::hi::svc::adapter;
using namespace com::xiaonei::xce;
using namespace ::xce::log;
using namespace xce::serverstate;

//---------------------------------------------------------------------------
void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.registerObjectCache(FOOTPRINT_SET, "FS", new SyncServantFactory(new FootprintFactoryI));
	service.getAdapter()->add(&FootprintManagerI::instance(), service.createIdentity("FM", ""));

	TaskManager::instance().config(TASK_LEVEL_BUILD, ThreadPoolConfig(0, 30));
	//ActiveTrack::instance().initialize();

        int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("BlogFootprint.Mod");
        int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BlogFootprint.Interval", 5);

        string ctrEndpoints = service.getCommunicator()->getProperties()
                ->getPropertyWithDefault("BlogFootprint.ControllerEndpoint", "ControllerBlogFootprint");

        ServerStateSubscriber::instance().initialize(ctrEndpoints,
                       &FootprintManagerI::instance(), mod, interval);
	//统计log
	ServiceMonitorManager::instance().start();
}

//------------------------FootprintFactoryI---------------------------------------
Ice::ObjectPtr FootprintFactoryI::create(Ice::Long id) {
	TaskManager::instance().execute(new FootprintBuildTask(id));
	return new FootprintSetI;
}

void
FootprintBuildTask::handle(){
	FootprintManagerI& manager = FootprintManagerI::instance();
	string table = manager.getTableName(_id);
	Statement sql;
	sql << "SELECT * FROM " << table << " WHERE " << COLUMN_HOST << "=" << _id;
	/* no limit here. Outdated item will be deleted later*/

	FootprintSetPtr obj = new FootprintSetI;
	QueryRunner(DB_NAME, CDbRServer, table).query(sql, FootprintSetHandler(obj));
	sort(obj->footprints.begin(), obj->footprints.end(), FootprintGreaterComparator());

//	ServiceI::instance().addObject(obj, FOOTPRINT_SET, (long)_id);
//	sleep(5);
	FootprintSetPtr initObj = ServiceI::instance().findObject<FootprintSetPtr>(FOOTPRINT_SET, (long)_id);
	if(!initObj){
		ServiceI::instance().addObject(obj, FOOTPRINT_SET, (long)_id);
		MCE_INFO("Loaded " << obj->footprints.size() << " for " << _id);
	}else{
		initObj->merge(obj);
		MCE_INFO("Refresh " << obj->footprints.size() << " for " << _id);
	}
}
/*
void
FootprintRefreshTask::handle(){
}
*/
bool FootprintSetHandler::handle(mysqlpp::Row& row) const {
	FootprintPtr footprint = new Footprint;
	// footprint->id is not the original meanings, now it is used to mark
	// persistenting status in db, 1 for persistent, 0 for not
	footprint->id = 1;//boost::lexical_cast<Ice::Long>(ent->getProperty(COLUMN_ID));
	footprint->host = (long)row[COLUMN_HOST]; // Is this a waste?
	footprint->guest = (int)row[COLUMN_GUEST];
#ifndef NEWARCH
	footprint->guestName = row[COLUMN_GUEST_NAME].get_string();
	footprint->guestHead = row[COLUMN_GUEST_HEAD].get_string();
	footprint->time = Date(row[COLUMN_TIME].get_string()).time();
#else
	footprint->guestName = row[COLUMN_GUEST_NAME].c_str();
	footprint->guestHead = row[COLUMN_GUEST_HEAD].c_str();
	footprint->time = Date(row[COLUMN_TIME].c_str()).time();
#endif
	footprint->time *= 1000;

	_fsp->footprints.push_back(footprint);
	return true;
}

//---------------------------------------------------------------------------
FootprintManagerI::FootprintManagerI() {
	ServiceI& service = ServiceI::instance();
	Ice::PropertiesPtr props = service.getCommunicator()->getProperties();

	_footprintCacheSize = props->getPropertyAsIntWithDefault(
			"Service." + service.getName() + ".FootprintSet.CacheSize", 20);
	_footprintDataSize = props->getPropertyAsIntWithDefault("Service." + service.getName() + ".FootprintSet.DataSize",
			6);

	_validFootprintInterval = props->getPropertyAsIntWithDefault("Service." + service.getName()
			+ ".FootprintSet.ValidFootprintInterval", 20);
	_footprintQueueSizeMax = props->getPropertyAsIntWithDefault("Service." + service.getName()
			+ ".FootprintSet.QueueSize", 200);
	_footprintQueueInterval = props->getPropertyAsIntWithDefault("Service." + service.getName()
			+ ".FootprintSet.QueueInterval", 30);
	_tableCount = props->getPropertyAsIntWithDefault("Service." + service.getName() + ".TableCount", 100);
	_tableName = props->getPropertyWithDefault("Service." + service.getName() + ".TableName", "footprint");
	_visitUserFootprint = service.getName().compare(0, strlen("Footprint"), "Footprint") == 0;
}

FootprintManagerI::~FootprintManagerI() {

}

FootprintSetPtr FootprintManagerI::getFootprintSet(Ice::Long userId) {
	return ServiceI::instance().locateObject<FootprintSetPtr>(FOOTPRINT_SET, userId);
}

string FootprintManagerI::getTableName(long userId) {
	ostringstream result;
	result << TABLE_NAME << "_" << (userId % TABLE_COUNT);
	return result.str();
}

bool FootprintManagerI::validFootprint(const FootprintPtr& f) {
	return abs(time(NULL) - f->time/1000) > _validFootprintInterval *60;
}

void FootprintManagerI::evictFootprint(FootprintSeq& footprints) {
	// we keep footprintDataSize footprints in memory and
	// keep n less than footprintDataSize in database
	// Remove outdated footprints from memory and database
	for (int i = footprints.size() - 1; i >= _footprintDataSize; --i) {
		FootprintPtr& f = footprints.at(i);
		if (f->id == 1) {
			MCE_DEBUG("remove one footprint in db " << f->host << " " << f->guest);
			remove(f);
			f->id = 0;
		}
	}
	while (footprints.size() > (size_t)_footprintCacheSize) {
		footprints.pop_back();
	}
}

void FootprintManagerI::visitHomepage(const FootprintPtr& footprint, const Ice::Current& current) {
	ostringstream oss;
	oss << " host: " << footprint->host << " visitor: " << footprint->guest;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	add(footprint, current);
}

Ice::Int FootprintManagerI::getSize(const Ice::Long uid, const Ice::Current& current) {
	ostringstream oss;
	oss << " uid: " << uid;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	return getFootprintSet(uid)->getSize();
}

FootprintSeq FootprintManagerI::getAll(Ice::Long uid, Ice::Int begin, Ice::Int limit, const Ice::Current& current) {
	ostringstream oss;
	oss << " uid: " << uid << " begin: " << begin << " limit: " << limit;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	return getFootprintSet(uid)->getAll(begin, limit);
}

void FootprintManagerI::add(const FootprintPtr& footprint, const Ice::Current& current) {
	ostringstream oss;
	oss << " host: " << footprint->host << " visitor: " << footprint->guest;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	// mark this footprint as new one for persist it in db
	footprint->id = 1;
	try {
		if (getFootprintSet(footprint->host)->add(footprint, current) && _visitUserFootprint) {
			MCE_DEBUG("Increase Visit Count for " << footprint->host);
			BatchUpdaterAdapter::instance().incUserViewCount(footprint->host);
			// UserAdapter::instance().incUserVisitCount(footprint->host);
			// 加入远程统计log 	--  2009.4.13 by zhanghan
			ostringstream object;
			object << footprint->guest;
			string ref = "UNKNOWN";
			map<string, string>::const_iterator itRef = current.ctx.find("REF");
			if ( itRef != current.ctx.end() ) {
				ref = (*itRef).second;
			}
			//ActiveTrack::instance().action(footprint->host, ref, "ADD_FOOTPRINT", object.str());
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("FootprintManagerI::add exception." << e);
	} catch (...) {
		MCE_WARN("Some exception. In FootprintManagerI::add");
	}

	IceUtil::Mutex::Lock lock(_mutexAdd);
	string tableName = getTableName(footprint->host);
	QueueTask& qt = _queueTaskAdd[tableName];
	qt.queue.push_back(footprint);
	time_t now = time(NULL);
	if (qt.queue.size() < (size_t)_footprintQueueSizeMax && abs(now - qt.time) < _footprintQueueInterval) {
		return;
	}
	qt.time = now;

	TaskManager::instance().execute(new AddTask(tableName, qt.queue));

}

void FootprintManagerI::remove(const FootprintPtr& footprint) {
	IceUtil::Mutex::Lock lock(_mutexRemove);
	string tableName = getTableName(footprint->host);
	QueueTask& qt = _queueTaskRemove[tableName];
	qt.queue.push_back(footprint);
	time_t now = time(NULL);
	if (qt.queue.size() < (size_t)_footprintQueueSizeMax && abs(now - qt.time) < _footprintQueueInterval) {
		return;
	}
	qt.time = now;

	TaskManager::instance().execute(new RemoveTask(tableName, qt.queue));
}

//-----------------------------------------------new table--------------------
AddTask::AddTask(string table, FootprintSeq& queue) :
	Task(TASK_LEVEL_ADD), _table(table) {
	queue.swap(_queue);
}

void AddTask::handle() {
	if (_queue.empty()) {
		return;
	}
	Statement sql;
	sql << "INSERT IGNORE INTO " << _table << " (host, guest, guest_name, guest_head, time) values";

	for (size_t i = 0; i < _queue.size(); ++i) {
		FootprintPtr& footprint = _queue.at(i);
		sql << "('" << footprint->host << "', '" << footprint->guest << "', '"
				<< StrUtil::fixString(footprint->guestName) << "', '" << StrUtil::fixString(footprint->guestHead)
				<< "', '" << Date(footprint->time/1000).str() << "')";
		if (i < _queue.size() - 1) {
			sql << ", ";
		}
	}
	QueryRunner(DB_NAME, CDbWServer, _table).execute(sql);
}

RemoveTask::RemoveTask(string table, FootprintSeq& queue) :
	Task(TASK_LEVEL_REMOVE), _table(table) {
	queue.swap(_queue);
}

void RemoveTask::handle() {
	MCE_DEBUG("DoRemoveTask Queue Size: " << _queue.size());
	if (_queue.empty()) {
		return;
	}
	Statement sql;
	sql << "DELETE FROM " << _table << " WHERE ";

	for (size_t i = 0; i < _queue.size(); ++i) {
		FootprintPtr& footprint = _queue.at(i);
		sql << "(host="<<footprint->host<<" AND guest=" << footprint->guest << ")";
		if (i < _queue.size() - 1) {
			sql << " OR ";
		}
	}
	QueryRunner(DB_NAME, CDbWServer, _table).execute(sql);
}

//------------------------FootprintSetI-----------------------------------------
Ice::Int FootprintSetI::getSize(const Ice::Current&) {
	IceUtil::Mutex::Lock lock(*this);
	return footprints.size();
}

FootprintSeq FootprintSetI::getAll(Ice::Int begin, Ice::Int limit, const Ice::Current& current) {
	IceUtil::Mutex::Lock lock(*this);
	begin = begin < 0 ? 0 : begin;
	limit = limit < 0 ? footprints.size() : limit;
	FootprintSeq result;
	for (size_t i = begin; i < footprints.size(); ++i) {
		if (result.size() >= (size_t)limit)
			break;
		result.push_back(footprints.at(i));
	}
	return result;
}

bool FootprintSetI::add(const FootprintPtr& footprint, const Ice::Current& current) {
	MCE_INFO("Visit: " << footprint->guest << "->" << footprint->host << " at " << Date(footprint->time/1000).str());
	//bool newFootprint = true;
	bool validFootprint = true;
	FootprintManagerI& manager = FootprintManagerI::instance();
	IceUtil::Mutex::Lock lock(*this);
	FootprintSeq::iterator pos = find_if(footprints.begin(), footprints.end(), FootprintEqualComparator(footprint->host,
			footprint->guest));
	if (pos != footprints.end()) {
		FootprintPtr f = *pos;
		MCE_DEBUG("Search: exist at " << Date(f->time/1000).str());
		//service.getFootprintManager()->remove(f);
		if (pos - footprints.begin() > 6 && manager.validFootprint(f)) {
			validFootprint = true;
		} else {
			validFootprint = false;
		}
		footprints.erase(pos);
	}
	// add the footprint to the front
	footprints.insert(footprints.begin(), footprint);

	// we keep footprintDataSize footprints in memory and
	// keep n less than footprintDataSize in database
	// Remove outdated footprints from memory and database
	manager.evictFootprint(footprints);

	const Ice::Int REACHLIMIT = 0;
	const Ice::Int PASS = 1;
	const Ice::Int PASS_RESET = 2;
	int limit_test_result;

        map<string, string>::const_iterator iterator=current.ctx.find("ignorelimitstatus");

                if (iterator == current.ctx.end() || iterator->second=="no") {
                        limit_test_result = isReachViewCountLimit();
                }else{
			MCE_INFO ("ignorelimitstatus");
                        _view_count_limit_count++;
                        limit_test_result = PASS;
                }

	switch (limit_test_result) {
	case REACHLIMIT :
		MCE_INFO("isReachViewCountLimit: " << footprint->guest << "->" << footprint->host );
		validFootprint = false;
		break;
	case PASS_RESET :
		MCE_INFO("time reset for: " << footprint->host );
		validFootprint = true;
		break;
	case PASS :
		validFootprint = true;
		break;
	}
	return validFootprint;
}

void FootprintSetI::merge(const FootprintSetPtr& ftprtst, const Ice::Current& current) {
	IceUtil::Mutex::Lock lock(*this);
	for(FootprintSeq::const_iterator it=ftprtst->footprints.begin();
		it != ftprtst->footprints.end(); ++it){
		FootprintSeq::iterator pos = find_if(footprints.begin(), footprints.end(),
				FootprintEqualComparator((*it)->host,(*it)->guest));
		if (pos == footprints.end()) {
			MCE_DEBUG("Merge: not exist " << (*it)->guest );
			footprints.push_back(*it);
		}
	}
}

int FootprintSetI::isReachViewCountLimit(){
	if ( _view_count_limit_count++ < VIEW_COUNT_LIMIT ) {
		return PASS;
	}
	if ( time(NULL) - _view_count_limit_time > 24 * 60 * 60 ) {
		_view_count_limit_time = time(NULL);
		_view_count_limit_count = 0;
		return PASS_RESET;
	}
	return REACHLIMIT;
}

