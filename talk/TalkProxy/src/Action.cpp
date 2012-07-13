#include "Action.h"
#include "Manager.h"
#include "Server.h"
#include "TalkCacheAdapter.h"

#include "ConnectionQuery.h"
#include "XmppTools.h"
#include "Base64.h"
#include "BuddyCoreAdapter.h"
//#include "TalkLogicAdapter.h"
#include "TalkRightAdapter.h"
#include "MessageType.h"
#include "CountCacheAdapter.h"
#include "PresenceAdapter.h"
//#include "TalkPresenceAdapter.h"
//#include "TalkIqAdapter.h"
//#include "TalkMessageAdapter.h"
#include "IqMsgHandlerAdapter.h"
#include "PresenceMsgHandlerAdapter.h"
#include "MessageMsgHandlerAdapter.h"
#include "LoginMsgHandlerAdapter.h"

#include "MucPresenceMsgHandlerAdapter.h"
#include "MucPresenceAdapter.h"
#include "MucMessageAdapter.h"
//#include "TalkLogAdapter.h"
#include "XmppTools.h"
#include <util/cpp/String.h>

using namespace com::xiaonei::talk;
using namespace com::xiaonei::xce;
using namespace com::xiaonei::talk::util;
using namespace mop::hi::svc::adapter;
using namespace com::xiaonei::service::buddy;
using namespace com::xiaonei::talk::adapter;
using namespace talk::adapter;
//using namespace xiaonei::talk;
//---------------------------------------------------------------------------
//void StartAction::execute() {
//	Server::instance().apply(_session);
//}
//
//void StopAction::execute() {
//	try {
//		if(_session->jid()) {
//			TalkLogicAdapter::instance().unbind(_session->jid());
//		}
//	} catch(Ice::Exception& e) {
//		MCE_WARN("StopAction::execute -> TalkLogicAdapter::unbind err:"<<e);
//	} catch(...) {
//		MCE_WARN("StopAction::execute -> TalkLogicAdapter::unbind err");
//	}
//	Server::instance().leave(_session);
//}

void StreamAction::execute() {
	//std::string version = tag->findAttribute("version");
	std::string version = _doc->first_child().attribute("version").value();

	if ((version != "")&&( !verifyVersion(version) )) {
		MCE_WARN(*_session<<"This client is not XMPP-compliant"
			" (it does not send a 'version' attribute). Please fix it or try another one.");
		_session->deliver(buildStreamEnd());
		_session->setShutdownAt(1);
		_session->shutdown();
		return;
	}
	MCE_DEBUG(*_session<< " StreamAction::handle --> session stat:"
			<<_session->state());
	_session->deliver(buildStreamBegin());
	if (_session->state() == SessionStateNone) {
		_session->state(SessionStateFirstStream);
		_session->deliver(buildFirstStreamFeature());
	} else if (_session->state() == SessionStateFirstStream) {
		_session->state(SessionStateSecondStream);
		_session->deliver(buildSecondStreamFeature());
	} else {
		_session->deliver(buildBindSessionFeature());
	}
}

bool StreamAction::verifyVersion(const std::string& version) {
	if (version.empty()) {
		return false;
	}

	int maj = 0;
	int min = 0;
	int myMajor = 1;

	size_t dot = version.find(".");
	if ( !version.empty() && dot && dot != std::string::npos) {
		maj = atoi(version.substr( 0, dot ).c_str() );
		min = atoi(version.substr( dot ).c_str() );
	}

	return myMajor >= maj;
}

string StreamAction::buildStreamBegin() {
	return "<?xml version='1.0'?><stream:stream from='" + TALKDOMAIN
			+ "' xmlns='jabber:client' xmlns:stream='http://etherx.jabber.org/streams' version='1.0'>";
}

string StreamAction::buildStreamEnd() {
	return "</stream:stream>";
}

string StreamAction::buildStreamError(const string& text) {
	return "<stream:error><defined-condition xmlns='urn:ietf:params:xml:ns:xmpp-streams'/>"
		"<text xmlns='urn:ietf:params:xml:ns:xmpp-streams' xml:lang='langcode'>"
			+ text + "</text></stream:error><error/>";
}

string StreamAction::buildFirstStreamFeature() {
	//return "<stream:features><starttls xmlns='urn:ietf:params:xml:ns:xmpp-tls'><required/></starttls>"
	//	"<mechanisms xmlns='urn:ietf:params:xml:ns:xmpp-sasl'><mechanism>PLAIN</mechanism></mechanisms></stream:features>";

	return "<stream:features><starttls xmlns='urn:ietf:params:xml:ns:xmpp-tls'/>"
		"<mechanisms xmlns='urn:ietf:params:xml:ns:xmpp-sasl'><mechanism>PLAIN</mechanism></mechanisms>"
		"</stream:features>";
}

