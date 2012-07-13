/*
 * =====================================================================================
 *
 *       Filename:  MiniGroupManagerI.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/25/2011 04:13:09 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *        Company:  renren-inc
 *
 * =====================================================================================
 */

#include "MiniGroupManagerI.h"

#include "ActiveTrackAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "MiniGroupWriterAdapter.h"

#include "util/cpp/TimeCost.h"
#include <Monitor/client/service/ServiceMonitor.h>
#include <boost/lexical_cast.hpp>

using namespace minigroup;
using namespace xce::mngp;
using namespace MyUtil;
using namespace ::xce::log;
using namespace com::xiaonei::xce;
using namespace xce::serverstate;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();

  service.registerObjectCache(MINIGROUP_INFO, "MNGPI", new MiniGroupInfoFactoryI, false);
  service.registerObjectCache(USER_2_MINIGROUP_INFO, "U2MNGPI", new User2MiniGroupInfoFactoryI, false);

  service.getAdapter()->add(&MiniGroupCacheI::instance(), service.createIdentity("MNGPC", ""));
  service.getAdapter()->add(&User2MiniGroupCacheI::instance(), service.createIdentity("UMNGPC", ""));
  service.getAdapter()->add(&MiniGroupManagerI::instance(), service.createIdentity("MNGPM", ""));
  ActiveTrack::instance().initialize();
  // 加入向Controller注册
  map<string, Ice::ObjectPtr> mapNameAndService;

  mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>("MNGPC", &MiniGroupCacheI::instance()));
  mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>("UMNGPC", &User2MiniGroupCacheI::instance()));
  mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>("MNGPM", &MiniGroupManagerI::instance()));

  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("MiniGroupManager.Mod");
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("MiniGroupManager.Interval", 5);
  ServerStateSubscriber::instance().initialize("ControllerMiniGroupManager", mapNameAndService, mod, interval);

  {
    int status = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("MiniGroupManager.Status", 0);
    ServerStateSubscriber::instance().setStatus(status, "MNGPC");
    ServerStateSubscriber::instance().setStatus(status, "UMNGPC");
    ServerStateSubscriber::instance().setStatus(status, "MNGPM");
    ServerStateSubscriber::instance().setStatus(status);

    std::ostringstream os;
    os << "MyUtil::initialize set status : " << status;
    MCE_WARN(os.str());
  }

  //统计log
  ServiceMonitorManager::instance().start();
}

bool MiniGroupCacheI::isValid() {
  IceUtil::Mutex::Lock lock(_mutex);
  return _valid;
}

void MiniGroupCacheI::setValid(bool valid, const Ice::Current& current) {
  MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, "[MiniGroupCacheI::setValid] valid = " + boost::lexical_cast<string>(valid), 
      MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
  {
    IceUtil::Mutex::Lock lock(_mutex);
    _valid = valid;
    xce::serverstate::ServerStateSubscriber::instance().setStatus((int)valid, "MNGPC");
  }
  //此处的false不起任何作用
  MiniGroupManagerI::instance().setValid(false);
}

void MiniGroupCacheI::setData(const MyUtil::ObjectResultPtr& miniGroupData, const Ice::Current& current) {
  if (miniGroupData->data.empty()) {
    return;
  }
  MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, "[MiniGroupCacheI::setData] data.size = "
      + boost::lexical_cast<string>(miniGroupData->data.size()), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
  ServiceI::instance().getObjectCache()->addObjects(MINIGROUP_INFO, 
      xce::mngp::parse<MiniGroupInfo, MiniGroupInfoPtr, MiniGroupPtr>(miniGroupData)->data);
}

bool User2MiniGroupCacheI::isValid() {
  IceUtil::Mutex::Lock lock(_mutex);
  return _valid;
}

void User2MiniGroupCacheI::setValid(bool valid, const Ice::Current& current) {
  MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, "[User2MiniGroupCacheI::setValid] valid = " + boost::lexical_cast<string>(valid), 
      MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
  {
    IceUtil::Mutex::Lock lock(_mutex);
    _valid = valid;
    xce::serverstate::ServerStateSubscriber::instance().setStatus((int)valid, "UMNGPC");
  }
  //此处的false不起任何作用
  MiniGroupManagerI::instance().setValid(false);
}

