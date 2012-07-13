#include "UserOnline.h"
#include "UserOnlineServiceI.h"
#include "OnlineManagerI.h"
#include "TaskManager.h"
#include "MainAdapter.h"
#include "ConnectionQuery.h"
#include "Session.h"
#include "ServiceI.h"
using namespace mop::hi::svc::online;
//using namespace mop::hi::svc::adapter;
using namespace com::xiaonei::xce;


//---------------------------------------------------------------------------
void
MyUtil::initialize()
{
	MCE_DEBUG("UserOnlineServiceI initialize.");
    Ice::PropertiesPtr props = ServiceI::instance().getCommunicator()->getProperties();
	int evictStep = props->getPropertyAsIntWithDefault(
			"Service."+ServiceI::instance().getName()+".evictStep", 5);
	int expStep = props->getPropertyAsIntWithDefault(
			"Service."+ServiceI::instance().getName()+".expStep", 15*60);
	string activeUserManagerId = props->getPropertyWithDefault(
			"Service."+ServiceI::instance().getName()+".ActiveUserManager", "AU@ActiveUser");
	
	OnlineManagerI::instance()._activeUserManager = ActiveUserManagerPrx::uncheckedCast(
		ServiceI::instance().getCommunicator()->stringToProxy(activeUserManagerId)->ice_oneway());
	OnlineManagerI::instance()._imOnlineDb = props->getPropertyWithDefault("Service."+ServiceI::instance().getName()+".ImOnlineDb", "imonline");
	OnlineManagerI::instance()._onlineDb = props->getPropertyWithDefault("Service."+ServiceI::instance().getName()+".OnlineDb", "online");
	
	
	OnlineManagerI::instance().init(evictStep, expStep);
	ServiceI::instance().getAdapter()->add(&OnlineManagerI::instance(),
			ServiceI::instance().getCommunicator()->stringToIdentity(MANAGER));
	
	mop::hi::svc::adapter::UserAdapter::instance().initializeWith(ServiceI::instance().getAdapter());	
	ConnectionPoolManager::instance().initialize(ServiceI::instance().getAdapter());
	mop::hi::svc::adapter::MainAdapter::instance().initializeWith(ServiceI::instance().getAdapter());
	//notify all web online user offline

	
}




