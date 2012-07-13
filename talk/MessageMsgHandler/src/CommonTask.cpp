#include "CommonTask.h"
/*
#include "MessageHandlerI.h"
#include "XmppTools.h"
#include "TalkCacheAdapter.h"
#include "PresenceAdapter.h"
#include "IceLogger.h"
//#include "OnlineBuddyAdapter.h"
#include "TalkDeliverAdapter.h"
#include "QueryRunner.h"
#include <boost/lexical_cast.hpp>
#include "CountCacheAdapter.h"
#include "MessageHolderAdapter.h"

using namespace com::xiaonei::talk::util;
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk;
using namespace mop::hi::svc::adapter;
using namespace MyUtil;
//using namespace talk::online::adapter;
using namespace com::xiaonei::xce;



StatManager::StatManager() {
	_total_msg = 0;
	_offline_msg = 0;

	MyUtil::TaskManager::instance().schedule(new StatTask(5000));

}
;

void StatManager::addToSnap(const StatInfoPtr& stat) {
	IceUtil::Mutex::Lock lock(_mutex);
	if (stat) {
		_snapSeq.push_back(stat);
	}
}
void StatManager::addToSnap(const StatInfoSeq& seq) {
	IceUtil::Mutex::Lock lock(_mutex);
	if (seq.empty()) {
		return;
	}
	_snapSeq.insert(_snapSeq.end(), seq.begin(), seq.end());
}

void StatManager::addToAction(const StatInfoPtr& stat) {
	IceUtil::Mutex::Lock lock(_mutex);
	if (stat) {
		_actionSeq.push_back(stat);
	}
}
void StatManager::addToAction(const StatInfoSeq& seq) {
	IceUtil::Mutex::Lock lock(_mutex);
	if (seq.empty()) {
		return;
	}
	_actionSeq.insert(_actionSeq.end(), seq.begin(), seq.end());
}

void StatManager::incMsgCount(bool OfflineMsg) {
	IceUtil::Mutex::Lock lock(_mutex);
	if (OfflineMsg) {
		_offline_msg++;
	} else {
		_total_msg++;
	}
}

void StatManager::doInsertSnap() {

	int total_msg, offline_msg;
	StatInfoSeq seq;
	{
		IceUtil::Mutex::Lock lock(_mutex);
		total_msg = _total_msg;
		offline_msg = _offline_msg;
		_total_msg = _offline_msg = 0;
		_snapSeq.swap(seq);

	}

	StatInfoPtr stat = new StatInfo();
	stat -> value = total_msg;
	stat -> action = "total_msg";
	seq.push_back(stat);

	stat = new StatInfo();
	stat -> value = offline_msg;
	stat -> action = "offline_msg";
	seq.push_back(stat);

	stat = new StatInfo();
	stat -> value = total_msg - offline_msg;
	stat -> action = "chating_msg";
	seq.push_back(stat);

	//-------------------------
	//do sql
	ostringstream sql;
	sql << "insert into xntalk_snap (name, time,value) values";
	for (size_t i = 0; i < seq.size(); i++) {
		if (i)
			sql<<",";

		sql<<"('" <<seq.at(i)->action <<"',from_unixtime(" <<seq.at(i)->timestamp<<"),"<<seq.at(i)->value<<")";
	}

	//	ConnectionHolder conn("im_stat", CDbWServer);
	//	mysqlpp::Query query = conn.query();
	//	query.exec(sql.str());
	QueryRunner("im_stat", CDbWServer).schedule(Statement(sql.str()));

}
void StatManager::doInsertAction() {
	StatInfoSeq seq;
	{
		IceUtil::Mutex::Lock lock(_mutex);
		_actionSeq.swap(seq);
	}
	if (!seq.empty()) {
		ostringstream sql;
		sql << "insert into xntalk_action (userid, time,action,value) values";
		for (size_t i = 0; i < seq.size(); i++) {
			if (i)
				sql<<",";

			sql<<"(" <<seq.at(i)->userId <<",from_unixtime(" <<seq.at(i)->timestamp<<"),'"<<seq.at(i)->action <<"',"<<seq.at(i)->value<<")";
		}

		//		ConnectionHolder conn("im_stat", CDbWServer);
		//		mysqlpp::Query query = conn.query();
		//		query.exec(sql.str());
		QueryRunner("im_stat", CDbWServer).schedule(Statement(sql.str()));
	}

}

//-----------------------------------------------------

void StatManager::StatTask::handle() {
	//MCE_DEBUG("run StatTask::handle()");
	MyUtil::TaskManager::instance().schedule(new StatManager::StatTask(_timeslot));
	StatManager::instance().doInsertSnap();
	StatManager::instance().doInsertAction();

}
*/
//------------------------------------------------------
//  Task
//------------------------------------------------------
/*
void LoadOfflineMessageTask::handle() {
	StatFunc statF("LoadOfflineMessageTask::handle");
	try {
		ostringstream sql;
		sql << "select * from offline_message where toid=" << _jid->userId;
		mysqlpp::Result res = QueryRunner("im", CDbRServer).store(Statement(sql.str()));

		for (size_t i = 0; i < res.size(); ++i) {
			mysqlpp::Row row = res.at(i);
			MessagePtr msg = new Message();
			msg->from = new Jid();
			msg->from->userId = row["fromid"];
			msg->from->index = -1;

			msg->to = _jid;
			msg->type = OFFLINE_MESSAGE;
			msg->msg = row["message"].get_string();

			TalkDeliverAdapter::instance().deliver(msg);
		}
		if (!res.empty()) {
			sql.str("");
			sql << "delete from offline_message where toid=" <<_jid->userId;
			QueryRunner("im", CDbWServer).schedule(Statement(sql.str()));
		}

		sql.str("");
		sql<<"delete from leave_words where toId=\""<<_jid->userId<<"\"";
		QueryRunner("im", CDbWServer).schedule(Statement(sql.str()));

		sql.str("");
		sql << "delete from im_leaveword where toid = "<< _jid->userId;
		QueryRunner("web_im_leaveword", CDbWServer).schedule(Statement(sql.str()));

		MCE_DEBUG("LogicManagerI::loadOfflineMessage --> userid:"<< _jid->userId<<"  do dec im leaveword!");
		//CountCacheAdapter::instance().decImLeavewordCount(_jid->userId, numeric_limits<int>::max());

		MCE_DEBUG("LogicManagerI::loadOfflineMessage --> dec im leaveword done");

	} catch(mysqlpp::Exception& e) {
		MCE_WARN("doselect leaveword err");
	} catch(...){

	}

	MessageHolderAdapter::instance().cleanOfflineMessageCount(_jid->userId);

}*/