void User2MiniGroupCacheI::setData(const MyUtil::ObjectResultPtr& user2MiniGroupData, const Ice::Current& current) {
  if (user2MiniGroupData->data.empty()) {
    return;
  }
  MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, "[User2MiniGroupCacheI::setData] data.size = " 
      + boost::lexical_cast<string>(user2MiniGroupData->data.size()), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
  ServiceI::instance().getObjectCache()->addObjects(USER_2_MINIGROUP_INFO,
      parse<User2MiniGroupInfo, User2MiniGroupInfoPtr, User2MiniGroupPtr>(user2MiniGroupData)->data);
}

MiniGroupManagerI::MiniGroupManagerI() {
}

MiniGroupSeq MiniGroupManagerI::getMiniGroups(int user_id, const Ice::Current& current) {
  MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, "userId = " + boost::lexical_cast<string>(user_id), 
      MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
  User2MiniGroupInfoPtr ptr = ServiceI::instance().locateObject<User2MiniGroupInfoPtr>(USER_2_MINIGROUP_INFO, user_id);
  if (ptr != 0) {
    //此处每个用户加入的小群数目比较少，所以没有使用批量取的接口
    MiniGroupSeq result =  getMiniGroups(ptr->getJoinedMiniGroupIds());
    return result;
  } else {
    MiniGroupSeq result;
    return result;
  }
}

MiniGroupSeq MiniGroupManagerI::getMiniGroups(const MyUtil::LongSeq& miniGroupIds) {
  MCE_INFO("[MiniGroupManagerI::getMiniGroups] miniGroupIds.size = " << miniGroupIds.size());
  MiniGroupSeq result;
  for (MyUtil::LongSeq::const_iterator it = miniGroupIds.begin(); it != miniGroupIds.end(); ++it) {
    MiniGroupInfoPtr minigroupinfo_ptr = ServiceI::instance().locateObject<MiniGroupInfoPtr>(MINIGROUP_INFO, *it);
    if (minigroupinfo_ptr != 0) {
      result.push_back(minigroupinfo_ptr);
    }
  }
  return result; 
}

MiniGroupSeq MiniGroupManagerI::getMiniGroupsWithIds(const MyUtil::LongSeq& miniGroupIds, const Ice::Current& current) {
  MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, "miniGroupIds.size = " + boost::lexical_cast<string>(miniGroupIds.size()), 
      MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
  return getMiniGroups(miniGroupIds);
}

void MiniGroupManagerI::addMiniGroups(const MiniGroupSeq& minigroups, const Ice::Current& current) {
  MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, "miniGroups.size = " + boost::lexical_cast<string>(minigroups.size()), 
      MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
  ServiceI::instance().getObjectCache()->addObjects(MINIGROUP_INFO, parse(minigroups)->data);
}

void MiniGroupManagerI::setJoinedMiniGroups(int user_id, const MyUtil::LongSeq& minigroup_ids, const Ice::Current& current) {
  MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, "userId = " + boost::lexical_cast<string>(user_id)
      + " miniGroupIds.size = " + boost::lexical_cast<string>(minigroup_ids.size()), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
  User2MiniGroupInfoPtr ptr = ServiceI::instance().locateObject<User2MiniGroupInfoPtr>(USER_2_MINIGROUP_INFO, user_id);
  if (ptr != 0) {
    ptr->setJoinedMiniGroups(minigroup_ids);
  }
}

void MiniGroupManagerI::addJoinedMiniGroups(int user_id, const MyUtil::LongSeq& minigroup_ids, const Ice::Current& current) {
  MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, "userId = " + boost::lexical_cast<string>(user_id)
      + " miniGroupIds.size = " + boost::lexical_cast<string>(minigroup_ids.size()), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
  User2MiniGroupInfoPtr ptr = ServiceI::instance().locateObject<User2MiniGroupInfoPtr>(USER_2_MINIGROUP_INFO, user_id);
  if (ptr != 0) {
    ptr->addJoinedMiniGroups(minigroup_ids);
  }
}

void MiniGroupManagerI::removeJoinedMiniGroups(int user_id, const MyUtil::LongSeq& minigroup_ids, const Ice::Current& current) {
  MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, "userId = " + boost::lexical_cast<string>(user_id)
      + " miniGroupIds.size = " + boost::lexical_cast<string>(minigroup_ids.size()), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
  User2MiniGroupInfoPtr ptr = ServiceI::instance().locateObject<User2MiniGroupInfoPtr>(USER_2_MINIGROUP_INFO, user_id);
  if (ptr != 0) {
    ptr->removeJoinedMiniGroups(minigroup_ids);
  }
}

