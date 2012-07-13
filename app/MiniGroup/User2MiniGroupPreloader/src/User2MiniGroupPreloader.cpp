/*
 * =====================================================================================
 *
 *       Filename:  User2MiniGroupPreloader.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/25/2011 04:45:57 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *        Company:  renren-inc
 *
 * =====================================================================================
 */
#include "User2MiniGroupPreloader.h"
#include "QueryRunner.h"
#include "ActiveTrackAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "GeneralPreloader/src/GeneralPreloader.h"
#include "MiniGroupManagerAdapter.h"

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
  service.getAdapter()->add(&User2MiniGroupPreloader::instance(), service.createIdentity("UMNGMP", ""));
  std::string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("MiniGroupManager.Controller",
      "ControllerMiniGroupManager");
  int subInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 5);
  int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("MiniGroupManager.Proxy.Timeout", 300);

  int cluster = 1;
  xce::generalpreloader::GeneralPreloader<User2MiniGroupCachePrx>::instance().initialize(
      new BatchUser2MiniGroupIdProducer,
      new User2MiniGroupInfoPreloaderFactory,
      ctlEndpoint,
      cluster,
      subInterval,
      timeout,
      PRX_DATAQUEUE_MAX_SIZE,
      PRODUCE_BATCH_SIZE,
      CONSUME_BATCH_SIZE,
      WRITE_BATCH_SIZE,
      CONSUME_BATCH_THREAD_SIZE,
      "UMNGPC", true, 100);
}

MyUtil::ObjectResultPtr User2MiniGroupInfoPreloaderFactory::create(const MyUtil::IntSeq& ids) {
  MyUtil::ObjectResultPtr result = new MyUtil::ObjectResult;
  if (ids.empty()) {
    return result;
  }

  map<int, vector<int> > seperatedIds = DBUtil<int>::seperateIdsByMod<int>(TABLE_MOD, ids);
  for (map<int, vector<int> >::const_iterator itv = seperatedIds.begin(); itv != seperatedIds.end(); ++itv) {
    vector<User2MiniGroupInfoPtr> ptrs;
    User2MiniGroupInfoResultHandlerI handler(ptrs);
    Statement sql;
    sql << "select " << TABLE_MEMBER_MINIGROUP_MNGP_ID << ", " << TABLE_MEMBER_MINIGROUP_MEMBER_ID
        << " from " << TABLE_MEMBER_MINIGROUP << "_" << itv->first % TABLE_MOD << " where "
        << TABLE_MEMBER_MINIGROUP_MEMBER_ID << " in " << DBUtil<int>::getIdsStr(itv->second) << " and "
        << TABLE_MEMBER_MINIGROUP_RESTRICT << TABLE_MEMBER_MINIGROUP_SORT;
    com::xiaonei::xce::QueryRunner(DB_SOURCE, com::xiaonei::xce::CDbRServer).query(sql, handler);
    for (vector<User2MiniGroupInfoPtr>::const_iterator it = ptrs.begin(); it != ptrs.end(); ++it) {
      result->data.insert(make_pair<long, User2MiniGroupInfoPtr>((*it)->userId, *it));
    }
  }
  return result;
}

std::vector<int> BatchUser2MiniGroupIdProducer::produce(int beginId, int load_batch_size, int& maxId, int& tableMod) {
  MCE_INFO("[BatchUser2MiniGroupIdProducer::produce] beginId = " << beginId << " maxId = " << maxId << " tableMod = " << tableMod);
  std::vector<int> results;
  com::xiaonei::xce::Statement sql;
  sql << "select distinct(member_id) from member_minigroup_" << tableMod << " where member_id >= " << beginId
      << " and " << TABLE_MEMBER_MINIGROUP_RESTRICT << " order by member_id limit "
      << load_batch_size;
  BatchIdResultHandler<int> handler("member_id", results);
  com::xiaonei::xce::QueryRunner(DB_SOURCE, com::xiaonei::xce::CDbRServer).query(sql, handler);
  if (!results.empty()) {
    maxId = *(results.end() - 1);
  }
  return results;
}
