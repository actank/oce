#include <boost/lexical_cast.hpp>
#include "SearchTripodProducer.h"
#include "search/tripodsearchcache/TripodSearchDbAdapter.h"
#include "util/cpp/TimeCost.h"

using namespace com::renren::searchtripod;
using namespace com::renren::tripod;
using namespace xce::search::tripodsearchdbadapter;
using namespace xce::search::tripodcachedata;
using namespace com::xiaonei::xce;
using namespace IceUtil;

DataMap SearchTripodProducer::create(const KeySeq& keys, bool isMaster) {
  MCE_DEBUG("[SearchTripodProducer::create] begin keys.size:" << keys.size());
  DataMap result;
  std::map<std::string, TripodCacheDataPtr> id2tripodData;
  std::vector<std::string> deleteKeys;
  std::vector<std::string> keyIds;
  //id为0的元素过滤
  for (std::vector<std::string>::const_iterator it = keys.begin(); it != keys.end(); ++it) {
    if (boost::lexical_cast<int>(*it) != 0) {
      keyIds.push_back(*it);
    }
  }

  if (keyIds.empty()) {
    MCE_WARN("[SearchTripodProducer::create] keyIds is empty");
    return result;
  }

  std::ostringstream ossIdList;
  ossIdList << "(" ;

  for (std::vector<std::string>::iterator iter = keyIds.begin(); iter != keyIds.end(); ++iter) {
    if (iter != keyIds.begin()) {
      ossIdList << ",";
    }
    ossIdList << *iter;
  }
  ossIdList << ")";
  Statement statusSql;
  statusSql << "select " << USER_STATUS_FIELD << " from " << TABLE_USER_PASSPORT << " where id in " << ossIdList.str();
  MCE_DEBUG("select " << USER_STATUS_FIELD << " from " << TABLE_USER_PASSPORT << " where id in " << ossIdList.str());
  BatchBasicStatusResultHandlerI statusHandler(id2tripodData, "id", deleteKeys);
  QueryRunner(DB_SEARCH2_USERBASIC_STATUS, CDbRServer).query(statusSql, statusHandler);
  MCE_DEBUG("[SearchTripodProducer::create] deleteKeys.size:" << deleteKeys.size());
  std::vector<std::string> userIds;
  if (!deleteKeys.empty()) {
    std::vector<std::string>::iterator deleteIter;
    for (std::vector<std::string>::iterator it = keyIds.begin(); it != keyIds.end(); ++it) {
      deleteIter = find(deleteKeys.begin(), deleteKeys.end(), *it);
      if (deleteIter != deleteKeys.end()) {
        result[*it] = Data();//status>3的用户写入一个空串
        MCE_DEBUG("[SearchTripodProducer::create] deleteID:" << *it);
      } else {
        userIds.push_back(*it);
      }
    }
  } else {
    userIds = keyIds;
  }
  MCE_DEBUG("[SearchTripodProducer::create] after delete status >= LIMIT_STATUS userIds.size:" << userIds.size());
  if (userIds.empty()) {
    MCE_WARN("[SearchTripodProducer::create] userIds is empty");
    return result;
  }
  try {
    loadSql(userIds, id2tripodData);
  } catch (std::exception& e) {
    MCE_ERROR("[SearchTripodProducer::create] ERROR loadSqlBatch : " << e.what());
  }
  for(map<std::string, TripodCacheDataPtr>::iterator iter = id2tripodData.begin(); iter != id2tripodData.end(); ++iter) {
    std::string userIdKey = iter->first;
    if(NULL == iter->second.get()) {
      MCE_DEBUG("[SearchTripodProducer::create] iter->second is null");
      continue;
    }
    std::string dataStr;
    iter->second->SerializeToString(&dataStr);
    result[userIdKey] = Data(dataStr.begin(), dataStr.end());
  }
  MCE_DEBUG("[SearchTripodProducer::create] end result.size:" << result.size());
  return result;
}

KeySeq SearchTripodProducer::createHotKeys(const std::string& beginKey, int limit) {
  std::vector<std::string> result;
  return result;
}

KeySeq SearchTripodProducer::createKeys(const std::string& beginKey, int limit) {
  MCE_DEBUG("[SearchTripodProducer::createKeys] begin beginKey:" << beginKey << " limit:" << limit);
  std::vector<std::string> userKeys;
  Statement sql;
  sql << "select id, status from " << TABLE_USER_PASSPORT << " where id>="<< beginKey << " limit " << limit;
  CreateKeysResultHandler handle(userKeys, "id");
  QueryRunner(DB_SEARCH2_USERBASIC_STATUS, CDbRServer).query(sql, handle);

  return userKeys;
}

