/*
 * =====================================================================================
 *
 *       Filename:  FriendsRecentPhotosWriterI.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/01/2011 07:09:15 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *        Company:  renren-inc
 *
 * =====================================================================================
 */

#include "FriendsRecentPhotosWriterI.h"
#include "FriendsRecentPhotosManagerAdapter.h"
#include "../../FriendsRecentPhotosCommon/src/FriendsRecentPhotosCommon.cpp"
#include "ActiveTrackAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "app/MiniGroup/MiniGroupCommon/src/MiniGroupCommon.h"

#include <algorithm>

#include "util/cpp/TimeCost.h"
#include <Monitor/client/service/ServiceMonitor.h>
#include <boost/lexical_cast.hpp>

using namespace MyUtil;
using namespace ::xce::log;
using namespace com::xiaonei::xce;
using namespace xce::serverstate;
using namespace xce::friendsphotos;
using namespace friendsrecentphotos;


using namespace xce::friendsphotos;
using namespace friendsrecentphotos;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();

  service.getAdapter()->add(&FriendsRecentPhotosWriterI::instance(), service.createIdentity("FRPW", ""));
  ActiveTrack::instance().initialize();
  // 加入向Controller注册

  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("FriendsRecentPhotosWriter.Mod");
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FriendsRecentPhotosWriter.Interval", 5);
  ServerStateSubscriber::instance().initialize("ControllerFriendsRecentPhotosWriter", &FriendsRecentPhotosWriterI::instance(), mod, interval);

  {
    int status = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FriendsRecentPhotosWriter.Status", 1);
    ServerStateSubscriber::instance().setStatus(status, "FRPW");
    ServerStateSubscriber::instance().setStatus(status);

    std::ostringstream os;
    os << "MyUtil::initialize set status : " << status;
    MCE_WARN(os.str());
  }
  //统计log
  ServiceMonitorManager::instance().start();
}


void FriendsRecentPhotosWriterI::addRecentPhotos(const UserPhotosSeq& userPhotos, const Ice::Current& current) {
  MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, "[FriendsRecentPhotosWriterI::addRecentPhotos] userPhotos.size = "
          + boost::lexical_cast<string>(userPhotos.size()), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
  FriendsRecentPhotosManagerAdapter::instance().addRecentPhotos(userPhotos);
  opUserPhotosSeq(userPhotos, 1);
}

void FriendsRecentPhotosWriterI::deleteUserPhotos(const UserPhotosSeq& userPhotos, const Ice::Current& current) {
  MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, "[FriendsRecentPhotosWriterI::deleteUserPhotos] userPhotos.size = "
          + boost::lexical_cast<string>(userPhotos.size()), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
  FriendsRecentPhotosManagerAdapter::instance().deleteUserPhotos(userPhotos);
  //写入数据库
  opUserPhotosSeq(userPhotos, 2);
}

void FriendsRecentPhotosWriterI::opUserPhotosSeq(const UserPhotosSeq& userPhotosSeq, int opCode) {
  MCE_INFO("[FriendsRecentPhotosWriterI::opUserPhotosSeq] userPhotosSeq.size = " << userPhotosSeq.size()
      << " opCode = " << opCode);
  vector<int> userIds = UserPhotosInfo::getUserIdsFromUserPhotosSeq(userPhotosSeq);
  MyUtil::ObjectResultPtr objectResultPtr = UserPhotosInfo::parseFromUserPhotosSeq(userPhotosSeq);
  map<int, vector<int> > seperatedIds = xce::mngp::DBUtil<int>::seperateIdsByMod(TABLE_MOD, userIds);
  for (map<int, vector<int> >::const_iterator itv = seperatedIds.begin(); itv != seperatedIds.end(); ++itv) {
    Statement sqlToExecute;
    bool toBeExecute = false;
    bool notFirst = false;
    string tableName = TABLE_NAME + "_" + boost::lexical_cast<string>(itv->first % TABLE_MOD);
    sqlToExecute << "insert into " << tableName << "(" << COLUMN_USERID << ", "
        << COLUMN_PHOTOS << ") values";
    //先从数据库中读出来
    vector<UserPhotosInfoPtr> ptrs;
    Statement sql;
    sql << "select " << COLUMN_USERID << ", " << COLUMN_PHOTOS << " from " << tableName
        << " where " << COLUMN_USERID << " in " << xce::mngp::DBUtil<int>::getIdsStr(itv->second);
    QueryRunner(DB_SOURCE, CDbRServer, tableName).query(sql, UserPhotosInfoResultHandlerI(ptrs));
    if (opCode == 1) {
      //如果数据库中没有相应的记录，那么要补充一条空记录
      vector<int> userIdsNotInDB = xce::friendsphotos::findElementsInANotB(itv->second, UserPhotosInfo::getUserIdsFromUserPhotosInfoSeq(ptrs));
      vector<UserPhotosInfoPtr> emptyPtrs = UserPhotosInfo::generateEmptyUserPhotosInfo(userIdsNotInDB);
      ptrs.insert(ptrs.end(), emptyPtrs.begin(), emptyPtrs.end());
    }
    for (vector<UserPhotosInfoPtr>::const_iterator it = ptrs.begin(); it != ptrs.end(); ++it) {
      map<long, Ice::ObjectPtr>::const_iterator itm = objectResultPtr->data.find((*it)->getUserId());
      if (itm != objectResultPtr->data.end()) {
        UserPhotosInfoPtr tempPtr = UserPhotosInfoPtr::dynamicCast(itm->second);
        if (opCode == 1) {
          (*it)->addPhotoInfo(tempPtr->getPhotoInfos());
        } else if(opCode == 2 && !((*it)->getPhotoInfos().empty())) {
          (*it)->deletePhotoInfoById(tempPtr->getPhotoIds());
        }
        //将数据写入sqlToExcute
        vector<int> uploadTimes = (*it)->getUploadTimes();
        vector<long> photoIds = (*it)->getPhotoIds();
        if (notFirst) {
          sqlToExecute << ", ";
        } else {
          notFirst = true;
        }
        toBeExecute = true;
        sqlToExecute << "(" << (*it)->getUserId() << ", " << mysqlpp::quote << serialize(uploadTimes, photoIds) << ")";
      }
    }
    sqlToExecute << " on duplicate key update photos = values(photos)";
    if (toBeExecute) {
      //写入数据库,执行sqlToExecute
      QueryRunner(DB_SOURCE, CDbWServer, tableName).store(sqlToExecute);
    }
  }
}

