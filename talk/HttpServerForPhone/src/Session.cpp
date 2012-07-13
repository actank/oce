#include "Session.h"
#include "Server.h"
//#include "WTalkProxyAdapter.h"

#include "compress.h"
//#include "MessageHolderAdapter.h"
#include "pugixml.hpp"
#include "MessageMsgHandlerAdapter.h"
#include "Cache.h"
#include "HttpParser.h"
#include "PageCacheAdapter.h"
#include "ClientBuffer.h"
#include "XmppTools.h"
#include "PTalkProxyAdapter.h"

using namespace std;
using namespace talk::http;
using namespace talk::adapter;
using namespace com::xiaonei::talk::util;
using namespace pugi;
using namespace MyUtil;




map<string,ConnectionStat::ActionConfig> ConnectionStat::_config = map<string,
		ConnectionStat::ActionConfig>();

void ConnectionStat::changeStat(const SessionPtr& session, ConnectionStat* stat) {
	session->connectionStat(stat);
}
void ConnectionStat::loadConfig(const string& path) {
	MCE_DEBUG("ConnectionStat::loadConfig");
	xml_document doc;
	if (doc.load_file(path.c_str())) {

		xml_node configNode = doc.child("config");
		for (xml_node n = configNode.child("action"); n; n
				= n.next_sibling("action")) {
			if (n.attribute("url")) {
				_config[n.attribute("url").value()].type = n.attribute("type").value();
				string index = n.attribute("index_by_id").value();
				if (index == "true") {
					_config[n.attribute("url").value()].has_index = true;
				} else {
					_config[n.attribute("url").value()].has_index = false;
				}
			}
		}

	} else {
		MCE_WARN("RequestAllocatee::loadConfig --> config file not find !!!");
	}

}

void ConnectionStat::responseClose(const SessionPtr& session, int status_code, const string& msg ){
	session->shutdown();
}

void ConnectionStat::deliver(const SessionPtr& session, int status_code,
		int cache_time, bool is_gzip, bool keep_alive,
		const string& extra_header, const string& body, bool bin_data,
		const string& content_type) {
	//MCE_INFO("-----deliver--- conn:"<<session->connectionId()<< " body:"
	//		<< body);
	ostringstream res;
	if (status_code != 200) {
		if (status_code == 403) {
			res << "HTTP/1.1 403 Forbidden\r\n";
		} else if(status_code == 408){
			res << "HTTP/1.1 408 Request Timeout\r\n";
		} else {
			res << "HTTP/1.1 404 Not Found\r\n";
		}
	} else {
		res << "HTTP/1.1 200 OK\r\n";
	}

	if (is_gzip && !body.empty() && (!bin_data) && (!Server::instance().isWap())) {
		res << "Content-Encoding: gzip\r\n";
	}

	res << "Server: XNHttpServer1.0\r\n" << "Content-Type: "<<content_type
			<<"\r\n" << "Content-Length: "<< body.length()<<"\r\n"
			<< extra_header;
	if (keep_alive) {
		res << "Connection: keep-alive\r\n" << "Keep-Alive: 300\r\n";
	} else {
		res << "Connection: close\r\n";
	}
	if (cache_time > 0 && status_code == 200) {
		res << "Cache-Control: max-age="<< cache_time << "\r\n";
		res << "Expires: "<< gmtTime(time(NULL)+cache_time)<< "\r\n";
	} else if (cache_time == -1) {
		res << "Cache-Control: max-age="<<60*60*24*10<<"\r\n";
		res << "Expires: "<< gmtTime(time(NULL)+60*60*24*10)<< "\r\n";
	} else if (cache_time == 0) {
		res << "Cache-Control: no-cache"<< "\r\n"
				<< "Expires: Thu, 01 Jan 1970 00:00:00 GMT"<< "\r\n";
	}
	res << "\r\n";
//	MCE_DEBUG("ConnectionStat -->deliver :"<< res.str() << " ISWAP:"<<IS_WAP);
	res.write(body.c_str(), body.length());
	//MCE_INFO("Session::deliver --> " << res.str());
	session->deliver(res.str());
	if (!keep_alive) {
		session->shutdown();
	}
}
//---------------------------------

