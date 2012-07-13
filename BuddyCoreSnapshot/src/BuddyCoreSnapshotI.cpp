#include "BuddyCoreSnapshotI.h"
#include "BuddyCoreLogicAdapter.h"

using namespace std;
using namespace com::xiaonei::service::buddy;
using namespace MyUtil;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();

	service.registerObjectCache(CATEGORY_ENTRY, "ES", NULL);

	// This is default. Working config stored in xcedescribe
	TaskManager::instance().config(LOAD_TASK_LEVEL, 1, 1);
	service.registerXceObserver(new BuddyCoreSnapshotServiceSchemaI);

	BuddyCoreSnapshotI* manager = new BuddyCoreSnapshotI;
	manager->boot();

	BuddyCoreSnapshotPrx prx =
			BuddyCoreSnapshotPrx::uncheckedCast(service.getAdapter()->add(manager, service.createIdentity("M", "")));

	manager->setMyProxy(prx);

	BuddyCoreLogicAdapter::instance().registerSnapshot(prx);
}

// **************************************************************************

LoadBootstrapThread::LoadBootstrapThread(BuddyCoreSnapshotI& manager) :
	_manager(manager) {
}
void LoadBootstrapThread::run() {
	_manager.loadBootstrap();
}
LoadIncreamentThread::LoadIncreamentThread(BuddyCoreSnapshotI& manager) :
	_manager(manager) {
}
void LoadIncreamentThread::run() {
	while (true) {
		try {
			_manager.loadIncreament();
		} catch(...) {
			MCE_WARN("loadIncreament should not throws exception.");
		}
		sleep(1);
	}
}

EnsureRegisterThread::EnsureRegisterThread(BuddyCoreSnapshotI& manager) :
	_manager(manager) {
}
void EnsureRegisterThread::run() {
	while (true) {
		try {
			_manager.ensureRegister();
		} catch(...) {
			MCE_WARN("ensureRegister should not throws exception.");
		}
		sleep(1);
	}
}

//***************************************************************************

BuddyCoreSnapshotI::BuddyCoreSnapshotI() {
	//_status = INITIALIZING;
	_status = INITIALIZED;
}

SnapshotStatus BuddyCoreSnapshotI::getStatus(const Ice::Current& current) {
	IceUtil::RWRecMutex::RLock lock(_statusMutex);
	return _status;
}

void BuddyCoreSnapshotI::setStatus(SnapshotStatus newStatus,
		const Ice::Current& current) {
	IceUtil::RWRecMutex::WLock lock(_statusMutex);
	_status = newStatus;
}

void BuddyCoreSnapshotI::patch(::Ice::Int timestamp,
		const SnapshotEntrySeqMap& data, const Ice::Current& current) {
	IceUtil::Mutex::Lock lock(_increamentsMutex);
	_increaments.insert(make_pair(timestamp, data));
}