string StreamAction::buildSecondStreamFeature() {
	return "<stream:features><mechanisms xmlns='urn:ietf:params:xml:ns:xmpp-sasl'>"
		"<mechanism>PLAIN</mechanism></mechanisms></stream:features>";
}
string StreamAction::buildBindSessionFeature() {
	return "<stream:features><bind xmlns='urn:ietf:params:xml:ns:xmpp-bind'/>"
		"<session xmlns='urn:ietf:params:xml:ns:xmpp-session'/></stream:features>";
}
//---------------------------------------------------------------------------
void StartTlsAction::execute() {
	LoginProcedureStat::instance().incTls();
	_session->deliver(buildTlsProceed());
	_session->handshake();
}

string StartTlsAction::buildTlsProceed() {
	return "<proceed xmlns='urn:ietf:params:xml:ns:xmpp-tls'/>";
}

string StartTlsAction::buildTlsFailure() {
	return "<failure xmlns='urn:ietf:params:xml:ns:xmpp-tls'/>";
}

void AuthAction::execute() {
	LoginProcedureStat::instance().incVerify();
	string data = Base64::decode64(_doc->first_child().first_child().value());
	size_t first = data.find_first_of('\0');
	size_t last = data.find_last_of('\0');
	if (first == string::npos || last == string::npos) {
		_session->deliver(buildSaslFailure("not-authorized"));
		_session->setShutdownAt(2);
		_session->shutdown();
		return;
	}

	string user = data.substr(first+1, last - first - 1);
	string pass = data.substr(last+1);
	MCE_DEBUG(*_session<<"SessionInSASL::handle -> " << user << ":" << pass);

	common::PassportPtr passport;
	//try two times
	for(int i=0; i<2; i++){
		try {
			passport = LoginMsgHandlerAdapter::instance().verify(user, pass);
		} catch(Ice::Exception& e) {
			MCE_WARN(*_session<<"AuthAction::handle() --> user:"<<user<<"  password:"<<pass<<"  err:"<<e);
			continue;
			// should deliver some error message to client, then shutdown the session
			//		_session->deliver(buildSaslFailure("not-authorized"));
			//_session->setShutdownAt(3);
			//_session->shutdown();
			//return;
		}
		break;
	}
	if(!passport){
		MCE_WARN("AuthAction::handle() --> verify exception user:"<<user<<"  password:"<<pass);
                // should deliver some error message to client, then shutdown the session
                //_session->deliver(buildSaslFailure("not-authorized"));
                _session->setShutdownAt(3);
                _session->shutdown();
                return;
	}

	JidPtr jid = new Jid;
	jid->userId = passport->userId;
	jid->endpoint = ServiceI::instance().getName();
	jid->index = _session->connectionId();
	_session->jid(jid);


	if (passport->result == -1) {
		// user or password invalid
		_session->deliver(buildSaslFailure("not-authorized"));
		MCE_WARN(*_session<<"VerifyUserPassAction::handle -> not-authorized -1 ->" << user << ":" << pass);
		_session->setShutdownAt(4);
		_session->shutdown();
	} else if (passport->result == -2) {
		// invalid user status
		_session->deliver(buildSaslFailure("invalid-authzid"));
		MCE_WARN(*_session<<"VerifyUserPassAction::handle -> not-authorized -2 ->"
				<< user << ":" << pass);
		_session->setShutdownAt(5);
		_session->shutdown();
	} else {
		MCE_INFO(*_session<<"AuthAction::handle --> verify success");
		ostringstream msg;
		msg
				<< "<success xmlns='urn:ietf:params:xml:ns:xmpp-sasl' xmlns:xn='xiaonei:verify' xn:ticket='NULL'/>";
		_session->deliver(msg.str());
		_session->state(SessionStateLogin);
	}
}

void AuthAction::exception(Ice::Exception& e) {
	MCE_WARN(*_session<<"AuthTask::exception -> " << e);
	//_session->shutdown();
}

string AuthAction::buildSaslSuccess() {
	return "<success xmlns='urn:ietf:params:xml:ns:xmpp-sasl'/>";
}

string AuthAction::buildSaslFailure(const string& tag) {
	return "<failure xmlns='urn:ietf:params:xml:ns:xmpp-sasl'><" + tag
			+ "/></failure>";
}

