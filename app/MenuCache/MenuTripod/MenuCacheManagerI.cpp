/*
 * =====================================================================================
 *
 *       Filename:  MenuCacheManagerI.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年12月14日 16时53分10秒
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *        Company:  renren-inc
 *
 * =====================================================================================
 */
#include "MenuCacheManagerI.h"
#include "MenuTripodShare.h"
#include "ServiceI.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "DbCxxPool/src/QueryRunner.h"
#include "app/Utils/src/DBUtils.h"

using namespace xce::menutripod;
using namespace xce::serverstate;
using namespace com::xiaonei::xce;

void MyUtil::initialize() {
  MCE_DEBUG("MenuCacheManagerI::initialize");
  ServiceI& service = ServiceI::instance();

  // 向icegrid加入服务
  service.getAdapter()->add(&MenuCacheManagerI::instance(), service.createIdentity("M", ""));

  // 注册HA
  int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("MenuCacheTripod.Mod", 1);
  string controllerEndpoint = service.getCommunicator()->getProperties()->getPropertyWithDefault("MenuCacheTripod.Endpoint", "ControllerMenuCacheTripod");
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("MenuCacheTripod.Interval", 5);
  ServerStateSubscriber::instance().initialize(controllerEndpoint, &MenuCacheManagerI::instance(), mod, interval);
  MenuCacheManagerI::instance().init();
}

void MenuCacheManagerI::init() {
  tripodClient_ = new TripodClient("UserZooKeeper1:2181,UserZooKeeper2:2181,UserZooKeeper3:2181,UserZooKeeper4:2181,UserZooKeeper5:2181/Tripod",
      "ne0", "SearchCache");
  allCacheTypes_.push_back(INSTALLED);
  allCacheTypes_.push_back(RECENTLY);
  allCacheTypes_.push_back(FAVORITE);
  loadThread_ = new ReloadFromTripodThread();
  writeThread_ = new WriteRecordToDBThread(loadThread_);
  writeThread_->run();
  loadThread_->run();
}

void MenuCacheManagerI::addKeysToLoad(const KeySeq& keys) {
  loadThread_->add(keys);
}

map<string, MenuTripodDataPtr> MenuCacheManagerI::getWithKeys(const KeySeq& keys) {
  map<string, MenuTripodDataPtr> result;
  KeySeq missedKeys;
  DataMap dataMap = tripodClient_->get(keys, missedKeys);
  for (DataMap::const_iterator it = dataMap.begin(); it != dataMap.end(); ++it) {
    string byteArray(it->second.begin(), it->second.end());
    MenuTripodDataPtr ptr = new MenuTripodData(byteArray);
    result.insert(make_pair<string, MenuTripodDataPtr>(it->first, ptr));
  }
  return result;
}

map<string, MenuTripodDataPtr> MenuCacheManagerI::getFromDB(const KeySeq& keys) {
  map<CacheType, vector<int> > cacheTypeToUserIds = TripodHelper::getCacheTypeToUserIds(keys);
  vector<MenuTripodDataPtr> ptrs;
  for (map<CacheType, vector<int> >::const_iterator itm = cacheTypeToUserIds.begin(); itm != cacheTypeToUserIds.end(); ++itm) {
    map<int, vector<int> > seperatedUserIds = xce::utils::DBUtils::seperateIdsByMod<int>(itm->second, TABLE_CLUSTER);
    for (map<int, vector<int> >::const_iterator it = seperatedUserIds.begin(); it != seperatedUserIds.end(); ++it) {
      Statement querySql;
      querySql << "select user_id, app_ids from " << TripodHelper::getTablePrefix(itm->first) << SPLITTOR << it->first << " where user_id in "
        << xce::utils::DBUtils::getIdsStr<int>(itm->second);
      BatchMenuTripodDataHandler handler(ptrs, itm->first);
      QueryRunner(TripodHelper::getDBSource(itm->first), CDbRServer).query(querySql, handler);
    }
  }
  map<string, MenuTripodDataPtr> resultMap;
  for (vector<MenuTripodDataPtr>::const_iterator it = ptrs.begin(); it != ptrs.end(); ++it) {
    resultMap.insert(make_pair<string, MenuTripodDataPtr>(TripodHelper::getKey(*it), *it));
  }
  return resultMap;
}

CacheType2AppIdSeqMap MenuCacheManagerI::get(int userId, const CacheTypeSeq& types, const Ice::Current& current) {
  MCE_INFO("[MenuCacheManagerI::get] userId = " << userId);
  CacheType2AppIdSeqMap resultMap;
  KeySeq keys = TripodHelper::getKeySeq(userId, types);
  map<string, MenuTripodDataPtr> tripodDataPtrMap = getWithKeys(keys);
  for (map<string, MenuTripodDataPtr>::const_iterator it = tripodDataPtrMap.begin(); it != tripodDataPtrMap.end(); ++it) {
    resultMap.insert(make_pair<CacheType, AppIdSeq>(it->second->getCacheType(), it->second->getAppIds()));
  }
  return resultMap;
}