SnapshotEntrySeqMap BuddyCoreSnapshotI::getEntry4CommonFriend(
		::Ice::Int userId, const Ice::Current& current) {
	SnapshotEntrySeqMap result;
	ServiceI& service = ServiceI::instance();
	EntryListHolderPtr owner = service.findObject<EntryListHolderPtr>(
			CATEGORY_ENTRY, userId);
	if (!owner) {
		return result;
	}
	EntryList ownerList = owner->getAll();
	SnapshotEntrySeq ownerSnap;
	for (EntryList::iterator ownerIt = ownerList.begin(); ownerIt
			!= ownerList.end(); ++ownerIt) {
		int ownerToId = ownerIt->to;
		uint32_t ownerDesc = ownerIt->desc;
		SnapshotEntry entryOwner;
		entryOwner.toId = ownerToId;
		entryOwner.desc = BuddyDescHelper::translateDesc(ownerDesc);
		ownerSnap.push_back(entryOwner);

		EntryListHolderPtr ownerBuddy = service.findObject<EntryListHolderPtr>(
				CATEGORY_ENTRY, ownerToId);
		if (!ownerBuddy) {
			continue;
		}

		if (ownerDesc == DESC_FRIEND) {
			EntryList buddyList = ownerBuddy->getAll();
			SnapshotEntrySeq buddySnap;
			for (EntryList::iterator buddyIt = buddyList.begin(); buddyIt
					!= buddyList.end(); ++buddyIt) {
				int buddyToId = buddyIt->to;
				uint32_t buddyDesc = buddyIt->desc;
				if (buddyDesc==DESC_FRIEND || buddyDesc == DESC_APPLY) {
					SnapshotEntry entryFF;
					entryFF.toId = buddyToId;
					entryFF.desc = BuddyDescHelper::translateDesc(buddyDesc);
					buddySnap.push_back(entryFF);
				}
			}
			result[ownerToId] = buddySnap;
		}

		if (ownerDesc == DESC_APPLY) {
			EntryList buddyList = ownerBuddy->getAll();
			SnapshotEntrySeq buddySnap;
			for (EntryList::iterator buddyIt = buddyList.begin(); buddyIt
					!= buddyList.end(); ++buddyIt) {
				int buddyToId = buddyIt->to;
				uint32_t buddyDesc = buddyIt->desc;
				if (buddyDesc==DESC_FRIEND) {
					SnapshotEntry entryAF;
					entryAF.toId = buddyToId;
					entryAF.desc = BuddyDescHelper::translateDesc(buddyDesc);
					buddySnap.push_back(entryAF);
				}
			}
			result[ownerToId] = buddySnap;
		}
	}
	result[userId] = ownerSnap;
	return result;
}

SnapshotEntrySeqMap BuddyCoreSnapshotI::getEntry4ShareFriend(
		const ::MyUtil::IntSeq& ids, const Ice::Current& current) {
	SnapshotEntrySeqMap result;
	ServiceI& service = ServiceI::instance();
	for (IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
		EntryListHolderPtr owner = service.findObject<EntryListHolderPtr>(
				CATEGORY_ENTRY, *it);
		if (!owner) {
			continue;
		}
		EntryList ownerList = owner->getAll();
		SnapshotEntrySeq ownerSnap;
		for (EntryList::iterator ownerIt = ownerList.begin(); ownerIt
				!= ownerList.end(); ++ownerIt) {
			int ownerToId = ownerIt->to;
			uint32_t ownerDesc = ownerIt->desc;
			//MCE_INFO("Owner:"<<*it <<" Buddy:"<<ownerToId << " Desc:"<<ownerDesc);
			if (ownerDesc!=DESC_FRIEND) {
				continue;
			}
			SnapshotEntry entryOwner;
			entryOwner.toId = ownerToId;
			entryOwner.desc = BuddyDescHelper::translateDesc(ownerDesc);
			ownerSnap.push_back(entryOwner);
		}
		result[*it] = ownerSnap;
	}
	return result;
}

SnapshotEntrySeqMap BuddyCoreSnapshotI::getFriends(const ::MyUtil::IntSeq& ids,
		const Ice::Current& current) {
	SnapshotEntrySeqMap result;
	ServiceI& service = ServiceI::instance();
	for (IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
		EntryListHolderPtr owner = service.findObject<EntryListHolderPtr>(
				CATEGORY_ENTRY, *it);
		if (!owner) {
			continue;
		}
		EntryList ownerList = owner->getAll();
		SnapshotEntrySeq ownerSnap;
		for (EntryList::iterator ownerIt = ownerList.begin(); ownerIt
				!= ownerList.end(); ++ownerIt) {
			int ownerToId = ownerIt->to;
			uint32_t ownerDesc = ownerIt->desc;
			//MCE_INFO("Owner:"<<*it <<" Buddy:"<<ownerToId << " Desc:"<<ownerDesc);
			if (ownerDesc!=DESC_FRIEND) {
				continue;
			}
			SnapshotEntry entryOwner;
			entryOwner.toId = ownerToId;
			entryOwner.desc = BuddyDescHelper::translateDesc(ownerDesc);
			ownerSnap.push_back(entryOwner);
		}
		result[*it] = ownerSnap;
	}
	return result;
}

