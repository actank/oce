/*
 * =====================================================================================
 *
 *       Filename:  HomeFootprintBufReaderPreloader.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年11月22日 14时39分05秒
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *        Company:  renren-inc
 *
 * =====================================================================================
 */
#include "../../../share/HomeFootprintBufPreloader.h"
#include "../../../HomeFootprint/HomeFootprintCacheI.h"
#include "QueryRunner.h"
#include "ActiveTrackAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "GeneralPreloader/src/GeneralPreloader.h"
#include "util/cpp/TimeCost.h"
#include <Monitor/client/service/ServiceMonitor.h>
#include <sstream>

using namespace xce::footprint;

void MyUtil::initialize() {

  MyUtil::ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&HomeFootprintBufPreloader::instance(), service.createIdentity("HFRP", ""));
  std::string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("FootprintUnite.Controller",
      "ControllerFootprintUnite");
  int subInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 5);
  int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FootprintUnite.Proxy.Timeout", 300);

  int cluster = 1;
  xce::generalpreloader::GeneralPreloader<HomeFootprintBufReaderPrx>::instance().initialize(
      new BatchHomeFootprintBufIdProducer,
      new HomeFootprintBufPreloaderFactory,
      ctlEndpoint,
      cluster,
      subInterval,
      timeout,
      PRX_DATAQUEUE_MAX_SIZE,
      PRODUCE_BATCH_SIZE,
      CONSUME_BATCH_SIZE,
      WRITE_BATCH_SIZE,
      CONSUME_BATCH_THREAD_SIZE,
      "HFBR", true, 100);
}

