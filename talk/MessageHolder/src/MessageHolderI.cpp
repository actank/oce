#include "MessageHolderI.h"
#include "ServiceI.h"
#include "pugixml.hpp"
#include "XmppTools.h"
#include "QueryRunner.h"
#include "Date.h"
#include "ServiceI.h"
#include "TalkStatAdapter.h"
#include "CountCacheAdapter.h"
#include "MessageHolderAdapter.h"
#include "TalkDeliverAdapter.h"
#include "MessageType.h"
#include "OnlineCenterAdapter.h"
#include "UserCacheAdapter.h"
#include "NotifyDispatcherAdapter.h"
#include <boost/lexical_cast.hpp>
#include "NotifyIndexAdapter.h"

using namespace boost;
using namespace xce::notify;
using namespace xce::usercache;
using namespace talk;
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk::util;
using namespace pugi;
using namespace com::xiaonei::xce;
using namespace talk::stat;
using namespace mop::hi::svc::adapter;
using namespace talk::adapter;
using namespace talk::online::adapter;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&HolderManagerI::instance(), service.createIdentity("M", ""));

	TaskManager::instance().schedule(new MsgStatTimer);
	TaskManager::instance().scheduleRepeated(&OfflineMessageQuery::instance());
	//service.registerObjectCache(LC, "LC", new AsyncServantFactory(new LeavewordCountFactoryI));
}
//-------------------------------------------
Ice::ObjectPtr LeavewordCountFactoryI::create(Ice::Int id) {
	Statement sql;
	sql << "select * from offline_message_count where userid = " << boost::lexical_cast<string>(id).c_str();
#ifndef NEWARCH
	mysqlpp::Result res = QueryRunner("im", CDbRServer).store(sql);
#else
	mysqlpp::StoreQueryResult res = QueryRunner("im", CDbRServer).store(sql);
#endif
	LeavewordCountPtr count = new LeavewordCount();
	if (res.num_rows()>0) {
		mysqlpp::Row row = res.at(0);
		count->inc((int)row["count"]);
	}
	return count;
}
Ice::ObjectPtr LeavewordCountFactoryI::delegate(Ice::Int id) {
	LeavewordCountPtr count = new LeavewordCount();
	return count;
}

//-------------------------------------------
/*
void MessageTimer::handle() {
	MCE_DEBUG("MessageTimer::handle");

	HolderManagerI::instance().removeMessage(_msg->msgKey);
	if (_isSend) {
		return;
	}

	MCE_DEBUG("MessageTimer::handle --> do save msg");

	xml_document_ptr doc = new xml_document();
	if (!doc->load(_msg->msg.c_str())) {
		return;
	}

	xml_node messageNode = doc->child("message");

	xml_node bodyNode = messageNode.child("body");
	if (bodyNode.empty()) {
		return;
	}

	messageNode.append_attribute("from").set_value(jidToString(_msg->from,
			false).c_str());

	size_t tm =:: time(NULL);
	xml_node delayNode = messageNode.append_child();
	delayNode.set_name("delay");
	delayNode.append_attribute("xmlns") = "urn:xmpp:delay";
	delayNode.append_attribute("from") = "talk.xiaonei.com";
	delayNode.append_attribute("stamp") = Date(tm).str(
			"%Y-%m-%dT%H:%M:%S+08:00Z").c_str();
	delayNode.append_child(node_pcdata).set_value("Offline Storage");

	ostringstream os;
	os << "(" << _msg->from->userId << "," << _msg->to->userId << ","
	<< mysqlpp::quote << doc->xml() << "," << ::time(NULL) << ")";

	OfflineMessageQuery::instance().push(os.str());
	OfflineMessageQuery::instance().incOfflineMsgCount(_msg->to->userId);
}
bool MessageTimer::isSend() {
	return _isSend;
}
void MessageTimer::send() {
	_isSend = true;
}*/

//--------------------------------------------