CacheType2AppIdSeqMap MenuCacheManagerI::getAll(int userId, const Ice::Current& current) {
  MCE_INFO("[MenuCacheManagerI::getAll] userId = " << userId);
  return get(userId, allCacheTypes_);
}

void MenuCacheManagerI::set(int userId, CacheType type, const AppIdSeq& appIds, const Ice::Current& current) {
  MCE_INFO("[MenuCacheManagerI::set] userId = " << userId << ", cacheType = " << type << ", appIds.size = " << appIds.size());
  vector<MenuTripodDataPtr> ptrs;
  ptrs.push_back(new MenuTripodData(userId, type, appIds));
  writeThread_->add(ptrs);
}

void MenuCacheManagerI::setBatch(const MenuCacheRecordSeq& records, const Ice::Current& current) {
  MCE_INFO("[MenuCacheManagerI::setBatch] records.size = " << records.size());
  vector<MenuTripodDataPtr> ptrs;
  for (MenuCacheRecordSeq::const_iterator it = records.begin(); it != records.end(); ++it) {
    ptrs.push_back(new MenuTripodData((*it)->userId, (*it)->type, (*it)->infoArray));
  }
  writeThread_->add(ptrs);
}

void MenuCacheManagerI::operate(const MenuCacheRecordSeq& records, OpType type) {
  KeySeq keys = TripodHelper::getKeySeq(records);
  map<string, MenuTripodDataPtr> tripodDataPtrs = getFromDB(keys);
  vector<MenuTripodDataPtr> ptrsToWriteDB;
  for (MenuCacheRecordSeq::const_iterator it = records.begin(); it != records.end(); ++it) {
    map<string, MenuTripodDataPtr>::iterator itm = tripodDataPtrs.find(TripodHelper::getKey(*it));
    if (itm != tripodDataPtrs.end()) {
      if (type == Delete) {
        itm->second->deleteAppIds((*it)->infoArray);
      } else {
        itm->second->addAppIds((*it)->infoArray);
      }
      ptrsToWriteDB.push_back(itm->second);
    } else {
      if (type == Add) {
        ptrsToWriteDB.push_back(new MenuTripodData(*it));
      }
    }
  }
  writeThread_->add(ptrsToWriteDB);  
}

void MenuCacheManagerI::addBatch(const MenuCacheRecordSeq& records, const Ice::Current& current) {
  MCE_INFO("[MenuCacheManagerI::addBatch] records.size = " << records.size());
  operate(records, Add);
}

void MenuCacheManagerI::deleteBatch(const MenuCacheRecordSeq& records, const Ice::Current& current) {
  MCE_INFO("[MenuCacheManagerI::deleteBatch] records.size = " << records.size());
  operate(records, Delete);
}

void MenuCacheManagerI::load(const UserIdSeq& userIds, const Ice::Current& current) {
  MCE_INFO("[MenuCacheManagerI::load] userIds.size = " << userIds.size());
  KeySeq keys;
  for (vector<int>::const_iterator it = userIds.begin(); it != userIds.end(); ++it) {
    string userIdStr = boost::lexical_cast<string>(*it);
    keys.push_back(TripodHelper::getCacheTypeStr(RECENTLY) + SPLITTOR + userIdStr);
    keys.push_back(TripodHelper::getCacheTypeStr(FAVORITE) + SPLITTOR + userIdStr);
  }
  addKeysToLoad(keys);
}

bool MenuCacheManagerI::isValid(const Ice::Current& current) {
  IceUtil::RWRecMutex::RLock lock(validMutex_);
  return isValid_;
}

void MenuCacheManagerI::setValid(bool newState, const Ice::Current& current) {
  MCE_INFO("MenuCacheManagerI::setValid valid = " << newState);
  IceUtil::RWRecMutex::WLock lock(validMutex_);
  isValid_ = newState;
  if (isValid_) {
    ServerStateSubscriber::instance().setStatus(1);
  } else {
    ServerStateSubscriber::instance().setStatus(0);
  }
}

void MenuCacheManagerI::setData(const MyUtil::ObjectResultPtr& objects, CacheType type,
      const Ice::Current& current) {
}

WriteRecordToDBThread::WriteRecordToDBThread(const ReloadFromTripodThreadPtr& ptr) : reloadThreadPtr_(ptr) {
}

void WriteRecordToDBThread::write(const map<int, vector<MenuTripodDataPtr> >& modToMenuTripodDataPtrs, const string& dbSource, const string tablePrefix) {
  for (map<int, vector<MenuTripodDataPtr> >::const_iterator it = modToMenuTripodDataPtrs.begin(); it != modToMenuTripodDataPtrs.end(); ++it) {
    Statement sql;
    string tableName = tablePrefix + SPLITTOR + boost::lexical_cast<string>(it->first);
    sql << "insert into " << tableName << "(user_id, app_ids) values";
    bool notEmpty = false;
    for (vector<MenuTripodDataPtr>::const_iterator itP = it->second.begin(); itP != it->second.end(); ++itP) {
      if (itP != it->second.begin()) {
        sql << ",";
      }
      sql << "(" << (*itP)->getUserId() << ", " << mysqlpp::quote << MyUtil::IntegerSeqSerialize<int>::serialize((*itP)->getAppIds()) << ")";
      notEmpty = true;
    }
    if (notEmpty) {
      sql << " on duplicate key update app_ids = values(app_ids)";
      QueryRunner(dbSource, CDbWServer, tableName).store(sql);
      reloadThreadPtr_->add(TripodHelper::getKeySeq(it->second));
    }
  }
}

