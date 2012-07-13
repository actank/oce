#include "ServerStateSubscriber.h"
#include "ReplicaCluster/SubjectObserver/src/client/cxx/SbjTopicI.h"
#include "IceExt/src/ServiceI.h"
#include "util/cpp/String.h"
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace MyUtil;
using namespace xce::serverstate;
using MyUtil::StrUtil;

Str2StrMap ServerStateBackDoorI::control(const Str2StrSeqMap& data, const Ice::Current& c) {
  Str2StrSeqMap::const_iterator i = data.find("setstatus");
  if (i != data.end()) {
    switch (i->second.size()) {
      case 1:
        ServerStateSubscriber::instance().setStatus(boost::lexical_cast<int>(i->second.at(0)));
        break;
      case 2:
        ServerStateSubscriber::instance().setStatus(boost::lexical_cast<int>(i->second.at(0)), i->second.at(1));
        break;
    }
    return Str2StrMap();
  }
  i = data.find("getstatus");
  if (i != data.end()) {
    Str2StrMap m;
    int status = -1;
    switch (i->second.size()) {
      case 0:
        status = ServerStateSubscriber::instance().getStatus();
        m.insert(make_pair<string, string>("status", boost::lexical_cast<string>(status)));
        break;
      case 1:
        status = ServerStateSubscriber::instance().getStatus(i->second.at(0));
        m.insert(make_pair<string, string>("status", boost::lexical_cast<string>(status)));
        break;
    }
    return m;
  }
  return Str2StrMap();
}

void ServerStateSubscriber::init_ServerStateBackDoor() {
  ServiceI& service = ServiceI::instance();
  _door = new ServerStateBackDoorI(service.getAdapter());
}

void ServerStateSubscriber::initialize(const string& ctrEndpoints, 
		const Ice::ObjectPtr pServer, int mod, 
		int interval,const MyUtil::ChannelPtr& channel,  const string& threadPoolSize, 
		const string& threadPoolSizeMax, const string& stackSize,
		const string& protocal) {
	if ( _inited ) {
		return;
	} else {
		_inited = true;
	}
  _channel=channel;
	bIsMutilServicesInOneProcess=false;
	// 基本参数赋值
	_ctrEndpoints = ctrEndpoints;
	_interval = interval;
	_mod = mod;
	_newSet = true;

	// 没有预加载status为0
	_status = 0;

	// 生成本地服务的Prx
	// _prx = ServiceI::instance().getCommunicator()->stringToProxy(identity+"@"+endpoints);	
	ServiceI& service = ServiceI::instance();
	string serverName = service.getAdapter()->getName();
	string localIP = MyUtil::IpAddr::inner();
	Ice::CommunicatorPtr _ic = _channel->getCommunicator();
	/* _ic->getProperties()->setProperty("ServerRandom.Endpoints","tcp -h " + localIP);
	_ic->getProperties()->setProperty("ServerRandom.ThreadPool.Size","10");
	_ic->getProperties()->setProperty("ServerRandom.ThreadPool.SizeMax","3000");
	Ice::ObjectAdapterPtr adapter=_ic->createObjectAdapter("ServerRandom"); */
	// _ic->getProperties()->setProperty(serverName+".Endpoints","tcp -h " + localIP);
	string endpoints = protocol2Endpoints(protocal,localIP);
	_ic->getProperties()->setProperty(serverName+".Endpoints", endpoints );
	_ic->getProperties()->setProperty(serverName+".ThreadPool.Size",threadPoolSize);
	_ic->getProperties()->setProperty(serverName+".ThreadPool.SizeMax",threadPoolSizeMax);
	_ic->getProperties()->setProperty(serverName+".ThreadPool.StackSize",stackSize);
	Ice::ObjectAdapterPtr adapter=_ic->createObjectAdapter(serverName);
	adapter->activate();
	_prx=adapter->addWithUUID(pServer)->ice_timeout(300);

	MCE_INFO("[ServerStateSubscriber::initialize] Protocal : " << protocal << " Endpoints : " << endpoints); 

	if ( !_hasTopic ) {	
		MCE_DEBUG("[ServerStateSubscriber::initialize]notopic");
		_topic = new SbjTopicI("ServerState", this, _ctrEndpoints, _interval, _channel);
		_hasTopic = true;
	}
  init_ServerStateBackDoor();
}

