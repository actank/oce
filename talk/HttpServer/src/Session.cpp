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

using namespace std;
using namespace talk::http;
using namespace talk::adapter;
using namespace com::xiaonei::talk::util;
using namespace pugi;
using namespace MyUtil;



void changeDomain(string& data, const string& src, const string& dst){
//校内改名人人
	if(data.empty() || src.empty()){
		return;
	}
	stringstream tmp;
	bool isInside = false;
	for(int i=0; i<data.size(); ){
		switch(data[i]){
			case '<' :
				isInside = true;
				break;
			case '>' :
				isInside = false;
				break;
			/*case '@':
				if(isInside && (data.size() - i) > 5 && strncmp(data.c_str()+i+1, "talk.", 5) == 0) {
					tmp << "@talk.";
					i += 6;
					tmp << data[i];
					i++;
					continue;
				}*/
			default:
				if(isInside && data[i]==src[0] && (data.size()-i)>src.size() &&
				(strncmp(data.c_str()+i+1, src.c_str()+1, src.size()-1) == 0)){
					tmp << dst;
					i += src.size();
					continue;
				}
				break;
		}
		tmp << data[i];
		i++;
	}
	data = tmp.str();
}


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

void ConnectionStat::responseClose(const SessionPtr& session, int status_code, const string& msg ) {
		session->shutdown();
}

