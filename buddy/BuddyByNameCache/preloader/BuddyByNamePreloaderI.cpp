/*
 * =====================================================================================
 *
 *       Filename:  BuddyByNamePreloaderI.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年05月26日 17时09分42秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */
#include "ServiceI.h"
#include "BuddyByNamePreloaderI.h"
//#include "GeneralPreloader/src/GeneralPreloader.h"
#include "buddy/BuddyByNameCache/share/BuddyByNameFactory.h"
#include "ReplicaCluster/CacheLoader/src/CacheLoader.h"

using namespace MyUtil;
//using namespace xce::generalpreloader;
using namespace xce::buddybynamepreloader;
using namespace xce::buddybyname;
using namespace xce::cacheloader;


void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&BuddyByNamePreloader::instance(), service.createIdentity("PL", ""));

  /*       string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("BuddyByNameCache.Controller", "ControllerBuddyByNameCache");
           int cInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 60);
           int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByNameCache.Proxy.Timeout", 300);
           int setDataQueueSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByNamePreloader.SetDataQueueSize", 100000);
           int produceBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByNamePreloader.ProduceBatchSize", 100);
           int consumeBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByNamePreloader.ConsumeBatchSize", 100);
           int writeBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByNamePreloader.WriteBatchSize", 100);
           int threadSize = 5;//ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByNamePreloader.ThreadSize", 5);

           int cluster = 10;

           MCE_DEBUG("ctlEndpoint: " << ctlEndpoint << " cluster: " << cluster << " setDataQueueSize: " << setDataQueueSize);
           MCE_DEBUG("cInterval: " << cInterval << " timeout : " << timeout << " produceBatchSize: " << produceBatchSize);
           MCE_DEBUG("consumBatchSize: " << consumeBatchSize << "writeBatchSize: " << writeBatchSize << " threadSize: " << threadSize);
           GeneralPreloader<xce::buddy::BuddyByNameCacheManagerPrx>::instance().initialize(
           new BatchUsersIdProducer,
           &BuddyByNameFactory::instance(),
           ctlEndpoint,
           cluster,
           cInterval,
           timeout,
           setDataQueueSize,
           produceBatchSize,
           consumeBatchSize ,
           writeBatchSize ,
           threadSize);*/
  string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("BuddyByNameCache.Controller", "ControllerBuddyByNameCache");
  int cInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 60);
  int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByNameCache.Proxy.Timeout", 300);
  int produceBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByNamePreLoader.ProduceBatchSize", 10000);
  int consumeBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByNamePreLoader.ConsumeBatchSize", 3000);
  int writeBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByNamePreLoader.WriteBatchSize", 3000);
  int threadSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByNamePreLoader.ThreadSize", 10);

  CacheLoader<BuddyByNameCacheManagerPrx>::instance().initialize(
      new BatchUsersIdProducer,
      &BuddyByNameFactory::instance(),
      ctlEndpoint,
      cInterval,
      timeout,
      produceBatchSize,
      consumeBatchSize,
      writeBatchSize,
      threadSize );

  //------------------------------------------------------------
  service.registerXceObserver(new ServiceSchemaI);

}

//--------------------------------------------------------------------

void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {

  string loaderMod = props->getProperty("Service.BuddyByNamePreloader.LoaderMod");
  if ("UserCache" == loaderMod) {
    BuddyByNameFactory::instance().changeLoaderMod(BuddyByNameFactory::USERCACHE);
  } else if ("Db" == loaderMod) {
    BuddyByNameFactory::instance().changeLoaderMod(BuddyByNameFactory::DB);
  }
}

