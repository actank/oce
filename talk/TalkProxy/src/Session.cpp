#include "Session.h"
#include "Manager.h"
#include "Server.h"
#include "Action.h"
//#include "TalkLogicAdapter.h"
#include "LoginMsgHandlerAdapter.h"
#include "PresenceMsgHandlerAdapter.h"


using namespace MyUtil;
using namespace com::xiaonei::talk;
//using namespace mop::hi::svc::adapter;
using namespace talk::adapter;
//----------------------------------------------------------------------
Session::Session() {
	_parser = new TagParser(&SessionListener::instance());
	_state = SessionStateNone;
	_counter = 0;
	_binding = 0;
	_shutdownAt = 0;
	//MCE_DEBUG("new session");
}

Session::~Session() {
	//MCE_DEBUG("delete session");
	Server::instance().leave();
	Date end(time(NULL));
	Date start(_timestamp);
	MCE_INFO("session start at " << start.toDateTime() << ", end at " << end.toDateTime()
			<< ", duration " << (end-start).toSeconds()
			<< ", state " << state()
			<< ", shutdown at " << _shutdownAt
			<< ", remote addr " << getRemoteAddr()
			<< ", jid " << jidToString(_jid));

}

void Session::started() {
	MCE_DEBUG((void*)this << "started");
	//MCE_DEBUG("apply_size:");
	Server::instance().join();
	LoginProcedureStat::instance().incConnect();
	_timestamp = time(NULL);
	//SessionListener::instance().started(this);
}
void Session::presence(const string& msg) {
	IceUtil::Mutex::Lock lock(_bindMutex);
	if (_binding == 0) {
		_binding = 1;
		Server::instance().bind();
	}

	try{
		if(_jid){
			MCE_INFO("~~Session::presence --> " << jidToString(_jid));
		}
		PresenceMsgHandlerAdapter::instance().message(_jid, msg);
	}catch(Ice::Exception& e){
		if(_jid){
			MCE_WARN("~~Session::presence --> err, " << jidToString(_jid) << ", " << e);
		}else{
			MCE_WARN("Session::presence --> " << e);
		}
	}
	//return true;
	//return LoginMsgHandlerAdapter::instance().bind(_jid);

}
void Session::stopped() {
	IceUtil::Mutex::Lock lock(_bindMutex);
	MCE_DEBUG((void*)this << "stop"<< "   binding = "<< _binding);
	if (_binding == 1) {
		//SessionListener::instance().stopped(this);
		try{
			LoginMsgHandlerAdapter::instance().unbind(jid());
			if(_jid){
				MCE_INFO("~~Session::stopped --> " << jidToString(_jid));
			}
			PresenceMsgHandlerAdapter::instance().offline(_jid);
		}catch(Ice::Exception& e){
			if(_jid){
				MCE_WARN("~~Session::stopped --> err, " << jidToString(_jid) << ", " << e);
			}else{
				MCE_WARN("Session::stopped --> unbind err:"<<e);
			}
		}catch(...){
			MCE_WARN("Session::stopped --> unbind exception");
		}
		Server::instance().unbind();
	}
	_binding = 2;
}

bool Session::messaged(const string& message) {
	if (_counter + message.length() > MAX_MSG_SIZE) {
		return false;
	} else {
		_counter += message.length();
		if (_parser) {
			_parser->feed(this, message);
		}
		return true;
	}
}

string& Session::ticket() {
	if (_ticket.empty()) {
		struct timeval tv;
		memset(&tv, 0, sizeof(tv));
		gettimeofday(&tv, NULL);
		ostringstream oss;
		oss <<_jid->userId<<"$"<<_jid->endpoint<<"$"<<_jid->index<< "$"
				<<tv.tv_sec << "$" << tv.tv_usec;

		_ticket = oss.str();
		MCE_INFO((void*)this << "ticket -> create a ticket:\"" << _ticket<<"\"");

	}
	return _ticket;
}

