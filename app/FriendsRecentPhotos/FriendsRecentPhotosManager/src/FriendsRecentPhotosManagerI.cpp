/*
 * =====================================================================================
 *
 *       Filename:  FriendsRecentPhotosManagerI.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/01/2011 06:47:41 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *        Company:  renren-inc
 *
 * =====================================================================================
 */
#include "FriendsRecentPhotosManagerI.h"
#include "../../FriendsRecentPhotosCommon/src/FriendsRecentPhotosCommon.cpp"
#include "ActiveTrackAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "FriendsRecentPhotosWriterAdapter.h"
#include "BuddyByIdReplicaAdapter.h"

#include <time.h>

#include "util/cpp/TimeCost.h"
#include <Monitor/client/service/ServiceMonitor.h>
#include <boost/lexical_cast.hpp>

using namespace MyUtil;
using namespace ::xce::log;
using namespace com::xiaonei::xce;
using namespace xce::serverstate;
using namespace xce::friendsphotos;
using namespace friendsrecentphotos;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  
  EMPTY_USER_PHOTOS_PTR->userId = -1;
  service.registerObjectCache(USER_PHOTOS_INFO, "UPI", new UserPhotosInfoFactoryI, false);
  service.registerObjectCache(USER_FRIENDS, "UF", new UserFriendsFactoryI, false);

  service.getAdapter()->add(&FriendsRecentPhotosManagerI::instance(), service.createIdentity("FRPM", ""));
  ActiveTrack::instance().initialize();
  // 加入向Controller注册

  map<string, Ice::ObjectPtr> mapNameAndService;
  mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>("FRPM", &FriendsRecentPhotosManagerI::instance()));

  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("FriendsRecentPhotosManager.Mod");
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FriendsRecentPhotosManager.Interval", 5);
  ServerStateSubscriber::instance().initialize("ControllerFriendsRecentPhotosManager", mapNameAndService, mod, interval);

  {
    int status = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FriendsRecentPhotosManager.Status", 0);
    ServerStateSubscriber::instance().setStatus(status, "FRPM");
    ServerStateSubscriber::instance().setStatus(status);

    std::ostringstream os;
    os << "MyUtil::initialize set status : " << status;
    MCE_WARN(os.str());
  }
  //统计log
  ServiceMonitorManager::instance().start();
}

PhotoResultPtr FriendsRecentPhotosManagerI::getFriendsRecentPhotos(int userId, int begin, int limit, const Ice::Current& current) {
  MCE_INFO("[FriendsRecentPhotosManagerI::getFriendsRecentPhotos] userId = " << userId << ", begin = " << begin
      << ", limit = " << limit);
  PhotoResultPtr ptr = new PhotoResult;
  ptr->total = 0;
  if (begin < 0 || limit < 0) {
    return ptr;
  }
  int friendCount;
  MyUtil::IntSeq friends;
  // 这段是用来测试的
  /*if (userId == 67913) {
    friends.push_back(67813);
    friends.push_back(36704);
    friends.push_back(42304);
    friends.push_back(33499);
  } else {*/
  if (!isSpecialUser(userId) && (friendCount = xce::buddy::adapter::BuddyByIdCacheAdapter::instance().getFriendCount(userId)) < MAX_FRIENDS_NUM) {
    //用户为非节点用户，直接计算
    friends = xce::buddy::adapter::BuddyByIdCacheAdapter::instance().getFriendListAsc(userId, friendCount);
  } else {
    //用户为节点用户，进行特殊处理
    addSpecialUsers(userId);
    UserFriendsPtr userFriendsPtr = ServiceI::instance().findObject<UserFriendsPtr>(USER_FRIENDS, userId);
    if (userFriendsPtr != 0) {
      friends = userFriendsPtr->getFriends();
    } else {
      //一个用户是节点用户，但是在USER_FRIENDS cache又没有找到，说明该用户为第一个访问，先取他的前MAX_FRIENDS_NUM个好友计算
      friends = xce::buddy::adapter::BuddyByIdCacheAdapter::instance().getFriendListAsc(userId, 0, MAX_FRIENDS_NUM);
    }
  }
  //}
  return getPhotoResultFromUsers(ServiceI::instance(), friends, begin, limit);
}

