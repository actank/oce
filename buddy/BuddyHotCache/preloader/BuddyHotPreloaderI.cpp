#include "ServiceI.h"
#include "BuddyHotCache.h"
#include "BuddyHotPreloaderI.h"
#include "GeneralPreloader/src/GeneralPreloader.h"
#include "../share/BuddyHotFactory.h"

using namespace MyUtil;
using namespace xce::generalpreloader;
using namespace xce::buddy;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&BuddyHotPreloader::instance(), service.createIdentity("PL", ""));

  string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("BuddyHotCache.Controller", "ControllerBuddyHotCache");
  int cInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 60);
  int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyHotCache.Proxy.Timeout", 300);
  int setDataQueueSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyHotPreloader.SetDataQueueSize", 100000);
  int produceBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyHotPreloader.ProduceBatchSize", 10000);
  int consumeBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyHotPreloader.ConsumeBatchSize", 3000);
  int writeBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyHotPreloader.WriteBatchSize", 3000);
  int threadSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyHotPreloader.ThreadSize", 5);

  int cluster = 1;

  MCE_DEBUG("ctlEndpoint: " << ctlEndpoint << " cluster: " << cluster << " setDataQueueSize: " << setDataQueueSize);
  MCE_DEBUG("cInterval: " << cInterval << " timeout : " << timeout << " produceBatchSize: " << produceBatchSize);
  MCE_DEBUG("consumBatchSize: " << consumeBatchSize << "writeBatchSize: " << writeBatchSize << " threadSize: " << threadSize);
  
  GeneralPreloader<BuddyHotCacheManagerPrx>::instance().initialize(
      new BatchUsersIdProducer,
      new BuddyHotLoaderFactory(),
      ctlEndpoint,
      cluster,
      cInterval,
      timeout,
      setDataQueueSize,
      produceBatchSize,
      consumeBatchSize ,
      writeBatchSize ,
      threadSize);
}
