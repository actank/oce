/*
 * =====================================================================================
 *
 *       Filename:  HomeUserVisitCountReplicaReaderPreloader.cpp
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
#include "../../../share/HomeUserVisitCountPreloader.h"
#include "ActiveTrackAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "GeneralPreloader/src/GeneralPreloader.h"
#include "util/cpp/TimeCost.h"
#include <Monitor/client/service/ServiceMonitor.h>
#include <sstream>
#include <iostream>

using namespace xce::footprint;
using namespace xce::utils;

void MyUtil::initialize() {
  cout << "[MyUtil::initialize] started" << endl;;
  MyUtil::ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&HomeUserVisitCountPreloader::instance(), service.createIdentity("HUVCRRP", ""));
  cout << "[MyUtil::initialize] getAdapter()->add finished" << endl;
  std::string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("HomeFootprintReplica.Controller",
      "ControllerHomeFootprintReplica");
  cout << "[MyUtil::initialize] controller is = " << ctlEndpoint << endl;
  int subInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 5);
  int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("HomeFootprintReplica.Proxy.Timeout", 300);

  int cluster = 1;
  xce::generalpreloader::GeneralPreloader<HomeUserVisitCountReaderPrx>::instance().initialize(
      new BatchHomeUserVisitCountIdProducer,
      new HomeUserVisitCountPreloaderFactory,
      ctlEndpoint,
      cluster,
      subInterval,
      timeout,
      PRX_DATAQUEUE_MAX_SIZE,
      PRODUCE_BATCH_SIZE,
      CONSUME_BATCH_SIZE,
      WRITE_BATCH_SIZE,
      CONSUME_BATCH_THREAD_SIZE,
      "HUVCRR", true);
  std::cout << "[MyUtil::initialize] finished" << endl;
}

