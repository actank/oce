
#include "BulletinAdminI.h"
#include "app/Bulletin/share/BulletinUtil.h"
#include "DbCxxPool/src/QueryRunner.h"
#include "OceCxxAdapter/src/BulletinAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using xce::blt::BulletinAdminI;
using xce::blt::BulletinAdapter;
using xce::blt::Bulletin;
using xce::blt::BulletinPtr;
using com::xiaonei::xce::CDbWServer;
using com::xiaonei::xce::Statement;
using com::xiaonei::xce::QueryRunner;

// ------- MyUtil::initialize ------------------------- 
void MyUtil::initialize() {
	// BulletinAdminI::instance().initialize();	
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&BulletinAdminI::instance(), 
			service.createIdentity("M", ""));

	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("BulletinAdmin.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BulletinAdmin.Interval", 5);
	xce::serverstate::ServerStateSubscriber::instance().initialize("ControllerBulletinAdmin",
			                        &BulletinAdminI::instance(), mod, interval);
}

// ------- BulletinAdminI 主类 ------------------------
void BulletinAdminI::add(const BulletinPtr& bulletin,
	const Ice::Current& current)	{

	map<string, string> mapBlt;
	BulletinUtil::instance().bulletin2Map(mapBlt, bulletin);

	Statement sql;
	sql << "INSERT " << TABLE_BULLETIN << " (";
	for (map<string, string>::const_iterator it = mapBlt.begin(); it != mapBlt.end(); ++it ) {
		if ( it != mapBlt.begin() ) {
			sql << ",";
		}
		sql << (*it).first;
	}
	sql << ") values(";
	for (map<string, string>::const_iterator it = mapBlt.begin(); it != mapBlt.end(); ++it ) {
		if ( it != mapBlt.begin() ) {
			sql << ",";
		}
		sql << mysqlpp::quote << (*it).second;
	}
	sql << ")";

	try {
		QueryRunner(DB_DESC_BULLETIN, CDbWServer).execute(sql);
		MCE_INFO("[BulletinAdminI::add]" << bulletin->bulletinid << " added");
	} catch (...) {
		MCE_WARN("[BulletinAdminI::add]" << bulletin->bulletinid << " exception");
	}

	// 通知所有Manager
	BulletinAdapter::instance().renew(NOREBUILDINDEX);
}

void BulletinAdminI::remove(::Ice::Int bulletinid,
	const Ice::Current& current)  {
	
	Statement sql;
	sql << "DELETE FROM " << TABLE_BULLETIN << " WHERE " << FIELD_BULLETINID << " = " << bulletinid;

	try {
		QueryRunner(DB_DESC_BULLETIN, CDbWServer).execute(sql);
		MCE_INFO("[BulletinAdminI::remove]" << bulletinid << " removeed");
	} catch (...) {
		MCE_WARN("[BulletinAdminI::remove]" << bulletinid << " exception");
	}

	// 通知所有Manager
	BulletinAdapter::instance().renew(false);
}

void BulletinAdminI::modify(::Ice::Int bulletinid,
	const BulletinPtr& bulletin, const Ice::Current& current) {
	
	map<string, string> mapBlt;
	BulletinUtil::instance().bulletin2Map(mapBlt, bulletin);

	Statement sql;
	sql << "UPDATE " << TABLE_BULLETIN << " SET ";
	for (map<string, string>::const_iterator it = mapBlt.begin(); it != mapBlt.end(); ++it ) {
		if ( it != mapBlt.begin() ) {
			sql << ",";
		}
		sql << (*it).first << "=" << mysqlpp::quote << (*it).second;
	}
	sql << " WHERE " << FIELD_BULLETINID << "=" << bulletinid;

	try {
		QueryRunner(DB_DESC_BULLETIN, CDbWServer).execute(sql);
		MCE_INFO("[BulletinAdminI::modify]" << bulletinid << " modified");
	} catch (...) {
		MCE_WARN("[BulletinAdminI::modify]" << bulletinid << " exception");
	}

	// 通知所有Manager
	BulletinAdapter::instance().renew(NOREBUILDINDEX);
}

void BulletinAdminI::online(::Ice::Int bulletinid,
	const Ice::Current& current) {

	Statement sql;
	sql << "UPDATE " << TABLE_BULLETIN << " SET " << FIELD_ONLINE << " = 1 " 
		<< " WHERE " << FIELD_BULLETINID << "=" << bulletinid;

	try {
		QueryRunner(DB_DESC_BULLETIN, CDbWServer).execute(sql);
		MCE_INFO("[BulletinAdminI::online]" << bulletinid << " onlined");
	} catch (...) {
		MCE_WARN("[BulletinAdminI::online]" << bulletinid << " exception");
	}

	// 通知所有Manager
	BulletinAdapter::instance().renew(REBUILDINDEX);
}

void BulletinAdminI::offline(::Ice::Int bulletinid,
	const Ice::Current& current)  {

	Statement sql;
	sql << "UPDATE " << TABLE_BULLETIN << " SET " << FIELD_ONLINE << " = 0 " 
		<< " WHERE " << FIELD_BULLETINID << "=" << bulletinid;

	try {
		QueryRunner(DB_DESC_BULLETIN, CDbWServer).execute(sql);
		MCE_INFO("[BulletinAdminI::offline]" << bulletinid << " offlined");
	} catch (...) {
		MCE_WARN("[BulletinAdminI::offline]" << bulletinid << " exception");
	}

	// 通知所有Manager
	BulletinAdapter::instance().renew(REBUILDINDEX);
}
