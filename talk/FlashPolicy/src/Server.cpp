#include "Server.h"
#include "XmppTools.h"
#include <sys/resource.h>

#include "IceLogger.h"
//#include "OnlineBuddyAdapter.h"
#include "UserCacheAdapter.h"
#include "PassportAdapter.h"
#include <boost/lexical_cast.hpp>
#include <util/cpp/String.h>

using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::util;
using namespace mop::hi::svc::adapter;
using namespace MyUtil;

//using namespace talk::online::adapter;
using namespace xce::usercache;

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

	//service.getAdapter()->add(&Manager::instance(), service.createIdentity(PROXY_MANAGER,
	//		""));



	MCE_DEBUG("Ready to start Server");
	MCE_INFO("LINUX_VERSION_CODE:"<< LINUX_VERSION_CODE
			<<"   epoll basic level:"<< KERNEL_VERSION(2, 5, 45));

	Server::instance().start(props);

	//TaskManager::instance().schedule(new OnlineSizeStater(60000));
	MCE_DEBUG("finish initialize...");
}

//----------------------------------------------------------------------
void Server::start(const Ice::PropertiesPtr& props) {

	int port = props->getPropertyAsIntWithDefault("Service.Acceptor.Port", 843);

	//::xce::nio::ConnectionPool::instance().initialize(0,new SessionFactoryI);
	_refactor = new Refactor(port,0,new SessionFactoryI, 20, "");
	_refactor->start();

	MCE_DEBUG("Server::started");

}

//void Server::stop() {
//	for (set<SessionPtr>::iterator it = _applies.begin(); it != _applies.end(); ++it) {
//		(*it)->shutdown();
//	}
//
//	_refactor->stop();
//	_refactor = 0;
//}

//void Server::apply(SessionPtr session) {
//	IceUtil::Mutex::Lock lock(_mutex);
//	if (session->getConnectionState() == ASIO_SHUTDOWN) {
//		return;
//	}
//	_applies.insert(session);
//	MCE_INFO("Server::apply -> " << session.get() << ", applies: "
//			<< _applies.size()) ;
//}
//
//void Server::leave(SessionPtr session) {
//	IceUtil::Mutex::Lock lock(_mutex);
//	_applies.erase(session);
//
//	MCE_INFO("Server::leave -> " << session.get() << ", applies: "
//			<< _applies.size() );
//}
//
//int Server::applySize() {
//	IceUtil::Mutex::Lock lock(_mutex);
//	return _applies.size();
//}