UserPhotosPtr FriendsRecentPhotosManagerI::getUserRecentPhotos(int userId, const Ice::Current& current) {
  MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, "[FriendsRecentPhotosManagerI::getUserRecentPhotos] userId = "
      + boost::lexical_cast<string>(userId), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
  UserPhotosInfoPtr ptr = ServiceI::instance().locateObject<UserPhotosInfoPtr>(USER_PHOTOS_INFO, userId);
  if (ptr == 0 || ptr == EMPTY_USER_PHOTOS_INFO_PTR) {
    return EMPTY_USER_PHOTOS_PTR;
  } else {
    return ptr->parseTo();
  }
}

void FriendsRecentPhotosManagerI::addRecentPhotos(const UserPhotosSeq& userPhotos, const Ice::Current& current) {
  MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, "[FriendsRecentPhotosManagerI::addRecentPhotos] userPhotos.size = "
      + boost::lexical_cast<string>(userPhotos.size()), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
  for (vector<UserPhotosPtr>::const_iterator it = userPhotos.begin(); it != userPhotos.end(); ++it) {
    UserPhotosInfoPtr ptr = ServiceI::instance().locateObject<UserPhotosInfoPtr>(USER_PHOTOS_INFO, (*it)->userId);
    if (ptr != 0 && ptr != EMPTY_USER_PHOTOS_INFO_PTR) {
      for (vector<PhotoPtr>::const_iterator itp = (*it)->photos.begin(); itp != (*it)->photos.end(); ++itp) {
        ptr->addPhotoInfo(new PhotoInfo((*itp)->uploadTime, (*itp)->photoId));
      }
    } else {
      UserPhotosInfoPtr tempPtr = new UserPhotosInfo(*it);
      ServiceI::instance().getObjectCache()->addObject(USER_PHOTOS_INFO, tempPtr->getUserId(), tempPtr);
    }
  }
}

void FriendsRecentPhotosManagerI::deleteUserPhotos(const UserPhotosSeq& userPhotos, const Ice::Current& current) {
  MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, "[FriendsRecentPhotosManagerI::deleteUserPhotos] userPhotos.size = "
      + boost::lexical_cast<string>(userPhotos.size()), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
  for (vector<UserPhotosPtr>::const_iterator it = userPhotos.begin(); it != userPhotos.end(); ++it) {
    UserPhotosInfoPtr ptr = ServiceI::instance().locateObject<UserPhotosInfoPtr>(USER_PHOTOS_INFO, (*it)->userId);
    if (ptr != 0 && ptr != EMPTY_USER_PHOTOS_INFO_PTR) {
      for (vector<PhotoPtr>::const_iterator itp = (*it)->photos.begin(); itp != (*it)->photos.end(); ++itp) {
        ptr->deletePhotoInfoById((*itp)->photoId);
      }
    }
  }
}

bool FriendsRecentPhotosManagerI::isValid() {
  IceUtil::Mutex::Lock lock(_mutex);
  return _valid;
}

void FriendsRecentPhotosManagerI::run() {
  while (true) {
    time_t seconds = time(NULL) % 86400;
    if (seconds > 14400 && seconds < 14500) {
      //计算节点用户的好友数，并set cache
      vector<int> specialUsers = getSpecialUsers();
      for (vector<int>::const_iterator it = specialUsers.begin(); it != specialUsers.end(); ++it) {
        int friendCount = xce::buddy::adapter::BuddyByIdCacheAdapter::instance().getFriendCount(*it);
        vector<int> friends;
        for (int i = 0; i < friendCount; i += MAX_FRIENDS_NUM) {
          vector<int> partialFriends = xce::buddy::adapter::BuddyByIdCacheAdapter::instance().getFriendListAsc(*it, i, MAX_FRIENDS_NUM);
          friends.insert(friends.end(), partialFriends.begin(), partialFriends.end());
        }
        UserFriendsPtr userFriendsPtr = new UserFriends(*it, friends);
        ServiceI::instance().getObjectCache()->addObject(USER_FRIENDS, *it, userFriendsPtr); 
      }
      //防止一天计算多次
      sleep(200);
    } else {
      sleep(60);
    }
  }
}

