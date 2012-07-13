#include "UpdateUserBackupLogic.h"
#include "LogWrapper.h"
#include "Date.h"


#include "time.h"

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

#include "util/cpp/TimeCost.h"
using namespace xce::updatebackuplogic;
using namespace MyUtil;
using namespace com::xiaonei::xce;

using namespace ::xce::serverstate;
//--------------------------------------------------------------------------------
/* the new instialize */
void MyUtil::initialize() {
	ServiceI& service=ServiceI::instance();
	service.getAdapter()->add(&UpdateUserBackupLogicI::instance(), service.createIdentity(
			"UU", ""));

	//用来setdata的task 	
    TaskManager::instance().config(TASK_LEVEL_UPDATE, MyUtil::ThreadPoolConfig(1, 100));
	
	//-----------------------------------HA---------------------------------------------
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("UpdateBackupLogic.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UpdateBackupLogic.Interval",5);
	
	map<string,Ice::ObjectPtr> mapNameAndService;
	
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>("UU",&UpdateUserBackupLogicI::instance()));
	
	ServerStateSubscriber::instance().initialize("ControllerUpdateBackupLogic",mapNameAndService,mod,interval);
	
}

