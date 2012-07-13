/*
 * =====================================================================================
 *
 *       Filename:  MenuTripodProducer.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年12月13日 17时03分15秒
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *        Company:  renren-inc
 *
 * =====================================================================================
 */
#include "MenuTripodProducer.h"
#include "app/Utils/src/DBUtils.h"
#include "app/MenuCache/MenuTripod/MenuTripodShare.h"
#include "app/MenuCache/MenuTripod/MenuTripodData.pb.cc"
#include "ServiceI.h"
#include "DbCxxPool/src/QueryRunner.h"

using namespace xce::menutripod;
using namespace com::renren::tripod;
using namespace com::xiaonei::xce;

void MenuTripodProducer::fillDataMap(DataMap& dataMap, const Statement& sql, CacheType cacheType) {
  vector<MenuTripodDataPtr> ptrs;
  BatchMenuTripodDataHandler handler(ptrs, cacheType);
  //warn
  QueryRunner(TripodHelper::getDBSource(cacheType), CDbRServer).query(sql, handler);
  for (vector<MenuTripodDataPtr>::const_iterator it = ptrs.begin(); it != ptrs.end(); ++it) {
    string byteArray = (*it)->serialize();
    string key = TripodHelper::getCacheTypeStr(cacheType) + SPLITTOR + boost::lexical_cast<string>((*it)->getUserId());
    dataMap[key] = Data(byteArray.begin(), byteArray.end());
  }
}

DataMap MenuTripodProducer::create(const KeySeq& keys, bool isMaster) {
  MCE_INFO("[MenuTripodProducer::create] keys.size = " << keys.size());
  map<CacheType, vector<int> > cacheTypeToUserIds = TripodHelper::getCacheTypeToUserIds(keys);

  DataMap resultDataMap;
  for (map<CacheType, vector<int> >::const_iterator itm = cacheTypeToUserIds.begin(); itm != cacheTypeToUserIds.end(); ++itm) {
    map<int, vector<int> > seperatedUserIds = xce::utils::DBUtils::seperateIdsByMod<int>(itm->second, TABLE_CLUSTER);
    for (map<int, vector<int> >::const_iterator it = seperatedUserIds.begin(); it != seperatedUserIds.end(); ++it) {
      Statement querySql;
      querySql << "select user_id, app_ids from " << TripodHelper::getTablePrefix(itm->first) << SPLITTOR << it->first << " where user_id in "
          << xce::utils::DBUtils::getIdsStr<int>(itm->second);
      fillDataMap(resultDataMap, querySql, itm->first);
    }
  }
  MCE_INFO("[MenuTripodProducer::create] keys.size = " << keys.size() << ", result.size = " << resultDataMap.size());
  return resultDataMap;
}

KeySeq MenuTripodProducer::createHotKeys(const string& beginKey, int limit) {
  KeySeq resultKeySeq;
  return resultKeySeq;
}

KeySeq MenuTripodProducer::createKeys(const string& beginKey, int limit) {
  MCE_INFO("[MenuTripodProducer::createKeys] beginKey = " << beginKey << ", limit = " << limit);
  KeySeq resultKeySeq;
  Statement querySql;
  querySql << "select " << COLUMN_USERID << " from " << TABLE_USER_PASSPORT << " where " << COLUMN_USERID
      << " >= " << beginKey << " limit " << limit;
  vector<int> userIds;
  xce::utils::BatchIdResultHandler<int> handler(COLUMN_USERID, userIds);
  QueryRunner(DB_DESC_USERPASSPORT, CDbRServer).query(querySql, handler);
  for (vector<int>::const_iterator it = userIds.begin(); it != userIds.end(); ++it) {
    string userIdStr = boost::lexical_cast<string>(*it);
    resultKeySeq.push_back(TripodHelper::getCacheTypeStr(RECENTLY) + SPLITTOR + userIdStr);
    resultKeySeq.push_back(TripodHelper::getCacheTypeStr(FAVORITE) + SPLITTOR + userIdStr);
  }
  MCE_INFO("[MenuTripodProducer::createKeys] beginKey = " << beginKey << ", limit = " << limit << ", result.size = " << resultKeySeq.size());
  return resultKeySeq;
}
