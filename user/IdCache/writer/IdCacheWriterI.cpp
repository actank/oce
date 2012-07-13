#include "IdCacheWriterI.h"

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include <Monitor/client/service/ServiceMonitor.h>
#include "util/cpp/InvokeMonitor.h"
#include "user/IdCache/share/IdCacheDataI.cpp"
#include "IdCacheReaderAdapter.h"

void MyUtil::initialize()
{
	MyUtil::ServiceI& service = MyUtil::ServiceI::instance();

	service.getAdapter()->add(&idcache::IdCacheWriterI::instance(), service.createIdentity(
			"M", ""));
	
//-----------------------------------HA---------------------------------------------
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("IdCacheWriter.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("IdCacheWriter.Interval",5);
	
	xce::serverstate::ServerStateSubscriber::instance().initialize("ControllerIdCacheWriter",&idcache::IdCacheWriterI::instance(),mod,interval);
	MyUtil::ServiceMonitorManager::instance().start();
}

namespace idcache
{

void IdCacheWriterI::createIdRelation(::Ice::Int id, ::Ice::Int transId, ::Ice::Int virtualId, const ::Ice::Current& current)
{
	std::ostringstream os;
	os << " userId = " << id << " transId = " << transId << " virtualId = " << virtualId;
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, os.str(), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
	IdCacheReaderAdapter::instance().createIdRelation(id, transId, virtualId);
}

void IdCacheWriterI::deleteIdRelation(::Ice::Int id, ::Ice::Int virtualId, const ::Ice::Current& current)
{
	std::ostringstream os;
	os << " userId = " << id << " virtualId = " << virtualId;
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, os.str(), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
	IdCacheReaderAdapter::instance().deleteIdRelation(id, virtualId);
}

void IdCacheWriterI::reload(::Ice::Int id, const ::Ice::Current& current)
{
	std::ostringstream os;
	os << " id = " << id;
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, os.str(), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);

	if( IdCacheReaderAdapter::instance().isVirtualId(id) )
	{
		VirtualIdInfoPtr data = VirtualIdInfoFactoryI::load(id);
		if(data)
		{
			id = data->userId;
		}
	}
	
	UserId2RelatedIdInfoPtr data = UserId2RelatedIdInfoFactoryI::load(id);
	if(data)
	{
		IdCacheReaderAdapter::instance().setData(id, data);
	}
}

void IdCacheWriterI::setValid(bool flag, const ::Ice::Current& current)
{
	std::ostringstream os;
	os << " valid = " << flag;
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, os.str(), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
	::xce::serverstate::ServerStateSubscriber::instance().setStatus((int)flag);
}

}
