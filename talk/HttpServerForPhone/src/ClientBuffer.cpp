#include "ClientBuffer.h"
#include "Server.h"
#include "pugixml.hpp"
#include "XmppTools.h"
#include "MessageType.h"
#include "MessageHolderAdapter.h"
#include "Cache.h"
#include "PTalkProxyAdapter.h"


using namespace talk::adapter;
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
	MCE_DEBUG("ClientBuffer::activeConnection --> active:"<<conn);
	_activeConnection = conn;
}
string ClientBuffer::msgToString(MessageMap msgs){
	ostringstream os;
	os << "<msglist>";
	MessageMap::iterator it = msgs.begin();
	for(; it!=msgs.end(); ++it){
		xml_document doc;
		MessagePtr msg = it->second;
		if (doc.load(msg->msg.c_str())) {
			if ((msg->type == MESSAGE) || (msg->type == OFFLINE_MESSAGE)) {
				string m = doc.child("message").child("body").first_child().value();
				if (m.empty()) {
					continue;
				}
				xml_document doc1;
				xml_node messageNode = doc1.append_child();
				messageNode.set_name("message");
				messageNode.append_attribute("type").set_value("chat");
				messageNode.append_attribute("id")
						= boost::lexical_cast<string>(it->first).c_str();
				messageNode.append_attribute("from").set_value(jidToString(msg->from).c_str());

				try {
					UserNamePtr un = ServiceI::instance().locateObject<UserNamePtr>(UN, msg->from->userId);

					if (un) {
						messageNode.append_attribute("fname").set_value(un->name.c_str());
					}
				} catch(...) {
					MCE_WARN("locate UserName err");
				}
				messageNode.append_attribute("to").set_value(jidToString(msg->to).c_str());

				xml_node bodyNode = messageNode.append_child();
				bodyNode.set_name("body");
				bodyNode.append_child(node_pcdata).set_value(m.c_str());

				msg->msg = doc1.xml();
			} else if (msg->type == NOTIFY_MESSAGE) {
				xml_node msgNode = doc.child("message");
				msgNode.append_attribute("type") = "notify";
				msgNode.append_attribute("id") = boost::lexical_cast<string>(it->first).c_str();
				msg->msg = doc.xml();
			} else if (msg->type == FEED_STATUS) {
				xml_node msgNode = doc.child("message");
				msgNode.append_attribute("type") = "feed_status";
				msgNode.append_attribute("id") = boost::lexical_cast<string>(it->first).c_str();
				msg->msg = doc.xml();
			} else if(msg->type == PHONE_NOTIFY){
				xml_node msgNode = doc.child("message");
				msgNode.append_attribute("type") = "phone_notify";
				msgNode.append_attribute("id") = boost::lexical_cast<string>(it->first).c_str();
				msg->msg = doc.xml();
			}
		}
		os << msg->msg;
	}
	os << "</msglist>";
	return os.str();
}

void ClientBuffer::pushMsg(const MessagePtr& msg) {
	//	MessagePtr& mptr = const_cast<MessagePtr&>(msg);

/*	if (msg) {
		MCE_INFO("ClientBuffer::pushMsg --> " << _jid->userId << " " << msg->msg);
		xml_document doc;
		if (doc.load(msg->msg.c_str())) {
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
				} catch(...) {
					MCE_WARN("locate UserName err");
				}
				messageNode.append_attribute("to").set_value(jidToString(msg->to).c_str());

				xml_node bodyNode = messageNode.append_child();
				bodyNode.set_name("body");
				bodyNode.append_child(node_pcdata).set_value(m.c_str());

				msg->msg = doc1.xml();

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
			} else if (msg->type == FEED_STATUS) {
				++_msgid;

				xml_node msgNode = doc.child("message");
				msgNode.append_attribute("type") = "feed_status";
				msgNode.append_attribute("id") = boost::lexical_cast<string>(_msgid).c_str();
				msg->msg = doc.xml();

				_requestTimes = 0;
				_msgs[_msgid] = msg;
			} else if(msg->type == PHONE_NOTIFY){
				++_msgid;

				xml_node msgNode = doc.child("message");
				msgNode.append_attribute("type") = "phone_notify";
				msgNode.append_attribute("id") = boost::lexical_cast<string>(_msgid).c_str();
				msg->msg = doc.xml();

				MCE_INFO("ClientBuffer::pushMsg*** --> " << _jid->userId << " " << msg->msg);
				_requestTimes = 0;
				_msgs[_msgid] = msg;

			}
		}
	}*/
}

