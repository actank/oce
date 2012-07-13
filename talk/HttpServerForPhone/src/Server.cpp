#include <sys/resource.h>

//#include "Request.h"
#include "Server.h"
#include "Cache.h"
#include "Session.h"
#include "ManagerI.h"
#include "ClientBuffer.h"
#include "PTalkProxyAdapter.h"

using namespace talk::http;
using namespace talk::adapter;

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

	ClientPoolManager::instance().startTasks();


	ServiceI& service = ServiceI::instance();
	Ice::PropertiesPtr props = service.getCommunicator()->getProperties();

	Server::instance().setWapFlag( (bool)props->getPropertyAsIntWithDefault("Service."+ServiceI::instance().getName()+".IsWap",1));

	service.getAdapter()->add(&ManagerI::instance(), service.createIdentity(PROXY_MANAGER,
			""));

	service.getObjectCache()->registerCategory(UPC, "UPC", new UserPageFactoryI, props, true);
	service.getObjectCache()->registerCategory(UN, "UN", new UserNameFactoryI, props, true);

	string configFilePath = props->getPropertyWithDefault("Service.ConfigFilePath", "/opt/XNTalk/etc/XNHttpServer.conf");
	ConnectingStat::instance().loadConfig(configFilePath);
	try{
		PTalkProxyAdapter::instance().closeAccessServer(service.getName());
	}catch(Ice::Exception& e){
		MCE_WARN("initialize --> close access client err:"<<e<<" file:"<<__FILE__<<" line:"<<__LINE__);
	}catch(std::exception& e){
		MCE_WARN("initialize --> close access client err:"<<e.what()<<" file:"<<__FILE__<<" line:"<<__LINE__);
	}
	MCE_DEBUG("Ready to start Server, ISWAP:"<<Server::instance().isWap());
	MCE_INFO("LINUX_VERSION_CODE:"<< LINUX_VERSION_CODE
			<<"   epoll basic level:"<< KERNEL_VERSION(2, 5, 45));

	Server::instance().start(props);

	MCE_DEBUG("finish initialize...");


}

//----------------------------------------------------------------------

void Server::start(const Ice::PropertiesPtr& props) {
	TaskManager::instance().scheduleRepeated(&(IdleConnManager::instance()));

	int port = props->getPropertyAsIntWithDefault("Service.Acceptor.Port", 80);
	_serverIndex = props->getPropertyAsIntWithDefault("Service.ServerIndex", 0);
	_refactor = new ::xce::nio::Refactor(port,0,new SessionFactoryI, 50, "");
	_refactor->start();

	MCE_DEBUG("Server::started");
}

void Server::stop() {
}

//void Server::popMsg(Ice::Long session_id, MessageSeq& mseq) {
//	IceUtil::Mutex::Lock lock(_mutex);
//	map<Ice::Long,CometClientPtr>::iterator it = _clients.find(session_id);
//	if (it != _clients.end()) {
//		mseq.swap(it->second->msgs);
//	}
//}

//bool Server::bind(Ice::Long connection, const JidPtr& j, const string& ticket) {
//	IceUtil::Mutex::Lock lock(_mutex);
//
//	MCE_DEBUG("BIND: conid:"<<connection<<" sessionid:"<<j->index);
//	map<Ice::Long,CometClientPtr>::iterator it = _clients.find(j->index);
//	if (it == _clients.end()) {
//		bool res = WTalkProxyAdapter::instance().bind(boost::lexical_cast<string>(j->index), ticket, j);
//		if(res) {
//			CometClientPtr c = new CometClient();
//			c->active_connection = connection;
//			c->jid = j;
//			_clients[j->index] = c;
//		}
//		return res;
//	} else {
//
//		Ice::Long oldConId = it->second->active_connection;
//		it->second->active_connection = connection;
//		if(oldConId != connection) {
//
//			xce::nio::ConnectionPtr c;
//			c = _refactor->getConnection(oldConId);
//			if (c) {
//				SessionPtr s = SessionPtr::dynamicCast(c->channel());
//				s->responseClose();
//			}
//		}
//		return true;
//	}
//
//}

SessionPtr Server::getSession(Ice::Long connId) {
	xce::nio::ConnectionPtr c;
	c = _refactor->getConnection(connId);
	if (c) {
		return SessionPtr::dynamicCast(c->channel());

	}
	return 0;
}

