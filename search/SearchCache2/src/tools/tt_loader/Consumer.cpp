#include "Consumer.h"

#include "MessageQueue.h"
 

Consumer::Consumer(MessageQueue& message_queue):message_queue_(message_queue) {
}


void Consumer::run()  {
  for (;;) {
    try {
      MessageUnit message;
      message_queue_.Get(message);
      int min_id = message.min_id_;
      int max_id = message.max_id_;
      Id2SearchCacheDataPtrMap& id_2_search_cache_data_ptr_map = *message.id_2_search_cache_data_ptr_map_ptr_.get();
      //MCE_INFO("min_id=" << min_id << ", max_id=" << max_id << ", id_2_search_cache_data_ptr_map.size()=" << id_2_search_cache_data_ptr_map.size());

      {
        Statement sql;
        sql << "SELECT " << USER_BORN_FIELD << " FROM user_born WHERE id > "
          << min_id << " and id <=" << max_id;
        BatchResultHandlerI handler("id", &SearchCacheData::setUserBornProperties, id_2_search_cache_data_ptr_map);
        QueryRunner(DB_SEARCH2_USERBORN, CDbRServer).query(sql, handler);
      }
      {
        Statement sql;
        sql << "SELECT " << USER_URL_FIELD << " FROM user_url WHERE id  > "
          << min_id << " and id <=" << max_id;
        BatchResultHandlerI handler("id", &SearchCacheData::setUserUrlProperties, id_2_search_cache_data_ptr_map);
        QueryRunner(DB_SEARCH2_USERURL, CDbRServer).query(sql, handler);
      }
      {
        Statement sql;
        sql << "SELECT " << USER_STATE_FIELD << " FROM user_state WHERE id  > "
          << min_id << " and id <=" << max_id;
        BatchResultHandlerI handler("id", &SearchCacheData::setUserStateProperties, id_2_search_cache_data_ptr_map);
        QueryRunner(DB_SEARCH2_USERSTATE, CDbRServer).query(sql, handler);
      }
      {
        Statement sql;
        sql << "SELECT " << USER_CONFIG_FIELD << " FROM user_config WHERE id > "
          << min_id << " and id <=" << max_id;
        BatchResultHandlerI handler("id", &SearchCacheData::setConfigProperties, id_2_search_cache_data_ptr_map);
        QueryRunner(DB_SEARCH2_USERCONFIG, CDbRServer).query(sql, handler);
      }
      {
        Statement sql;
        sql << "SELECT " << USER_AUTH_FIELD << " FROM user_right WHERE id > "
          << min_id << " and id <=" << max_id;
        BatchResultHandlerI handler("id", &SearchCacheData::setUserAuthProperties, id_2_search_cache_data_ptr_map);
        QueryRunner(DB_USER_RIGHT, CDbRServer).query(sql, handler);
      }
      {
        Statement sql;
        sql << "SELECT " << USER_VIEWCOUNT_FIELD << " FROM view_count WHERE id  > "
          << min_id << " and id <=" << max_id;
        BatchResultHandlerI handler("id", &SearchCacheData::setViewCountProperties, id_2_search_cache_data_ptr_map);
        QueryRunner(DB_VIEW_COUNT, CDbRServer).query(sql, handler);
      }
      {
        Statement sql;
        sql << "SELECT " << USER_NAME_FIELD << " FROM user_names WHERE id  > "
          << min_id << " and id <=" << max_id;
        BatchResultHandlerI handler("id", &SearchCacheData::setUserNameProperties, id_2_search_cache_data_ptr_map);
        QueryRunner(DB_SEARCH2_USERNAME, CDbRServer).query(sql, handler);
      }
      {
        Statement sql;
        sql << "SELECT " << USER_STAGE_FIELD<< " FROM user_stage WHERE id  > "
          << min_id << " and id <=" << max_id;

        BatchResultHandlerI handler("id", &SearchCacheData::setUserStageProperties, id_2_search_cache_data_ptr_map);
        QueryRunner(DB_SEARCH2_USERSTAGE, CDbRServer).query(sql, handler);
      }
      {
        Statement sql;
        sql
          //<< "SELECT " << NETWORK_HISTORY_FIELD << " FROM network_history WHERE userid  > "
          << "SELECT " << NETWORK_HISTORY_FIELD << " FROM network_history WHERE userid  > "
          << min_id << " and userid <=" << max_id << " AND status <=0 ";

        map<int, vector<mysqlpp::Row> > id2RowsMap;
        BatchVectorResultHandlerI handler("userid", id2RowsMap);
        QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);

        for (map<int, vector<mysqlpp::Row> >::iterator iter = id2RowsMap.begin();
            iter != id2RowsMap.end(); 
            ++iter) {
          //MCE_DEBUG("NetWork history: uid=" << iter->first << ", rows.size()=" << (iter->second).size());

          map<int,SearchCacheDataPtr>::iterator  search_map_iter = id_2_search_cache_data_ptr_map.find(iter->first);
          if (id_2_search_cache_data_ptr_map.end() != search_map_iter) { 
            SearchCacheDataPtr data = search_map_iter->second;
            if (NULL != data.get()) {
              data->setNetworkPropertiesAccordingVector(iter->second);
              //MCE_DEBUG("NetWork history: uid=" << iter->first << ", data._networks.size()=" << data->_networks.size());
            } else {
              MCE_DEBUG("data.get()==NULL");
            }
          } else {
            //MCE_DEBUG("cannot find " << iter->first << " in search_map");
          }

        }
      }
      {
        Statement sql;
        sql << "SELECT " << ELEMENTARY_SCHOOL_INFO_FIELD << " FROM elementary_school_info WHERE userid  > "
          << min_id << " and userid <=" << max_id;

        map<int, vector<mysqlpp::Row> > id2RowsMap;
        BatchVectorResultHandlerI handler("userid", id2RowsMap);
        QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);

        for (map<int, vector<mysqlpp::Row> >::iterator iter = id2RowsMap.begin();
            iter != id2RowsMap.end(); 
            ++iter) {

          map<int,SearchCacheDataPtr>::iterator  search_map_iter = id_2_search_cache_data_ptr_map.find(iter->first);
          if (id_2_search_cache_data_ptr_map.end() != search_map_iter) {
            SearchCacheDataPtr data = search_map_iter->second;
            if (NULL != data.get()) {
              data->setElementarySchoolPropertiesAccordingVector(iter->second);
            }
          }
        }
      }
      {
        Statement sql;
        sql << "SELECT " << JUNIOR_HIGH_SCHOOL_INFO_FIELD << " FROM junior_high_school_info WHERE userid  > "
          << min_id << " and userid <=" << max_id;

        map<int, vector<mysqlpp::Row> > id2RowsMap;
        BatchVectorResultHandlerI handler("userid", id2RowsMap);
        QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);

        for (map<int, vector<mysqlpp::Row> >::iterator iter = id2RowsMap.begin();
            iter != id2RowsMap.end(); 
            ++iter) {

          map<int,SearchCacheDataPtr>::iterator  search_map_iter = id_2_search_cache_data_ptr_map.find(iter->first);
          if (id_2_search_cache_data_ptr_map.end() != search_map_iter) {
            SearchCacheDataPtr data = search_map_iter->second;
            if (NULL != data.get()) {
              data->setJuniorSchoolPropertiesAccordingVector(iter->second);
            }
          }
        }
      }

      {
        Statement sql;
        sql << "SELECT " << HIGH_SCHOOL_INFO_FIELD << " FROM high_school_info WHERE userid  > "
          << min_id << " and userid <=" << max_id;
        map<int, vector<mysqlpp::Row> > id2RowsMap;
        BatchVectorResultHandlerI handler("userid", id2RowsMap);
        QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);

        for (map<int, vector<mysqlpp::Row> >::iterator iter = id2RowsMap.begin();
            iter != id2RowsMap.end(); 
            ++iter) {
          map<int,SearchCacheDataPtr>::iterator  search_map_iter = id_2_search_cache_data_ptr_map.find(iter->first);
          if (id_2_search_cache_data_ptr_map.end() != search_map_iter) {
            SearchCacheDataPtr data = search_map_iter->second;
            if (NULL != data.get()) {
              data->setHighSchoolPropertiesAccordingVector(iter->second);
            }
          }
        }
      }
      {
        Statement sql;
        sql << "SELECT " << COLLEGE_INFO_FIELD << " FROM college_info WHERE userid  > "
          << min_id << " and userid <=" << max_id;

        map<int, vector<mysqlpp::Row> > id2RowsMap;
        BatchVectorResultHandlerI handler("userid", id2RowsMap);
        QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);


        for (map<int, vector<mysqlpp::Row> >::iterator iter = id2RowsMap.begin();
            iter != id2RowsMap.end(); 
            ++iter) {

          map<int,SearchCacheDataPtr>::iterator  search_map_iter = id_2_search_cache_data_ptr_map.find(iter->first);
          if (id_2_search_cache_data_ptr_map.end() != search_map_iter) {
            SearchCacheDataPtr data = search_map_iter->second;

            if (NULL != data.get()) {
              data->setCollegePropertiesAccordingVector(iter->second);
            }
          }
        }
      }
      {
        Statement sql;
        sql << "SELECT " << UNIVERSITY_INFO_FIELD << " FROM university_info WHERE userid  > "
          << min_id << " and userid <=" << max_id;


        map<int, vector<mysqlpp::Row> > id2RowsMap;
        BatchVectorResultHandlerI handler("userid", id2RowsMap);
        QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);


        for (map<int, vector<mysqlpp::Row> >::iterator iter = id2RowsMap.begin();
            iter != id2RowsMap.end(); 
            ++iter) {
          map<int,SearchCacheDataPtr>::iterator  search_map_iter = id_2_search_cache_data_ptr_map.find(iter->first);
          if (id_2_search_cache_data_ptr_map.end() != search_map_iter) {
            SearchCacheDataPtr data = search_map_iter->second;
            if (NULL != data.get()) {
              data->setUniversityPropertiesAccordingVector(iter->second);
            }
          }
        }
      }
      {
        Statement sql;
        sql << "SELECT " << WORKPLACE_INFO_FIELD << " FROM workplace_info WHERE userid  > " 
          << min_id << " and userid <=" << max_id;

        map<int, vector<mysqlpp::Row> > id2RowsMap;
        BatchVectorResultHandlerI handler("userid", id2RowsMap);
        QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);


        for (map<int, vector<mysqlpp::Row> >::iterator iter = id2RowsMap.begin();
            iter != id2RowsMap.end(); 
            ++iter) {

          map<int,SearchCacheDataPtr>::iterator  search_map_iter = id_2_search_cache_data_ptr_map.find(iter->first);
          if (id_2_search_cache_data_ptr_map.end() != search_map_iter) {
            SearchCacheDataPtr data = search_map_iter->second;

            if (NULL != data.get()) {
              data->setWorkplacePropertiesAccordingVector(iter->second);
            }
          }
        }
      }
      {
        Statement sql;
        sql << "SELECT " << REGION_INFO_FIELD << " FROM region_info WHERE userid  > "
          << min_id << " and userid <=" << max_id;

        map<int, vector<mysqlpp::Row> > id2RowsMap;
        BatchVectorResultHandlerI handler("userid", id2RowsMap);
        QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);


        for (map<int, vector<mysqlpp::Row> >::iterator iter = id2RowsMap.begin();
            iter != id2RowsMap.end(); 
            ++iter) {
          map<int,SearchCacheDataPtr>::iterator  search_map_iter = id_2_search_cache_data_ptr_map.find(iter->first);
          if (id_2_search_cache_data_ptr_map.end() != search_map_iter) {
            SearchCacheDataPtr data = search_map_iter->second;

            if (NULL != data.get()) {
              data->setRegionPropertiesAccordingVector(iter->second);
            }
          }
        }
      }
      {
        for(int i=0; i<100; i++) {
          ostringstream table_name;
          table_name << "user_fond_" << i;
          Statement sql;
          sql  << "SELECT " << USER_FOND_FIELD << " FROM "<<table_name.str()<<" WHERE user_id  > "
            << min_id << " and user_id <=" << max_id;
          BatchResultHandlerI handler("user_id", &SearchCacheData::setUserFondProperties, id_2_search_cache_data_ptr_map);
          QueryRunner(DB_SEARCH2_USERFOND, CDbRServer).query(sql, handler);
        }
      }
      {
        Statement sql;
        sql  << "SELECT " << USER_TIME_FIELD << " FROM user_time WHERE id  > " 
          << min_id << " and id <=" << max_id;

        BatchResultHandlerI handler("id", &SearchCacheData::setLastlogintimeProperties, id_2_search_cache_data_ptr_map);
        QueryRunner(DB_SEARCH2_USERTIME, CDbRServer).query(sql, handler);
      }


      std::map<int,SearchCacheDataPtr>::iterator iter;
      for(iter=id_2_search_cache_data_ptr_map.begin(); iter!=id_2_search_cache_data_ptr_map.end(); ++iter) {
        int userId = iter->first;
        boost::shared_ptr<IDbCache> db_handler = DbRead::instance().getDbHandler(userId); 
        if (db_handler) {
          if (!db_handler->Insert(userId, *iter->second)) {
            MCE_WARN("db_handler->Insert("<<userId<<") Failed");
            //std::cerr<<"db_handler->Insert("<<userId<<") Failed"<<std::endl;
          }
        } else {
          MCE_WARN("DB handle " << userId << " is error");
          //std::cerr<<"DB handle " << userId << " is error"<<std::endl;
        }
      }

      MCE_INFO("done min_id=" << min_id << ", max_id=" << max_id << ", id_2_search_cache_data_ptr_map.size()=" << id_2_search_cache_data_ptr_map.size());
    } catch (IceUtil::Exception& e) {
      MCE_WARN("Consumer thread exception " <<  e.what());
    } catch (mysqlpp::Exception& e) {
      MCE_WARN("Consumer thread exception -> " << e.what());
    } catch (std::exception& e) {
      MCE_WARN("Consumer thread exception " << e.what());
    }
  }
}

