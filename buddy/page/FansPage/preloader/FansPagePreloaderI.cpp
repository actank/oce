/*
 * =====================================================================================
 *
 *       Filename:  FansPagePreloaderI.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/17/2010 05:00:37 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@opi-corp.com
 *        Company:  Opi-Corp
 *
 * =====================================================================================
 */
#include "buddy/page/share/common.h"
#include "ServiceI.h"
#include "FansPage.h"
#include "FansPagePreloaderI.h"
#include "FansPageCacheAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include <QueryRunner.h>
#include <time.h>

using namespace xce::generalpreloader;
using namespace xce::buddy::adapter;
using namespace xce::serverstate;
using namespace xce::buddy;
using namespace ::com::xiaonei::xce;
using namespace MyUtil;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.registerXceObserver(new ServiceSchemaI);
  string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("FansPageCache.Controller", "ControllerFansPageCache");
  int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FansPageCache.Proxy.Timeout", 300);
  int produceBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FansPagePreloader.ProduceBatchSize", 10000);
  int consumeBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FansPagePreloader.ConsumeBatchSize", 3000);
  int writeBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FansPagePreloader.WriteBatchSize", 1000);
  int threadSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FansPagePreloader.ThreadSize", 5);
  int cluster = 1;
  string identifier = "";
  int subInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 5);
  GeneralPreloader<FansPageCachePrx>::instance().initialize(
      new BatchFansIdProducer,
      new FansPagePreloaderFactory,
      ctlEndpoint,
      cluster,
      subInterval,
      timeout,
      PRX_DATAQUEUE_MAX_SIZE ,
      produceBatchSize,
      consumeBatchSize,
      writeBatchSize,
      threadSize, 
      identifier,
      false,
      100);
}

//***************************************************************************************************************************

MyUtil::ObjectResultPtr FansPagePreloaderFactory::create(const MyUtil::IntSeq& ids) {
  MCE_INFO("[FansPagePreloaderFactory::create] ids.size = " << ids.size());
  MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult();
  for(MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
    Statement sql;
    sql << "select id, page_id, user_id, state, UNIX_TIMESTAMP(add_time), operator from " << PageHelper::getTableName(*it) << " where user_id = " << *it;
    int maxId = -1, maxPageId = -1;
    PageEntryList list;
    BatchPageEntryResultHandlerI handler(maxId, maxPageId, list);
    QueryRunner(DATA_SOURCE, CDbRServer).query(sql, handler);
    PageEntrySetPtr ptr = new PageEntrySet;
    sort(list.begin(), list.end(), CompPageEntry());
    ptr->pageEntries = list;
    res->data.insert(make_pair(*it, ptr));
  }
  MCE_INFO("[FansPagePreloaderFactory::create] res.size = " << res->data.size());
  return res;
}
  
void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
  //TaskManager::instance().config(props, "PreloadAll", PRELOAD_ALL_TASK_LEVEL, ThreadPoolConfig(1, 5));
}