void MessageTub::insert(const MessagePtr& msg) {
	IceUtil::Mutex::Lock lock(_mutex);
	//_tub[msg->msgKey] = new MessageTimer(msg);
	if(!msg){
		return;
	}
	MCE_INFO("MessageTub::insert FROM:"<< msg->from->userId << " TO:" << msg->to->userId << " KEY:" << msg->msgKey);
	//MCE_INFO("@@@ insert userid:" << msg->to->userId << " key:" << msg->msgKey << " " << msg->msg);
	_userMsgs[msg->to->userId][msg->msgKey] = msg;
	MsgTimerInfoPtr msgTimerinfo = new MsgTimerInfo;
	//msgTimerinfo->userId = msg->to->userId;
	//msgTimerinfo->msgKey = msg->msgKey;
	msgTimerinfo->msg = msg;
	msgTimerinfo->time = time(NULL);
	if(msg->to->index == -1){
		msgTimerinfo->resent = false;
	}else{
		msgTimerinfo->resent = true;
	}
	_msgTimes.push_back(msgTimerinfo);
	/*pair<MsgPos, bool> res = _msgs.insert(make_pair(msg->msgKey, msg));
	if(res.second){
		_userMsgs[msg->to].insert(res.first);
		_msgTimes.push_back(make_pair(res.first, time(NULL)));
	}*/
}
;
void MessageTub::send(int userId, Ice::Long msgKey) {
	bool clear = false;
	{
		IceUtil::Mutex::Lock lock(_mutex);
		map<int, map<Ice::Long, MessagePtr> >::iterator it = _userMsgs.find(userId);
		if(it != _userMsgs.end()){
			map<Ice::Long, MessagePtr>::iterator msgIt = it->second.find(msgKey);
			if(msgIt!=it->second.end()){
				it->second.erase(msgIt);
				clear = true;
			}
			if(it->second.empty()){
				_userMsgs.erase(it);
			}
		}
	}
	if(!clear){
		try{
			Statement sql;
			sql << "delete from offline_message where id=" << msgKey;
			QueryRunner("im", CDbWServer).schedule(sql);
		}catch(std::exception& e){
			MCE_WARN("MessageTub::send --> delete from db err, " << e.what() << " " << userId << " " << msgKey);
		}
	}
	MCE_INFO("MessageTub::send --> delete in mem " << clear << " TO:" << userId << " KEY:" << msgKey);
}
/*
void MessageTub::remove(int userId, Ice::Long msgKey) {
	IceUtil::Mutex::Lock lock(_mutex);
	map<int, map<Ice::Long, MessagePtr> >::iterator it = _userMsgs.find(userId);
	if(it != _userMsgs.end()){
		map<Ice::Long, MessagePtr>::iterator msgIt = it->second.find(msgKey);
		if(msgIt!=it->second.end()){
			it->second.erase(msgIt);
		}
		if(it->second.empty()){
			_userMsgs.erase(it);
		}
	}
	//_tub.erase(msgKey);
}
*/
void MessageTub::getTimeoutMsgs(vector<MessagePtr>& timeoutSeq, vector<MessagePtr>& resentSeq){
	IceUtil::Mutex::Lock lock(_mutex);
	for(; !_msgTimes.empty();){
		MsgTimerInfoPtr msgTimeInfo = _msgTimes.front();
		if((time(NULL)-msgTimeInfo->time) < MSG_TIMEOUT){
			break;
		}
		_msgTimes.pop_front();

		map<int, map<Ice::Long, MessagePtr> >::iterator it = _userMsgs.find(msgTimeInfo->msg->to->userId);
		if(it != _userMsgs.end()){
			map<Ice::Long, MessagePtr>::iterator msgIt = it->second.find(msgTimeInfo->msg->msgKey);
			if(msgIt!=it->second.end()){
				if(msgTimeInfo->resent){
					resentSeq.push_back(msgTimeInfo->msg);
					msgTimeInfo->time = time(NULL);
					msgTimeInfo->resent = false;
					_msgTimes.push_back(msgTimeInfo);

				}else{
					timeoutSeq.push_back(msgTimeInfo->msg);
					it->second.erase(msgIt);
				}
				//seq.push_back(msgIt->second);
				//it->second.erase(msgIt);
			}
			if(it->second.empty()){
				_userMsgs.erase(it);
			}
		}

	}
}

vector<MessagePtr> MessageTub::getUserMsgs(int userId){
	IceUtil::Mutex::Lock lock(_mutex);
	vector<MessagePtr> msgSeq;
	map<int, map<Ice::Long, MessagePtr> >::iterator it = _userMsgs.find(userId);
	if(it != _userMsgs.end()){
		for(map<Ice::Long, MessagePtr>::iterator sIt=it->second.begin(); sIt!=it->second.end(); ++sIt){
			msgSeq.push_back(sIt->second);
		}
		_userMsgs.erase(it);
	}
	return msgSeq;
}