void FriendsRecentPhotosWriterI::insertEmptyData(const vector<int>& userIds) {
  MCE_INFO("[FriendsRecentPhotosWriterI::insertEmptyData] userIds.size = " << userIds.size());
  map<int, vector<int> > seperatedIds = xce::mngp::DBUtil<int>::seperateIdsByMod(TABLE_MOD, userIds);
  for (map<int, vector<int> >::const_iterator itv = seperatedIds.begin(); itv != seperatedIds.end(); ++itv) {
    //写入数据库
    Statement sql;
    sql << "insert into " << TABLE_NAME << "_" << itv->first % TABLE_MOD << "(" << COLUMN_USERID << ", " << COLUMN_PHOTOS
        << ") values";
    for (vector<int>::const_iterator it = itv->second.begin(); it != itv->second.end(); ++it) {
      if (it != itv->second.begin()) {
        sql << ", ";
      }
      sql << "(" << (*it) << ", " << mysqlpp::quote << "empty)";
    }
    QueryRunner(DB_SOURCE, CDbWServer, TABLE_NAME + "_" + boost::lexical_cast<string>(itv->first % TABLE_MOD)).store(sql);
  }
}

void FriendsRecentPhotosWriterI::reloadUserPhotos(const MyUtil::IntSeq& userIds, const Ice::Current& current) {
  MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, "[FriendsRecentPhotosWriterI::reloadUserPhotos] userIds.size = "
        + boost::lexical_cast<string>(userIds.size()), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
  MyUtil::ObjectResultPtr result = new MyUtil::ObjectResult;
  vector<UserPhotosInfoPtr> ptrs;
  map<int, vector<int> > seperatedIds = xce::mngp::DBUtil<int>::seperateIdsByMod(TABLE_MOD, userIds);
  for (map<int, vector<int> >::const_iterator itv = seperatedIds.begin(); itv != seperatedIds.end(); ++itv) {
    Statement sql;
    string tableName = TABLE_NAME + "_" + boost::lexical_cast<string>(itv->first % TABLE_MOD);
    sql << "select user_id, photos from " << tableName << " where user_id in "
        << xce::mngp::DBUtil<int>::getIdsStr(itv->second);
    QueryRunner(DB_SOURCE, CDbRServer, tableName).query(sql, UserPhotosInfoResultHandlerI(ptrs));
    //如果数据库中没有那条数据的话，向其中插入一个空数据
  }
  vector<int> notFoundIds = findElementsInANotB(userIds, UserPhotosInfo::getUserIdsFromUserPhotosInfoSeq(ptrs));
  for (vector<int>::const_iterator it = notFoundIds.begin(); it != notFoundIds.end(); ++it) {
    ptrs.push_back(new UserPhotosInfo(*it));
  }
  //向数据库中写入notFoundIds对应的空数据
  insertEmptyData(notFoundIds);
  MCE_INFO("[FriendsRecentPhotosWriterI::reloadUserPhotos] reload ptrs.size = " << ptrs.size());
  FriendsRecentPhotosManagerAdapter::instance().setData(UserPhotosInfo::changeToUserPhotosInfoMap(ptrs));
}