void ConnectingStat::recvRequest(const RequestPtr& request,
		const SessionPtr& session) {
	//MCE_INFO("Request URL:"<<request->path() << " conn:"
	//		<<session->connectionId());
	map<string,ActionConfig>::iterator it = _config.find(request->path());

	if (it != _config.end()) {
		//		MCE_DEBUG("ConnectingStat::recvRequest --> line:"<<__LINE__);
		if (it->second.type == "wap_comet_get") {
			changeStat(session, &WapCometRecvStat::instance());
		} else if (it->second.type == "wap_comet_broadcast") {
			changeStat(session, &WapBroadcastResponseStat::instance());
		} else {
			changeStat(session, &DefaultResponseStat::instance());
		}
	} else {
		MCE_WARN("ERROR[404]: unknowen url:" << request->path() << " " << request->getProperty("uid") << " " << session->getRemoteAddr());
		deliver(session, 404, 0, false, false, "", "ERROR[404]:unknowen url");
	}
}

//---------
void WapCometRecvStat::response(const RequestPtr& request,
		const SessionPtr& session) {
	MCE_DEBUG("WapCometRecvStat::response --> connection:"
			<<session->connectionId());
	string sid = request->getProperty("sid");
	string uid = request->getProperty("uid");
	string pw = request->getProperty("pw");

	MCE_INFO("@@WapCometRecvStat::response --> " << uid << " " << sid << " " << pw);
	if (uid.empty() || (sid.empty() && pw.empty())) {
		MCE_INFO("WapCometRecvStat::response --> parameters error");
		deliver(session, 403, 0, false, false, "",
				"Authentication Error");
//		changeStat(session, &ConnectingStat::instance());
		return;
	}

	if (sid.empty()) {
		session->jid(boost::lexical_cast<int>(uid),session->connectionId() + Server::instance().getServerIndex());
	} else {
		session->jid(boost::lexical_cast<int>(uid),boost::lexical_cast<Ice::Long>(sid) + Server::instance().getServerIndex());
	}

	Ice::Long msgId = 0;
	try {
		msgId = boost::lexical_cast<Ice::Long>(request->getProperty("mid"));
	} catch(std::exception& e) {
		MCE_WARN("WapCometRecvStat::response --> cast mid err:"<<e.what());
	}

	//一个比较特殊的处理，mid为-1时，用户立即下线
	//考虑再增加一种请求类型
	if(msgId == -1){
		MCE_INFO("WapCometRecvStat::response --> offline now, " << jidToString(session->jid()));
		ClientPoolManager::instance().getClientPool(session->jid()->index).unbind(session->jid()->index, session->connectionId(), true);
		ConnectionStat::deliver(session, 200, 0, false, false, "", "offline now");
		return;
	}
	//

	session->needUnbind(true);
	bool success = false;
	if(!pw.empty()){
		success = session->bind(pw,msgId == 0,true);
	}else{
		success = session->bind(sid,msgId == 0,true);
	}
	if(!success) {
		MCE_INFO("WapCometRecvStat::response --> bind error");
		deliver(session, 403, 0, false, false, "",
		"Authentication Error or Session Expired");
		changeStat(session, &ConnectingStat::instance());
		return;
	}
	session->isGzip(request->encoding());

	if(sid.empty()) {
		deliver(session, 200, 0, false, true, "",boost::lexical_cast<string>(session->connectionId()));
		changeStat(session,&ConnectingStat::instance());
		return;
	}
	//MCE_DEBUG("WapCometRecvStat::response --> line:"<<__LINE__);
	changeStat(session, &WapCometResponseStat::instance());
	//MCE_DEBUG("WapCometRecvStat::response --> line:"<<__LINE__);

	session->deliverNotify(msgId);
	//MCE_DEBUG("CometRecvStat::response --> line:"<<__LINE__);
}

void WapCometRecvStat::deliver(const SessionPtr& session, int status_code,
		int cache_time, bool is_gzip, bool keep_alive,
		const string& extra_header, const string& body, bool bin_data,
		const string& content_type) {
	ConnectionStat::deliver(session, status_code, cache_time, is_gzip,
			keep_alive, extra_header, body, bin_data, content_type);
	if (keep_alive || status_code == 200) {
		ClientPoolManager::instance().getClientPool(session->jid()->index).unbind(session->jid()->index, session->connectionId());
	}
}

