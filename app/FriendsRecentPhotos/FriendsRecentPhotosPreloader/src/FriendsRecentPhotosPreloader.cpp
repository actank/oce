/*
 * =====================================================================================
 *
 *       Filename:  FriendsRecentPhotosPreloader.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/02/2011 09:24:48 AM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *        Company:  renren-inc
 *
 * =====================================================================================
 */
#include "FriendsRecentPhotosPreloader.h"
#include "../../FriendsRecentPhotosCommon/src/FriendsRecentPhotosCommon.cpp"
#include "QueryRunner.h"
#include "ActiveTrackAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "GeneralPreloader/src/GeneralPreloader.h"
#include "app/MiniGroup/MiniGroupCommon/src/MiniGroupCommon.h"

#include "util/cpp/TimeCost.h"
#include <Monitor/client/service/ServiceMonitor.h>
#include <boost/lexical_cast.hpp>
#include <sstream>
#include <limits>

using namespace xce::friendsphotos;
using namespace MyUtil;
using namespace ::xce::log;
using namespace com::xiaonei::xce;
using namespace xce::serverstate;

void MyUtil::initialize() {
  MyUtil::ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&FriendsRecentPhotosPreloader::instance(), service.createIdentity("FRPP", ""));
  std::string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("FriendsRecentPhotosManager.Controller",
      "ControllerFriendsRecentPhotosManager");
  int subInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 5);
  int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FriendsRecentPhotosManager.Proxy.Timeout", 300);

  int cluster = 1;
  xce::generalpreloader::GeneralPreloader<FriendsRecentPhotosManagerPrx>::instance().initialize(
      new BatchFriendsRecentPhotosIdProducer,
      new FriendsRecentPhotosPreloaderFactory,
      ctlEndpoint,
      cluster,
      subInterval,
      timeout,
      PRX_DATAQUEUE_MAX_SIZE,
      PRODUCE_BATCH_SIZE,
      CONSUME_BATCH_SIZE,
      WRITE_BATCH_SIZE,
      CONSUME_BATCH_THREAD_SIZE,
      "FRPM", true, 100);
}

MyUtil::ObjectResultPtr FriendsRecentPhotosPreloaderFactory::create(const MyUtil::IntSeq& ids) {
  MCE_INFO("[FriendsRecentPhotosPreloaderFactory::create] ids.size = " << ids.size());
  MyUtil::ObjectResultPtr result = new MyUtil::ObjectResult;
  if (ids.empty()) {
    return result;
  }
  vector<UserPhotosInfoPtr> resultPtrs;
  //此处一定要注意，ids并不一定属于同一个尾号
  map<int, vector<int> > seperatedIds = xce::mngp::DBUtil<int>::seperateIdsByMod<int>(TABLE_MOD, ids);
  for (map<int, vector<int> >::const_iterator itv = seperatedIds.begin(); itv != seperatedIds.end(); ++itv) {
    Statement sql;
    string tableName = TABLE_NAME + "_" + boost::lexical_cast<string>(itv->first % TABLE_MOD);
    sql << "select " << COLUMN_USERID << ", " << COLUMN_PHOTOS << " from " << tableName << " where "
        << COLUMN_USERID << " in " << xce::mngp::DBUtil<int>::getIdsStr<int>(itv->second);
    QueryRunner(DB_SOURCE, CDbRServer, tableName).query(sql, UserPhotosInfoResultHandlerI(resultPtrs));
  }
  return UserPhotosInfo::changeToUserPhotosInfoMap(resultPtrs);
}

vector<int> BatchFriendsRecentPhotosIdProducer::produce(int beginId, int load_batch_size, int& maxId, int& tableMod) {
  MCE_INFO("[BatchFriendsRecentPhotosIdProducer::produce] beginId = " << beginId << " tableMod = " << tableMod);
  std::vector<int> results;
  com::xiaonei::xce::Statement sql;
  string tableName = TABLE_NAME + "_" + boost::lexical_cast<string>(tableMod);
  sql << "select " << COLUMN_USERID << " from " << tableName << " where " << COLUMN_USERID << " >= " << beginId
    << " order by " << COLUMN_USERID << " limit " << load_batch_size;
  QueryRunner(DB_SOURCE, CDbRServer, tableName).query(sql, BatchIdResultHandler<int>(COLUMN_USERID, results));
  if (!results.empty()) {
    maxId = *(results.end() - 1);
  } else {
    maxId = numeric_limits<int>::max();  
  }
  return results;
}