void ServerStateSubscriber::initialize(const string& ctrEndpoints, const Ice::ObjectPtr pServer, const string& endpoints, const string& identity, int cluster, int mod, int interval, const MyUtil::ChannelPtr& channel, const string& threadPoolSize, const string& threadPoolSizeMax, const string& stackSize) {
	initialize(ctrEndpoints, pServer, mod, interval, channel, threadPoolSize, threadPoolSizeMax, stackSize);
}

void ServerStateSubscriber::initialize(const string& ctrEndpoints, 
		const map<string,Ice::ObjectPtr>& pServerList, int mod, int interval, const MyUtil::ChannelPtr& channel, const string& threadPoolSize, const string& threadPoolSizeMax, const string& stackSize) {
  initialize(ctrEndpoints, pServerList, 0, mod, interval, channel, threadPoolSize, threadPoolSizeMax, stackSize);
}

void ServerStateSubscriber::initialize(const string& ctrEndpoints, 
		const map<string,Ice::ObjectPtr>& pServerList, const Ice::ObjectPtr pServer, int mod, int interval, const MyUtil::ChannelPtr& channel, const string& threadPoolSize, const string& threadPoolSizeMax, const string& stackSize, const string& protocal) {
	if ( _inited ) {
		return;
	} else {
		_inited = true;
	}
  _channel = channel;
	bIsMutilServicesInOneProcess=true;
	
	if(pServerList.size()<=0)
	{
		return;
	}
	// 基本参数赋值
	_ctrEndpoints = ctrEndpoints;
	_interval = interval;
	_mod = mod;
	_newSet = true;

	// 没有预加载status为0
	_status = 0;

	// 生成本地服务的Prx
	// _prx = ServiceI::instance().getCommunicator()->stringToProxy(identity+"@"+endpoints);	
	string localIP = MyUtil::IpAddr::inner();

	//为各个小服务生成代理
	map<string,Ice::ObjectPtr>::const_iterator it = pServerList.begin();
  if (pServer != 0 && _prx == 0) {
	  ServiceI& service = ServiceI::instance();
	  string serverName = service.getAdapter()->getName();
		Ice::CommunicatorPtr _ic = _channel->getCommunicator();
	  string endpoints = protocol2Endpoints(protocal,localIP);
		_ic->getProperties()->setProperty(serverName+".Endpoints",endpoints);
		_ic->getProperties()->setProperty(serverName+".ThreadPool.Size",threadPoolSize);
		_ic->getProperties()->setProperty(serverName+".ThreadPool.SizeMax",threadPoolSizeMax);
		_ic->getProperties()->setProperty(serverName+".ThreadPool.StackSize",stackSize);
	
		Ice::ObjectAdapterPtr adapter=_ic->createObjectAdapter(serverName);
		adapter->activate();
		Ice::ObjectPrx prx=adapter->addWithUUID(pServer)->ice_timeout(300);
    _prx = prx;
  }
	for(;it!=pServerList.end();it++)
	{
		Ice::CommunicatorPtr _ic = _channel->getCommunicator();
		_ic->getProperties()->setProperty(it->first+".Endpoints","tcp -h " + localIP);
		_ic->getProperties()->setProperty(it->first+".ThreadPool.Size",threadPoolSize);
		_ic->getProperties()->setProperty(it->first+".ThreadPool.SizeMax",threadPoolSizeMax);
		_ic->getProperties()->setProperty(it->first+".ThreadPool.StackSize",stackSize);
	
		Ice::ObjectAdapterPtr adapter=_ic->createObjectAdapter(it->first);
		adapter->activate();
		Ice::ObjectPrx prx=adapter->addWithUUID(it->second)->ice_timeout(300);
		if(_prx==0)
		{
			_prx=prx;//随便给原来的_prx赋一个值，为了以前的进行兼容
		}
		_prxMap.insert(make_pair<string,Ice::ObjectPrx>(it->first,prx));
		_statusMap.insert(make_pair<string,int>(it->first,0));
	}
	
	if ( !_hasTopic ) {	
		MCE_DEBUG("notopic");
		_topic = new SbjTopicI("ServerState", this, _ctrEndpoints, _interval, _channel);
		_hasTopic = true;
	}
  init_ServerStateBackDoor();
}