//-------------------------
void WapCometResponseStat::deliverNotify(const SessionPtr& session,
		Ice::Long msgId) {
	//MCE_INFO("WapCometResponseStat::deliverNotify --> connection:" << session->connectionId());
	string msglist = ClientPoolManager::instance().getClientPool(session->jid()->index).getMsg(session->jid()->index, msgId);
	//MCE_INFO("deliver Notify size=" << msglist.size() << " " << msglist);
	if (msglist.empty()) {
		return;
	}

	if (session->isGzip()) {
		msglist = gzip_compress(msglist);
	}

	deliver(session, 200, 0, session->isGzip(), true, "", msglist);
	changeStat(session, &ConnectingStat::instance());
}

void WapCometResponseStat::responseClose(const SessionPtr& session,int status_code,const string& msg) {
	MCE_DEBUG("WapCometResponseStat::responseClose --> connection:"
			<<session->connectionId());
	deliver(session, status_code, 0, false, false, "",msg);
}

void WapCometResponseStat::deliver(const SessionPtr& session, int status_code,
		int cache_time, bool is_gzip, bool keep_alive,
		const string& extra_header, const string& body, bool bin_data,
		const string& content_type) {
	ConnectionStat::deliver(session, status_code, cache_time, is_gzip,
			keep_alive, extra_header, body, bin_data, content_type);
	if (keep_alive || status_code == 200) {
		ClientPoolManager::instance().getClientPool(session->jid()->index).unbind(session->jid()->index, session->connectionId());
	}
}

//---------
void WapBroadcastResponseStat::response(const RequestPtr& request,
		const SessionPtr& session) {

	Ice::Long sid = 0;
	try {
		sid = boost::lexical_cast<Ice::Long>(request->getProperty("sid"));
	} catch(std::exception& e) {
		MCE_WARN("WapBroadcastResponseStat::response --> cast sid err:"<< e.what());
	}
	int uid = 0;
	try {
		uid = boost::lexical_cast<Ice::Long>(request->getProperty("uid"));
	} catch(std::exception& e) {
		MCE_WARN("WapBroadcastResponseStat::response --> cast uid err: " << e.what());
	}

	if (sid <=0) {
		//		MCE_DEBUG("BroadcastResponseStat::handle()  --> " << __LINE__);
		deliver(session, 403, 0, false, false, "",
				"do comet broadcast , but not login");
		changeStat(session, &ConnectingStat::instance());
		return;
	}
	//JidPtr from = ClientPoolManager::instance().getClientPool(sid).getJid(sid);

	//if (!from) {
	//	deliver(session, 403, 0, false, false, "", "session is not exist");
	//	changeStat(session, &ConnectingStat::instance());
	//	return;
	//}
	xml_document doc;
	if (!doc.load(request->body().c_str())) {
		deliver(session, 403, 0, false, false, "", "msg is not a xml");
		changeStat(session, &ConnectingStat::instance());
		return;
	}
	MCE_DEBUG("WapBroadcastResponseStat::handle() --> msg:"<< doc.xml());
	int suc = true;
	for (xml_node mNode = doc.child("sendlist").child("message"); mNode; mNode
			= mNode.next_sibling("message")) {

		string to = mNode.attribute("to").value();

		ostringstream os;
		mNode.print(os);
		//		MCE_DEBUG("send msg:"<<os.str()<< " to:"<<to << "  from:"<<jidToString(from));
		//		from->endpoint = "WTalkProxyN";
		//		from->index = 0;
		JidPtr tmp = new Jid();
		tmp->userId = uid;
		//tmp->endpoint = "PTalkProxy"+boost::lexical_cast<string>(uid %10);
		tmp->index = sid;
		try{
			suc = PTalkProxyAdapter::instance().upDeliver(tmp, os.str());
		}catch(Ice::Exception& e){
			MCE_WARN("WapBroadcastResponseStat::response --> up deliver err, " << e)
		}

	}
	if(suc){
		changeStat(session, &ConnectingStat::instance());
		deliver(session, 200, 0, false, false, "", "");
	}else{
		MCE_WARN("BroadcastResponseStat::response --> up deliver err, userid:" << uid);
		deliver(session, 403, 0, false, false, "", "session is not exist");
	}
}