bool SearchTripodProducer::loadSql(const std::vector<std::string>& keys, std::map<std::string, TripodCacheDataPtr>& id2tripodData) {
  MCE_DEBUG("[SearchTripodProducer::loadSql] begin keys.size:" << keys.size());
  if (keys.size() <= 0) {
    MCE_WARN("[SearchTripodProducer::loadSql] keys is empty");
    return false;
  }
  if (keys.size() == 1 && boost::lexical_cast<int>(keys[0]) == 0) {
    MCE_WARN("[SearchTripodProducer::loadSql] keys.size is 1, value is 0");
    return false;
  }
 
  std::vector<ThreadPtr> threads;
  //从user_born中查询id, birth_day,birth_month,birth_year,astrology,gender,home_province,home_city
  threads.push_back(new BatchReadThread(keys, TABLE_USER_BORN, USER_BORN_FIELD, "id", 1, &TripodCacheData::                     setUserBornProperties, id2tripodData, DB_SEARCH2_USERBORN));
  //TODO: 
  //从user_url总查询id, tinyurl, headurl
  threads.push_back(new BatchReadThread(keys, TABLE_USER_URL, USER_URL_FIELD, "id", 1, &TripodCacheData::setUserUrlProperties,  id2tripodData, DB_SEARCH2_USERURL));
  //从user_state中查询id, state, star
  threads.push_back(new BatchReadThread(keys, TABLE_USER_STATE, USER_STATE_FIELD, "id", 1, &TripodCacheData::                   setUserStateProperties, id2tripodData, DB_SEARCH2_USERSTATE));
  //TODO:
  //从user_config中查询id, browse_config, status_config, basic_config, profile_privacy
  threads.push_back(new BatchReadThread(keys, TABLE_USER_CONFIG, USER_CONFIG_FIELD, "id", 1, &TripodCacheData::                 setConfigProperties, id2tripodData, DB_SEARCH2_USERCONFIG));
  //从user_names中查询id, nickname, name
  threads.push_back(new BatchReadThread(keys, TABLE_USER_NAME, USER_NAME_FIELD, "id", 1, &TripodCacheData::                     setUserNameProperties, id2tripodData, DB_SEARCH2_USERNAME));
  //从user_stage中查询id, stage
  threads.push_back(new BatchReadThread(keys, TABLE_USER_STAGE, USER_STAGE_FIELD, "id", 1, &TripodCacheData::                   setUserStageProperties, id2tripodData, DB_SEARCH2_USERSTAGE));
  //从user_fond_*中查询user_id, type, content
  threads.push_back(new BatchReadThread(keys, "user_fond_", USER_FOND_FIELD, "user_id", 100, &TripodCacheData::                 setUserFondProperties, id2tripodData, DB_SEARCH2_USERFOND));
  //从network_history中查询userid, network_id, network_name, stage
  threads.push_back(new BatchReadVectorThread(keys, TABLE_NETWORK_HISTORY, NETWORK_HISTORY_FIELD, "userid", &TripodCacheData::  setNetworkPropertiesAccordingVector, id2tripodData, DB_SEARCH2_USERBASIC));
  //从elementary_school_info查询userid, elementary_school_id, elementary_school_name, elementary_school_year
  threads.push_back(new BatchReadVectorThread(keys, TABLE_ELEMENTARY_SCHOOL, ELEMENTARY_SCHOOL_INFO_FIELD, "userid",            &TripodCacheData::setElementarySchoolPropertiesAccordingVector, id2tripodData, DB_SEARCH2_USERBASIC));
  //从junior_high_school_info查询userid, junior_high_school_id, junior_high_school_name, junior_high_school_year
  threads.push_back(new BatchReadVectorThread(keys, TABLE_JUNIOR_SCHOOL, JUNIOR_HIGH_SCHOOL_INFO_FIELD, "userid",               &TripodCacheData::setJuniorSchoolPropertiesAccordingVector, id2tripodData, DB_SEARCH2_USERBASIC));
  //从high_school_info中查询userid, high_school_id, high_school_name, enroll_year, h_class1, h_class2, h_class3
  threads.push_back(new BatchReadVectorThread(keys, TABLE_HIGH_SCHOOL, HIGH_SCHOOL_INFO_FIELD, "userid", &TripodCacheData::     setHighSchoolPropertiesAccordingVector, id2tripodData, DB_SEARCH2_USERBASIC));
  //从college_info中查询userid, college_id, college_name, department,enroll_year
  threads.push_back(new BatchReadVectorThread(keys, TABLE_COLLEGE_INFO, COLLEGE_INFO_FIELD, "userid", &TripodCacheData::        setCollegePropertiesAccordingVector, id2tripodData, DB_SEARCH2_USERBASIC));
  //从university_info中查询userid, university_id, university_name, department, enroll_year, dorm
  threads.push_back(new BatchReadVectorThread(keys, TABLE_UNIVERSITY_INFO, UNIVERSITY_INFO_FIELD, "userid", &TripodCacheData::  setUniversityPropertiesAccordingVector, id2tripodData, DB_SEARCH2_USERBASIC));
  //从workplace_info中查询userid, workplace_id, workplace_name
  threads.push_back(new BatchReadVectorThread(keys, TABLE_WORKPLACE_INFO, WORKPLACE_INFO_FIELD, "userid", &TripodCacheData::    setWorkplacePropertiesAccordingVector, id2tripodData, DB_SEARCH2_USERBASIC));
  //从region_info中查询userid, region_id, province_name, city_name
  threads.push_back(new BatchReadVectorThread(keys, TABLE_REGION_INFO, REGION_INFO_FIELD, "userid", &TripodCacheData::          setRegionPropertiesAccordingVector, id2tripodData, DB_SEARCH2_USERBASIC));
  for (std::vector<ThreadPtr>::iterator itp = threads.begin(); itp != threads.end(); ++itp) {
    (*itp)->start();
  }
  for (std::vector<ThreadPtr>::iterator itc = threads.begin(); itc != threads.end(); ++itc) {
    (*itc)->getThreadControl().join();
  }

  return true;
}