void MiniGroupManagerI::removeAllJoinedMiniGroups(int userId, const Ice::Current& current) {
  MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, "userId = " + boost::lexical_cast<string>(userId),
      MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
  User2MiniGroupInfoPtr ptr = ServiceI::instance().locateObject<User2MiniGroupInfoPtr>(USER_2_MINIGROUP_INFO, userId);
  if (ptr != 0) {
    ptr->clearJoinedMiniGroups();
  }
}

void MiniGroupManagerI::setValid(bool valid, const Ice::Current& current) {
  IceUtil::Mutex::Lock lock(_mutex);
  int value = MiniGroupCacheI::instance().isValid() && User2MiniGroupCacheI::instance().isValid();
  MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, "[MiniGroupManagerI::setValid] valid = "
      + boost::lexical_cast<string>(value), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
  xce::serverstate::ServerStateSubscriber::instance().setStatus(value, "MNGPM");
  xce::serverstate::ServerStateSubscriber::instance().setStatus(value); 
}

bool MiniGroupManagerI::isValid() {
  IceUtil::Mutex::Lock lock(_mutex);
  return _valid;
}

Ice::ObjectPtr MiniGroupInfoFactoryI::create(Ice::Int id) {
  Statement sql;
  vector<MiniGroupInfoPtr> result;
  string tableName = DBUtil<int>::getTableName<int>(id, TABLE_MOD, TABLE_MINIGROUP);
  sql << "select " << TABLE_MINIGROUP_ID << ", " << TABLE_MINIGROUP_NAME << ", " << TABLE_MINIGROUP_ICON
      << " from " << TABLE_MINIGROUP << "_" << id % TABLE_MOD << " where " << TABLE_MINIGROUP_ID << " = " << id; 
  QueryRunner(DB_SOURCE, CDbRServer, tableName).query(sql, MiniGroupInfoResultHandlerI(result));
  if (!result.empty()) {
    //通知MiniGroupManagerI进行reload
    MyUtil::LongSeq miniGroupIds;
    miniGroupIds.push_back(id);
    try {
      minigroup::MiniGroupWriterAdapter::instance().reloadMiniGroups(miniGroupIds);
    } catch(exception ex) {
      MCE_WARN("[MiniGroupInfoFactoryI::create] MiniGroupWriterAdapter::reloadMiniGroups " << ex.what());
    } catch(...) {
      MCE_WARN("[MiniGroupInfoFactoryI::create] MiniGroupWriterAdapter::reloadMiniGroups unknown exception");
    }
    MCE_INFO("[MiniGroupInfoFactoryI::create] return ");
    return *result.begin();
  }
  return 0;
} 

Ice::ObjectPtr User2MiniGroupInfoFactoryI::create(Ice::Int id) {
  //MCE_INFO("[UserMiniGroupInfoFactoryI::create] id = " << id);
  Statement sql;
  vector<User2MiniGroupInfoPtr> result;
  string tableName = DBUtil<int>::getTableName<int>(id, TABLE_MOD, TABLE_MEMBER_MINIGROUP);
  sql << "select " << TABLE_MEMBER_MINIGROUP_MNGP_ID << ", " << TABLE_MEMBER_MINIGROUP_MEMBER_ID
      << " from " << TABLE_MEMBER_MINIGROUP << "_" << id % TABLE_MOD << " where " << TABLE_MEMBER_MINIGROUP_MEMBER_ID
      << " = " << id << " and " << TABLE_MEMBER_MINIGROUP_RESTRICT << TABLE_MEMBER_MINIGROUP_SORT;
  QueryRunner(DB_SOURCE, CDbRServer, tableName).query(sql, User2MiniGroupInfoResultHandlerI(result));
  if (!result.empty()) {
    //通知User2MiniGroupManagerI进行reload
    MyUtil::IntSeq userIds;
    userIds.push_back(id);
    try {
      minigroup::MiniGroupWriterAdapter::instance().reloadUsers(userIds);
    } catch(exception ex) {
      MCE_WARN("[User2MiniGroupInfoFactoryI::create] MiniGroupWriterAdapter::reloadUsers " << ex.what());
    } catch(...) {
      MCE_WARN("[User2MiniGroupInfoFactoryI::create] MiniGroupWriterAdapter::reloadUsers unknown exception");
    }
    return *result.begin();
  }
  return 0;
}

