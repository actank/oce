#include "TipsAdminI.h"
#include "app/Tips/share/TipsUtil.h"
#include "DbCxxPool/src/QueryRunner.h"
#include "OceCxxAdapter/src/TipsAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using xce::tps::TipsAdminI;
using xce::tps::TipsAdapter;
using xce::tps::Tips;
using xce::tps::TipsPtr;
using com::xiaonei::xce::CDbWServer;
using com::xiaonei::xce::Statement;
using com::xiaonei::xce::QueryRunner;

// ------- MyUtil::initialize ------------------------- 
void MyUtil::initialize() {
	// TipsAdminI::instance().initialize();	
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&TipsAdminI::instance(), 
			service.createIdentity("M", ""));

	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("TipsAdmin.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("TipsAdmin.Interval", 5);
	xce::serverstate::ServerStateSubscriber::instance().initialize("ControllerTipsAdmin",
			                        &TipsAdminI::instance(), mod, interval);
}

// ------- TipsAdminI 主类 ------------------------
void TipsAdminI::add(const TipsPtr& tips,
	const Ice::Current& current)	{

	map<string, string> mapTps;
	TipsUtil::instance().tips2Map(mapTps, tips);

	Statement sql;
	sql << "INSERT " << TABLE_TIPS << " (";
	for (map<string, string>::const_iterator it = mapTps.begin(); it != mapTps.end(); ++it ) {
		if ( it != mapTps.begin() ) {
			sql << ",";
		}
		sql << (*it).first;
	}
	sql << ") values(";
	for (map<string, string>::const_iterator it = mapTps.begin(); it != mapTps.end(); ++it ) {
		if ( it != mapTps.begin() ) {
			sql << ",";
		}
		sql << mysqlpp::quote << (*it).second;
	}
	sql << ")";

	try {
		QueryRunner(DB_DESC_TIPS, CDbWServer).execute(sql);
		MCE_INFO("[TipsAdminI::add]" << tips->tipsid << " added");
	} catch (...) {
		MCE_WARN("[TipsAdminI::add]" << tips->tipsid << " exception");
	}

	// 通知所有Manager
	TipsAdapter::instance().renew(NOREBUILDINDEX);
}

void TipsAdminI::remove(::Ice::Int tipsid,
	const Ice::Current& current)  {
	
	Statement sql;
	sql << "DELETE FROM " << TABLE_TIPS << " WHERE " << FIELD_TIPSID << " = " << tipsid;

	try {
		QueryRunner(DB_DESC_TIPS, CDbWServer).execute(sql);
		MCE_INFO("[TipsAdminI::remove]" << tipsid << " removeed");
	} catch (...) {
		MCE_WARN("[TipsAdminI::remove]" << tipsid << " exception");
	}

	// 通知所有Manager
	TipsAdapter::instance().renew(false);
}

void TipsAdminI::modify(::Ice::Int tipsid,
	const TipsPtr& tips, const Ice::Current& current) {
	
	map<string, string> mapTps;
	TipsUtil::instance().tips2Map(mapTps, tips);

	Statement sql;
	sql << "UPDATE " << TABLE_TIPS << " SET ";
	for (map<string, string>::const_iterator it = mapTps.begin(); it != mapTps.end(); ++it ) {
		if ( it != mapTps.begin() ) {
			sql << ",";
		}
		sql << (*it).first << "=" << mysqlpp::quote << (*it).second;
	}
	sql << " WHERE " << FIELD_TIPSID << "=" << tipsid;

	try {
		QueryRunner(DB_DESC_TIPS, CDbWServer).execute(sql);
		MCE_INFO("[TipsAdminI::modify]" << tipsid << " modified");
	} catch (...) {
		MCE_WARN("[TipsAdminI::modify]" << tipsid << " exception");
	}

	// 通知所有Manager
	TipsAdapter::instance().renew(NOREBUILDINDEX);
}

void TipsAdminI::online(::Ice::Int tipsid,
	const Ice::Current& current) {

	Statement sql;
	sql << "UPDATE " << TABLE_TIPS << " SET " << FIELD_ONLINE << " = 1 " 
		<< " WHERE " << FIELD_TIPSID << "=" << tipsid;

	try {
		QueryRunner(DB_DESC_TIPS, CDbWServer).execute(sql);
		MCE_INFO("[TipsAdminI::online]" << tipsid << " onlined");
	} catch (...) {
		MCE_WARN("[TipsAdminI::online]" << tipsid << " exception");
	}

	// 通知所有Manager
	TipsAdapter::instance().renew(REBUILDINDEX);
}

void TipsAdminI::offline(::Ice::Int tipsid,
	const Ice::Current& current)  {

	Statement sql;
	sql << "UPDATE " << TABLE_TIPS << " SET " << FIELD_ONLINE << " = 0 " 
		<< " WHERE " << FIELD_TIPSID << "=" << tipsid;

	try {
		QueryRunner(DB_DESC_TIPS, CDbWServer).execute(sql);
		MCE_INFO("[TipsAdminI::offline]" << tipsid << " offlined");
	} catch (...) {
		MCE_WARN("[TipsAdminI::offline]" << tipsid << " exception");
	}

	// 通知所有Manager
	TipsAdapter::instance().renew(REBUILDINDEX);
}