void BatchReadThread::run() {
  MyUtil::TimeCost tc = MyUtil::TimeCost::create("BatchReadThread::handle laodSql",1);
  if (tableCluster_ == 1) {
    Statement sql;
    sql = generateSql();
    BatchResultHandlerI handler(pFunc_, id2tripodData_, condition_);
    QueryRunner(dbName_, CDbRServer).query(sql, handler);
  } else {
    std::vector<std::vector<std::string> > bucket(tableCluster_);
    for (std::vector<std::string>::const_iterator it = ids_.begin(); it != ids_.end(); ++it) {
      int mod = (boost::lexical_cast<int>(*it)) % tableCluster_;
      bucket[mod].push_back(*it);
    }
    for(int i=0; i<tableCluster_; i++) {
      if (bucket[i].empty()) {
        continue;
      }
      Statement sql;
      sql = generateSql(bucket[i], i);
      BatchResultHandlerI handler(pFunc_, id2tripodData_, condition_);
      QueryRunner(dbName_, CDbRServer).query(sql, handler);
    }
  }
  tc.step(tableName_);
}

Statement BatchReadThread::generateSql() {
  std::ostringstream oss;
  oss << "(" ;
  for (std::vector<std::string>::const_iterator iter = ids_.begin(); iter != ids_.end(); ++iter) {
    if (iter != ids_.begin()) {
      oss << ",";
    }
    if(boost::lexical_cast<int>(*iter) != 0)  {
      oss << *iter;
    }  else  {
      continue;
    }
  }
  oss << ")";
  Statement sql;
  sql << "select " << resultFields_ << " from " << tableName_ << " where " << condition_ << " in " << oss.str();
  MCE_DEBUG("select " << resultFields_ << " from " << tableName_ << " where " << condition_ << " in " << oss.str());
  return sql;
}

Statement BatchReadThread::generateSql(const std::vector<std::string>& bucketIds, int cluster) {
  std::ostringstream oss;
  oss << "(" ;
  for (std::vector<std::string>::const_iterator iter = bucketIds.begin(); iter != bucketIds.end(); ++iter) {
    if (iter != bucketIds.begin()) {
      oss << ",";
    }
    if(boost::lexical_cast<int>(*iter) != 0)  {
      oss << *iter;
    }  else  {
      continue;
    }
  }
  oss << ")";
  Statement sql;
  sql << "select " << resultFields_ << " from " << tableName_ << cluster << " where " << condition_ << " in " <<   oss.str();
  MCE_DEBUG("select " << resultFields_ << " from " << tableName_ << cluster << " where " << condition_ << " in " << oss.str());
  return sql;
}

void BatchReadVectorThread::run() {
  MyUtil::TimeCost tc = MyUtil::TimeCost::create("BatchReadVectorThread::handle laodSql",1);
  Statement sql;
  sql = generateSql();
  std::map<std::string, vector<mysqlpp::Row> > id2RowsMap;
  BatchVectorResultHandlerI handler(id2RowsMap, condition_);
  QueryRunner(dbName_, CDbRServer).query(sql, handler);
  for (std::map<std::string, vector<mysqlpp::Row> >::iterator iter = id2RowsMap.begin(); iter != id2RowsMap.end(); ++iter) {
    if (NULL != id2tripodData_[iter->first].get()) {
      (id2tripodData_[iter->first].get()->*pFunc_)(iter->second);
    }
  }
  tc.step(tableName_);
}

Statement BatchReadVectorThread::generateSql() {
  std::ostringstream oss;
  oss << "(" ;
  for (std::vector<std::string>::const_iterator iter = ids_.begin(); iter != ids_.end(); ++iter) {
    if (iter != ids_.begin()) {
      oss << ",";
    }
    if(boost::lexical_cast<int>(*iter) != 0)  {
      oss << *iter;
    }  else  {
      continue;
    }
  }
  oss << ")";
  Statement sql;
  sql << "select " << resultFields_ << " from " << tableName_ << " where " << condition_ << " in " << oss.str();
  MCE_DEBUG("select " << resultFields_ << " from " << tableName_ << " where " << condition_ << " in " << oss.str());
  return sql;
}

