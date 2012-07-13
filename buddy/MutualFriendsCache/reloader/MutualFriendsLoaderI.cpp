#include "MutualFriendsLoaderI.h"
#include "MutualFriendsAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "StatusCache.h"

using namespace std;
using namespace xce::buddy;
using namespace xce::mutualfriends;
using namespace xce::buddy::adapter;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::clusterstate;
using namespace xce::serverstate;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&MutualFriendsLoaderI::instance(), service.createIdentity("L", ""));

	TaskManager::instance().config(TASK_LEVEL_FANS_LOADER_RELOAD, ThreadPoolConfig(1, 5));
	service.registerXceObserver(new ServiceSchemaI);

	SetDataHelper::instance().initialize();

	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("MutualFriendsLoader.Mod");
	int sInterval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("MutualFriendsLoader.Interval", 5);
	// 向Controller注册
	ServerStateSubscriber::instance().initialize("ControllerMutualFriendsLoader", &MutualFriendsLoaderI::instance(), mod, sInterval);
}

//--------------------------MutualFriendsLoaderI--------------------------------
void MutualFriendsLoaderI::reload(Ice::Long gid, const Ice::Current& current) {
	MCE_INFO( "[MutualFriendsLoaderI::reload] gid=" << gid );
	TaskManager::instance().execute(new ReloadTask(gid));
}

//-----------------------------SetDataHelper------------------------------------
void SetDataHelper::initialize() {
	setter_ = new SetDataThread(*this);
	setter_->start(1024 * 1024).detach();
}

void SetDataHelper::invokeDelay(Ice::Long gid, const Ice::ObjectPtr& o) {
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
	datas_.push_back(make_pair<long, Ice::ObjectPtr> (gid, o));
	if (datas_.size() >= 100)
		mutex_.notify();
}
void SetDataHelper::invokeNow(Ice::Long gid, const Ice::ObjectPtr& o) {
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
	datas_.push_back(make_pair<long, Ice::ObjectPtr> (gid, o));
	mutex_.notify();
}
void SetDataHelper::swap(vector<pair<long, Ice::ObjectPtr> >& empty_vector) {
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
	if (datas_.size() < 100)
		mutex_.timedWait(IceUtil::Time::seconds(3));
	datas_.swap(empty_vector);
}

void SetDataHelper::SetDataThread::run() {
	while (true) {
		std::vector<pair<long, Ice::ObjectPtr> > to_be_set_;
		helper_.swap(to_be_set_);
		if (to_be_set_.empty()) {
			MCE_INFO("[SetDataHelper::SetDataThread::run] setData: 0 objs." );
			continue;
		}
		MyUtil::ObjectResultPtr objs = new MyUtil::ObjectResult();
		for (std::vector<pair<long, Ice::ObjectPtr> >::iterator it = to_be_set_.begin(); it != to_be_set_.end(); ++it) {
			objs->data.insert(*it);
		}
		MutualFriendsCacheAdapter::instance().setData(objs);
	}
}

//--------------------------------Task------------------------------------------
void ReloadTask::handle() {
	FansObjectPtr fansObj;
	try {
		fansObj = FansObjectPtr::dynamicCast(FansFactory::instance().create(_gid));
	} catch (Ice::Exception& e) {
		MCE_WARN("[ReloadTask] Ice::Exception (create:" << _gid << ") : " << e.what());
		return;
	} catch (mysqlpp::Exception& e) {
		MCE_WARN("[ReloadTask] mysqlpp::Exception (create:" << _gid << ") : " << e.what());
		return;
	} catch (std::exception& e) {
		MCE_WARN("[ReloadTask] std::excetpion (create:" << _gid << ") : " << ": " << e.what());
		return;
	} catch (...) {
		MCE_WARN("[ReloadTask] unknown exception (create:" << _gid << ") ." );
		return;
	}
	if (fansObj == 0) {
		return;
	}
	SetDataHelper::instance().invokeDelay(_gid, fansObj);
}

//--------------------------------Config----------------------------------------
void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
	TaskManager::instance().config(props, "Reload", TASK_LEVEL_FANS_LOADER_RELOAD, ThreadPoolConfig(1, 5));
}

