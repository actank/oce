#include "ClientBuffer.h"
#include "Server.h"
#include "pugixml.hpp"
#include "XmppTools.h"
#include "MessageType.h"
#include "MessageHolderAdapter.h"
#include "Cache.h"

using namespace talk::http;
using namespace pugi;
using namespace com::xiaonei::talk::util;

void ClientBuffer::timestamp(time_t time) {
	_timestamp = time;
}
time_t ClientBuffer::timestamp() {
	return _timestamp;
}
bool ClientBuffer::bind(const JidPtr j, Ice::Long activeConnection) {
	if (!_jid) {
		_jid = j;
	}
	if (_jid->userId != j->userId) {
		return false;
	}
	_activeConnection = activeConnection;
	_timestamp = time(NULL);
	return true;
}
JidPtr ClientBuffer::jid() {
	return _jid;
}
Ice::Long ClientBuffer::activeConnection() {
	return _activeConnection;
}
void ClientBuffer::activeConnection(Ice::Long conn) {
	_activeConnection = conn;
}
void ClientBuffer::pushMsg(const MessagePtr& msg) {
	//	MessagePtr& mptr = const_cast<MessagePtr&>(msg);
	if (msg) {
		xml_document doc;
		if (doc.load(msg->msg.c_str())) {
			//MCE_INFO("ClientBuffer::pushMsg --> from " << jidToString(msg->from) << ", to " << jidToString(msg->to) << ", jid " << jidToString(_jid) << ", type " << msg->type << ", id " << _msgid+1);
			if ((msg->type == MESSAGE) || (msg->type == OFFLINE_MESSAGE)) {
				string m = doc.child("message").child("body").first_child().value();
				if (m.empty()) {
					return;
				}

				++_msgid;

				xml_document doc1;
				xml_node messageNode = doc1.append_child();
				messageNode.set_name("message");
				messageNode.append_attribute("type").set_value("chat");
				messageNode.append_attribute("id")
						= boost::lexical_cast<string>(_msgid).c_str();
				messageNode.append_attribute("from").set_value(jidToString(msg->from).c_str());

				try {
					UserNamePtr un = ServiceI::instance().locateObject<UserNamePtr>(UN, msg->from->userId);

					if (un) {
						messageNode.append_attribute("fname").set_value(un->name.c_str());
					}
				} catch(Ice::Exception& e){
					MCE_WARN("locate UserName err, " << e);
				}
				//pager对外的resource为WTalkProxn_0，这里如果设置resource为HttpServern_index，
				//pager下次发消息的时候，from里的index格式为HttpServern_index，第三发客户端就不能识别这个from
				//messageNode.append_attribute("to").set_value(jidToString(msg->to).c_str());
				ostringstream os;
				os << msg->to->userId << "@talk.xiaonei.com/WTalkProxy"
					<< msg->to->userId%10 << "_0";
				messageNode.append_attribute("to").set_value(os.str().c_str());

				xml_node bodyNode = messageNode.append_child();
				bodyNode.set_name("body");
				bodyNode.append_child(node_pcdata).set_value(m.c_str());

				msg->msg = doc1.xml();
				MCE_INFO("ClientBuffer::pushMsg --> deliver message to jsclient from=" << jidToString(msg->from) << " to=" << jidToString(msg->to));
				_requestTimes = 0;
				_msgs[_msgid] = msg;
			} else if (msg->type == NOTIFY_MESSAGE) {
				++_msgid;

				xml_node msgNode = doc.child("message");
				msgNode.append_attribute("type") = "notify";
				msgNode.append_attribute("id") = boost::lexical_cast<string>(_msgid).c_str();

				msg->msg = doc.xml();

				_requestTimes = 0;
				_msgs[_msgid] = msg;
			} else if (msg->type == NOTIFY2_MESSAGE) {
				++_msgid;
				MCE_INFO("ClientBuffer::pushMsg --> deliver notify2 to jsclient from=" << jidToString(msg->from) << " to=" << jidToString(msg->to));
				if(doc.xml().find("ugc_content") != string::npos){
					MCE_INFO("ClientBuffer::pushMsg --> deliver muc_ugc to jsclient from=" << jidToString(msg->from) << " to=" << jidToString(msg->to));
				}
				xml_node msgNode = doc.child("message");
				msgNode.append_attribute("type") = "notify2";
				msgNode.append_attribute("id") = boost::lexical_cast<string>(_msgid).c_str();

				msg->msg = doc.xml();

				_requestTimes = 0;
				_msgs[_msgid] = msg;
			}else if (msg->type == FEED_STATUS) {
				++_msgid;

				xml_node msgNode = doc.child("message");
				msgNode.append_attribute("type") = "feed_status";
				msgNode.append_attribute("id") = boost::lexical_cast<string>(_msgid).c_str();
				msg->msg = doc.xml();

				_requestTimes = 0;
				_msgs[_msgid] = msg;
			}else if (msg->type == MUC_MESSAGE) {
				string identity = doc.child("message").attribute("from").value();
				string m = doc.child("message").child("body").first_child().value();
				if (m.empty() || identity.empty()) {
					return;
				}
				++_msgid;
				xml_document doc1;
				xml_node messageNode = doc1.append_child();
				messageNode.set_name("message");
				messageNode.append_attribute("type").set_value("groupchat");
				messageNode.append_attribute("id") = boost::lexical_cast<string>(_msgid).c_str();
				messageNode.append_attribute("from").set_value(identity.c_str());
				ostringstream os;
				os << msg->to->userId << "@talk.xiaonei.com/WTalkProxy" << msg->to->userId%10 << "_0";
				messageNode.append_attribute("to").set_value(os.str().c_str());
				xml_node bodyNode = messageNode.append_child();
				bodyNode.set_name("body");
				
				size_t roomidpos = identity.find("@");
				if(roomidpos == string::npos){
					return;
				}
				string srid = identity.substr(0, roomidpos);
				os.str("");
				string roomname = doc.child("message").child("roomname").first_child().value();
				os << "{type:\"groupchat\",info:{roomid:\"" << srid << "\", roomname:\"" << roomname << "\",userid:\"" << msg->from->userId << "\"}, chat:\"" << m << "\"}";
				bodyNode.append_child(node_pcdata).set_value(os.str().c_str());
				msg->msg = doc1.xml();
				_requestTimes = 0;
				_msgs[_msgid] = msg;
			}else if(MUC_PRESENCE == msg->type){
				//NOW TYPE ： entergroup leftgroup exitgruop
				string identity = doc.child("presence").attribute("from").value();
				string type = doc.child("presence").attribute("type").value();
				string roomname = doc.child("presence").child("roomname").first_child().value();
				if (identity.empty() || type.empty()) {
					return;
				}
				++_msgid;
				xml_document doc1;
				xml_node messageNode = doc1.append_child();
				messageNode.set_name("message");
				messageNode.append_attribute("type").set_value("groupchat");
				messageNode.append_attribute("id") = boost::lexical_cast<string>(_msgid).c_str();
				messageNode.append_attribute("from").set_value(identity.c_str());
				ostringstream os;
				os << msg->to->userId << "@talk.xiaonei.com/WTalkProxy" << msg->to->userId%10 << "_0";
				messageNode.append_attribute("to").set_value(os.str().c_str());
				xml_node bodyNode = messageNode.append_child();
				bodyNode.set_name("body");
				size_t roomidpos = identity.find("@");
				if(roomidpos == string::npos){
					return;
				}
				string srid = identity.substr(0, roomidpos);
				os.str("");
				if("entergroup" == type){
					string tiny = doc.child("presence").child("tinyurl").first_child().value();
					string name = doc.child("presence").child("username").first_child().value();
					string status = doc.child("presence").child("onlineType").first_child().value();
					if(status.empty()){
						return;
					}
					os << "{type:\"" << type << "\",info:{roomid:\"" << srid << "\",roomname:\"" << roomname << "\",userid:\"" << msg->from->userId << "\"}, status:\"" << status << "\", tiny:\"" << tiny << "\", name:\"" << name << "\"}";
				}
				else if("settingchange" == type){
					string set = doc.child("presence").child("set").first_child().value();
					if(set.empty()){
						return;
					}
					os << "{type:\"" << type << "\",info:{roomid:\"" << srid << "\",roomname:\"" << roomname << "\",userid:\"" << msg->from->userId << "\"}, set:\"" << set << "\"}";
				}
				else{
					string status = doc.child("presence").child("onlineType").first_child().value();
					if(status.empty()){
						return;
					}
					os << "{type:\"" << type << "\",info:{roomid:\"" << srid << "\",roomname:\"" << roomname << "\",userid:\"" << msg->from->userId << "\"}, status:\"" << status << "\"}";
				}
				bodyNode.append_child(node_pcdata).set_value(os.str().c_str());
				msg->msg = doc1.xml();
				_requestTimes = 0;
				_msgs[_msgid] = msg;
			}
		}
	}

}