//-------------
void DefaultResponseStat::response(const RequestPtr& request,
		const SessionPtr& session) {
	//	MCE_DEBUG("DefaultResponseStat::response --> config.size:"<<_config.size()
	//			<<" path:"<< request->path());
	map<string,ActionConfig>::iterator it = _config.find(request->path());
	if (it == _config.end()) {
		deliver(session, 404, 0, false, false, "", "");
		changeStat(session, &ConnectingStat::instance());
		return;
	}
	if (it->second.has_index) {
		int id = 0;
		string uid = request->getProperty("uid");
		if(uid != "") {
			try {
				id = boost::lexical_cast<int>(uid);
			} catch(...) {
				MCE_WARN("DefaultResponseStat::respons -->cast uid err, " << uid);
			}
		}

		if(id>0) {
			UserPagePtr up = ServiceI::instance().locateObject<UserPagePtr>(UPC, id);
			ContentPtr body = up->body(request, id);
			if(body) {
				deliver(session, body->statusCode, body->timeout, (bool)request->encoding(), false, "", body->data, body->isBin ,body->contentType);
				changeStat(session,&ConnectingStat::instance());
				return;
			}
		}
	} else {
		UserPagePtr up = ServiceI::instance().locateObject<UserPagePtr>(UPC, 0);
		ContentPtr body = up->body(request, 0);
		if(body) {
			deliver(session, body->statusCode, body->timeout, (bool)request->encoding(), false, "", body->data,body->isBin ,body->contentType);

			changeStat(session,&ConnectingStat::instance());
			return;
		}
	}
	deliver(session, 404, 0, false, false, "", "");
	changeStat(session,&ConnectingStat::instance());
	return;
}
//-----------------------------------
Session::Session() {
	_parser = new HttpParser();
	_counter = 0;
	_isClose = false;
	_verify = false;
	_isGzip = false;
	_binding = 0;
	_isClose = false;
	_needUnbind = false;
	_isReadSomething = false;
	_stat = &ConnectingStat::instance();
}

Session::~Session() {
	//	Server::instance().leave();
	MCE_INFO("~~session start");
	if (_jid && _needUnbind) {
		MCE_DEBUG("~~session sid:"<<_jid->index << " connid:"<<connectionId());
		ClientPoolManager::instance().getClientPool(_jid->index).unbind(_jid->index, connectionId());
	}
	MCE_DEBUG("~~session end");
}

void Session::started() {
	//	Server::instance().join();
	IdleConnManager::instance().push(connectionId());

}

bool Session::bind(const string& ticket, bool flush, bool isWap) {
	return ClientPoolManager::instance().getClientPool(_jid->index).bind(connectionId(), _jid, ticket, flush, isWap);
}

void Session::stopped() {
	MCE_DEBUG((void*)this << "stop  isBind:"<<_binding);

	/*	if(_verify && _jid){
	 Ice::Long connId = ClientPoolManager::instance().
	 getClientPool(_jid->index).getActiveConnection(_jid->index);
	 IdleConnManager::connStopped(connectionId(), _isReadSomething,
	 _verify, connId);
	 }else{
	 IdleConnManager::connStopped(connectionId(), _isReadSomething, _verify, 0);
	 }
	 */

	//IdleConnManager::instance().connStopped(connectionId(), _isReadSomething, _verify);
	/*commented by yuanfei, unbind in ~Session
	 if (_jid) {
	 ClientPoolManager::instance().getClientPool(_jid->index).unbind(_jid->index, connectionId());
	 }
	 */
}

bool Session::messaged(const string& message) {
	//	MCE_DEBUG("Session::message -->"<< message);
	if (!_isReadSomething) {
		_isReadSomething = true;
	}
	if (_counter + message.length()> MAX_MSG_SIZE) {
		return false;
	} else {
		_counter += message.length();
		if (_parser) {
			try {
				return _parser->feed(this, const_cast<string&>(message));
			} catch(Ice::Exception& e) {
				MCE_WARN("Session::messaged --> do parser err:" << e);
			}catch(std::exception& e){
				MCE_WARN("Session::messaged --> do parser std err:" << e.what());
			}
		}
		return true;
	}
}

void Session::finishWrite() {
	//	MCE_DEBUG("Session::finishWrite");
	//	shutdown();
}