void IqAction::execute() {


	string type = _doc->child("iq").attribute("type").value();
	if (type == "set" && _doc->child("iq").child("bind")) {
		//JidPtr j;
		LoginProcedureStat::instance().incBind();
		if(_doc->child("iq").child("bind").child("resource").first_child()){
			string version = _doc->child("iq").child("bind").child("resource").first_child().value();
			MCE_INFO("VERSION:"<<version);
		}
		//bool flag = true;


		try {
			Ice::Context ctx;
                        ctx["ip"] = _session->getRemoteAddr();
			LoginMsgHandlerAdapter::instance().bind(_session->jid(), ctx);
			MCE_DEBUG("IqAction::execute ---> call LoginMsgHandlerAdapter -> bing( jid = " << _session->jid() << " ip = " << ctx["ip"]);
		} catch(Ice::Exception& e) {
			MCE_WARN(*_session << " IqTask::handle<IqSetBind> --> jid bind err: "<< e << " --> will bind again");
		}
		/*if (!flag) {
			_session->shutdown();
			MCE_WARN(*_session << " IqTask::handle<IqSetBind> --> bind err");
			return;
		}*/
		//_session->jid(j);
		//Server::instance().join(_session);

		xml_document reply;
		xml_node iqNode = reply.append_child();
		iqNode.set_name("iq");
		iqNode.append_attribute("id") = _doc->child("iq").attribute("id").value().c_str();
		iqNode.append_attribute("type") = "result";

		xml_node bindNode = iqNode.append_child();
		bindNode.set_name("bind");
		bindNode.append_attribute("xmlns") = "urn:ietf:params:xml:ns:xmpp-bind";
		bindNode.append_attribute("xmlns:xn") = "xiaonei:verify";
		bindNode.append_attribute("xn:ticket") = _session->ticket().c_str();
		xml_node jidNode = bindNode.append_child();
		jidNode.set_name("jid");
		string jidStr = jidToString(_session->jid());
		if(Server::instance().isRenren()){
			MyUtil::StrUtil::string_replace(jidStr, "xiaonei", "renren");
		}
		jidNode.append_child(node_pcdata).set_value(jidStr.c_str());
		_session->deliver(reply.xml());
	} else {
		MCE_DEBUG("IqAction::execute --> call TalkIqAdapter");
		IqMsgHandlerAdapter::instance().message(_session->jid(), _doc->xml());
	}
}

void PresenceAction::execute() {
	/*	if (_doc->child("presence").attribute("type").value() == "unavailable") {
	 // One client turns his status to hidden
	 // One client exits a room
	 MCE_DEBUG("PresenceAction::execute --> ");
	 if(_doc->child("presence").attribute("to")){
	 MCE_DEBUG("PresenceAction::execute --> invoke");
	 TalkPresenceAdapter::instance().message(_session->jid(), _doc->xml());
	 MCE_DEBUG("PresenceAction::execute --> TalkPresence end");
	 }

	 return;
	 }*/


	string to = _doc->child("presence").attribute("to").value();
	if (to.empty()) {
		_session->presence(_doc->xml());
	} else {
		MCE_DEBUG("presence to muc, " << to);
		IdType type = idType(to);
		if (type == ROOMID || type == ROOMJID) {
			MCE_DEBUG("presence to muc, " << to);
      //MucPresenceAdapter::instance().message(_session->jid(), _doc->xml());
			MucPresenceMsgHandlerAdapter::instance().message(_session->jid(), _doc->xml());
		} else if (type == JID) {
			_session->presence(_doc->xml());
		} else {
			MCE_INFO("DefultAction::execute --> invalid id");
		}

	}


	if (_session->state() == SessionStateLogin) {
		LoginProcedureStat::instance().incPresence();
		string type = _doc->child("presence").attribute("type").value();
		if (type != "subscribe" && type != "subscribed" && type
				!= "unsubscribed") {
			string ver = "";
			string subver = "";
			string ext = "";
			string show = "";
			string secver = "";
			xml_node cNode = _doc->child("presence").child("c");
			if (cNode) {
				ver = cNode.attribute("ver").value();
				ext = cNode.attribute("ext").value();
				subver = cNode.attribute("subver").value();
				secver = cNode.attribute("secver").value();
			}
			xml_node showNode = _doc->child("presence").child("show");
			if (showNode) {
				show = showNode.first_child().value();
			}
			try {
				PresenceMsgHandlerAdapter::instance().presenceWithSecver(_session->jid(),show,ver,subver,ext, secver);
			} catch(Ice::Exception& e) {
				MCE_WARN("PresenceAction::handle --> talklogic presence err :"<<e);
			}
			_doc->child("presence").remove_child("status");
			_session->state(SessionStateChat);
		}
	}

}