SbjStatePtr ServerStateSubscriber::opera(const ::MyUtil::SbjStatePtr& sbjstate, const ::Ice::Current& current) {

	if ( _newSet ) {
	_newSet = false;	/* 被查过一次后, _newSet就是false了 */
	}

  /*
   * 根据日志级别设置来进行输出，保证会输出到日志文件。
   * 硬盘出问题，该Prx在Conroller会被及时踢掉。
   */
  LogLevel level = Logger::getInstance("Mce").getLogLevel();
  switch(level){
    case FATAL_LOG_LEVEL:{
      MCE_FATAL("ServerStateSubscriber::opera bIsMutilServicesInOneProcess:" << bIsMutilServicesInOneProcess);
    }break;
    case WARN_LOG_LEVEL:{
      MCE_WARN("ServerStateSubscriber::opera bIsMutilServicesInOneProcess:" << bIsMutilServicesInOneProcess);
    }break;
    case INFO_LOG_LEVEL:{
      MCE_INFO("ServerStateSubscriber::opera bIsMutilServicesInOneProcess:" << bIsMutilServicesInOneProcess);
    }break;
    case DEBUG_LOG_LEVEL:{
      MCE_DEBUG("ServerStateSubscriber::opera bIsMutilServicesInOneProcess:" << bIsMutilServicesInOneProcess);
    }break;
    case TRACE_LOG_LEVEL:{
      MCE_TRACE("ServerStateSubscriber::opera bIsMutilServicesInOneProcess:" << bIsMutilServicesInOneProcess);
    }break;
    default:{
      MCE_INFO("ServerStateSubscriber::opera bIsMutilServicesInOneProcess:" << bIsMutilServicesInOneProcess);
    }
  }	

	if(bIsMutilServicesInOneProcess==true)
	{
		ServerStateInfoMutilPtr ssStatePtr = new ServerStateInfoMutil;
		ssStatePtr->prx = _prx;
		ssStatePtr->cluster = 0;
		ssStatePtr->mod = _mod;
		ssStatePtr->newset = _newSet;
		ssStatePtr->status = _status;		// 默认的策略是只要能调到这个服务，就返回服务可用 //

		ssStatePtr->prxMap=_prxMap;
		ssStatePtr->statusMap=_statusMap;

		return ssStatePtr;
	}else
	{
		ServerStateInfoPtr ssStatePtr = new ServerStateInfo;
		ssStatePtr->prx = _prx;
		ssStatePtr->cluster = 0;
		ssStatePtr->mod = _mod;
		ssStatePtr->newset = _newSet;
		ssStatePtr->status = _status;		/* 默认的策略是只要能调到这个服务，就返回服务可用 */

		return ssStatePtr;
	}

}

void ServerStateSubscriber::setStatus(int status, string identifier)
{
  MCE_INFO("ServerStateSubscriber::setStatus "<< identifier << " " << status);
	ServerPrxStatusMap::iterator it = _statusMap.find(identifier);
	if(it != _statusMap.end())
	{
		it->second = status;
	}
}

void ServerStateSubscriber::setStatus(int status) {
  MCE_INFO("ServerStateSubscriber::setStatus " << status);
	_status = status;
}

int ServerStateSubscriber::getStatus() {
  return _status;
}

int ServerStateSubscriber::getStatus(const string& identifier) {
  ServerPrxStatusMap::iterator it = _statusMap.find(identifier);
  if(it != _statusMap.end()) {
    return it->second;
  }
  return 0;
}

Ice::ObjectPrx ServerStateSubscriber::getProxySelf() {
	return _prx;
}

string ServerStateSubscriber::protocol2Endpoints(const string& protocal, const string& localIP) {

	string endpoints = "";
	vector<string> protocals = StrUtil::split(protocal, ":");
	for (size_t i = 0; i < protocals.size(); ++i) {
		vector<string> params = StrUtil::split(protocals[i], " ");
		string theEndpoint = "";
		if ( params.size() >= 1 ) {
			theEndpoint = params[0] + " -h " + localIP;
		}
		for ( size_t j = 1; j < params.size(); ++j ) {
			theEndpoint += " " + params[j];
		}
		if ( i > 0 ) {
			endpoints += ":";
		}
		endpoints += theEndpoint;
	}

	return endpoints;	
}