//--------------------------------------------
/*
void OfflineMessageQuery::push(const string& sql) {
	IceUtil::Mutex::Lock lock(_mutexSqlToken);
	_sqlToken.push_back(sql);
}
*/
void OfflineMessageQuery::incOfflineMsgCount(int userId) {
	IceUtil::Mutex::Lock lock(_mutexUsers);

	map<int,int>::iterator it = _offlineMsgCount.find(userId);
	if (it == _offlineMsgCount.end()) {
		_offlineMsgCount[userId] = 1;
	}else{
		_offlineMsgCount[userId] += 1;
	}
}

void OfflineMessageQuery::_popOfflineMsgCountMap(map<int,int>& countMap) {
	IceUtil::Mutex::Lock lock(_mutexUsers);
	_offlineMsgCount.swap(countMap);
}

void OfflineMessageQuery::cleanOfflineMsgCount(int userId) {
	IceUtil::Mutex::Lock lock(_mutexUsers);
	_offlineMsgCount[userId] = 0;
}

int OfflineMessageQuery::getOfflineMsgCount(int usrId) {
	return ServiceI::instance().locateObject<LeavewordCountPtr>(LC, usrId)->count();
}

void OfflineMessageQuery::handle() {
	//TaskManager::instance().schedule(this);
	MCE_DEBUG("OfflineMessageQuery::handle --> save offline message");
	_processOfflineMsg();
	_processOfflineMsgCount();
}

void OfflineMessageQuery::_processOfflineMsgCount() {
	map<int,int> countMap;
	_popOfflineMsgCountMap(countMap);
	if (countMap.empty()) {
		return;
	}

	Statement sql1, sql2;

	int count1 = 0;
	int count2 = 0;
	for (map<int,int>::iterator it = countMap.begin(); it != countMap.end(); ++it) {
		if (it->second > 0) {
			LeavewordCountPtr lc = ServiceI::instance().findObject<LeavewordCountPtr>(LC, it->first);
			if(lc){
				lc->inc(it->second);
			}

			if(count1 == 0){
				sql1.clear();
				sql1 << "insert into offline_message_count (userid, count) values";
			}else{
				sql1 << ",";
			}
			count1++;
			sql1 << "(" << boost::lexical_cast<string>(it->first).c_str() << ","
					<< boost::lexical_cast<string>(it->second).c_str() << ")";
			if(count1%1000 == 0){
				sql1 << "on duplicate key update count = count + values(count)";
				QueryRunner("im", CDbWServer).schedule(sql1);
				count1 = 0;
			}
		} else if (it ->second == 0) {
			LeavewordCountPtr lc = ServiceI::instance().findObject<LeavewordCountPtr>(LC, it->first);
			if(lc){
				lc->clean();
			}
			if(count2 == 0){
				sql2.clear();
				sql2 << "insert into offline_message_count (userid, count) values";
			}else{
				sql2 << ",";
			}
			count2++;
			sql2 << "(" << boost::lexical_cast<string>(it->first).c_str()
			<< "," << boost::lexical_cast<string>(it->second).c_str() << ")";
			if(count2%1000 == 0){
				sql2 << "on duplicate key update count = values(count)";
				QueryRunner("im", CDbWServer).schedule(sql2);
				count2 = 0;
			}
		}
	}

	if(count1){
		sql1 << "on duplicate key update count = count + values(count)";
		QueryRunner("im", CDbWServer).schedule(sql1);
	}
	if(count2){
		sql2 << "on duplicate key update count = values(count)";
		QueryRunner("im", CDbWServer).schedule(sql2);
	}
}



