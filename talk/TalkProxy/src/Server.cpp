#include "Server.h"
#include "Manager.h"
#include "XmppTools.h"
#include <sys/resource.h>
#include "TalkCacheAdapter.h"

#include "PresenceAdapter.h"
#include "OnlineCenterAdapter.h"
#include "TalkLogAdapter.h"
#include "Action.h"
using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::util;
using namespace mop::hi::svc::adapter;
using namespace MyUtil;
using namespace talk::online::adapter;
using namespace xiaonei::talk;
//---------------------------------------------------------------------------
void MyUtil::initialize() {
	MCE_DEBUG("begin initialize...");
	//set openfiles
	struct rlimit limit;
	if (getrlimit(RLIMIT_NOFILE, &limit) < 0) {
		MCE_WARN("no right to get open files");
	} else {
		MCE_INFO("openfile  cur:"<<limit.rlim_cur<<"  max:"<<limit.rlim_max);
	}
	limit.rlim_cur = limit.rlim_max = RLIM_INFINITY;
	if (setrlimit(RLIMIT_NOFILE, &limit) < 0) {
		MCE_WARN("no right to set open files");
	} else {
		MCE_INFO("openfile  cur:"<<limit.rlim_cur<<"  max:"<<limit.rlim_max);
	}

	ServiceI& service = ServiceI::instance();
	Ice::PropertiesPtr props = service.getCommunicator()->getProperties();

	//	Manager::instance().setLogic(LogicManagerPrx::uncheckedCast(service.getCommunicator()->stringToProxy(props->getPropertyWithDefault("Service."
	//			+ service.getName() + ".TalkLogic", "M@TalkLogic"))));

	service.getAdapter()->add(&Manager::instance(), service.createIdentity(PROXY_MANAGER,
			""));

	MCE_DEBUG("Ready to start Server");
	MCE_INFO("LINUX_VERSION_CODE:"<< LINUX_VERSION_CODE
			<<"   epoll basic level:"<< KERNEL_VERSION(2, 5, 45));

	Server::instance().start(props);
	try{
		OnlineCenterAdapter::instance().closeProxyServer(service.getName());
	}catch(Ice::Exception& e){
		MCE_WARN("initialize --> call close proxy err:"<<e<< "  file:"<<__FILE__<<"  line:"<<__LINE__);
	}catch(std::exception& e){
		MCE_WARN("initialize --> call close proxy err:"<<e.what()<< "  file:"<<__FILE__<<"  line:"<<__LINE__);
	}

	TaskManager::instance().scheduleRepeated(new OnlineSizeStater(60*1000));
	TaskManager::instance().scheduleRepeated(new OnlineSizeMoniter(1000));
	TaskManager::instance().scheduleRepeated(&LoginProcedureStat::instance());
	//	service.registerXceObserver("TalkProxy", new ServiceSchema);
	MCE_DEBUG("finish initialize...");
}

//----------------------------------------------------------------------
void Server::start(const Ice::PropertiesPtr& props) {
	int port =
			props->getPropertyAsIntWithDefault("Service.Acceptor.Port", 5222);
	int serverId = props->getPropertyAsIntWithDefault("Service.ServerIndex", -1);
	string ssl_key = props->getPropertyWithDefault("Service.SSLKey","/opt/XNTalk/etc/server.pem");
	if(serverId == -1){
		MCE_WARN("Server::start --> not config server index err");
		return;
	}
	_renren = (bool)props->getPropertyAsIntWithDefault("Service.Renren", 0);
	//::xce::nio::ConnectionPool::instance().initialize(serverId,new SessionFactoryI);
	_refactor = new Refactor(port,serverId,new SessionFactoryI, 20, ssl_key, 500000);
	_refactor->start();
	MCE_DEBUG("Server::started");
}

void Server::stop() {
//	for (set<SessionPtr>::iterator it = _applies.begin(); it != _applies.end(); ++it) {
//		(*it)->shutdown();
//	}
//	for (SessionMap::iterator it = _sessions.begin(); it != _sessions.end(); ++it) {
//		it->second->shutdown();
//	}
//	_refactor->stop();
//	_refactor = 0;
}

//void Server::apply(const SessionPtr& session) {
//	IceUtil::Mutex::Lock lock(_mutex);
//
//	if (session->getConnectionState() == ASIO_SHUTDOWN) {
//		MCE_WARN("Server::apply --> connection stat shutdown");
//		return;
//	}
//
//	_applies.insert(session);
//	MCE_INFO("Server::apply -> " << session.get() << ", applies: "
//			<< _applies.size() << ", size: " << _sessions.size());
//
//}