//bool Session::saslHandshake(SaslHandshakeStep step, const string& instr) {
//
//	switch (step) {
//	case SaslStart: {
//		int ret = gsasl_init(&_sasl_ctx);
//		if (ret != GSASL_OK) {
//			MCE_WARN("Session::saslHandshake --> gsasl init err  errcode:"<<ret);
//			gsasl_done(_sasl_ctx);
//			_sasl_ctx = NULL;
//			return false;
//		}
//		gsasl_callback_set(_sasl_ctx, sasl_callback);
//
//		ret = gsasl_server_start(_sasl_ctx, "DIGEST-MD5", &_sasl_session);
//
//		if (ret != GSASL_OK) {
//			MCE_WARN("Session::saslHandshake --> gsasl server start err, errcode:"
//					<<ret);
//			gsasl_finish(_sasl_session);
//			_sasl_session = NULL;
//			gsasl_done(_sasl_ctx);
//			_sasl_ctx = NULL;
//			return false;
//		}
//		gsasl_property_set(_sasl_session, GSASL_SERVICE, "xntalk");
//		gsasl_property_set(_sasl_session, GSASL_REALM, "talk.xiaonei.com");
//		gsasl_property_set(_sasl_session, GSASL_HOSTNAME, "talk.xiaonei.com");
//
//		char* out = NULL;
//		int outlen = 0;
//
//		ret = gsasl_step(_sasl_session, NULL, 0, &out, (size_t *) &outlen);
//
//		string outstr(out, outlen);
//		if (out != NULL) {
//			free(out);
//		}
//		MCE_DEBUG("Session::saslHandshake --> first gsasl step:"<<outstr);
//		int pos = outstr.find(", auth-int");
//		if (pos != string::npos) {
//			outstr.replace(pos, strlen(", auth-int"), "");
//		}
//		MCE_DEBUG("Session::saslHandshake --> first gsasl step change into:"
//				<<outstr);
//
//		if (ret == GSASL_NEEDS_MORE) {
//			out = NULL;
//			outlen = 0;
//			ret = gsasl_base64_to(outstr.c_str(), outstr.length(), &out,
//					&outlen);
//
//			if (ret == GSASL_OK) {
//				ostringstream msg;
//				msg << "<challenge xmlns='urn:ietf:params:xml:ns:xmpp-sasl'>"
//						<< string(out, outlen) <<"</challenge>";
//				free(out);
//
//				try {
//					deliverMsg(msg.str().c_str());
//					return true;
//				} catch(...) {
//					MCE_WARN("Session::saslHandshake --> deliver first handshake msg err");
//				}
//			} else {
//				MCE_WARN("Session::saslHandshake --> deliver first handshake msg err");
//			}
//		} else {
//			MCE_WARN("Session::saslHandshake --> gsasl server first step err, errcode:"
//					<<ret);
//		}
//		break;
//	}
//	case SaslSecondHandshake: {
//
//		if(!instr.empty()){
//			MCE_DEBUG("Session::saslHandshake --> second handshake --> input:"<<instr);
//			char* out = NULL;
//			int outlen = 0;
//			int ret = gsasl_base64_from(instr.c_str(), instr.length(), &out, &outlen);
//
//			if(ret == GSASL_OK){
//				string bufstr(out, outlen);
//				free(out);
//				out = NULL;
//				outlen = 0;
//				MCE_DEBUG("Session::saslHandshake --> second handshake --> input decode:"<<instr);
//				ret = gsasl_step(_sasl_session, bufstr.c_str(), bufstr.length(), &out, &outlen);
//
//				if(ret == GSASL_NEEDS_MORE){
//					char* buf = NULL;
//					int buflen = 0;
//
//					ret = gsasl_base64_to(out,outlen,&buf,&buflen);
//					free(out);
//					if(ret == GSASL_OK){
//						ostringstream msg ;
//						msg<< "<challenge xmlns='urn:ietf:params:xml:ns:xmpp-sasl'>"<<string(buf,buflen)<<"</challenge>";
//						free(buf);
//						try{
//							deliverMsg(msg.str());
//							return true;
//						}catch(...){
//							MCE_WARN("Session::saslHandshake --> second handshake --> deliver msg err");
//						}
//					}
//				}else{
//					free(out);
//				}
//			}
//		}
//		break;
//	}
//	case SaslFinish: {
//		char* out = NULL;
//		int outlen = 0;
//		int ret = gsasl_step(_sasl_session, NULL, 0, &out, &outlen);
//		if(out != NULL){
//			free(out);
//		}
//		if(ret == GSASL_OK){
//			string uid;
//			try{
//				string uid = gsasl_property_fast(_sasl_session,GSASL_AUTHID);
//				int id = boost::lexical_cast<int>(uid);
//				JidPtr jid = new Jid;
//				jid->userId = id;
//				jid->endpoint = ServiceI::instance().getName();
//				jid(jid);
//
//				deliverMsg("<success xmlns='urn:ietf:params:xml:ns:xmpp-sasl'/>");
//				state(SessionStateLogin);
//				return true;
//			}catch(...){
//				MCE_WARN("Session::saslHandshake --> sasl get property AUTHID err");
//			}
//
//		}
//		break;
//	}
//	}
//	gsasl_finish(_sasl_session);
//	_sasl_session = NULL;
//	gsasl_done(_sasl_ctx);
//	_sasl_ctx = NULL;
//	return false;
//
//}
