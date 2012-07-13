#include "FriendDistI.h"
#include "QueryRunner.h"

using namespace mop::hi::svc::model;
using namespace MyUtil;
using namespace com::xiaonei::xce;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.registerObjectCache(FRIEND_DIST, "FD", new SyncServantFactory(new FriendDistFactoryI));
	service.getAdapter()->add(new FriendDistManagerI, service.createIdentity(
			FRIEND_DIST_MANAGER, ""));
	service.registerXceObserver(new FriendDistServiceSchemaI);
}

//***************************************************************************

Ice::ObjectPtr FriendDistFactoryI::create(Ice::Int id) {
	MCE_INFO("FriendDistFactoryI::create " << id);
	ostringstream sqlRebuild;
	sqlRebuild << " SELECT count(1) AS count FROM relation_" << (id % 100)
			<< " WHERE host=" << id;

	ostringstream sqlPatten;
	sqlPatten << "relation_" << (id % 100);

	int total = 0;
	QueryRunner("frienddist_relation", CDbRServer, sqlPatten.str()).query(
			Statement(sqlRebuild.str()), TotalCountHandler(total, "count"));

	FriendDistPtr result = new FriendDist(Univ2CountMap(), Count2UnivMap(),
			total);

	TaskManager::instance().execute(new FriendDistRebuildTask(id));
	MCE_INFO("FriendDistFactory::create id=" << id << " total=" << total);

	return result;
}

//***************************************************************************

FriendDistPtr FriendDistManagerI::findFriendDist(int userId) {
	return ServiceI::instance().locateObject<FriendDistPtr> (FRIEND_DIST,
			userId);
}

Int2IntMap FriendDistManagerI::getFriendDist(::Ice::Int userId,
		const Ice::Current& current) {
	return findFriendDist(userId)->getFriendDist();
}

FriendDistWithTotalPtr FriendDistManagerI::getFriendDistWithTotal(
		::Ice::Int userId, const Ice::Current& current) {
	return findFriendDist(userId)->getFriendDistWithTotal();
}

Ice::Int FriendDistManagerI::getFriendDistByUniv(::Ice::Int userId,
		::Ice::Int univId, const Ice::Current& current) {
	return findFriendDist(userId)->getFriendDistByUniv(univId);
}

void FriendDistManagerI::rebuildFriendDist(::Ice::Int userId,
		const Ice::Current& current) {
	TaskPtr task = new FriendDistRebuildTask(userId);
	if(current.ctx.count("LEVEL")){
		task->level(TASK_LEVEL_REBUILD_DIFF);
	}
	TaskManager::instance().execute(task);
}

Ice::Int FriendDistManagerI::getTotalCount(::Ice::Int userId,
		const Ice::Current& current) {
	return findFriendDist(userId)->getTotalCount();
}

//***************************************************************************

FriendDist::FriendDist(Univ2CountMap map, Count2UnivMap reverseMap, int total) :
	_map(map) {
	_top = new FriendDistWithTotal();
	for (Count2UnivMap::reverse_iterator it = reverseMap.rbegin(); it
			!= reverseMap.rend(); ++it) {
		if (_top->networks.size() < TOPN) {
			_top->networks.push_back(new FriendCount(it->second, it->first));
		}
		_seq.push_back(new FriendCount(it->second, it->first));
	}
	_top->totalCount = total;
}

Int2IntMap FriendDist::getFriendDist() {
	IceUtil::Mutex::Lock lock(*this);
	return _map;
}

FriendDistWithTotalPtr FriendDist::getFriendDistWithTotal() {
	IceUtil::Mutex::Lock lock(*this);
	return _top;
}

Ice::Int FriendDist::getFriendDistByUniv(Ice::Int univId) {
	IceUtil::Mutex::Lock lock(*this);
	Univ2CountMap::iterator it = _map.find(univId);
	return it == _map.end() ? 0 : it->second;
}

Ice::Int FriendDist::getTotalCount() {
	IceUtil::Mutex::Lock lock(*this);
	return _top->totalCount;
}

FriendCountSeq FriendDist::getFriendDistSeq(int begin, int limit) {
	IceUtil::Mutex::Lock lock(*this);
	if (begin == 0 && limit == (int) TOPN) {
		return _top->networks;
	} else if (begin == 0 && limit == -1) {
		return _seq;
	} else {
		MCE_INFO("FriendDist::getFriendDistSeq begin=" << begin << " limit="
				<< limit);
		return _seq;
	}
}

//***************************************************************************

void FriendDistRebuildTask::handle() {
	MCE_INFO("FriendDistRebuildTask::handle() " << _id);
	ostringstream sqlRebuild;
	sqlRebuild << "SELECT b.status, b.univ FROM relation_" << (_id
			% 100) << " AS a, user_basic AS b WHERE a.guest=b.id AND a.host="
			<< _id;

	ostringstream sqlPatten;
	sqlPatten << "relation_" << (_id % 100);

	Univ2CountMap map;
	int total = 0;
	QueryRunner("frienddist_relation", CDbRServer, sqlPatten.str()).query(
			Statement(sqlRebuild.str()), FriendDistIHandler(map, total, "univ",
					"status"));

	Count2UnivMap reverseMap;
	for (Univ2CountMap::iterator it = map.begin(); it != map.end(); ++it) {
		reverseMap.insert(make_pair(it->second, it->first));
	}

	FriendDistPtr result = new FriendDist(map, reverseMap, total);

	ServiceI::instance().addObject(result, FRIEND_DIST, _id);

	// 将用户好友数同步到数据库
	ostringstream sqlSync;
	sqlSync << "INSERT INTO user_friendcount (id, friendcount) VALUES(" << _id << "," << total << ")"
		<< "ON DUPLICATE KEY UPDATE friendcount = " << total;
	try {	
		QueryRunner("user_friendcount", CDbWServer).schedule(Statement(sqlSync.str()));
	} catch (...) {
		MCE_WARN("Update user_friendcount Exception " << _id);
	}
}

FriendDistRebuildTask::FriendDistRebuildTask(int id) :
	MyUtil::Task(TASK_LEVEL_REBUILD), _id(id) {
	MCE_INFO("FriendDistRebuildTask::FriendDistRebuildTask("<<id<<")");
}

//***************************************************************************

FriendDistIHandler::FriendDistIHandler(Univ2CountMap& map, int& total,
		const std::string& univField, const std::string& statusField) :
	_map(map), _total(total), _univField(univField), _statusField(statusField) {
}

bool FriendDistIHandler::handle(mysqlpp::Row& row) const {
	int status = (int) row[_statusField.c_str()];
	int univ = (int) row[_univField.c_str()];
	if (status != 0) {
		return true;
	}
	_total++;
	if (univ <= 0) {
		return true;
	};

	Univ2CountMap::iterator it = _map.find(univ);
	if (it == _map.end()) {
		_map.insert(make_pair(univ, 1));
	} else {
		it->second++;
	}
	return true;
}

TotalCountHandler::TotalCountHandler(int& total, const std::string& countField) :
	_total(total), _countField(countField) {
}

bool TotalCountHandler::handle(mysqlpp::Row& row) const {
	_total = (int) row[_countField.c_str()];
	return false;
}

//***************************************************************************

void FriendDistServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
	ThreadPoolConfig cfg(1, 100);
	TaskManager::instance().config(props, "Rebuild", TASK_LEVEL_REBUILD, cfg);
	TaskManager::instance().config(props, "RebuildDiff", TASK_LEVEL_REBUILD_DIFF, cfg);
}
