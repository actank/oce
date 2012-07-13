#include "OfferFriendsPreloader.h"
#include "OfferFriends.h"
#include "../share/OfferCacheDataFactory.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace std;
using namespace xce::offerfriends;
using namespace com::xiaonei::xce;
using namespace xce::clusterstate;
using namespace xce::serverstate;
using namespace xce::cacheloader;

void MyUtil::initialize(){
	string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("OfferFriendsCache.Controller", "ControllerOfferFriendsCache");
	int cInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 60);
	int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("OfferFriendsCache.Proxy.Timeout", 300);
	int produceBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("OfferFriendsPreloader.ProduceBatchSize", 10000);
	int consumeBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("OfferFriendsPreloader.ConsumeBatchSize", 3000);
	int writeBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("OfferFriendsPreloader.WriteBatchSize", 1000);
	int threadSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("OfferFriendsPreloader.ThreadSize", 1);

	CacheLoader<OfferFriendsCacheNManagerPrx>::instance().initialize(
			new BatchUsersIdProducer,
			&OfferCacheDataFactory::instance(),
			ctlEndpoint,
			cInterval,
			timeout,
			produceBatchSize,
			consumeBatchSize,
			writeBatchSize,
			threadSize );
}

/*****************************************************************************/