map<CacheType, map<int, vector<MenuTripodDataPtr> > > WriteRecordToDBThread::seperate(const vector<MenuTripodDataPtr>& menuTripodDataPtrs, int mod) {
  map<CacheType, map<int, vector<MenuTripodDataPtr> > > resultMap;
  for (vector<MenuTripodDataPtr>::const_iterator it = menuTripodDataPtrs.begin(); it != menuTripodDataPtrs.end(); ++it) {
    map<CacheType, map<int, vector<MenuTripodDataPtr> > >::iterator itFound = resultMap.find((*it)->getCacheType());
    if (itFound == resultMap.end()) {
      map<int, vector<MenuTripodDataPtr> > mapToInsert;
      vector<MenuTripodDataPtr> dataToInsert;
      dataToInsert.push_back(*it);
      mapToInsert.insert(make_pair<int, vector<MenuTripodDataPtr> >((*it)->getUserId() % mod, dataToInsert));
      resultMap.insert(make_pair<CacheType, map<int, vector<MenuTripodDataPtr> > >((*it)->getCacheType(), mapToInsert));
    } else {
      map<int, vector<MenuTripodDataPtr> >::iterator itMod = itFound->second.find((*it)->getUserId() % mod);
      if (itMod == itFound->second.end()) {
        vector<MenuTripodDataPtr> dataToInsert;
        dataToInsert.push_back(*it);
        itFound->second.insert(make_pair<int, vector<MenuTripodDataPtr> >((*it)->getUserId() % mod, dataToInsert));
      } else {
        itMod->second.push_back(*it);
      }
    }
  }
  return resultMap;
}

void WriteRecordToDBThread::write(const vector<MenuTripodDataPtr>& menuTripodDataPtrs) {
  MCE_INFO("[WriteRecordToDBThread::write] records.size = " << menuTripodDataPtrs.size());
  //因为数据库表是散成100份的，所以数据写入的时候也要散好，同时区分开Favorite和Recently，
  //两个是在不同的数据表里，使用的数据源也不相同
  map<CacheType, map<int, vector<MenuTripodDataPtr> > > dividedRecords = seperate(menuTripodDataPtrs, TABLE_CLUSTER);
  map<CacheType, map<int, vector<MenuTripodDataPtr> > >::const_iterator recentIt = dividedRecords.find(RECENTLY);
  if (recentIt != dividedRecords.end()) {
    write(recentIt->second, DB_DESC_RECMENUCACHE, REC_TABLE_PREFIX); 
  }
  map<CacheType, map<int, vector<MenuTripodDataPtr> > >::const_iterator favoriteIt = dividedRecords.find(FAVORITE);
  if (favoriteIt != dividedRecords.end()) {
    write(favoriteIt->second, DB_DESC_FAVMENUCACHE, FAV_TABLE_PREFIX); 
  } 
}

void WriteRecordToDBThread::add(const vector<MenuTripodDataPtr>& menuTripodDataPtrs) {
  IceUtil::Monitor<IceUtil::Mutex>::Lock lock(this->mutex_);
  this->menuTripodDataPtrs_.insert(this->menuTripodDataPtrs_.begin(), menuTripodDataPtrs.begin(), menuTripodDataPtrs.end());
  this->mutex_.notify();
}

void WriteRecordToDBThread::run() {
  while (true) {
    vector<MenuTripodDataPtr> dataToWrite;
    {
      IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
      if (menuTripodDataPtrs_.empty()) {
        mutex_.wait();
      }
      dataToWrite.insert(dataToWrite.end(), menuTripodDataPtrs_.begin(), menuTripodDataPtrs_.end());
    }
    if (!dataToWrite.empty()) {
      write(dataToWrite);
    }
  }
}

ReloadFromTripodThread::ReloadFromTripodThread() {
  tripodClient_ = new TripodClient("UserZooKeeper1:2181,UserZooKeeper2:2181,UserZooKeeper3:2181,UserZooKeeper4:2181,UserZooKeeper5:2181/Tripod",
      "ne0", "SearchCache");
}

void ReloadFromTripodThread::add(const KeySeq& keys) {
  IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
  this->keysToReload_.insert(this->keysToReload_.begin(), keys.begin(), keys.end());
  mutex_.notify();
}

void ReloadFromTripodThread::run() {
  while (true) {
    KeySeq keysToReload;
    {
      IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
      if (this->keysToReload_.empty()) {
        mutex_.wait();
      }
      keysToReload.insert(keysToReload.end(), this->keysToReload_.begin(), this->keysToReload_.end());
    }
    if (!keysToReload.empty()) {
      this->tripodClient_->load(keysToReload, 2000, true);
    }
  }
}