//void Server::join(const SessionPtr& session) {
//	IceUtil::Mutex::Lock lock(_mutex);
//
//	if (session->getConnectionState() == ASIO_SHUTDOWN) {
//		return;
//	}
//
//	_applies.erase(session);
//	if (session->jid()) {
//		_sessions[session->jid()] = session;
//
//	}
//	MCE_INFO("Server::join -> " << session.get() << ", applies: "
//			<< _applies.size() << ", size: " << _sessions.size());
//
//}
//
//void Server::leave(const SessionPtr& session) {
//	IceUtil::Mutex::Lock lock(_mutex);
//
//	_applies.erase(session);
//	if (session->jid()) {
//		_sessions.erase(session->jid());
//
//	}
//	MCE_INFO("Server::leave -> " << session.get() <<", applies: "
//			<<_applies.size()<<", size: "<<_sessions.size());
//
//
//}
//
//void Server::leave(const JidPtr& jid) {
//	SessionPtr session;
//	{
//		IceUtil::Mutex::Lock lock(_mutex);
//
//		SessionMap::iterator pos = _sessions.find(jid);
//		if (pos != _sessions.end()) {
//			session = pos->second;
//		}
//	}
//	if (session.get()) {
//		session->shutdown();
//	}
//}

bool Server::verify(const JidPtr& jid, const string& ticket) {
	IceUtil::Mutex::Lock lock(_mutex);
	MCE_DEBUG("call Server::checkTicket --> jid:"<<jid->userId
			<<"@talk.xiaonei.com/"<<jid->endpoint<<"_"<<jid->index
			<<"   ticket:"<<ticket);

	//SessionMap::iterator it = _sessions.find(jid);
	::xce::nio::ConnectionPtr conn = _refactor->getConnection(jid->index);
	if(conn){
		AsioChannelPtr channel = conn->channel();
		SessionPtr session = SessionPtr::dynamicCast(channel);
		if(session){
			if(session->ticket() == ticket){
				return true;
			}
		}
	}
	return false;
//	if (it == _sessions.end()) {
//		MCE_DEBUG("Server::checkTicket --> not find jid");
//		return false;
//	}
//	MCE_DEBUG("Server::checkTicket --> ticket:"<<it->second->ticket() <<"   t:"
//			<<ticket);
//	if (it->second->ticket() == ticket) {
//		return true;
//	}
//	return false;
}

string Server::GetTicket(const JidPtr& jid) {

  MCE_DEBUG("call Server::GetTicket --> jid:"<<jid->userId
      <<"@talk.xiaonei.com/"<<jid->endpoint<<"_"<<jid->index);

  IceUtil::Mutex::Lock lock(_mutex);
  //SessionMap::iterator it = _sessions.find(jid);
  ::xce::nio::ConnectionPtr conn = _refactor->getConnection(jid->index);
  if(conn){
    AsioChannelPtr channel = conn->channel();
    SessionPtr session = SessionPtr::dynamicCast(channel);
    if(session){
      return session->ticket();
    }
  }
  return "";
}

bool Server::deliver(const JidPtr& jid, const string& message) {
//	SessionPtr session;
//	{
//		IceUtil::Mutex::Lock lock(_mutex);
//		SessionMap::iterator pos = _sessions.find(jid);
//		if (pos != _sessions.end()) {
//			session = pos->second;
//		}
//	}
//	if (session.get()) {
//		if (session->deliver(message)) {
//			return true;
//		} else {
//			MCE_WARN("Server::deliver -> " << session.get() << "->"
//					<< jidToString(jid)
//					<< " should be destroyed, but it exists");
//			return false;
//		}
//	}
//	return false;
	if(_renren){
		changeDomain(const_cast<string&>(message), "xiaonei.com", "renren.com");
	}else{
		changeDomain(const_cast<string&>(message), "renren.com", "xiaonei.com");
	}
	return _refactor->deliver(jid->index,message);
}

int Server::size(bool apply) {
	IceUtil::Mutex::Lock lock(_mutex);
	//	if (apply) {
	//		ostringstream os;
	//		for (set<SessionPtr>::iterator it = _applies.begin(); it
	//				!= _applies.end(); ++it) {
	//			os <<"apply_session_ptr:"<<*(it->get()) <<"\t";
	//		}
	//		MCE_INFO(os.str());
	//	}
	_refactor->connectionPoolSize();
	if(apply){
		return getJoinCount() - getOnlineUserCount();
	}
	return getOnlineUserCount();
}

bool Server::indexExist(Ice::Long index){
	return (_refactor->getConnection(index) != 0);
}

/*void Server::sendLog(const JidPtr& from, const JidPtr& to, const string& msg){
	if(!_sendLog){
		return;
	}
	try{
		TalkLogAdapter::instance().feed(from, to, msg);
	}catch(...){
		MCE_WARN("Invoke TalkLog error");
	}
}
*/

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

void Server::kick(const JidPtr& jid){
	_refactor->connectionShutdown(jid->index, false);
}