void ConnectionStat::deliver(const SessionPtr& session, int status_code,
		int cache_time, bool is_gzip, bool keep_alive,
		const string& extra_header, const string& body, bool bin_data,
		const string& content_type, const string& nogzipmsg) {
	//MCE_DEBUG("ConnectionStat::deliver --> " << jidToString(session->jid()) << " body:" << body);
	ostringstream res;
	if (status_code != 200) {
		if (status_code == 403) {
			res << "HTTP/1.1 403 Forbidden\r\n";
		} else if(status_code == 408){
			res << "HTTP/1.1 408 Request Timeout\r\n";
		} else if(status_code == 211){
			res << "HTTP/1.1 211 Comet Switch\r\n";
		}else {
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
	bool ok = false;
	ok = session->deliver(res.str());
	if (!keep_alive) {
		session->shutdown();
	}


	string msg = nogzipmsg;
	xml_document doc;
	if (doc.load(msg.c_str())) {
		xml_node n = doc.child("msglist").child("message");
		for (; n; n = n.next_sibling("message")) {
			string s = n.child("body").first_child().value();
			if( s.find("ugc_content") != string::npos){
				MCE_INFO("ConnectionStat::deliver -->deliver ugc_content ok = " << ok);
			}
		}
	}else{
		MCE_WARN("CometResponseStat::deliver -->deliver ugc_content document.load error");
	}
}

//-----------------------------------------------------------------
void ConnectingStat::recvRequest(const RequestPtr& request,
		const SessionPtr& session) {
	//MCE_INFO("Request URL:"<<request->path() << " conn:"
	//		<<session->connectionId());
	ostringstream os;
	os << "ConnectingStat::recvRequest --> url:" << request->path() << " connectionId:" << session->connectionId() << " type:";
	map<string,ActionConfig>::iterator it = _config.find(request->path());

	if (it != _config.end()) {
		os << it->second.type;
		//		MCE_DEBUG("ConnectingStat::recvRequest --> line:"<<__LINE__);
		if (it->second.type == "comet_get") {
			changeStat(session, &CometRecvStat::instance());
		} else if (it->second.type == "wap_comet_get") {
			//changeStat(session, &WapCometRecvStat::instance());
		} else if (it->second.type == "comet_broadcast") {
			changeStat(session, &BroadcastResponseStat::instance());
		} else if (it->second.type == "wap_comet_broadcast") {
			//changeStat(session, &WapBroadcastResponseStat::instance());
		} else {
			changeStat(session, &DefaultResponseStat::instance());
		}

	} else {
		MCE_WARN("ERROR[404]: unknowen url:" << request->path() << " " << request->referer());
		deliver(session, 404, 0, false, false, "", "ERROR[404]:unknowen url");
	}
	MCE_DEBUG(os.str());
}



//-----------------------------------------------------------------
void CometRecvStat::response(const RequestPtr& request,
		const SessionPtr& session) {
	//MCE_DEBUG("CometRecvStat::response --> connection:"
	//		<<session->connectionId());
	string strHostId = request->cookie("id");
	int hostid = 0;
	string ticket = request->cookie("t");
	string sessionid = request->cookie("wpsid");
	string header;
	if(!strHostId.empty()){
		try{
			hostid = boost::lexical_cast<int>(strHostId);
		}catch(...){
			MCE_WARN("CometRecvStat::response --> cast err, " << strHostId);
		}
	}
	/*if (kl != "") {
		size_t p = kl.find("_");
		if (p != string::npos) {
			try {
				if(p+1 < kl.length()) {
					hostid = boost::lexical_cast<int>(kl.substr(p+1));
				}
			} catch(std::exception& e) {
				MCE_WARN("CometResponseStat::response --> hostid  not a int  err:"<<e.what());
			}

		}
	}*/

	if (hostid<=0 || ticket == "") {
		deliver(session, 403, 0, false, false, "",
				"ERROR[403]: please login first");
		changeStat(session, &ConnectingStat::instance());
		return;
	}

	Ice::Long msgId = 0;
	try {
		msgId = boost::lexical_cast<Ice::Long>(request->getProperty("mid"));
	} catch(std::exception& e) {
		MCE_WARN("CometRecvStat::response --> cast mid err:"<<e.what());
	}

	bool success = false;
	//if (!session->verify()) {
		try {
			bool needSetCookie = false;
			string domain = "wpi.renren.com";
			if(request->host().find("kaixin") != string::npos){
				domain = "wpi.kaixin.com";
			}
			if(sessionid == "") {
				ostringstream os;
				os << "Set-Cookie: wpsid=" << session->connectionId() <<"; domain=."<<domain<<"; path=/\r\n";
				header = os.str();

				//用户刷新过程中，同一个sessionid可能连接到不同的接入端，导致用户状态不准，所以sessionid+serverid保证唯一
				session->jid(hostid,session->connectionId()+Server::instance().getServerIndex());
				needSetCookie = true;
				MCE_DEBUG("CometRecvStat::response --> sid absent, set cookie " << jidToString(session->jid()));
			} else {
				////////////
				//added by yuanfei
				//new user, set a new session id
				Ice::Long sid = boost::lexical_cast<Ice::Long>(sessionid);
				sid += Server::instance().getServerIndex();
				JidPtr jid = ClientPoolManager::instance().getClientPool(sid).getJid(sid);
				if(jid && jid->userId!=hostid) {
					ostringstream os;
					os << "Set-Cookie: wpsid=" << session->connectionId() <<"; domain=."
					   << domain <<"; path=/\r\n";
					header = os.str();
					session->jid(hostid,session->connectionId()+Server::instance().getServerIndex());
					needSetCookie = true;
					MCE_DEBUG("CometRecvStat::response --> sid present, userid incorrect, set cookie " << jidToString(session->jid()));
				} else {
					session->jid(hostid,sid);
				}
			}

			session->needUnbind(true);
			//MCE_INFO("ClientPool::bind --> " << jidToString(session->jid()) << ", ac_conn " << session->connectionId());
			success = session->bind(ticket,true,false);
			if (success) {
				session->verify(true);
				session->isGzip(request->encoding());

				//for new connection
				if(needSetCookie) {
					//MCE_DEBUG();
					deliver(session, 200, 0, false, true, header,"<msglist><message type='system'><body>login successfully</body></message></msglist>");
					changeStat(session,&ConnectingStat::instance());
					return;
				}
			}else{
				MCE_DEBUG("CometRecvStat::response --> verify failed, " << jidToString(session->jid()));
				deliver(session, 403, 0, false, false, "",
					"ERROR[403]: verify is false");
				changeStat(session, &ConnectingStat::instance());
				return;
			}
		} catch(Ice::Exception& e) {
			MCE_WARN("CometRecvStat::response -> failure binding, userid: " << hostid << ", " << e);
			deliver(session, 403, 0, false, false, "","ERROR[403]: userid bind err");
			changeStat(session,&ConnectingStat::instance());
			return;
		}


	//}else{
	//	success = session->bind(ticket, msgId==0, false);
	//}

	map<string,string> props = request->getProperties();
	if (props.find("ins") != props.end()) {
		MCE_DEBUG("CometRecvStat::response --> do instant return " << jidToString(session->jid()));
		deliver(
				session,
				200,
				0,
				false,
				true,
				"",
				"<msglist><message type='system'><body>init ajax loop</body></message></msglist>");
		changeStat(session, &ConnectingStat::instance());
		return;
	}

	changeStat(session, &CometResponseStat::instance());
	session->deliverNotify(msgId);
}

void CometRecvStat::deliver(const SessionPtr& session, int status_code,
		int cache_time, bool is_gzip, bool keep_alive,
		const string& extra_header, const string& body, bool bin_data,
		const string& content_type) {
	ConnectionStat::deliver(session, status_code, cache_time, is_gzip,
			keep_alive, extra_header, body, bin_data, content_type);
	//MCE_DEBUG("CometRecvStat::deliver --> " << jidToString(session->jid()) << " " << body);
	if (keep_alive || status_code == 200) {
		ClientPoolManager::instance().getClientPool(session->jid()->index).unbind(session->jid()->index, session->connectionId());
	}
}

//-----------------------------------------------------------------
void CometResponseStat::deliverNotify(const SessionPtr& session, Ice::Long msgId) {
	string msglist = ClientPoolManager::instance().getClientPool(session->jid()->index).getMsg(session->jid()->index, msgId);
	string msg = msglist;
	if (msglist.empty()) {
		return;
	}
	//MCE_DEBUG("CometResponseStat::deliverNotify --> " << jidToString(session->jid()) << " " << msglist);
	if (session->isGzip()) {
		msglist = gzip_compress(msglist);
	}
	deliver(session, 200, 0, session->isGzip(), true, "", msglist, false, "text/html;charset=UTF-8", msg);
	changeStat(session, &ConnectingStat::instance());
}

void CometResponseStat::responseClose(const SessionPtr& session,int status_code, const string& msg) {
	MCE_DEBUG("CometResponseStat::responseClose --> connection:"
			<<session->connectionId());
	deliver(session, status_code, 0, false, false, "",msg);
	//	changeStat(session, &ConnectingStat::instance());
}

void CometResponseStat::deliver(const SessionPtr& session, int status_code,
		int cache_time, bool is_gzip, bool keep_alive,
		const string& extra_header, const string& body, bool bin_data,
		const string& content_type, const string& nogzipmsg) {
	ConnectionStat::deliver(session, status_code, cache_time, is_gzip,
			keep_alive, extra_header, body, bin_data, content_type, nogzipmsg);
	if (keep_alive || status_code == 200) {
		ClientPoolManager::instance().getClientPool(session->jid()->index).unbind(session->jid()->index, session->connectionId());
	}
}

//-----------------------------------------------------------------
void BroadcastResponseStat::response(const RequestPtr& request,
		const SessionPtr& session) {
	//	MCE_DEBUG("BroadcastResponseStat::handle()  --> " << __LINE__);
	string strHostId = request->cookie("id");
	int hostid = 0;
	if(!strHostId.empty()){
		try{
			hostid = boost::lexical_cast<int>(strHostId);
		}catch(...){
			MCE_WARN("BroadcastResponseStat::response --> cast err, " << strHostId);
		}
	}
	if(hostid<=0){
		deliver(session, 403, 0, false, false, "",
				"invalid user id");
		//changeStat(session, &ConnectingStat::instance());
		return;
	}

	Ice::Long sid = 0;
	try {
		sid = boost::lexical_cast<Ice::Long>(request->cookie("wpsid"));
	} catch(std::exception& e) {
		MCE_WARN("BroadcastResponseStat::response --> cast sid err:"<< e.what());
	}

	if (sid <=0) {
		//		MCE_DEBUG("BroadcastResponseStat::handle()  --> " << __LINE__);
		deliver(session, 403, 0, false, false, "",
				"do comet broadcast , but not login");
		changeStat(session, &ConnectingStat::instance());
		return;
	}
	/*JidPtr from = ClientPoolManager::instance().getClientPool(sid).getJid(sid);

	if (!from) {
		deliver(session, 403, 0, false, false, "", "session is not exist");
		changeStat(session, &ConnectingStat::instance());
		return;
	}*/
	xml_document doc;

	//通过人人接入的话，修改域名
	string body = request->body();
	changeDomain(body, "renren.com", "xiaonei.com");

	if (!doc.load(body.c_str())) {
		deliver(session, 403, 0, false, false, "", "msg is not a xml");
		changeStat(session, &ConnectingStat::instance());
		return;
	}
	MCE_DEBUG("BroadcastResponseStat::handle() --> msg:"<< doc.xml());
	bool suc = true;
	for (xml_node mNode = doc.child("sendlist").child("message"); mNode; mNode
			= mNode.next_sibling("message")) {

		string to = mNode.attribute("to").value();

		ostringstream os;
		mNode.print(os);
		//		MCE_DEBUG("send msg:"<<os.str()<< " to:"<<to << "  from:"<<jidToString(from));
		//		from->endpoint = "WTalkProxyN";
		//		from->index = 0;
		JidPtr tmp = new Jid();
		tmp->userId = hostid;
		//tmp->endpoint = "WTalkProxy"+boost::lexical_cast<string>(hostid %10);
		tmp->index = sid;
		//MessageMsgHandlerAdapter::instance().message(tmp, os.str());
		try{
			suc = WTalkProxyAdapter::instance().upDeliver(tmp, os.str());
		}catch(Ice::Exception& e){
			MCE_WARN("BroadcastResponseStat::response --> up deliver err, userid:" << hostid << " " << e);
		}
	}
	if(suc){
		changeStat(session, &ConnectingStat::instance());
		deliver(session, 200, 0, false, false, "", "");
	}else{
		MCE_WARN("BroadcastResponseStat::response --> up deliver err, userid:" << hostid);
		deliver(session, 403, 0, false, false, "", "session is not exist");
	}

}

//-----------------------------------------------------------------
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
		string strHostId = request->cookie("id");
		if (!strHostId.empty()) {
			try{
				id = boost::lexical_cast<int>(strHostId);
			}catch(...){
				MCE_WARN("DefaultResponseStat::response --> cast err, " << strHostId);
			}
		} else {
			string uid = request->getProperty("uid");
			if(uid != "") {
				try {
					id = boost::lexical_cast<int>(uid);
				} catch(...) {
					MCE_WARN("DefaultResponseStat::respons -->cast uid err");
				}
			}
		}

		if(id>0) {
			ContentPtr body;
			int realid = id;
			if("/getroominfo.do" == request->path() || "/getgrouphistory.do" == request->path()){
				int gid = -1;
				Str2StrMap props = request->getProperties();
				Str2StrMap::iterator it = props.find("gid");
				if(it != props.end()){
					try {
						gid = boost::lexical_cast<int>(it->second);
					} catch (std::exception& e) {
						MCE_WARN("DefaultResponseStat::respons cast gid = " << it->second << "--> ERROR  err:"<<e.what());
					}
				}
				realid = gid;
			}
			if(realid > 0){
				body = PageCacheAdapter::instance().GetContent(realid, request->path(), request->cookies(), request->getProperties());
				if(!body->isBin) {
					try{
						body->data = gzip_compress(body->data);
					}catch(std::exception& e){
						MCE_WARN("Page::bodyGzip --> gzip err:"<<e.what());
						body->data = "";
						body->statusCode = 404;
					}catch(...){
						MCE_WARN("Page::bodyGzip --> gzip err");
						body->data = "";
						body->statusCode = 404;
					}
				}
			}else{
			}
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
	MCE_WARN("DefaultResponseStat::respons --> retrun 404 " << request->path());
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
	MCE_DEBUG("~~session sid:"<< jidToString(_jid) << " connid:" << connectionId() << "needUnbind " << _needUnbind);
	if (_jid && _needUnbind) {
		ClientPoolManager::instance().getClientPool(_jid->index).unbind(_jid->index, connectionId());
	}
}

void Session::started() {
	//	Server::instance().join();
	MCE_DEBUG("Session::started --> connectionId: " << connectionId());
	IdleConnManager::instance().push(connectionId());

}

bool Session::bind(const string& ticket, bool flush, bool isWap) {
	return ClientPoolManager::instance().getClientPool(_jid->index).bind(connectionId(), _jid, ticket, flush);
}

void Session::stopped() {
	MCE_DEBUG((void*)this << " stop connectionId:" << connectionId() << " isBind:"<<_binding);

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
			}
		}
		return true;
	}
}

void Session::finishWrite() {
	//	MCE_DEBUG("Session::finishWrite");
	//	shutdown();
}