void OfflineMessageQuery::_processOfflineMsg() {
	vector<MessagePtr> timeoutMsgs;
	vector<MessagePtr> resentMsgs;
	HolderManagerI::instance().getTimeoutMsgs(timeoutMsgs, resentMsgs);

	MCE_INFO("OfflineMessageQuery::_processOfflineMsg --> offline msg size="<<timeoutMsgs.size()
			<< "resent msg size=" << resentMsgs.size());

	//resend msg to other avatars
	if(!resentMsgs.empty()){
		IntSeq ids;
		for(size_t i=0; i<resentMsgs.size(); i++){
			ids.push_back(resentMsgs.at(i)->to->userId);
		}
		JidSeqMap userJids;
		try{
			userJids = OnlineCenterAdapter::instance().getUsersJids(ids);
		}catch(Ice::Exception& e){
			MCE_WARN("OfflineMessageQuery::_processOfflineMsg-->OnlineCenterAdapter::getUsersJids-->" <<e);
		}
		MessageSeq msgSeq;
		for(size_t i=0; i<resentMsgs.size(); i++){
			JidSeqMap::iterator it = userJids.find(resentMsgs.at(i)->to->userId);
			if(it!=userJids.end()){
				for(size_t j=0; j<it->second.size(); j++){
					if(it->second.at(j)==1 || it->second.at(j)==resentMsgs.at(i)->to){
						continue;
					}
					MessagePtr m = new Message;
					m -> from = resentMsgs.at(i)->from;
					m -> to = it->second.at(j);
					m -> type = resentMsgs.at(i)->type;
					m -> msg = resentMsgs.at(i)->msg;
					m -> msgKey = resentMsgs.at(i)->msgKey;
					msgSeq.push_back(m);
				}
			}
		}
		try{
			MCE_DEBUG("OnlineCenterAdapter::deliver --> deliver size=" << msgSeq.size());
			TalkDeliverAdapter::instance().deliver(msgSeq);
		}catch(Ice::Exception& e){
			MCE_WARN("OfflineMessageQuery::_processOfflineMsg-->OnlineCenterAdapter::deliver-->"<<e);
		}
	}

  MCE_INFO("@@@@@@@@ process offline msgs");
  vector<int> fromids;
	map<int, string> names;
  //save offline msg to database
  try{
	if(!timeoutMsgs.empty()){
		MCE_DEBUG("OfflineMessageQuery::_processOfflineMsg --> timeout msg size " << timeoutMsgs.size());
    ConnectionHolder conn("im", CDbWServer," ");
		mysqlpp::Query query = conn.query();
		//Statement sql;
		query << "insert into offline_message (id, fromid, toid, message, time) values";
		int count=0;
		for (size_t i = 0; i < timeoutMsgs.size(); i++) {
      fromids.push_back(timeoutMsgs.at(i)->from->userId);
			incOfflineMsgCount(timeoutMsgs.at(i)->to->userId);

			xml_document doc;
			if (!doc.load(timeoutMsgs.at(i)->msg.c_str())) {
				continue;
			}
			xml_node messageNode = doc.child("message");
			xml_node bodyNode = messageNode.child("body");
			if (bodyNode.empty()) {
				continue;
			}

			messageNode.append_attribute("from").set_value(jidToString(timeoutMsgs.at(i)->from,
					false).c_str());

			size_t tm =:: time(NULL);
			xml_node delayNode = messageNode.append_child();
			delayNode.set_name("delay");
			delayNode.append_attribute("xmlns") = "urn:xmpp:delay";
			delayNode.append_attribute("from") = "talk.xiaonei.com";
			delayNode.append_attribute("stamp") = Date(tm).str(
					"%Y-%m-%dT%H:%M:%S+08:00Z").c_str();
			delayNode.append_child(node_pcdata).set_value("Offline Storage");

			if (count)
				query << ",";
			query << "(" << timeoutMsgs.at(i)->msgKey << "," << timeoutMsgs.at(i)->from->userId << "," << timeoutMsgs.at(i)->to->userId << ","
					<< mysqlpp::quote << doc.xml() << "," << ::time(NULL) << ")";
			count++;
		}
		//MCE_DEBUG("OfflineMessageQuery::_processOfflineMsg -- > " << sql);
		if(count>0){
			query.store();
			//QueryRunner("im", CDbWServer).schedule(sql);
		}
	}
  }catch(std::exception& e){
    MCE_WARN("####### " << e.what());
  }catch(...){
    MCE_WARN("####### exception ");
  }
  MCE_INFO("@@@@@@@@ --> from size:" << fromids.size());
  if(fromids.empty()){
    MCE_INFO("OfflineMessageQuery::_processResentMsg --> fromid empty");
    return;
  }
  //ObjectResultPtr uPtr;
  map<int, UserCachePtr> userinfos;
  try {
    for(size_t i=0; i<fromids.size(); i++){
      MCE_INFO("call TalkCache --> " << fromids.at(i));
    }
		userinfos = UserCacheAdapter::instance().GetUserCache(fromids);
  } catch (Ice::Exception& e) {
    MCE_WARN("OfflineMessageQuery::_processOfflineMsg-->userCacheAdapter::GetUserCache-->" << e);
  }
  MCE_INFO("userCacheAdapter::GetUserCache --> get UserCacheList " << fromids.size() << " props:" << userinfos.size());
  for(size_t i=0; i<timeoutMsgs.size(); i++){
    MessagePtr msg = timeoutMsgs.at(i);
    try{
      map<string, string> pars;
      pars["schema_id"] = "14";
      pars["source"] = lexical_cast<string>(msg->to->userId);
      pars["owner"] = lexical_cast<string>(msg->to->userId);
      pars["from"] = lexical_cast<string>(msg->from->userId);
      pars["to"] = lexical_cast<string>(msg->to->userId);
      pars["type"] = "14";
      pars["time"] = lexical_cast<string>(time(NULL));
      pars["from_name"] = userinfos[msg->from->userId]->name();
      pars["whisper"] = "&whisper=1";

      MCE_INFO("OfflineMessageQuery::_processResentMsg --> call notify, from:" << msg->from->userId << " to:" << msg->to->userId);
      NotifyDispatcherAdapter::instance().dispatch(pars);
    }catch(KeyAbsentException& e){
      MCE_WARN("OfflineMessageQuery::_processOfflineMsg-->NotifyDispatcherAdapter::dispatch-->" << e.what());
    }catch(Ice::Exception& e){
      MCE_WARN("OfflineMessageQuery::_processOfflineMsg-->NotifyDispatcherAdapter::dispatch-->" << e);
    }catch(std::exception& e){
      MCE_WARN("OfflineMessageQuery::_processOfflineMsg-->call NotifyDispatcherAdapter::dispatch-->" << e.what());
    }
  } 
}