string ClientBuffer::getMsg(Ice::Long lastMsgId) {

	if (_msgs.empty()) {
		return "";
	}

	++_requestTimes;
	if (_requestTimes > 3) {
		_msgs.clear();
		_requestTimes = 0;
		return "";
	}
	//	MessageSeq res;

	int msgCount = 0;
	ostringstream os;
	os << "<msglist>";
	for (map<Ice::Long,MessagePtr>::iterator it = _msgs.begin(); it
			!= _msgs.end();) {
		if (it->first <= lastMsgId) {
			try {
				if(it->second->type==MESSAGE && it->second->msgKey> 0) {
					MessageHolderAdapter::instance().send(it->second->to->userId,it->second->msgKey);
				}
			} catch(Ice::Exception& e) {
				MCE_WARN("ClientBuffer::getMsg --> call MessageHolderAdapter clean offline msg");
			}
			_msgs.erase(it++);
			continue;
		}
		//		res.push_back(it->second);
		if (!it->second->msg.empty()) {
			os << it->second->msg;
			++msgCount;
		}

		++it;
	}
	os << "</msglist>";
	//MCE_INFO("ClientBuffer::getMsg --> get msg size " << msgCount << ", " << jidToString(_jid));
	if (msgCount) {
		return os.str();
	} else {
		return "";
	}
}

