#include "Action.h"
#include "Manager.h"
#include "Server.h"
#include "TalkCacheAdapter.h"

#include "ConnectionQuery.h"
#include "XmppTools.h"

//#include "TalkLogicAdapter.h"
#include "TalkRightAdapter.h"
#include "MessageType.h"
#include "CountCacheAdapter.h"
#include <boost/lexical_cast.hpp>
//#include "TalkPresenceAdapter.h"
//#include "TalkIqAdapter.h"
//#include "TalkMessageAdapter.h"
#include "MessageMsgHandlerAdapter.h"

using namespace com::xiaonei::talk;
using namespace com::xiaonei::xce;
using namespace com::xiaonei::talk::util;
using namespace mop::hi::svc::adapter;
using namespace com::xiaonei::talk::adapter;
using namespace talk::adapter;

/*
void StartAction::execute() {
	Server::instance().apply(_session);
}

void StopAction::execute() {
	MCE_DEBUG("StopAction::execute()");
	try {
		if(_session->jid()) {
			WTalkProxyAdapter::instance().unbind(_session->token(), _session->jid());
		}
	} catch (...) {
		MCE_WARN("StopAction::execute -> call unbind err -> ");
	}
	Server::instance().leave(_session);
}
*/

void LoginAction::execute() {
	xml_node hiNode = _doc->child("hi");
	string userid = hiNode.child("userid").child_value();
	string ticket = hiNode.child("ticket").child_value();
	string token = hiNode.child("session").child_value();

	MCE_INFO("LoginAction::execute -> userid: " << userid << ", ticket: "
			<< ticket << ", token: " << token);
	JidPtr jid = new Jid();
	try {
		jid->userId = boost::lexical_cast<int>(userid);
	} catch (std::exception& e) {
		MCE_WARN("LoginAction::execute -> invalid userid: " << userid << ", " << e.what());
		_session->shutdown();
		return;
	} catch (...) {
		MCE_WARN("LoginAction::execute -> invalid userid: " << userid);
		_session->shutdown();
		return;
	}
	jid->endpoint = ServiceI::instance().getName();
	_session->jid(jid);
	bool success = false;
	try {
		success = _session->bind(token, ticket);//WTalkProxyAdapter::instance().bind(token, ticket, jid);
	} catch (Ice::Exception& e) {
		MCE_WARN("LoginAction::execute -> failure binding, jid: " << jidToString(jid) << ", " << e);
		_session->shutdown();
		return;
	}
	if (success) {
		//_session->jid(jid);
		_session->token(token);
		_session->verify(true);

		//Server::instance().join(_session, jid);
		string
				msg =
						"<?xml version=\"1.0\" encoding=\"utf-8\"?><hi cmd=\"login_res\"><res>0</res></hi>";
		msg.push_back('\0');
		if (_session->deliver(msg)) {
			MCE_INFO("LoginAction::execute -> success -> userid:"<<userid
					<<"  ticket:"<<ticket);
		} else {
			//Server::instance().leave(_session);
			_session->shutdown();
		}
	} else {
		MCE_WARN("LoginAction::execute -> failure binding, userid:" << userid);
		string
				msg =
						"<?xml version=\"1.0\" encoding=\"utf-8\"?><hi cmd='login_res'><res>1</res></hi>";
		msg.push_back('\0');
		_session->deliver(msg);
		_session->shutdown();
	}
}

void ChatAction::execute() {
	MCE_DEBUG("call MessageTask::handle()");
	if (!_session->verify()) {
		_session->shutdown();
		return;
	}
	xml_node hi = _doc->child("hi");
	string toid = hi.child("toid").child_value();
	string content = hi.child("content").child_value();

	ostringstream msg;
	msg << "<message to='"<<toid<<"@talk.xiaonei.com' from='"
			<<jidToString(_session->jid())<<"' type='chat'><body>"<<content
			<<"</body></message>";

	MCE_INFO("Send msg : "<< msg.str());

	MessageMsgHandlerAdapter::instance().message(_session->jid(), msg.str());
}

//---------------------------------------------------------------------------
/*
void SessionListener::started(Ice::ObjectPtr channel) {
	SessionPtr session = SessionPtr::dynamicCast(channel);
	session->schedule(new StartAction, 0);
}

void SessionListener::stopped(Ice::ObjectPtr channel) {
	SessionPtr session = SessionPtr::dynamicCast(channel);
	session->schedule(new StopAction, 0);
}
*/

