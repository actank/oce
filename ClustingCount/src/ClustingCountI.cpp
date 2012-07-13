#include "ClustingCountI.h"

using namespace mop::hi::svc::model;
using namespace MyUtil;

extern "C"
{
//
// Factory function
//
CLUSTING_COUNT_API ::IceBox::Service*
create(Ice::CommunicatorPtr communicator)
{
    return &ClustingCountServiceI::instance();
}
}


//=============================================================================
void
ClustingCountServiceI::initialize()
{	
	getAdapter()->add(new ClustingCounterI(*this), createIdentity(CLUSTING_COUNTER,""));	
};

//=============================================================================
ClustingCounterI::ClustingCounterI(ServiceI& service)
{
	Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
	_userServiceClustingCount = props->getPropertyAsIntWithDefault(
		"Service."+service.getName()+".UserServiceClustingCount", 10);
	_relationServiceClustingCount = props->getPropertyAsIntWithDefault(
		"Service."+service.getName()+".RelationServiceClustingCount", 10);
	_classServiceClustingCount = props->getPropertyAsIntWithDefault(
		"Service."+service.getName()+".ClassServiceClustingCount", 10);
	_tribeServiceClustingCount = props->getPropertyAsIntWithDefault(
		"Service."+service.getName()+".TribeServiceClustingCount", 5);
	_footprintServiceClustingCount = props->getPropertyAsIntWithDefault(
		"Service."+service.getName()+".FootprintServiceClustingCount", 10);
	_blogFootprintServiceClustingCount = props->getPropertyAsIntWithDefault(
		"Service."+service.getName()+".BlogFootprintServiceClustingCount", 1);
	_classFootprintServiceClustingCount = props->getPropertyAsIntWithDefault(
		"Service."+service.getName()+".ClassFootprintServiceClustingCount", 1);
	_notifyPnlServiceClustingCount = props->getPropertyAsIntWithDefault(
		"Service."+service.getName()+".NotifyPnlServiceClustingCount", 10);
	_notifyRplServiceClustingCount = props->getPropertyAsIntWithDefault(
		"Service."+service.getName()+".NotifyRplServiceClustingCount", 10);
	_notifyGplServiceClustingCount = props->getPropertyAsIntWithDefault(
		"Service."+service.getName()+".NotifyGplServiceClustingCount", 10);
	_notifyCplServiceClustingCount = props->getPropertyAsIntWithDefault(
		"Service."+service.getName()+".NotifyCplServiceClustingCount", 10);
	_notifyMnlServiceClustingCount = props->getPropertyAsIntWithDefault(
		"Service."+service.getName()+".NotifyMnlServiceClustingCount", 10);
	_notifyNewbieServiceClustingCount = props->getPropertyAsIntWithDefault(
		"Service."+service.getName()+".NotifyNewbieServiceClustingCount", 10);
}

ClustingCounterI::~ClustingCounterI()
{
	
}

Ice::Int
ClustingCounterI::getServiceClustingCount(
	const string& name, 
	const Ice::Current& )
{
	if (name == "User") {
		return _userServiceClustingCount;
	} else if (name == "Relation") {
		return _relationServiceClustingCount;
	} else if (name == "Class") {
		return _classServiceClustingCount;
	} else if (name == "Tribe") {
		return _tribeServiceClustingCount;
	} else if (name == "Footprint") {
		return _footprintServiceClustingCount;
	} else if (name == "BlogFootprint") {
		return _blogFootprintServiceClustingCount;
	} else if (name == "ClassFootprint") {
		return _classFootprintServiceClustingCount;
	} else if (name == "NotifyPnl") {
		return _notifyPnlServiceClustingCount;
	} else if (name == "NotifyRpl") {
		return _notifyRplServiceClustingCount;
	} else if (name == "NotifyGpl") {
		return _notifyGplServiceClustingCount;
	} else if (name == "NotifyCpl") {
		return _notifyCplServiceClustingCount;
	} else if (name == "NotifyMnl") {
		return _notifyMnlServiceClustingCount;
	} else if (name == "NotifyNewbie") {
		return _notifyNewbieServiceClustingCount;
	} 
	return 1;	
}