void BuddyCoreSnapshotI::loadBootstrap() {
	{
		IceUtil::RWRecMutex::RLock lock(_statusMutex);
		if (_status != INITIALIZING) {
			MCE_WARN("loadBootstrap should be called only once.");
			return;
		}
	}
	int batchSize=20000;
	int minUserId = 0;
	int maxUserId = 0;
	while (true) {
		try {
			ostringstream sql_user;
			sql_user << "SELECT id FROM user_basic WHERE id > "<<minUserId<<" LIMIT " << batchSize;
			BootstrapUserHandler handler("id", maxUserId);
			QueryRunner("buddycore_snapshot_userbasic", CDbRServer).query(Statement(sql_user.str()), handler);
		} catch(Ice::Exception& e) {
			MCE_WARN("LoadBootstrapTask exception while loading user min="<<minUserId<<" max=" << maxUserId << " e=" << e);
		} catch(std::exception& e) {
			MCE_WARN("LoadBootstrapTask exception while loading user min="<<minUserId<<" max=" << maxUserId << " e.what()=" << e.what());
		} catch(...) {
			MCE_WARN("LoadBootstrapTask exception while loading user min="<<minUserId<<" max=" << maxUserId);
		}

		if (maxUserId <=minUserId) {
			MCE_INFO("LoadBootstrap finished.");
			break;
		}
		TaskManager::instance().execute(new LoadBootstrapTask(minUserId, maxUserId, *this));
		minUserId = maxUserId;
	}
}

bool BuddyCoreSnapshotI::loadIncreament() {
	multimap<int, SnapshotEntrySeqMap> incs;
	{
		IceUtil::Mutex::Lock lock(_increamentsMutex);
		if (_increaments.empty()) {
			MCE_DEBUG("Increaments is empty.");
			return false;
		}
		incs.swap(_increaments);
	}
	int usersize4log = 0;
	for (multimap<int, SnapshotEntrySeqMap>::iterator it = incs.begin(); it
			!= incs.end(); ++it) {
		SnapshotEntrySeqMap smaps = it -> second;
		usersize4log += smaps.size();
		for (SnapshotEntrySeqMap::iterator smapsIt = smaps.begin(); smapsIt
				!= smaps.end(); ++smapsIt) {
			int userId = smapsIt -> first;
			EntryList el;
			for (SnapshotEntrySeq::iterator sentryIt = smapsIt->second.begin(); sentryIt
					!= smapsIt->second.end(); ++sentryIt) {
				SnapshotEntry entry = *sentryIt;
				switch (entry.desc) {
				case Friend:
					el.push_back(Entry(entry.toId, DESC_FRIEND));
					break;
				case Apply:
					el.push_back(Entry(entry.toId, DESC_APPLY));
					break;
				case Block:
					el.push_back(Entry(entry.toId, DESC_BLOCK));
					break;
				default:
					continue;
					break;
				}
			}
			ServiceI::instance().addObject(new EntryListHolder(userId, el), CATEGORY_ENTRY, userId);
			MCE_DEBUG("loadIncreament for id="<<userId<<" done");
		}
	}
	MCE_INFO("loadIncreament for "<<usersize4log<<" users in "<<incs.size()
			<<" batches. done.");
	return true;
}

void BuddyCoreSnapshotI::ensureRegister() {
	BuddyCoreLogicAdapter::instance().registerSnapshot(_myProxy);
}

//***************************************************************************

void BuddyCoreSnapshotServiceSchemaI::configure(const Ice::PropertiesPtr& props) {

	int later_min = props->getPropertyAsIntWithDefault(
			"Service.ThreadPool.TaskHandler.LoadBootstrapTask.Size", 1);
	int later_max = props->getPropertyAsIntWithDefault(
			"Service.ThreadPool.TaskHandler.LoadBootstrapTask.SizeMax", 1);

	TaskManager::instance().config(LOAD_TASK_LEVEL, later_min, later_max);
}

