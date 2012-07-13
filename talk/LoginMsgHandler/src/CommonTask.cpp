#include "CommonTask.h"
#include "LoginHandlerI.h"
#include "XmppTools.h"
#include "TalkCacheAdapter.h"
#include "PresenceAdapter.h"
#include "IceLogger.h"
//#include "OnlineBuddyAdapter.h"
#include "TalkDeliverAdapter.h"
#include "QueryRunner.h"
#include <boost/lexical_cast.hpp>
#include "CountCacheAdapter.h"
#include "TalkStatAdapter.h"
//#include "Date.h"

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
	Statement sql;
	sql << "insert into xntalk_snap (name, time,value) values";
	for (size_t i = 0; i < seq.size(); i++) {
		if (i)
			sql<<",";

		sql<<"('" <<seq.at(i)->action <<"',from_unixtime(" <<seq.at(i)->timestamp<<"),"<<seq.at(i)->value<<")";
	}

	//	ConnectionHolder conn("im_stat", CDbWServer);
	//	mysqlpp::Query query = conn.query();
	//	query.exec(sql);
	QueryRunner("im_stat", CDbWServer).schedule(sql);

}
void StatManager::doInsertAction() {
	StatInfoSeq seq;
	{
		IceUtil::Mutex::Lock lock(_mutex);
		_actionSeq.swap(seq);
	}
	if (!seq.empty()) {
		MyUtil::IntSeq loginSeq,wloginSeq;

		Statement sql;
		sql << "insert into xntalk_action (userid, time,action,value) values";
		for (size_t i = 0; i < seq.size(); i++) {
			if(seq.at(i)->action == "talk_login"){
				loginSeq.push_back(seq.at(i)->userId);
			}else if(seq.at(i)->action == "webpager_login"){
				wloginSeq.push_back(seq.at(i)->userId);
			}

			if (i)
				sql<<",";

			sql<<"(" <<seq.at(i)->userId <<",from_unixtime(" <<seq.at(i)->timestamp<<"),'"<<seq.at(i)->action <<"',"<<seq.at(i)->value<<")";
		}

		if(!loginSeq.empty()){
			try{
				::talk::stat::TalkStatAdapter::instance().login(loginSeq);
			}catch(Ice::Exception & e){
				MCE_WARN("StatManager::doInsertAction --> do TalkStat.login err:"<< e);
			}
			catch(...){
				MCE_WARN("StatManager::doInsertAction --> do TalkStat.login err");
			}
		}
		if(!wloginSeq.empty()){
			try{
				::talk::stat::TalkStatAdapter::instance().wLogin(wloginSeq);
			}catch(Ice::Exception& e){
				MCE_WARN("StatManager::doInsertAction --> do TalkStat.wlogin err:"<<e);
			}
			catch(...){
				MCE_WARN("StatManager::doInsertAction --> do TalkStat.wlogin err");
			}
		}

		QueryRunner("im_stat", CDbWServer).store(sql);
	}

}

//-----------------------------------------------------

void StatManager::StatTask::handle() {
	MCE_DEBUG("run StatTask::handle()");
	MyUtil::TaskManager::instance().schedule(new StatManager::StatTask(_timeslot));
	StatManager::instance().doInsertSnap();
	StatManager::instance().doInsertAction();

}

void IpLogTask::handle(){
        if(ip_.empty()){
                return;
        }
        Statement sql;
        sql << "insert into iplog (id, ip, time, type) values(" << userid_ << ", '" << ip_ << "', FROM_UNIXTIME(" << time(NULL) << "), 3);";
        QueryRunner("logging", CDbWServer).store(sql);
        //QueryRunner("im_stat", CDbWServer).store(sql);
}



