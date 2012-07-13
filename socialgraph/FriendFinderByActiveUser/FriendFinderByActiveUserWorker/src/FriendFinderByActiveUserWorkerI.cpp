#include "FriendFinderByActiveUserWorkerI.h"

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace xce::friendfinder;
using namespace xce::serverstate;
using namespace MyUtil;
using namespace xce::logindata;
using namespace com::xiaonei::xce;
using namespace std;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add( &FriendFinderByActiveUserWorkerI::instance(),
			service.createIdentity("FFBUW", "") );

	ConnectionPoolManager::instance().initialize();        //连接数据库
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("FriendFinderByActiveUserWorker.Mod");
	int sInterval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FriendFinderByActiveUserWorker.Interval", 5);

	//向controller注册
	ServerStateSubscriber::instance().initialize("ControllerFriendFinderByActiveUserWorker", &FriendFinderByActiveUserWorkerI::instance(), mod, sInterval);
}

void FriendFinderByActiveUserWorkerI::load(const Ice::Current& current) {
	MCE_INFO("[FriendFinderByActiveUserWorkerI] load");
	PreloadLoginData worker;
	worker.loadFromDB();      //加载数据

	assert(false);  //计算结束重启服务
}