//--------------------------------------

int ClientPool::size() {
	IceUtil::Mutex::Lock lock(_mutex);
	return _clients.size();
}
void ClientPool::pushMsg(Ice::Long sessionId, const MessagePtr& msg) {

	Ice::Long connid = 0;
	{
		IceUtil::Mutex::Lock lock(_mutex);
		map<Ice::Long,ClientBufferPtr>::iterator it = _clients.find(sessionId);
		if (it != _clients.end()) {
			it->second->pushMsg(msg);
			connid = it->second->activeConnection();
		}
	}

	//connid may be changed here by other thread, but it does't matter.
	//messages are cleared according mid in next request. if this deliver is failed, the mid will not be updated.
	if (connid) {
		SessionPtr s = Server::instance().getSession(connid);
		if (s) {
			s->deliverNotify(0);
		}
	}
}

string ClientPool::getMsg(Ice::Long sessionId, Ice::Long msgid) {
	IceUtil::Mutex::Lock lock(_mutex);
	map<Ice::Long,ClientBufferPtr>::iterator it = _clients.find(sessionId);
	if (it != _clients.end()) {
		return it->second->getMsg(msgid);
	}
	return "";
}

JidPtr ClientPool::getJid(Ice::Long sessionid) {
	IceUtil::Mutex::Lock lock(_mutex);
	map<Ice::Long,ClientBufferPtr>::iterator it = _clients.find(sessionid);
	if (it == _clients.end()) {
		return 0;
	} else {
		return it->second->jid();
	}
}

Ice::Long ClientPool::getActiveConnection(Ice::Long sessionid) {
	IceUtil::Mutex::Lock lock(_mutex);
	map<Ice::Long,ClientBufferPtr>::iterator it = _clients.find(sessionid);
	if (it == _clients.end()) {
		return 0;
	} else {
		return it->second->activeConnection();
	}
}