//---------------------------------------------
void HolderManagerI::message(const MessagePtr& msg, const Ice::Current&) {
	_tubs[msg->to->userId % POOL_SIZE].insert(msg);
	incTotalMsgCount();
	try{
	  TalkStatAdapter::instance().msgStat(msg->from, msg->to);
	}
	catch(Ice::Exception& e){
		MCE_WARN("HolderManagerI::message-->call TalkStatAdapter::msgStat-->" << e);	
	}
}

void HolderManagerI::send(int userId, Ice::Long messageKey, const Ice::Current&) {
	MCE_DEBUG("HolderManagerI::send --> msgKey:" << messageKey);
	incChatingMsgCount();
	_tubs[userId % POOL_SIZE].send(userId, messageKey);
}

void HolderManagerI::cleanOfflineMessageCount(int userid, const Ice::Current&) {
	MCE_DEBUG("HolderManagerI::cleanOfflineMessageCount");
	OfflineMessageQuery::instance().cleanOfflineMsgCount(userid);
}

int HolderManagerI::getOfflineMessageCount(int userid, const Ice::Current&) {
	return ServiceI::instance().locateObject<LeavewordCountPtr>(LC, userid)->count();
}

void HolderManagerI::loadOfflineMessage(const JidPtr& jid, const Ice::Current&){
	MCE_DEBUG("HolderManagerI::loadOfflineMessage --> " << jid->userId);
	TaskManager::instance().execute(new LoadOfflineMessageTask(jid));
}

void HolderManagerI::LoadUnreadNotifyInfoCount(const JidPtr& jid, const Ice::Current&){
	if(jid->endpoint[0] != 'W'){
		return;
	}
	TaskManager::instance().execute(new LoadUnreadNotifyInfoCountTask(jid));
}

void HolderManagerI::getTimeoutMsgs(vector<MessagePtr>& timeoutSeq, vector<MessagePtr>& resentSeq){
	for(int i=0; i<POOL_SIZE; i++){
		_tubs[i].getTimeoutMsgs(timeoutSeq, resentSeq);
	}
}

vector<MessagePtr> HolderManagerI::getUserMsgs(int userId){
	return _tubs[userId%POOL_SIZE].getUserMsgs(userId);
}

//---------------------------------------------------------------------------------------

void LoadUnreadNotifyInfoCountTask::handle(){
	
	int unreadcount = 0;
	try{
		unreadcount = NotifyIndexAdapter::instance().getUnreadCountByView(_jid->userId, 1);
	}catch(Ice::Exception& e){
		MCE_WARN("LoadUnreadNotifyInfoCountTask::handle --> jid:" << jidToString(_jid) << " Error : " << e);
	}
	if(!unreadcount){
		return;
	}
	MCE_INFO("LoadUnreadNotifyInfoCountTask::handle --> jid:" << jidToString(_jid) << " has " << unreadcount << " unread infos");
	MessageSeq seq;
	MessagePtr msg = new Message();
	msg->from = _jid;
	msg->to = _jid;
	msg->type = NOTIFY_MESSAGE;
	ostringstream messageos;
	string json = "{type:'info_unread_count',count:" + boost::lexical_cast<string>(unreadcount) + "}";
	messageos << "<message from=\"" << jidToString(_jid) << "\"> <body>" << json << "</body> </message>";
	msg->msg = messageos.str();
	seq.push_back(msg);
	try{
		TalkDeliverAdapter::instance().deliver(seq);
	}catch(Ice::Exception& e){
		MCE_WARN("LoadUnreadNotifyInfoCountTask::handle --> TalkDeliverAdapter::deliver --> error : " << e);
	}
}