bool Server::indexExist(Ice::Long index){
	return _refactor->getConnection(index);
}
//void Server::deliver(const Long2MsgMap & msgs) {
//	xce::nio::ConnectionPtr c;
//	{
//		IceUtil::Mutex::Lock lock(_mutex);
//		map<Ice::Long,CometClientPtr>::iterator it =
//				_clients.find(message->to->index);
//		if (it != _clients.end()) {
//			MCE_DEBUG("Server::deliver --> active_connection:"
//					<<it->second->active_connection);
//
////			it->second->msgs.push_back(message);
//			for(Long2MsgMap::const_iterator mit = msgs.begin(); mit != msgs.end(); ++mit){
//				it->second->msgs[it->first] = it->second;
//			}
//
//			c = _refactor->getConnection(it->second->active_connection);
//		}
//	}
//	if (c) {
//		SessionPtr s = SessionPtr::dynamicCast(c->channel());
//		s->deliverNotify();
//	}
//
//}

//int Server::size(bool apply) {
//	IceUtil::Mutex::Lock lock(_mutex);
//
//	_refactor->connectionPoolSize();
//	if (apply) {
//		return getJoinCount() - getOnlineUserCount();
//	}
//	return getOnlineUserCount();
//}
void IdleConnManager::handle(){

		//SessionPtr
	int readNothingCount = 0;	//read nothing

	int readSomethingCount = 0;

	int verifyCount = 0;		//verify count
	int notVerifyCount = 0;

	int validCometCount = 0;
	int nullCometCount = 0;
	int otherConnIdCount = 0;

	while(!_sessions.empty()){
		pair<Ice::Long, time_t> p;
		{
			IceUtil::Mutex::Lock lock(_mutex);
			p = _sessions.front();
			if(::time(NULL) - p.second < CONN_TIMEOUT){
				break;
			}
			_sessions.pop_front();
		}

		SessionPtr s = Server::instance().getSession(p.first);
		if(!s){
			continue;
		}
		if(!s->isReadSomething()){
			readNothingCount++;
			MCE_WARN("~~~DOS Attack~~~");
			s->shutdown(true);
		}else{
			readSomethingCount++;
			if(s->verify()){
				verifyCount++;
				JidPtr jid = s->jid();
				if(!jid){
					continue;
				}
				Ice::Long connId = ClientPoolManager::instance().getClientPool(jid->index).getActiveConnection(jid->index);
				if(connId==0){
					nullCometCount++;
				}else if(jid->index == connId){
					validCometCount++;
				}else{
					otherConnIdCount++;
				}
			}else{
				notVerifyCount++;
			}
		}
	}
	/*MCE_INFO("~! readNothingCount:" << readNothingCount
			<< ", readSomethingCount:" << readSomethingCount
			<< ", verifyCount:" << verifyCount
			<< ", notVerifyCount:" << notVerifyCount
			<< ", nullCometCount:" << nullCometCount
			<< ", validCometCount:" << validCometCount
			<< ", otherConnIdCount:" << otherConnIdCount
			<< ", cometConnSwitch" << _cometConnSwitch
			);*/


/*	MCE_INFO("~@ stoppedReadNothing:" << _stoppedReadNothing
			<< ", _stoppedReadSomething:" << _stoppedReadSomething
			<< ", _stoppedVerify:" << _stoppedVerify
			<< ", _stoppedNotVerify:" << _stoppedNotVerify
			<< ", _stopedNullComet:" << _stopedNullComet
			<< ", _stopedOtherConnId:" << _stopedOtherConnId
			<< ", _stopedValidComet:" << _stopedValidComet
			);*/
	_cometConnSwitch = 0;
	_stoppedReadNothing = 0;
	_stoppedVerify = 0;
	_stoppedNotVerify = 0;
	_stoppedReadSomething =0 ;
	_stopedNullComet = 0;
	_stopedOtherConnId = 0;
	_stopedValidComet = 0;
	//TaskManager::instance().schedule(this);
}
/*
void IdleConnManager::connStopped(Ice::Long connId, bool isReadSomthing,
								bool isVerify){

	if(!isReadSomthing){
		_stoppedReadNothing++;
	}else{
		_stoppedReadSomething++;
		if(isVerify){
			_stoppedVerify++;
			Ice::Long activeConnId = 0;
			SessionPtr s = Server::instance().getSession(connId);
			if(s){
				JidPtr jid = s->jid();
				if(jid){
					activeConnId = ClientPoolManager::instance().getClientPool(jid->index).
										getActiveConnection(jid->index);
				}
			}

			if(activeConnId==0){
				_stopedNullComet++;
			}else if(activeConnId == connId){
				_stopedValidComet++;
			}else{
				_stopedOtherConnId++;
			}
		}else{
			_stoppedNotVerify++;
		}
	}
}*/