void SessionListener::messaged(SharedPtr channel, xml_document_ptr& doc) {
	MCE_DEBUG("RequestHandler::handle -> " << doc->xml());
	SessionPtr session = SessionPtr::dynamicCast(channel);
	string name = doc->first_child().attribute("cmd").value();
	if (name == "login") {
		//MCE_DEBUG("SessionListener::messaged --> CONN_ID:"<<session->connectionId());
		//session->schedule(new LoginAction, doc);
		//do not start a thread, otherwise a stop may be processed before this login
		processPresence(session, doc);

	} else if (name == "msg") {
		session->schedule(new ChatAction, doc);
	} else {
		if (doc->child("policy-file-request")) {
			string msg =
			//xiaonei
							"<cross-domain-policy><allow-access-from domain=\"*.xiaonei.com\" to-ports=\"39000\" /></cross-domain-policy>";
			//kaixin
			//"<cross-domain-policy><allow-access-from domain=\"*.kaixin.com\" to-ports=\"39000\" /></cross-domain-policy>";

			msg.push_back('\0');
			session->deliver(msg);
			return;
		}
		session->shutdown();
	}
}

void SessionListener::processPresence(const SessionPtr& session, const xml_document_ptr& doc){
	xml_node hiNode = doc->child("hi");
	string userid = hiNode.child("userid").child_value();
	string ticket = hiNode.child("ticket").child_value();
	string token = hiNode.child("session").child_value();

	MCE_INFO("LoginAction::execute -> userid: " << userid << ", ticket: "
			<< ticket << ", token: " << token);
	JidPtr jid = new Jid();
	try {
		jid->userId = boost::lexical_cast<int>(userid);
	} catch (std::exception& e) {
		MCE_WARN("LoginAction::execute -> invalid userid: " << userid << ", " << e.what());
		session->shutdown();
		return;
	} catch (...) {
		MCE_WARN("LoginAction::execute -> invalid userid: " << userid);
		session->shutdown();
		return;
	}
	jid->endpoint = ServiceI::instance().getName();
	session->jid(jid);
	bool success = false;
	try {
		success = session->bind(token, ticket);//WTalkProxyAdapter::instance().bind(token, ticket, jid);
	} catch (Ice::Exception& e) {
		MCE_WARN("LoginAction::execute -> failure binding, jid: " << jidToString(jid) << ", " << e);
		session->shutdown();
		return;
	}
	if (success) {
		//_session->jid(jid);
		session->token(token);
		session->verify(true);

		//Server::instance().join(_session, jid);
		string
				msg =
						"<?xml version=\"1.0\" encoding=\"utf-8\"?><hi cmd=\"login_res\"><res>0</res></hi>";
		msg.push_back('\0');
		if (session->deliver(msg)) {
			MCE_INFO("LoginAction::execute -> success -> userid:"<<userid
					<<"  ticket:"<<ticket);
		} else {
			//Server::instance().leave(_session);
			session->shutdown();
		}
	} else {
		MCE_WARN("LoginAction::execute -> failure binding, userid:" << userid);
		string
				msg =
						"<?xml version=\"1.0\" encoding=\"utf-8\"?><hi cmd='login_res'><res>1</res></hi>";
		msg.push_back('\0');
		session->deliver(msg);
		session->shutdown();
	}
}

//---------------------------------------------------------------------------
void OnlineSizeStater::handle() {

	//TaskManager::instance().schedule(this);
	int applySize = Server::instance().size(true);
	int onlineSize = Server::instance().size(false);

	MCE_INFO("STAT_ONLINE_SIZE -->  TOTAL_ONLINE_SIZE:"<<onlineSize
			<< "   APPLY_SIZE:"<<applySize);

	ostringstream sql;
	sql << "insert into xntalk_snap (name, time,value) values ('"
			<<ServiceI::instance().getName()<<"_online_size',from_unixtime(" <<::time(NULL)
	<<"),"<<onlineSize <<"),('" <<ServiceI::instance().getName()<<"_apply_size',from_unixtime(" <<::time(NULL)<<"),"
	<<applySize <<")";

	ConnectionQuery conn("im_stat", CDbWServer);
	conn.doInsert(sql.str());
}

void OnlineSizeMoniter::handle() {

	//TaskManager::instance().schedule(this);
	int applySize = Server::instance().size(true);
	int onlineSize = Server::instance().size(false);


	MCE_INFO("STAT_ONLINE_SIZE -->  TOTAL_ONLINE_SIZE:"<<onlineSize
			<< "   APPLY_SIZE:"<<applySize);
}

