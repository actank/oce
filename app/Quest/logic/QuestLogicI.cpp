/* ---------------------------------------------
 * QuestLogicI.h
 *
 *  Created on: 2009-12-24
 *      Author: han.zhang
 -------------------------------------------- */

#include "QuestLogicI.h"
#include "OceCxxAdapter/src/QuestCacheAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace xce::quest;
using std::string;
using std::vector;
using com::xiaonei::xce::CDbWServer;
using com::xiaonei::xce::CDbRServer;
using com::xiaonei::xce::Statement;
using com::xiaonei::xce::QueryRunner;
using xce::serverstate::ServerStateSubscriber;

// ------------------ MyUtil::initialize ------------------------
void MyUtil::initialize() {
	
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&QuestLogicI::instance(), service.createIdentity("M", ""));
	QuestCacheAdapter::instance();

	int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("QuestLogic.Mod", 0);
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("QuestLogic.Interval", 5);
	ServerStateSubscriber::instance().initialize("ControllerQuestLogic", &QuestLogicI::instance(), mod, interval);
}

// ------------------ QuestLogicI 主类 --------------------------
void QuestLogicI::addTip(const string& title, const string& content, const string& url, int showCount, const Ice::Current& crt) {
	// 向db中写入一条新的tip
	Statement sql;
	sql << "INSERT IGNORE INTO " << TABLE_TIP 
		<< "(" << FIELD_TIP_TITLE << "," << FIELD_TIP_CONTENT << "," << FIELD_TIP_URL << ") VALUES(" 
		<< mysqlpp::quote << title << "," << mysqlpp::quote << content << "," << mysqlpp::quote << url << ")"; 

	try {
		QueryRunner(DESC_QUEST_TIPFLOW, CDbWServer).execute(sql);
		MCE_INFO("[QuestLogicI::addTip] new tip '" << title << "' added");
	} catch (mysqlpp::Exception& e) {
		MCE_WARN("[QuestLogicI::addTip] new tip '" << title << "' add failed. mysqlpp::Exception : " << e.what());
	} catch (Ice::Exception& e) {
		MCE_WARN("[QuestLogicI::addTip] new tip '" << title << "' add failed. Ice::Exception : " << e.what());
	} catch (...) {
		MCE_WARN("[QuestLogicI::addTip] new tip '" << title << "' add failed. unknown exception");
	}
}

void QuestLogicI::removeTip(int tipid, const Ice::Current& crt) {
	// 从db中删除一个tip
	Statement sql;
	sql << "DELETE FROM " << TABLE_TIP << " WHERE " << FIELD_TIP_TIPID << " = " << tipid;

	try {
		QueryRunner(DESC_QUEST_TIPFLOW, CDbWServer).execute(sql);
		MCE_INFO("[QuestLogicI::removeTip] remove tip " << tipid << " ok");
	} catch (mysqlpp::Exception& e) {
		MCE_WARN("[QuestLogicI::removeTip] remove tip " << tipid << " failed. mysqlpp::Exception : " << e.what());
	} catch (Ice::Exception& e) {
		MCE_WARN("[QuestLogicI::removeTip] remove tip " << tipid << " failed. Ice::Exception : " << e.what());
	} catch (...) {
		MCE_WARN("[QuestLogicI::removeTip] remove tip " << tipid << " failed. unknown exception");
	}
} 

void QuestLogicI::modifyTip(int tipid, const string& title, const string& content, const string& url, const Ice::Current& crt) {

	// 判断是否无须修改
	if ( (title == "") && (content == "") && (url == "") ) {
		MCE_INFO("[QuestLogicI::modifyTip] nothing to modify of tip " << tipid);
		return;
	}
	
	// 从DB中修改该tip
	Statement sql;
	map<string, string> fields;
	sql << "UPDATE " << TABLE_TIP << " SET ";
	if ( title != "" ) {
		fields.insert( make_pair<string, string>(FIELD_TIP_TITLE, title) );
	}
	if ( content != "" ) {
		fields.insert( make_pair<string, string>(FIELD_TIP_CONTENT, content) );
	}
	if ( url != "" ) {
		fields.insert( make_pair<string, string>(FIELD_TIP_URL, url) );
	}
	for (map<string, string>::iterator it = fields.begin(); it != fields.end(); ++it ) {
		if ( it != fields.begin() ) {
			sql << ",";
		}
		sql << (*it).first << " = " << mysqlpp::quote << (*it).second;
	}
	sql << "WHERE " << FIELD_TIP_TIPID << " = " << tipid;  
		
	try {
		QueryRunner(DESC_QUEST_TIPFLOW, CDbWServer).execute(sql);
		MCE_INFO("[QuestLogicI::modifyTip] modify tip " << tipid << " ok");
	} catch (mysqlpp::Exception& e) {
		MCE_WARN("[QuestLogicI::modifyTip] modify tip " << tipid << " failed. mysqlpp::Exception : " << e.what());
	} catch (Ice::Exception& e) {
		MCE_WARN("[QuestLogicI::modifyTip] modify tip " << tipid << " failed. Ice::Exception : " << e.what());
	} catch (...) {
		MCE_WARN("[QuestLogicI::modifyTip] modify tip " << tipid << " failed. unknown exception");
	}
}

void QuestLogicI::setTipFlow(const TipFlowPtr& flow, const Ice::Current& crt) {

	// 向DB中写入新的Flow
	ostringstream tips;
	for (size_t i = 0; i < flow->seq.size(); ++i ) {
		if ( i != 0 ) {
			tips << ";";
		}
		tips << flow->seq[i].tipId << "-" << flow->seq[i].showCount;
	}
	
	Statement sql;
	sql << "INSERT IGNORE INTO " << TABLE_FLOW 
		<< " (" << FIELD_FLOW_TIPS << ") VALUES(" << mysqlpp::quote << tips.str() << ")";

	try {
		QueryRunner(DESC_QUEST_TIPFLOW, CDbWServer).execute(sql);
		MCE_INFO("[QuestLogicI::setTipFlow] new flow added ok");
	} catch (mysqlpp::Exception& e) {
		MCE_WARN("[QuestLogicI::setTipFlow] new flow added failed. mysqlpp::Exception : " << e.what());
	} catch (Ice::Exception& e) {
		MCE_WARN("[QuestLogicI::setTipFlow] new flow added failed. Ice::Exception : " << e.what());
	} catch (...) {
		MCE_WARN("[QuestLogicI::setTipFlow] new flow added failed. unknown exception");
	}
}

void QuestLogicI::renew(const Ice::Current& crt) {

	// 更新所有Cache服务的tip列表
	QuestCacheAdapter::instance().renewTips();

	// 更新所有Cache服务的flow列表
	QuestCacheAdapter::instance().renewFlow();
}
