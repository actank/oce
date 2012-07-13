#include "NotifyUpdateUserBackupLogic.h"
#include "LogWrapper.h"
#include "Date.h"


#include "time.h"

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

#include "util/cpp/TimeCost.h"
//#include "UpdaterBackupLogicI.h"
using namespace xce::notifyupdatebackuplogic;
using namespace MyUtil;
using namespace com::xiaonei::xce;

using namespace ::xce::serverstate;
//--------------------------------------------------------------------------------
/* the new instialize */
void MyUtil::initialize() {
	ServiceI& service=ServiceI::instance();
	service.getAdapter()->add(&NotifyUpdateUserBackupLogicI::instance(), service.createIdentity(
			"NUU", ""));

	//用来setdata的task 	
    TaskManager::instance().config(TASK_LEVEL_NOTIFYUPDATE, MyUtil::ThreadPoolConfig(1, 100));
	
	//-----------------------------------HA---------------------------------------------
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("NotifyUpdateBackupLogic.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("NotifyUpdateBackupLogic.Interval",5);
	
	map<string,Ice::ObjectPtr> mapNameAndService;
	
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>("NUU",&NotifyUpdateUserBackupLogicI::instance()));
	
	ServerStateSubscriber::instance().initialize("ControllerNotifyUpdateBackupLogic",mapNameAndService,mod,interval);
	
}

