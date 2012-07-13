/*
 * =====================================================================================
 *
 *       Filename:  HomeFootprintReplicaReaderPreloader.cpp
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

#include <FootprintUnite.h>
#include "ServiceI.h"
#include "../../../share/HomeFootprintPreloader.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "ActiveTrackAdapter.h"

#include "util/cpp/InvokeMonitor.h"
#include <Monitor/client/service/ServiceMonitor.h>

using namespace MyUtil;
using namespace xce::footprint;
using namespace com::xiaonei::xce; 
using namespace xce::log;
using namespace xce::serverstate;

void MyUtil::initialize() {

  MyUtil::ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&HomeFootprintPreloader::instance(), service.createIdentity("HFRRP", ""));
  std::string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("HomeFootprintReplica.Controller",
      "ControllerHomeFootprintReplica");
  int subInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 5);
  int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("HomeFootprintReplica.Proxy.Timeout", 300);

  int cluster = 1;
  xce::generalpreloader::GeneralPreloader<HomeFootprintReaderPrx>::instance().initialize(
      new BatchHomeFootprintIdProducer,
      new HomeFootprintPreloaderFactory,
      ctlEndpoint,
      cluster,
      subInterval,
      timeout,
      PRX_DATAQUEUE_MAX_SIZE,
      PRODUCE_BATCH_SIZE,
      CONSUME_BATCH_SIZE,
      WRITE_BATCH_SIZE,
      CONSUME_BATCH_THREAD_SIZE,
      "HFRR", true, 100);
}