void LoadOfflineMessageTask::handle() {
	MCE_DEBUG("LoadOfflineMessageTask::handle --> userId=" << _jid->userId);
	MessageSeq seq;
	vector<MessagePtr> msgs = HolderManagerI::instance().getUserMsgs(_jid->userId);
	for(size_t i=0; i<msgs.size(); i++){
		msgs.at(i)->type = OFFLINE_MESSAGE;
		msgs.at(i)->to = _jid;

		xml_document doc;
		if (!doc.load(msgs.at(i)->msg.c_str())) {
			continue;
		}
		xml_node messageNode = doc.child("message");
		xml_node bodyNode = messageNode.child("body");
		if (bodyNode.empty()) {
			continue;
		}
		messageNode.append_attribute("from").set_value(jidToString(msgs.at(i)->from,
				false).c_str());

		size_t tm =:: time(NULL);
		xml_node delayNode = messageNode.append_child();
		delayNode.set_name("delay");
		delayNode.append_attribute("xmlns") = "urn:xmpp:delay";
		delayNode.append_attribute("from") = "talk.xiaonei.com";
		delayNode.append_attribute("stamp") = Date(tm).str(
				"%Y-%m-%dT%H:%M:%S+08:00Z").c_str();
		delayNode.append_child(node_pcdata).set_value("Offline Storage");
		msgs.at(i)->msg = doc.xml();
	}
	MCE_INFO("@@@ in mem " << _jid->userId << " " << msgs.size())
	size_t now_tm =:: time(NULL);
	try {
		Statement sql;
		sql << "select * from offline_message where toid=" << boost::lexical_cast<string>(_jid->userId).c_str();
#ifndef NEWARCH
		mysqlpp::Result res = QueryRunner("im", CDbRServer).store(sql);
#else
		mysqlpp::StoreQueryResult res = QueryRunner("im", CDbRServer).store(sql);
#endif
		
#ifndef NEWARCH
		for (size_t i = 0; i < res.size(); ++i) {
#else
		for (size_t i = 0; i < res.num_rows(); ++i) {
#endif
			mysqlpp::Row row = res.at(i);
			MessagePtr msg = new Message();
			msg->from = new Jid();
			msg->from->userId = row["fromid"];
			msg->from->index = -1;

			msg->to = _jid;
			msg->type = OFFLINE_MESSAGE;
#ifndef NEWARCH
			msg->msg = row["message"].get_string();
#else
			msg->msg = row["message"].c_str();
#endif
			if(_jid->endpoint[0] == 'T'){
				if(now_tm - row["time"] <= 432000){
					seq.push_back(msg);
				}
			}else{
				seq.push_back(msg);
			}
		}
		if (!res.empty()) {
			sql.clear();
			sql << "delete from offline_message where toid=" << boost::lexical_cast<string>(_jid->userId).c_str();
			QueryRunner("im", CDbWServer).schedule(sql);
		}

	} catch(mysqlpp::Exception& e) {
		MCE_WARN("LoadOfflineMessageTask::handle doselect leaveword err");
	} catch(...){
		MCE_WARN("LoadOfflineMessageTask::handle do select err");
	}

	MCE_INFO("@@@ in db" << _jid->userId << " " << seq.size());
	seq.insert(seq.end(), msgs.begin(), msgs.end());
	if(seq.empty()){
		return;
	}
	try{
		MCE_DEBUG("LoadOfflineMessageTask::handle --> deliver size=" << seq.size());
		if(!_jid->endpoint.empty() && _jid->endpoint.at(0)=='T'){
			MCE_INFO("LoadOfflineMessageTask::handle-->deliver offline message to " << jidToString(_jid) << " size = " << seq.size());
      TalkDeliverAdapter::instance().deliver(seq);
    }
	}catch(mysqlpp::Exception& e) {
		MCE_WARN("LoadOfflineMessageTask::handle-->TalkDeliverAdapter::deliver-->sqlerror");
	}catch(Ice::Exception& e){
		MCE_WARN("LoadOfflineMessageTask::handle-->TalkDeliverAdapter::deliver-->" << e);	
	}

	HolderManagerI::instance().cleanOfflineMessageCount(_jid->userId);
}