string ClientBuffer::getMsg(Ice::Long lastMsgId) {
	MessageMap msgs;
	try{
		msgs = PTalkProxyAdapter::instance().getMsg(_jid->userId, lastMsgId);
	}catch(Ice::Exception& e){
		MCE_WARN("ClientBuffer::getMsg --> invoke PTalkProxy to get msgs err, " << e);
	}
	if (msgs.empty()) {
		return "";
	}
	return msgToString(msgs);
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
			//it->second->pushMsg(msg);
			connid = it->second->activeConnection();
		}
	}
	if (connid) {
		SessionPtr s = Server::instance().getSession(connid);
		if (s) {
			//MCE_INFO("CLientPool::pushMsg --> deliver " << msg->msg);
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
		const string& ticket, bool flush, bool isWap) {
	int isBind = 0;
	Ice::Long oldConId = 0;
	bool res = true;
	{
		IceUtil::Mutex::Lock lock(_mutex);
		//MCE_INFO("ClientPool::bind --> " << jidToString(j) << ", ac_conn " << connection);
		map<Ice::Long,ClientBufferPtr>::iterator it = _clients.find(j->index);
		if (it == _clients.end()) {
			ClientBufferPtr c = new ClientBuffer();
			c->bind(j,connection);
			_clients[j->index] = c;

			pair<Ice::Long,time_t> t(j->index,time(NULL));
			pushTimeoutQueue(t);
			c->timestamp(t.second);

			isBind = 1;
		} else {
			if(flush && it->second->isFlush()) {
				isBind = 2;
			}
			oldConId = it->second->activeConnection();
			bool res = it->second->bind(j,connection);
			if(res == false) {
				return false;
			}

			pair<Ice::Long,time_t> t(j->index,time(NULL));
			pushTimeoutQueue(t);
			it->second->timestamp(t.second);

			if(oldConId != connection && oldConId) {
				IdleConnManager::instance().cometConnSwitch();
				/*SessionPtr s = Server::instance().getSession(oldConId);
				if (s) {
					s->needUnbind(false);
					s->responseClose(404,"code:404, connection close");
				}*/
			}else{
				oldConId = 0;
			}
			//return true;
		}
	}


	if(isBind > 0){
		try {
			MCE_INFO("ClientPool::bind --> invoke PTalkProxy bind " << isBind << ", " << jidToString(j) << ", ac_conn " << connection);
			res = PTalkProxyAdapter::instance().bind(boost::lexical_cast<string>(j->index), ticket, j);
		} catch(Ice::Exception& e) {
			MCE_WARN("ClientPool::bind --> invoke PTalkProxy bind err" << isBind << ", " << jidToString(j) << ", ac_conn " << connection << ", " << e);
		}
	}
	if(oldConId > 0 ){
		SessionPtr s = Server::instance().getSession(oldConId);
		if (s) {
			s->needUnbind(false);
			s->responseClose(404,"code:404, connection close");
		}
	}
	return res;
}

void ClientPool::unbind(Ice::Long sessionId, Ice::Long connection, bool unbindNow) {

	MCE_DEBUG("ClientPool::unbind --> sid:"<<sessionId << "  connid:"
			<<connection);

	Ice::Long aconnid = 0;
	{
		IceUtil::Mutex::Lock lock(_mutex);
		map<Ice::Long,ClientBufferPtr>::iterator it = _clients.find(sessionId);
		if (it != _clients.end()) {
			MCE_DEBUG("ClientPool::unbind --> sid:"<<sessionId << "  connid:"
					<<connection << "  LINE:"<<__LINE__);
			aconnid = it->second->activeConnection();
			if(unbindNow){
				//立即退出
				MCE_INFO("ClientPool::unbind --> " << jidToString(it->second->jid()));
				PTalkProxyAdapter::instance().unbind(boost::lexical_cast<string>(it->second->jid()->index),it->second->jid());
				_clients.erase(sessionId);
			} else if (aconnid == connection) {
				MCE_DEBUG("ClientPool::unbind --> sid:"<<sessionId << "  connid:"
						<<connection << "  LINE:"<<__LINE__);
				it->second->activeConnection(0);
				time_t t = time(NULL);
				it->second->timestamp(t);
				pair<Ice::Long,time_t> p(sessionId, t);
				pushOfflineQueue(p);

			}
		}
	}
	if(unbindNow && aconnid && aconnid!=connection){
		MCE_INFO("ClientPool::unbind --> unbind now, close pre connection " << aconnid);
		SessionPtr s = Server::instance().getSession(aconnid);
		if (s) {
			s->needUnbind(false);
			s->responseClose(404,"code:404, connection close");
		}
	}
}

//-----------------------------------


bool ClientPool::offline() {

	pair<Ice::Long,time_t> o;

	{
		IceUtil::Mutex::Lock lock(_mutex);

		if (_offlineQueue.empty()) {
			return false;
		}
		o = _offlineQueue.front();
		if (time(NULL) - o.second < 10*60) {
			return false;
		}
		map<Ice::Long,ClientBufferPtr>::iterator it = _clients.find(o.first);
		if (it != _clients.end()) {
			MCE_DEBUG("ClientPool::offline -->  uid:"<< it->second->jid()->userId << " index:"<<it->second->jid()->index<< " active:"<<it->second->activeConnection());
			if (it->second->timestamp() == o.second
					&& it->second->activeConnection() == 0) {
				MCE_DEBUG("ClientPool::offline --> unbind uid:"
						<< it->second->jid()->userId << " index:"<<it->second->jid()->index);

				MCE_INFO("ClientPool::offline --> " << jidToString(it->second->jid()));
				PTalkProxyAdapter::instance().unbind(boost::lexical_cast<string>(it->second->jid()->index),it->second->jid());
				_clients.erase(o.first);

			}
		}
		_offlineQueue.pop_front();
	}
	return true;
}
void ClientPool::pushOfflineQueue(pair<Ice::Long,time_t>& o) {
	//IceUtil::Mutex::Lock lock(_queueMutex);
	_offlineQueue.push_back(o);
}

bool ClientPool::timeout() {

	pair<Ice::Long,time_t> t;

	SessionPtr s;
	JidPtr j;
	{
		IceUtil::Mutex::Lock lock(_mutex);
		if (_timeoutQueue.empty()) {
			return false;
		}
		t = _timeoutQueue.front();
		if (time(NULL) - t.second < 30) {

			return false;
		}

		map<Ice::Long,ClientBufferPtr>::iterator it = _clients.find(t.first);
		if (it != _clients.end()) {
			if (it->second->timestamp() == t.second) {
				if (it->second->activeConnection()) {
					s = Server::instance().getSession(it->second->activeConnection());
					j = it->second->jid();
				}
			}
		}
		_timeoutQueue.pop_front();
	}
	if (s) {
		//MCE_INFO("ClientPool::timeout --> return 408, " << jidToString(j));
		MCE_DEBUG("ClientPool::timeout --> 408 request time  sid:"<<t.first << " connid:"<<s->connectionId() << "  uid:"<<j->userId);
		s->responseClose(408, "Request Timeout");
//		WTalkProxyAdapter::instance().unbind(boost::lexical_cast<string>(j->index),j);
//		_clients.erase(t.first);
	}
	//			}

	return true;
}
void ClientPool::pushTimeoutQueue(pair<Ice::Long,time_t>& t) {
	//IceUtil::Mutex::Lock lock(_timeoutMutex);
	MCE_DEBUG("ClientPool::pushTimeoutQueue --> sid:"<<t.first << "  time:"
			<<t.second);
	_timeoutQueue.push_back(t);
}

