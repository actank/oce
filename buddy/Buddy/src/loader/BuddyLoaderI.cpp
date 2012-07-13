#include "BuddyLoaderI.h"
#include "BuddyAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "../../../BuddyEntry/src/BuddyCreate.h"
#include "../../../BuddyEntry/src/BuddyRelation.h"

using namespace std;
using namespace xce::buddy;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::clusterstate;
using namespace xce::serverstate;
using namespace xce::cacheloader;

void MyUtil::initialize(){
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&BuddyLoaderI::instance(), service.createIdentity("L", ""));

       // service.registerXceObserver(new ServiceSchemaI);

	string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("Buddy.Controller", "ControllerBuddy");
	int cInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 60);
	int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Buddy.Proxy.Timeout", 300);
	int produceBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByIdLoader.ProduceBatchSize", 10000);
        int consumeBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByIdLoader.ConsumeBatchSize", 3000);
        int writeBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByIdLoader.WriteBatchSize", 1000);
        int threadSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByIdLoader.ThreadSize", 8);


	CacheLoader<BuddyManagerPrx>::instance().initialize( 
		new BatchUsersIdProducer,
		&BuddyRelationHoldersILoaderFactory::instance(),
		ctlEndpoint,
		cInterval,
		timeout,
		produceBatchSize,
                consumeBatchSize,
                writeBatchSize,
		threadSize );

        int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("BuddyLoader.Mod");
        int sInterval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyLoader.Interval", 5);
        // 向Controller注册
        ServerStateSubscriber::instance().initialize("ControllerBuddyLoader",
                        &BuddyLoaderI::instance(), mod, sInterval);
}

void BuddyLoaderI::load( ::Ice::Int id, const Ice::Current& current ){
        MCE_INFO( "[BuddyLoaderI::load] id=" << id );
	
	Ice::ObjectPtr o = BuddyRelationHoldersILoaderFactory::instance().create(id);
	
	BuddyAdapter::instance().load( id, o );	
}

/*
void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
}
*/