vector<int> FriendsRecentPhotosManagerI::getSpecialUsers() {
  IceUtil::Mutex::Lock lock(_mutex);
  return vector<int>(_specialUsers.begin(), _specialUsers.end()); 
}

bool FriendsRecentPhotosManagerI::isSpecialUser(int userId) {
  IceUtil::Mutex::Lock lock(_mutex);
  set<int>::const_iterator it = _specialUsers.find(userId);
  return it != _specialUsers.end();
}

void FriendsRecentPhotosManagerI::addSpecialUsers(int userId) {
  IceUtil::Mutex::Lock lock(_mutex);
  _specialUsers.insert(userId);
}

void FriendsRecentPhotosManagerI::setValid(bool valid, const Ice::Current& current) {
  IceUtil::Mutex::Lock lock(_mutex);
  xce::serverstate::ServerStateSubscriber::instance().setStatus(valid, "FRPM");
  xce::serverstate::ServerStateSubscriber::instance().setStatus(valid); 
  MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, "[FriendsRecentPhotosManagerI::setValid] valid = "
      + boost::lexical_cast<string>(valid), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
}

void FriendsRecentPhotosManagerI::setData(const MyUtil::ObjectResultPtr& userPhotoData, const Ice::Current& current) {
  MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, "[FriendsRecentPhotosManagerI::setData] data.size = "
      + boost::lexical_cast<string>(userPhotoData->data.size()), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
  if (userPhotoData->data.empty()) {
    return;
  }
  MyUtil::ObjectResultPtr userPhotoInfoResultPtr = UserPhotosInfo::parseFromUserPhotosResultPtr(userPhotoData);
  //将所有空对象替换为EMPTY_USER_PHOTOS_INFO_PTR
  for (map<long, Ice::ObjectPtr>::iterator it = userPhotoInfoResultPtr->data.begin(); it != userPhotoInfoResultPtr->data.end(); ++it) {
    if (UserPhotosInfoPtr::dynamicCast(it->second)->getPhotoInfos().empty()) {
      it->second = EMPTY_USER_PHOTOS_INFO_PTR;
    }
  }
  ServiceI::instance().getObjectCache()->addObjects(USER_PHOTOS_INFO, userPhotoInfoResultPtr->data);
}

Ice::ObjectPtr UserPhotosInfoFactoryI::create(Ice::Int id) {
  MCE_INFO("[UserPhotosInfoFactoryI::create] userId = " << id);
  vector<UserPhotosInfoPtr> ptrs;
  Statement sql;
  string tableName = TABLE_NAME + "_" + boost::lexical_cast<string>(id % TABLE_MOD);
  sql << "select " << COLUMN_USERID << ", " << COLUMN_PHOTOS << " from " << tableName << " where "
      << COLUMN_USERID << " = " << id;
  QueryRunner(DB_SOURCE, CDbRServer, tableName).query(sql, UserPhotosInfoResultHandlerI(ptrs));
  if (ptrs.empty() || (*ptrs.begin())->getPhotoInfos().empty()) {
    return EMPTY_USER_PHOTOS_INFO_PTR;
  } else {
    MyUtil::IntSeq userIds;
    userIds.push_back(id);
    FriendsRecentPhotosWriterAdapter::instance().reloadUserPhotos(userIds);
    return *ptrs.begin(); 
  }
}

//不会调用这个函数
Ice::ObjectPtr UserFriendsFactoryI::create(Ice::Int id) {
  return 0;
}
