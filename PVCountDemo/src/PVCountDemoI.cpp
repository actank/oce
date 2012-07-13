/*#######################################################################
 # Author: tangzhaohui
 # Created Time: 2010年09月15日 星期三 16时50分23秒
 # File Name: PVCountDemoI.cpp
 # Description:
 #######################################################################*/
#include "PVCountDemoI.h"
#include "ServiceI.h"
#include "QueryRunner.h"
#include <sstream>

using namespace std;
using namespace Demo;
using namespace com::xiaonei::xce;
using namespace MyUtil;

/*
 * screen the cache per 10 minute, remove the item which's pvcount is bigger than 5
 */
class CacheScreenTimer: public MyUtil::Timer {
public:
	CacheScreenTimer(int delay) :
		Timer(delay) {
	}

	virtual void handle() {
		MCE_INFO("CacheScreenTimer::handle() started.");
		PVCountDemoI::instance().checkCache();
		MCE_INFO("CacheScreenTimer::handle() complete.");
	}
};

PVCountDemoI::PVCountDemoI() {
	_cache.size(10);
}

PVCountDemoI::PVCountDemoI(int cacheSize) {
	_cache.size(cacheSize);
}

void PVCountDemoI::checkCache() {
	IceUtil::Mutex::Lock lock(_mutex);
	for (CacheIterator iter = _cache.begin(); iter
			!= _cache.end(); iter++) {
		int thisValue = _cache.getObject(iter, -1);
		if (thisValue > 5) {
			_cache.removeByIterator(iter);
			std::ostringstream out;
			out << thisValue;
			MCE_INFO("PVCountDemoI::checkCache() remove the value: "
					+ out.str());
		}
	}
}

void PVCountDemoI::add(Ice::Int userId, Ice::Int pvCount, const Ice::Current&) {
	MCE_INFO("DB_INSTANCE: " + DB_INSTANCE);
	try {
		Statement sql;
		sql << "INSERT INTO " << DB_INSTANCE << "." << TABLE_PVCOUNT << " ("
				<< COL_ID << ", " << COL_PVCOUNT << ") VALUES( "
				<< mysqlpp::quote << userId << " , " << mysqlpp::quote
				<< pvCount << ")";
		QueryRunner(DB_INSTANCE, CDbWServer, TABLE_PVCOUNT).store(sql);
		{
			IceUtil::Mutex::Lock lock(_mutex);
			if (_cache.find(userId, -1) != pvCount) {
				_cache.remove(userId);
				_cache.add(pvCount, userId);
			}
		}
	} catch (const exception& e) {
		MCE_INFO(e.what());
	} catch (...) {
		MCE_INFO("PVCountDemoI::add(), add PVCount Try error");
	}
}

void PVCountDemoI::update(Ice::Int userId, Ice::Int pvCount,
		const Ice::Current&) {
	if (_cache.find(userId, -1) == pvCount) {
		return;
	} else {
		_cache.remove(userId);
	}
	try {
		Statement sql;
		sql << "UPDATE " << TABLE_PVCOUNT << " SET " << COL_PVCOUNT << " = "
				<< mysqlpp::quote << pvCount << " WHERE " << COL_ID << " = "
				<< mysqlpp::quote << userId;
		QueryRunner(DB_INSTANCE, CDbWServer, TABLE_PVCOUNT).store(sql);
		_cache.add(pvCount, userId);
	} catch (const exception& e) {
		MCE_INFO(e.what());
	} catch (...) {
		MCE_INFO("PVCountDemoI::update(), update PVCount Try error");
	}
}

void PVCountDemoI::remove(Ice::Int userId, const Ice::Current&) {
	if (_cache.has(userId)) {
		_cache.remove(userId);
	}
	try {
		Statement sql;
		sql << "DELETE FROM " << TABLE_PVCOUNT << " WHERE " << COL_ID << " = "
				<< mysqlpp::quote << userId;
		QueryRunner(DB_INSTANCE, CDbWServer, TABLE_PVCOUNT).store(sql);
	} catch (const exception& e) {
		MCE_INFO(e.what());
	} catch (...) {
		MCE_INFO("PVCountDemoI::remove(), Remove PVCount Try error");
	}
}

Ice::Int PVCountDemoI::getPVCountByUserId(Ice::Int userId, const Ice::Current&) {
	int pvCount = _cache.locate(userId, -1);
	if (pvCount > 0) {
		MCE_INFO("PVCountDemoI::getPVCountByUserId(), Get PVCount from cache.");
		return pvCount;
	}
	try {
		Statement sql;
		sql << "SELECT " << COL_PVCOUNT << " FROM " << TABLE_PVCOUNT
				<< " WHERE " << COL_ID << " = " << mysqlpp::quote << userId;
		mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbWServer,
				TABLE_PVCOUNT).store(sql);
		if (!res) {
			MCE_INFO(
					"PVCountDemoI::getPVCountByUserId(), Get PVCount Try error");
		}
		if (res.num_rows() > 0) {
			mysqlpp::Row row = res.at(0);
			pvCount = row[COL_PVCOUNT];
		}
		_cache.add(pvCount, userId);
	} catch (const exception& e) {
		MCE_INFO(e.what());
	} catch (...) {
		MCE_INFO("PVCountDemoI::getPVCountByUserId(), Get PVCount Try error");
	}

	return pvCount;
}

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&PVCountDemoI::instance(),
			service.createIdentity("PVCountDemo", ""));

	TaskManager::instance().scheduleRepeated(new CacheScreenTimer(10 * 60
			* 1000));
}