//***************************************************************************

void LoadBootstrapTask::handle() {
	EntryListMap onebatch;
	for (int relationCluster = 0; relationCluster < 100; ++relationCluster) {
		ostringstream table;
		table << "relation_" << relationCluster;
		ostringstream patten;
		patten << "relation_" << relationCluster;
		try {
			ostringstream sql_relation;
			sql_relation << "SELECT host, guest FROM " << table.str()
			<< " WHERE host > " << _minUserId << " AND host <= "
			<< _maxUserId;

			BootstrapEntryHandler handler(onebatch, "host", "guest", DESC_FRIEND);

			QueryRunner("buddycore_snapshot_relation", CDbRServer, patten.str()).query(Statement(sql_relation.str()), handler);
		} catch(Ice::Exception& e) {
			MCE_WARN("LoadBootstrapTask exception while loading relation min="<<_minUserId<<" max=" << _maxUserId << " e=" << e);
		} catch(std::exception& e) {
			MCE_WARN("LoadBootstrapTask exception while loading relation min="<<_minUserId<<" max=" << _maxUserId << " e.what()=" << e.what());
		} catch(...) {
			MCE_WARN("LoadBootstrapTask exception while loading relation min="<<_minUserId<<" max=" << _maxUserId);
		}
	}

	try {
		ostringstream sql_buddyapply;
		sql_buddyapply
		<< "SELECT applicant, accepter FROM buddyapply WHERE applicant > "
		<<_minUserId<<" AND applicant <= "<<_maxUserId;

		BootstrapEntryHandler handler(onebatch, "applicant", "accepter",
				DESC_APPLY);

		QueryRunner("buddycore_snapshot_relation_info", CDbRServer).query(
				Statement(sql_buddyapply.str()), handler);
	} catch(...) {
		MCE_WARN("LoadBootstrapTask exception while loading buddyapply min="<<_minUserId<<" max=" << _maxUserId);
	}

	try {
		ostringstream sql_relationblock;
		sql_relationblock
		<< "SELECT host, guest FROM relation_block WHERE host > "
		<<_minUserId<<" AND host <= "<<_maxUserId;

		BootstrapEntryHandler handler(onebatch, "host", "guest",
				DESC_BLOCK);

		QueryRunner("buddycore_snapshot_relation_info", CDbRServer).query(
				Statement(sql_relationblock.str()), handler);
	} catch(...) {
		MCE_WARN("LoadBootstrapTask exception while loading relation_block min="<<_minUserId<<" max=" << _maxUserId);
	}

	try {
		ServiceI& service = ServiceI::instance();
		for(EntryListMap::iterator it = onebatch.begin();it != onebatch.end();++it) {
			service.addObject(new EntryListHolder(it->first, it->second), CATEGORY_ENTRY, it->first);
		}
	} catch(...) {
		MCE_WARN("LoadBootstrapTask exception while save min="<<_minUserId<<" max=" << _maxUserId);
	}
}

//***************************************************************************

BootstrapEntryHandler::BootstrapEntryHandler(EntryListMap& result,
		const std::string& keyField, const std::string& valueField,
		const uint32_t desc) :
	_result(result), _keyField(keyField), _valueField(valueField), _desc(desc) {
}

bool BootstrapEntryHandler::handle(mysqlpp::Row& row) const {
	int key = (int)row[_keyField.c_str()];
	int value = (int)row[_valueField.c_str()];
	_result[key].push_back(Entry(value, _desc));
	return true;
}

//***************************************************************************

BootstrapUserHandler::BootstrapUserHandler(const std::string& field, int& maxId) :
	_field(field), _maxId(maxId) {
}

bool BootstrapUserHandler::handle(mysqlpp::Row& row) const {
	int id = (int)row[_field.c_str()];
	_maxId = id > _maxId ? id : _maxId;
	return true;
}
