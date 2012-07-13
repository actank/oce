#include "BuddyRelationLoaderI.h"
#include "BuddyRelationReplicaAdapter.h"
#include "buddy/BuddyRelationCache/share/BuddyRelationShare.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace std;
using namespace xce::buddy;
using namespace xce::buddy::adapter;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::clusterstate;
using namespace xce::serverstate;
using namespace xce::generalpreloader;

void MyUtil::initialize(){
        ServiceI& service = ServiceI::instance();
        service.getAdapter()->add(&BuddyRelationLoaderI::instance(), service.createIdentity("L", ""));

        TaskManager::instance().config(TASK_LEVEL_RELOAD, ThreadPoolConfig(1, 1));
        service.registerXceObserver(new ServiceSchemaI);

        int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("BuddyRelationLoader.Mod");
        int sInterval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyRelationLoader.Interval", 5);
        // 向Controller注册
        ServerStateSubscriber::instance().initialize("ControllerBuddyRelationLoader",
                        &BuddyRelationLoaderI::instance(), mod, sInterval);

}

/*****************************************************************************/

void BuddyRelationLoaderI::reload(::Ice::Int id, const Ice::Current& current ){
        MCE_INFO( "[BuddyRelationLoaderI::reload] id=" << id );
        TaskManager::instance().execute( new ReloadTask( id ) );
}

void BuddyRelationLoaderI::setRelation(const Relationship& relation, BuddyDesc desc, const Ice::Current& current) {
        MCE_INFO( "[BuddyRelationLoaderI::setRelation] relation[" << relation.from << ":" << relation.to << "]"  );
	try {
		BuddyRelationCacheAdapter::instance().setRelation(relation, desc);
		BuddyRelationTTAdapter::instance().setRelation(relation, desc);
	} catch (Ice::Exception& e) {
		MCE_WARN("BuddyRelationLoaderI::setRelation, relation[" << relation.from << "," << relation.to << "] " << e.what());
	}

}

/*****************************************************************************/

void ReloadTask::handle(){
	try {
		BuddyRelationDataPtr data = BuddyRelationDataPtr::dynamicCast(BuddyRelationFactory::instance().create(_id));
		BuddyRelationCacheAdapter::instance().addBuddyRelationData(_id, data);
		BuddyRelationTTAdapter::instance().addBuddyRelationData(_id, data);
	} catch (Ice::Exception& e) {
		MCE_WARN("[ReloadTask::handle()] " << " id=" << _id << " " << e.what());
	}
}

/*****************************************************************************/

void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
        TaskManager::instance().config(props, "Load", TASK_LEVEL_RELOAD, ThreadPoolConfig(1,1));
}

