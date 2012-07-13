#include "ReplicaClusterDemoI.h"
#include "ServerStateSubscriber.h"

using namespace MyUtil;
using namespace xce::serverstate;
using namespace xce::replicaclusterdemo;
	
void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(new DemoManagerI, service.createIdentity("M", ""));


	Ice::PropertiesPtr prop = service.getCommunicator()->getProperties();
	string serverName = prop->getPropertyWithDefault("Service.Name", "");
	ServerStateSubscriber::instance().initialize("ControllerDemo", serverName, 40);
}

void SetServerName(const string& serverName) {
	_serverName = serverName;
}

StrSeq DemoManagerI::PrintClients(const Ice::Current& current)
{
    return _serverName;
}
