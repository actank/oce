#include "ScorePreloaderI.h"
#include "ScoreCache.h"
#include "../share/ScoreCacheDataFactory.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace std;
using namespace xce::scorecache;
using namespace com::xiaonei::xce;
using namespace xce::clusterstate;
using namespace xce::serverstate;
using namespace xce::cacheloader;

void MyUtil::initialize(){
	{
		string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("ScoreCacheN.Controller", "ControllerScoreCacheN");
		int cInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 60);
		int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("ScoreCacheN.Proxy.Timeout", 300);
		int produceBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("ScorePreloader.ProduceBatchSize", 10000);
		int consumeBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("ScorePreloader.ConsumeBatchSize", 3000);
		int writeBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("ScorePreloader.WriteBatchSize", 1000);
		int threadSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("ScorePreloader.ThreadSize", 4);

		CacheLoader<ScoreCacheNManagerPrx>::instance().initialize(
				new BatchUsersIdProducer,
				&ScoreCacheDataNFactory::instance(),
				ctlEndpoint,
				cInterval,
				timeout,
				produceBatchSize,
				consumeBatchSize,
				writeBatchSize,
				threadSize );
	}
}

/*****************************************************************************/