void DefultAction::execute() {

	if (_session->state() != SessionStateChat) {
		MCE_WARN(*_session
				<<" --> not login and want to send message,shutdown it now!!!");
		_session->setShutdownAt(6);
		_session->shutdown();
		return;
	}

	string to = _doc->child("message").attribute("to").value();
	if (!to.empty()) {
		IdType type = idType(to);
		if (type == ROOMID || type == ROOMJID) {
			MCE_DEBUG("msg to muc, " << to);
			MucMessageAdapter::instance().message(_session->jid(), _doc->xml());
		} else if (type == JID) {
			MessageMsgHandlerAdapter::instance().message(_session->jid(), _doc->xml());
		} else {
			MCE_INFO("DefultAction::execute --> invalid id");
		}
	} else {
		Ice::Context ctx;
                ctx["ip"] = _session->getRemoteAddr();
		MessageMsgHandlerAdapter::instance().message(_session->jid(), _doc->xml(), ctx);
	}
}

//---------------------------------------------------------------------------
//void SessionListener::started(Ice::ObjectPtr channel) {
//	SessionPtr session = SessionPtr::dynamicCast(channel);
//	session->schedule(new StartAction, 0);
//}
//
//void SessionListener::stopped(Ice::ObjectPtr channel) {
//	SessionPtr session = SessionPtr::dynamicCast(channel);
//	session->schedule(new StopAction, 0);
//}

void SessionListener::messaged(SharedPtr channel, xml_document_ptr& doc) {
	SessionPtr session = SessionPtr::dynamicCast(channel);
	session->clearCounter();
	if(Server::instance().isRenren()){
		string str = doc->xml();
		changeDomain(str, "renren.com", "xiaonei.com");
		if(!doc->load(str.c_str())){
			MCE_WARN("SessionListener::messaged --> change domain error");
			return;
		}
	}
	string name = doc->first_child().name();
	if (name == "stream:stream") {
		session->schedule(new StreamAction, doc);
	} else if (name == "starttls") {
		session->execute(new StartTlsAction, doc);
	} else if (name == "auth") {
		session->schedule(new AuthAction, doc);
	} else if (name == "iq") {
		session->schedule(new IqAction, doc);
	} else if (name == "presence") {
		//session->schedule(new PresenceAction, doc);
		//do not start a thread, or a stop may be processed before this presence
		processPresence(session, doc);
	} else {
		session->schedule(new DefultAction, doc);
	}
}
void SessionListener::processPresence(const SessionPtr& session, const xml_document_ptr& doc){
	string to = doc->child("presence").attribute("to").value();
	if (to.empty()) {
		session->presence(doc->xml());
	} else {
		MCE_DEBUG("presence to muc, " << to);
		IdType type = idType(to);
		if (type == ROOMID || type == ROOMJID) {
			MCE_DEBUG("presence to muc, " << to);
      //MucPresenceAdapter::instance().message(session->jid(), doc->xml());
			MucPresenceMsgHandlerAdapter::instance().message(session->jid(), doc->xml());
		} else if (type == JID) {
			session->presence(doc->xml());
		} else {
			MCE_INFO("DefultAction::execute --> invalid id");
		}

	}

	if (session->state() == SessionStateLogin) {
		LoginProcedureStat::instance().incPresence();
		string type = doc->child("presence").attribute("type").value();
		if (type != "subscribe" && type != "subscribed" && type
				!= "unsubscribed") {
			string ver = "";
			string subver = "";
			string ext = "";
			string secver = "";
			string show = "";
			xml_node cNode = doc->child("presence").child("c");
			if (cNode) {
				ver = cNode.attribute("ver").value();
				ext = cNode.attribute("ext").value();
				subver = cNode.attribute("subver").value();
				secver = cNode.attribute("secver").value();
			}
			xml_node showNode = doc->child("presence").child("show");
			if (showNode) {
				show = showNode.first_child().value();
			}
			try {
				PresenceMsgHandlerAdapter::instance().presenceWithSecver(session->jid(),show,ver,subver,ext,secver);
			} catch(Ice::Exception& e) {
				MCE_WARN("PresenceAction::handle --> talklogic presence err :"<<e);
			}
			doc->child("presence").remove_child("status");
			session->state(SessionStateChat);
		}
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
//---------------------------------------------------------------------------

void OnlineSizeMoniter::handle() {

	//TaskManager::instance().schedule(this);
	int applySize = Server::instance().size(true);
	int onlineSize = Server::instance().size(false);

	MCE_INFO("STAT_ONLINE_SIZE -->  TOTAL_ONLINE_SIZE:"<<onlineSize
			<< "   APPLY_SIZE:"<<applySize);
}

