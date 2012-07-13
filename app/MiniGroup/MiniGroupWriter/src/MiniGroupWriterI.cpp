/*
 * =====================================================================================
 *
 *       Filename:  MiniGroupWriterI.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/27/2011 08:10:56 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *        Company:  renren-inc
 *
 * =====================================================================================
 */

#include "MiniGroupWriterI.h"
#include "ActiveTrackAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "MiniGroupManagerAdapter.h"

#include <set>

#include "util/cpp/TimeCost.h"
#include <Monitor/client/service/ServiceMonitor.h>
#include <boost/lexical_cast.hpp>

using namespace xce::mngp;
using namespace MyUtil;
using namespace ::xce::log;
using namespace com::xiaonei::xce;
using namespace xce::serverstate;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();

  service.getAdapter()->add(&MiniGroupWriterI::instance(), service.createIdentity("MNGPW", ""));
  ActiveTrack::instance().initialize();
  // 加入向Controller注册
  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("MiniGroupWriter.Mod");
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("MiniGroupWriter.Interval", 5);
  ServerStateSubscriber::instance().initialize("ControllerMiniGroupWriter", &MiniGroupWriterI::instance(), mod, interval);

  //统计log
  ServiceMonitorManager::instance().start();
}


void MiniGroupWriterI::addJoinedMiniGroups(int user_id, const MyUtil::LongSeq& minigroup_ids, const Ice::Current& current) {
  minigroup::MiniGroupManagerAdapter::instance().addJoinedMiniGroups(user_id, minigroup_ids);
}     

void MiniGroupWriterI::setJoinedMiniGroups(int user_id, const MyUtil::LongSeq& minigroup_ids, const Ice::Current& current) {
  minigroup::MiniGroupManagerAdapter::instance().setJoinedMiniGroups(user_id, minigroup_ids);
}

void MiniGroupWriterI::removeJoinedMiniGroups(int user_id, const MyUtil::LongSeq& minigroup_ids, const Ice::Current& current) {
  minigroup::MiniGroupManagerAdapter::instance().removeJoinedMiniGroups(user_id, minigroup_ids);
}     

void MiniGroupWriterI::reloadUser2MiniGroups(const MyUtil::IntSeq& userIds, int mod) {
  MCE_INFO("[MiniGroupWriterI::reloadUser2MiniGroups] userIds.size = " << userIds.size() << " mod = " << mod);
  vector<User2MiniGroupInfoPtr> ptrs;
  User2MiniGroupInfoResultHandlerI handler(ptrs);
  Statement sql;
  sql << "select " << TABLE_MEMBER_MINIGROUP_MNGP_ID << ", " << TABLE_MEMBER_MINIGROUP_MEMBER_ID
    << " from " << TABLE_MEMBER_MINIGROUP << "_" << mod << " where " << TABLE_MEMBER_MINIGROUP_MEMBER_ID
    << " in " << DBUtil<int>::getIdsStr(userIds) << " and " << TABLE_MEMBER_MINIGROUP_RESTRICT
    << TABLE_MEMBER_MINIGROUP_SORT;
  com::xiaonei::xce::QueryRunner(DB_SOURCE, com::xiaonei::xce::CDbRServer).query(sql, handler);
  set<int> userIdsToClear(userIds.begin(), userIds.end());
  for (vector<User2MiniGroupInfoPtr>::const_iterator it = ptrs.begin(); it != ptrs.end(); ++it) {
    minigroup::MiniGroupManagerAdapter::instance().setJoinedMiniGroups((*it)->userId, (*it)->miniGroupIds);
    userIdsToClear.erase((*it)->userId);
  }
  for (set<int>::const_iterator it = userIdsToClear.begin(); it != userIdsToClear.end(); ++it) {
    minigroup::MiniGroupManagerAdapter::instance().removeAllJoinedMiniGroups(*it);
  }
}

void MiniGroupWriterI::reloadUsers(const MyUtil::IntSeq& user_ids, const Ice::Current& current) {
  map<int, vector<int> > seperatedIds = DBUtil<int>::seperateIdsByMod<int>(TABLE_MOD, user_ids);
  MCE_INFO("[MiniGroupWriterI::reloadUsers] userIds.size = " << user_ids.size());
  for (map<int, vector<int> >::const_iterator it = seperatedIds.begin(); it != seperatedIds.end(); ++it) {
    reloadUser2MiniGroups(it->second, it->first);
  }
}
void MiniGroupWriterI::reloadMiniGroups(const MyUtil::LongSeq& miniGroupIds, const Ice::Current& current) {
  MCE_INFO("[MiniGroupWriterI::reloadMiniGroups] miniGroupIds.size = " << miniGroupIds.size());
  map<long, vector<long> > seperatedIds = DBUtil<int>::seperateIdsByMod<long>(TABLE_MOD, miniGroupIds);
  for (map<long, vector<long> >::const_iterator it = seperatedIds.begin(); it != seperatedIds.end(); ++it) {
    reloadMiniGroups(it->second, it->first);
  }
}

void MiniGroupWriterI::reloadMiniGroups(const MyUtil::LongSeq& miniGroupIds, int mod) {
  vector<MiniGroupInfoPtr> ptrs;
  MiniGroupInfoResultHandlerI handler(ptrs);
  Statement sql;
  sql << "select " << TABLE_MINIGROUP_ID << ", " << TABLE_MINIGROUP_NAME << ", " << TABLE_MINIGROUP_ICON
    << " from " << TABLE_MINIGROUP << "_" << mod << " where " << TABLE_MINIGROUP_ID << " in "
    << DBUtil<int>::getIdsStr(miniGroupIds);
  com::xiaonei::xce::QueryRunner(DB_SOURCE, com::xiaonei::xce::CDbRServer).query(sql, handler);
  MiniGroupSeq seqToAdd;
  for (vector<MiniGroupInfoPtr>::const_iterator it = ptrs.begin(); it != ptrs.end(); ++it) {
    seqToAdd.push_back(*it);
  }
  minigroup::MiniGroupManagerAdapter::instance().addMiniGroups(seqToAdd);
}

void MiniGroupWriterI::addMiniGroups(const MiniGroupSeq& miniGroups, const Ice::Current& current) {
  minigroup::MiniGroupManagerAdapter::instance().addMiniGroups(miniGroups);
}

