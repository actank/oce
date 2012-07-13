#include "Server.h"
#include "Manager.h"
#include "XmppTools.h"
#include <sys/resource.h>
#include "TalkCacheAdapter.h"

#include "PresenceAdapter.h"
//#include "TalkLogicAdapter.h"
#include "Action.h"
#include "MessageHolderAdapter.h"

using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::util;
using namespace mop::hi::svc::adapter;
using namespace MyUtil;
using namespace ::talk::adapter;

/*
long Server::_applycount0 = 0;
long Server::_applycount1 = 0;
long Server::_joincount0 = 0;
long Server::_joincount1 = 0;
long Server::_joincount2 = 0;
long Server::_joincount3 = 0;
long Server::_joincount4 = 0;
long Server::_leavecount0 = 0;
long Server::_leavecount1 = 0;
long Server::_leavecount2 = 0;
*/

//---------------------------------------------------------------------------
Ice::ObjectPtr ActiveUserFactoryI::create(Ice::Int id) {
	ActiveUserPtr u = new ActiveUser();
	u->name = TalkCacheAdapter::instance().getUser(id)->name;
	return u;
}

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

	service.getAdapter()->add(&Manager::instance(), service.createIdentity(PROXY_MANAGER,
			""));

	service.getObjectCache()->registerCategory(0, "AU", new ActiveUserFactoryI, props);

	MCE_DEBUG("Ready to start Server");
	MCE_INFO("LINUX_VERSION_CODE:"<< LINUX_VERSION_CODE
			<<"   epoll basic level:"<< KERNEL_VERSION(2, 5, 45));

	Server::instance().start(props);
	WTalkProxyAdapter::instance().closeAccessServer(service.getName());

	TaskManager::instance().scheduleRepeated(new OnlineSizeStater(60*1000));
	TaskManager::instance().scheduleRepeated(new OnlineSizeMoniter(1000));

	MCE_DEBUG("finish initialize...");
}

//----------------------------------------------------------------------
void Server::start(const Ice::PropertiesPtr& props) {

	int port = props->getPropertyAsIntWithDefault("Service.Acceptor.Port",
			39000);
	int serverId = props->getPropertyAsIntWithDefault("Service.ServerIndex", -1);
		if(serverId == -1){
			MCE_WARN("Server::start --> not config server index err");
			return;
		}
	//::xce::nio::ConnectionPool::instance().initialize(serverId,new SessionFactoryI);
	_refactor = new Refactor(port,serverId,new SessionFactoryI, 100, "");
	_refactor->start();

	MCE_DEBUG("Server::started");

}

void Server::stop() {
/*	for (set<SessionPtr>::iterator it = _applies.begin(); it != _applies.end(); ++it) {
		(*it)->shutdown();
	}
	for (SessionMap::iterator it = _sessions.begin(); it != _sessions.end(); ++it) {
		it->second->shutdown();
	}
	_refactor->stop();
	_refactor = 0;*/
}

/*
void Server::apply(SessionPtr session) {
	IceUtil::Mutex::Lock lock(_mutex);

	_applycount0++;
	if (session->getConnectionState() == ASIO_SHUTDOWN) {
		return;
	}
	_applycount1++;
	_applies.insert(session);
	MCE_INFO("Server::apply -> " << session.get() << ", applies: "
			<< _applies.size() << ", size: " << _sessions.size());

	MCE_INFO("Server::apply -> "<<session.get()<<",  COUNT0:"<<_applycount0<<" COUNT1:"<<_applycount1);
}



void Server::join(SessionPtr session,JidPtr jid) {
	IceUtil::Mutex::Lock lock(_mutex);
	_joincount0 ++;
	if (session->getConnectionState() == ASIO_SHUTDOWN) {
		_joincount1 ++;
		_applies.erase(session);
		return;
	}
	if(_applies.find(session) == _applies.end()){
		MCE_WARN("Server::join --> "<<session.get()<<", session object is not in applies set");
		_joincount2 ++;
		return;
	}
	_joincount3 ++;
	_applies.erase(session);

	session->jid(jid);
	if (session->jid()) {
		_sessions[session->jid()] = session;
		_joincount4 ++;
	}
	MCE_INFO("Server::join -> " << session.get() << ", applies: "
			<< _applies.size() << ", size: " << _sessions.size());

	MCE_INFO("Server::join -> "<<session.get()<<",  COUNT0:"<<_joincount0<<" COUNT1:"<<_joincount1<<" COUNT2:"<<_joincount2<<" COUNT3:"<<_joincount3<<" COUNT4:"<<_joincount4);
}

void Server::leave(SessionPtr session) {
	IceUtil::Mutex::Lock lock(_mutex);
	_leavecount0++;
	_applies.erase(session);
	if (session->jid()) {
		if(_sessions.find(session->jid()) == _sessions.end()){
			_leavecount1++;
			MCE_WARN("Server::leave --> "<<session.get()<<" leave not find :"<<jidToString(session->jid()));
		}
		_sessions.erase(session->jid());
		_leavecount2++;
	}
	MCE_INFO("Server::leave -> " << session.get() << ", applies: "
			<< _applies.size() << ", size: " << _sessions.size());

	MCE_INFO("Server::leave -> "<<session.get()<<", COUNT0:"<<_leavecount0 << " COUNT1:"<<_leavecount1<<" COUNT2:"<<_leavecount2);
}

int Server::applySize() {
	IceUtil::Mutex::Lock lock(_mutex);
	return _applies.size();
}
int Server::onlineSize() {
	IceUtil::Mutex::Lock lock(_mutex);
	return _sessions.size();
}
*/
bool Server::deliver(const JidPtr& jid, const string& message) {
/*	SessionPtr session;
	{
		IceUtil::Mutex::Lock lock(_mutex);
		SessionMap::iterator pos = _sessions.find(jid);
		if (pos != _sessions.end()) {
			session = pos->second;
		}
	}
	if (session.get()) {
		if (session->deliver(message)) {
			return true;
		} else {
			MCE_WARN("Server::deliver -> " << session.get() << "->"
					<< jidToString(jid)
					<< " should be destroy already, but it exists");
			return false;
		}
	}
	return false;
*/
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
	return _refactor->getConnection(index);
}