bool ClientPool::bind(Ice::Long connection, const JidPtr& j,
		const string& ticket, bool flush) {
	MCE_DEBUG("ClientPool::bind --> connectionId:" << connection << " jid:" << jidToString(j));
	int isBind = 0;
	Ice::Long oldConId = 0;
	bool res = true;
	{
		IceUtil::Mutex::Lock lock(_mutex);
		map<Ice::Long,ClientBufferPtr>::iterator it = _clients.find(j->index);
		if (it == _clients.end()) {
			ClientBufferPtr c = new ClientBuffer();
			c->bind(j,connection);
			_clients[j->index] = c;
			c->timestamp(time(NULL));
			isBind = 1;
		} else {
			//if(flush && it->second->isFlush()) {
			//	isBind = 2;
			//}
			oldConId = it->second->activeConnection();
			bool res = it->second->bind(j,connection);
			if(res == false) {
				return false;
			}
			it->second->timestamp(time(NULL));

			if(oldConId != connection && oldConId) {
				IdleConnManager::instance().cometConnSwitch();
			}else{
				oldConId = 0;
			}
		}
	}

	if(isBind > 0){
		try {
			MCE_INFO("ClientPool::bind --> invoke WTalkProxy bind " << isBind << ", " << jidToString(j) << ", ac_conn " << connection);
			res = WTalkProxyAdapter::instance().bind(boost::lexical_cast<string>(j->index), ticket, j);
		} catch(Ice::Exception& e) {
			MCE_WARN("ClientPool::bind --> invoke WTalkProxy bind err" << isBind << ", " << jidToString(j) << ", ac_conn " << connection << ", " << e);
		}
	}
	if(oldConId > 0){
		SessionPtr s = Server::instance().getSession(oldConId);
		SessionPtr newSession = Server::instance().getSession(connection);
		if (s) {
			s->needUnbind(false);
			MCE_DEBUG("ClientPool::bind --> jid " << jidToString(j)
				<< " change from " << s->jid()->userId << "_" << oldConId
				<< " to " << newSession->jid()->userId << "_" << connection);
			s->responseClose(211,"code:211, comet old connection close");
		}
	}
	return res;
}

void ClientPool::unbind(Ice::Long sessionId, Ice::Long connection) {
	IceUtil::Mutex::Lock lock(_mutex);
	//MCE_DEBUG("ClientPool::unbind --> sid:"<<sessionId << "  connid:"
	//		<<connection);
	map<Ice::Long,ClientBufferPtr>::iterator it = _clients.find(sessionId);
	if (it != _clients.end()) {
		if (it->second->activeConnection() == connection) {
			it->second->activeConnection(0);
			time_t t = time(NULL);
			it->second->timestamp(t);
			pair<Ice::Long,time_t> p(sessionId, t);
			//pushOfflineQueue(p);
			_offlineQueue.push_back(p);
			MCE_DEBUG("ClientPool::unbind --> sessionId:" << sessionId << " connectionId:" << connection);
		}
	}
}

//-----------------------------------


bool ClientPool::offline() {
	TimeStat ts;
	bool notifyWTalkProxy = false;
	Ice::Long connectionId = 0;
	pair<Ice::Long,time_t> o;
	{
		IceUtil::Mutex::Lock lock(_mutex);
		
		if (_offlineQueue.empty()) {
			//MCE_DEBUG("ClientPool::offline --> _offlineQueue is empty usetime:" << ts.getTime());
			return false;
		}
		o = _offlineQueue.front();
		if (time(NULL) - o.second < SESSION_TIMEOUT) {
			//MCE_DEBUG("ClientPool::offline --> SESSION_TIMEOUT usetime:" << ts.getTime());
			return false;
		}

		map<Ice::Long,ClientBufferPtr>::iterator it = _clients.find(o.first);
		if (it != _clients.end()) {
			connectionId = it->second->activeConnection();
			if (it->second->timestamp() == o.second && it->second->activeConnection() == 0) {
				//one way
				notifyWTalkProxy = true;
				int retry = 2;
				for(int i = 0; i < retry; ++i){
					try{
						WTalkProxyAdapter::instance().unbind(boost::lexical_cast<string>(it->second->jid()->index),it->second->jid());
						break;
					}catch(Ice::Exception& e){
						MCE_WARN("ClientPool::offline -->WTalkProxyAdapter::unbind-->retry:" << i << " error:" << e);
					}
				}
				_clients.erase(o.first);
			}
		}
		_offlineQueue.pop_front();
		MCE_DEBUG("ClientPool::offline --> sessionId:" << o.first << " activeConnection:" << connectionId << " notifyWTalkProxy:" << notifyWTalkProxy << " offlinequeue.size:" << _offlineQueue.size() << " usetime:" << ts.getTime());
	}
	return true;
}
/*
void ClientPool::pushOfflineQueue(pair<Ice::Long,time_t>& o) {
	IceUtil::Mutex::Lock lock(_queueMutex);
	_offlineQueue.push_back(o);
}*/