BatchResultHandlerI::BatchResultHandlerI(std::string id, void (SearchCacheData::*p)(mysqlpp::Row& row), Id2SearchCacheDataPtrMap& id_2_search_cache_data_ptr_map):
  _id(id),   _pFunc(p), id_2_search_cache_data_ptr_map_(id_2_search_cache_data_ptr_map) {
  }

bool BatchResultHandlerI::handle(mysqlpp::Row& row) const {
  int id = (int) row[_id.c_str()];
  Id2SearchCacheDataPtrMap::iterator iter;
  iter = id_2_search_cache_data_ptr_map_.find(id);
  if (iter == id_2_search_cache_data_ptr_map_.end()) {
    //MCE_WARN("IMPOSSIABLE, user_config has id=" << id << " but user_basic not. "
    //<< __FILE__ << " " << __LINE__);
  } else {
    (((iter->second).get())->*_pFunc)(row);
  }
  return true;
}

//***************************************************************************
BatchVectorResultHandlerI::BatchVectorResultHandlerI(std::string idField, map<int, vector<mysqlpp::Row> > &id2RowsMap):
  _idField(idField), _id2RowsMap(id2RowsMap) {
  }

bool BatchVectorResultHandlerI::handle(mysqlpp::Row& row) const {
  int id = (int) row[_idField.c_str()];
  _id2RowsMap[id].push_back(row);

  return true;
}


