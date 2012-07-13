/*
 * =====================================================================================
 *
 *       Filename:  MiniGroupPreloader.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/13/2011 09:56:47 AM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *        Company:  renren-inc
 *
 * =====================================================================================
 */
#include "MiniGroupPreloader.h"
#include "QueryRunner.h"
#include "ActiveTrackAdapter.h"
#include "MiniGroupManagerAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "GeneralPreloader/src/GeneralPreloader.h"

#include "util/cpp/TimeCost.h"
#include <Monitor/client/service/ServiceMonitor.h>
#include <boost/lexical_cast.hpp>
#include <sstream>

using namespace xce::mngp;
using namespace MyUtil;
using namespace ::xce::log;
using namespace com::xiaonei::xce;
using namespace xce::serverstate;

void MyUtil::initialize() {
  MyUtil::ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&MiniGroupPreloader::instance(), service.createIdentity("MNGPP", ""));
  std::string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("MiniGroupManager.Controller",
      "ControllerMiniGroupManager");
  int subInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 5);
  int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("MiniGroupManager.Proxy.Timeout", 300);

  int cluster = 1;
  xce::generalpreloader::GeneralPreloader<MiniGroupCachePrx>::instance().initialize(
      new BatchMiniGroupIdProducer,
      new MiniGroupPreloaderFactory,
      ctlEndpoint,
      cluster,
      subInterval,
      timeout,
      PRX_DATAQUEUE_MAX_SIZE,
      PRODUCE_BATCH_SIZE,
      CONSUME_BATCH_SIZE,
      WRITE_BATCH_SIZE,
      CONSUME_BATCH_THREAD_SIZE,
      "MNGPC", true, 100);
} 

MyUtil::ObjectResultPtr MiniGroupPreloaderFactory::create(const MyUtil::IntSeq& ids) {
  MCE_INFO("[MiniGroupPreloaderFactory::create] ids.size = " << ids.size());
  MyUtil::ObjectResultPtr result = new MyUtil::ObjectResult;
  if (ids.empty()) {
    return result;
  }
  //此处一定要注意，ids并不一定属于同一个尾号
  map<int, vector<int> > seperatedIds = DBUtil<int>::seperateIdsByMod<int>(TABLE_MOD, ids);
  for (map<int, vector<int> >::const_iterator itv = seperatedIds.begin(); itv != seperatedIds.end(); ++itv) {
    vector<MiniGroupInfoPtr> ptrs;
    MiniGroupInfoResultHandlerI handler(ptrs);
    Statement sql;
    sql << "select " << TABLE_MINIGROUP_ID << ", " << TABLE_MINIGROUP_NAME << ", " << TABLE_MINIGROUP_ICON
        << " from " << TABLE_MINIGROUP << "_" << itv->first % TABLE_MOD << " where " << TABLE_MINIGROUP_ID
        << " in " << DBUtil<int>::getIdsStr(itv->second);
    com::xiaonei::xce::QueryRunner(DB_SOURCE, com::xiaonei::xce::CDbRServer).query(sql, handler);
    for (vector<MiniGroupInfoPtr>::const_iterator it = ptrs.begin(); it != ptrs.end(); ++it) {
      result->data.insert(make_pair<long, MiniGroupInfoPtr>((*it)->getId(), *it));
    }
  }
  return result;
}

vector<int> BatchMiniGroupIdProducer::produce(int beginId, int load_batch_size, int& maxId, int& tableMod) {
  MCE_INFO("[BatchMiniGroupIdProducer::produce] beginId = " << beginId << " tableMod = " << tableMod);
  std::vector<int> results;
  com::xiaonei::xce::Statement sql;
  sql << "select distinct(id) from minigroup_" << tableMod << " where id >= " << beginId
    << " order by id limit " << load_batch_size;
  BatchIdResultHandler<int> handler("id", results);
  com::xiaonei::xce::QueryRunner(DB_SOURCE, com::xiaonei::xce::CDbRServer).query(sql, handler);
  if (!results.empty()) {
    maxId = *(results.end() - 1);
  }
  return results;
}
