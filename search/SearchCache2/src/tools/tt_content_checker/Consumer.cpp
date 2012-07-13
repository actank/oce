#include "Consumer.h"
#include "MessageQueue.h"
#include "search/SearchCache2/src/DistSearchCacheLogic/DbOperation.h"
using namespace oce::searchcache;

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
          << min_id << " and userid <=" << max_id << " AND status <=0 order by network_id";

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
        int user_id = iter->first;
       xce::searchcache::dboperation::SearchCacheMemDataPtr searchcache_data = xce::dist::searchcacheadapter::DistSearchCacheAdapter::instance().getData(user_id);
      
      if((*iter->second)._status <= STATUS_SET_VALUE)  {
       if(NULL == searchcache_data.get() || (searchcache_data->name() == "" && searchcache_data->headurl() == ""))  {
        MCE_INFO(user_id <<" status 正常: mysql中有数据而SearchCache中无此数据");
        transformData(iter->second, searchcache_data);
        xce::dist::searchcacheadapter::DistSearchCacheAdapter::instance().setData(user_id, searchcache_data);

      }
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

string Consumer::splitProfilePrivacy(std::string profile_privacy)  {
  string bew_privacy = "";//仅判断basic,education和workplace三种隐私
  string::size_type indexB = profile_privacy.find("b");
  string::size_type indexE = profile_privacy.find("e");
  string::size_type indexW= profile_privacy.find("w");
  string::size_type indexWait;
  bew_privacy += "{";
  if(indexB != string::npos)  {
    indexWait = profile_privacy.find(",",indexB);
    if(indexWait == string::npos)  {
      indexWait =profile_privacy.find("}",indexB);
    }
    bew_privacy += profile_privacy.substr(indexB, indexWait - indexB);
    bew_privacy += ",";
  }
  if(indexE != string::npos)  {
    indexWait = profile_privacy.find(",", indexE);
    if(indexWait == string::npos)  {
      indexWait =profile_privacy.find("}",indexB);
    }
    bew_privacy += profile_privacy.substr(indexE, indexWait - indexE);
    bew_privacy += ",";
  }
  if(indexW != string::npos)  {
    indexWait = profile_privacy.find(",", indexW);
    if(indexWait == string::npos)  {
      indexWait =profile_privacy.find("}",indexB);
    }
    bew_privacy += profile_privacy.substr(indexW, indexWait - indexW);
  }
  bew_privacy += "}";
  return bew_privacy;
}


void Consumer::transformData(xce::searchcache::SearchCacheDataPtr original, xce::searchcache::dboperation::SearchCacheMemDataPtr destination)  {
  destination->set_name(original->_name);
  destination->set_gender(original->_gender);
  destination->set_status(original->_status);
  destination->set_stage(original->_stage);
  destination->set_state(original->_state);
  destination->set_tinyurl(original->_tinyurl);
  destination->set_headurl(original->_headurl);
  destination->set_browseconfig(original->_browse_config);
  destination->set_statusconfig(original->_status_config);
  destination->set_basicconfig(original->_basic_config);
  destination->set_profileprivacy(splitProfilePrivacy(original->_profile_privacy));
  destination->set_homecity(original->_home_city); 
  destination->set_homeprovince(original->_home_province);
  destination->set_astrology(original->_astrology);
  destination->set_birthyear(original->_birthday_year);
  destination->set_birthmonth(original->_birthday_month);
  destination->set_birthday(original->_birthday_day);
  
  
  for(int i = 0; i < original->_networks.size(); i++)  {
    NetWorkInfo * network = destination->add_networks();
    network->set_id(original->_networks.at(i).id); 
    network->set_name(original->_networks.at(i).name); 
    network->set_stage(original->_networks.at(i).stage); 
  }
  for(int i = 0; i < original->_elementarySchools.size(); i++)  {
    ElementarySchoolInfo * elementaryschool = destination->add_elementaryschools();
    elementaryschool->set_id(original->_elementarySchools.at(i).id);
    elementaryschool->set_year(original->_elementarySchools.at(i).year);
    elementaryschool->set_name(original->_elementarySchools.at(i).name);
  }

  for(int i = 0; i < original->_juniorSchools.size(); i++)  {
    JuniorSchoolInfo * juniorschool = destination->add_juniorschools();
    juniorschool->set_id(original->_juniorSchools.at(i).id);
    juniorschool->set_year(original->_juniorSchools.at(i).year);
    juniorschool->set_name(original->_juniorSchools.at(i).name);
  }

  for(int i = 0; i < original->_highSchools.size(); i++)  {
    HighSchoolInfo * highschool = destination->add_highschools();
    highschool->set_id(original->_highSchools.at(i).id);
    highschool->set_year(original->_highSchools.at(i).year);
    highschool->set_name(original->_highSchools.at(i).name);
    highschool->set_class1(original->_highSchools.at(i).class1);
    highschool->set_class2(original->_highSchools.at(i).class2);
    highschool->set_class3(original->_highSchools.at(i).class3);
  }

  for(int i = 0; i < original->_colleges.size(); i++)  {
    CollegeInfo * collegeschool = destination->add_collegeschools();
    collegeschool->set_id(original->_colleges.at(i).id);
    collegeschool->set_year(original->_colleges.at(i).year);
    collegeschool->set_name(original->_colleges.at(i).name);
    collegeschool->set_department(original->_colleges.at(i).department);
  }

  for(int i = 0; i < original->_universities.size(); i++)  {
    UniversityInfo * universityschool = destination->add_universityschools();
    universityschool->set_id(original->_universities.at(i).id);
    universityschool->set_year(original->_universities.at(i).year);
    universityschool->set_name(original->_universities.at(i).name);
    universityschool->set_department(original->_universities.at(i).department);
    universityschool->set_dorm(original->_universities.at(i).dorm);
  }

  for(int i = 0; i < original->_workplaces.size(); i++)  {
    WorkplaceInfo * workplace = destination->add_workplaces();
    workplace->set_id(original->_workplaces.at(i).id);
    workplace->set_name(original->_workplaces.at(i).name);
  }

  for(int i = 0; i < original->_regions.size(); i++)  {
    RegionInfo * region = destination->add_regions();
    region->set_id(original->_regions.at(i).id);
    region->set_city(original->_regions.at(i).city);
    region->set_province(original->_regions.at(i).province);
  }
  return;

}

void Consumer::transform(xce::searchcache::dboperation::SearchCacheMemDataPtr original, SearchCacheData& destination) {
  destination._name = original->name();
  destination._gender = original->gender();
  destination._status = original->status();
  destination._stage = original->stage();
  destination._state = original->state();
  destination._tinyurl = original->tinyurl();
  destination._headurl = original->headurl();
  destination._browse_config = original->browseconfig();
  destination._status_config = original->statusconfig();
  destination._basic_config = original->basicconfig();
  destination._profile_privacy = original->profileprivacy();
  for(int i = 0; i < original->networks_size(); i++)  {
     Network network; 
     network.id = original->networks(i).id();
     network.stage = original->networks(i).stage();
     network.name = original->networks(i).name();
     destination._networks.push_back(network);
  } 
 for(int i = 0; i < original->elementaryschools_size(); i++)  {
     ElementarySchool elementaryschool; 
     elementaryschool.id = original->elementaryschools(i).id();
     elementaryschool.year = original->elementaryschools(i).year();
     elementaryschool.name = original->elementaryschools(i).name();
     destination._elementarySchools.push_back(elementaryschool);  
 }

 for(int i = 0; i < original->juniorschools_size(); i++)  {
     JuniorSchool juniorschool; 
     juniorschool.id = original->juniorschools(i).id();
     juniorschool.year = original->juniorschools(i).year();
     juniorschool.name = original->juniorschools(i).name();
     destination._juniorSchools.push_back(juniorschool);  
 }

 for(int i = 0; i < original->highschools_size(); i++)  {
     HighSchool highschool; 
     highschool.id = original->highschools(i).id();
     highschool.year = original->highschools(i).year();
     highschool.name = original->highschools(i).name();
     highschool.class1 = original->highschools(i).class1();
     highschool.class2 = original->highschools(i).class2();
     highschool.class3 = original->highschools(i).class3();
     destination._highSchools.push_back(highschool);  
 
 }

 for(int i = 0; i < original->collegeschools_size(); i++)  {
     College collegeschool; 
     collegeschool.id = original->collegeschools(i).id();
     collegeschool.year = original->collegeschools(i).year();
     collegeschool.name = original->collegeschools(i).name();
     collegeschool.department = original->collegeschools(i).department();
     destination._colleges.push_back(collegeschool); 
 }

 for(int i = 0; i < original->universityschools_size(); i++)  {
     University universityschool; 
     universityschool.id = original->universityschools(i).id();
     universityschool.year = original->universityschools(i).year();
     universityschool.name = original->universityschools(i).name();
     universityschool.department = original->universityschools(i).department();
     universityschool.dorm = original->universityschools(i).dorm();
     destination._universities.push_back(universityschool); 
 }

for(int i = 0; i < original->workplaces_size(); i++)  {
     Workplace workplace; 
     workplace.id = original->workplaces(i).id();
     workplace.name = original->workplaces(i).name();
     destination._workplaces.push_back(workplace); 
}

for(int i = 0; i < original->regions_size(); i++)  {
     Region region; 
     region.id = original->regions(i).id();
     region.city = original->regions(i).city();
     region.province = original->regions(i).province();
     destination._regions.push_back(region); 
 }

} 



void printLogger(bool isTT,  const string& notes)  {
   if(isTT)  {
     MCE_DEBUG(notes.c_str()<<" in TT!");
   }  else  {
     MCE_INFO(notes.c_str()<<" in SearchCache!");
   } 
}


bool Consumer::CompareSearchCacheData(SearchCacheData& tt_data, SearchCacheData& mysql_data, bool isTT) {
  char id_char[15];
  itoa(mysql_data._id, id_char);
  string id_string = (string)id_char; 
  if(tt_data._id != mysql_data._id) {
    printLogger(isTT,"ID= " + id_string + " id不一致");
    goto not_equal;
  }
  if(0 != tt_data._name.compare(mysql_data._name)) {
    printLogger(isTT,"ID= " + id_string + " name不一致 tt_name = " + tt_data._name + " mysql_name = " + mysql_data._name);
    goto not_equal;
  }
  if(0 != tt_data._gender.compare(mysql_data._gender)) {
    printLogger(isTT,"ID= " + id_string + " 性别不一致");
    goto not_equal;
  }
  if(tt_data._status != mysql_data._status) {
    printLogger(isTT,"ID= " + id_string + " status不一致");
    goto not_equal;
  }
  if(tt_data._state != mysql_data._state) {
    printLogger(isTT,"ID= " + id_string + " state不一致");
    goto not_equal;
  }
  if(tt_data._stage != mysql_data._stage) {
    printLogger(isTT,"ID= " + id_string + " stage不一致");
    goto not_equal;
  }
  if(0 != tt_data._tinyurl.compare(mysql_data._tinyurl)){
    printLogger(isTT,"ID= " + id_string + " tinyurl不一致");
    goto not_equal;
  }
  if(0 != tt_data._headurl.compare(mysql_data._headurl)){
    printLogger(isTT,"ID= " + id_string + " headurl不一致");
    goto not_equal;
  }
  if(tt_data._browse_config != mysql_data._browse_config){
    printLogger(isTT,"ID= " + id_string + " browse_config不一致");
    goto not_equal;
  }
  if(tt_data._status_config != mysql_data._status_config){
    printLogger(isTT,"ID= " + id_string + " status_config不一致");
    goto not_equal;
  }
  if(isTT)  {
    if(0 != tt_data._home_city.compare(mysql_data._home_city)){
      printLogger(isTT,"ID= " + id_string + " home_city不一致");
      goto not_equal;
    }
    if(tt_data._home_province.compare(mysql_data._home_province)){
      printLogger(isTT,"ID= " + id_string + " home_province不一致");
      goto not_equal;
    }
    if(tt_data._star != mysql_data._star){
      printLogger(isTT,"ID= " + id_string + " star不一致");
      goto not_equal;
    }
    if(tt_data._profile_privacy != mysql_data._profile_privacy){
      printLogger(isTT,"ID= " + id_string + " profile_privacy不一致");
      goto not_equal;
    }
  }  else  {  //SearchCache中仅存三种隐私
    if(tt_data._profile_privacy != splitProfilePrivacy(mysql_data._profile_privacy)){
      printLogger(isTT,"ID= " + id_string + " profile_privacy不一致");
      goto not_equal;
    }
  }
  if (tt_data._networks.size() != mysql_data._networks.size()){
    printLogger(isTT,"ID= " + id_string + " networks.size不一致");
    goto not_equal;
  }

  for (size_t i=0; i< tt_data._networks.size(); ++i) { 
     if(tt_data._networks[i].id != mysql_data._networks[i].id){
       printLogger(isTT,"ID= " + id_string + " networks.id不一致");
       goto not_equal;
     }
     if(tt_data._networks[i].stage != mysql_data._networks[i].stage){
       printLogger(isTT,"ID= " + id_string + " networks.stage不一致");
       goto not_equal;
     }
     if(0 != tt_data._networks[i].name.compare(mysql_data._networks[i].name)){
       printLogger(isTT,"ID= " + id_string + " networks.name不一致");
       goto not_equal;
     }
  } 

  if (tt_data._regions.size() != mysql_data._regions.size()) {
    printLogger(isTT,"ID= " + id_string + " regions.size不一致");
    goto not_equal;
  }
  for (size_t i=0; i< tt_data._regions.size(); ++i) { 
     if(tt_data._regions[i].id != mysql_data._regions[i].id){
       printLogger(isTT,"ID= " + id_string + " regions.id不一致");
     }
     if(0 != tt_data._regions[i].city.compare(mysql_data._regions[i].city)){
       printLogger(isTT,"ID= " + id_string + " regions.city不一致");
       goto not_equal;
     }
     if(0 != tt_data._regions[i].province.compare(mysql_data._regions[i].province)){
       printLogger(isTT,"ID= " + id_string + " regions.province不一致");
       goto not_equal;
     }
  } 

  if (tt_data._workplaces.size() != mysql_data._workplaces.size()){
    printLogger(isTT,"ID= " + id_string + " workplaces.size不一致");
       goto not_equal;
  }
  for (size_t i=0; i<tt_data._workplaces.size(); ++i) { 
     if(tt_data._workplaces[i].id != mysql_data._workplaces[i].id){
       printLogger(isTT,"ID= " + id_string + " workplaces.id不一致");
       goto not_equal;
     }
     if(0 != tt_data._workplaces[i].name.compare(mysql_data._workplaces[i].name)){
       printLogger(isTT,"ID= " + id_string + " workplaces.name不一致");
       goto not_equal;
     }
  } 

  if (tt_data._universities.size() != mysql_data._universities.size()){
    printLogger(isTT,"ID= " + id_string + " universities.size不一致");
       goto not_equal;
  }
  for (size_t i=0; i<tt_data._universities.size(); ++i) { 
     if(tt_data._universities[i].id != mysql_data._universities[i].id){
       printLogger(isTT,"ID= " + id_string + " universities.id不一致");
       goto not_equal;
     }
     if(tt_data._universities[i].year != mysql_data._universities[i].year){
       printLogger(isTT,"ID= " + id_string + " universities.year不一致");
       goto not_equal;
     }
     if(0 != tt_data._universities[i].name.compare(mysql_data._universities[i].name)){
       printLogger(isTT,"ID= " + id_string + " universities.name不一致");
       goto not_equal;
     }
     if(0 != tt_data._universities[i].department.compare(mysql_data._universities[i].department)){
       printLogger(isTT,"ID= " + id_string + " universities.department不一致");
       goto not_equal;
     }
     if(0 != tt_data._universities[i].dorm.compare(mysql_data._universities[i].dorm)){
       printLogger(isTT,"ID= " + id_string + " universities.dorm不一致");
       goto not_equal;
     }
  } 
  //MCE_INFO("&&&&&&&&&&&&&&&&&&&&&数据完全一致" << mysql_data._id);
  return true;

not_equal:
  //ShowCompareSearchCacheData(tt_data, mysql_data);
  MCE_DEBUG("&&&&&&&&&&&&&&&&&&&&&数据不一致" << mysql_data._id);
  if(isTT) UpdateDataInTT(mysql_data._id);
  else UpdateDataInSearchCache(mysql_data._id);
  return false;
}

void Consumer::ShowCompareSearchCacheData(SearchCacheData& tt_data, SearchCacheData& mysql_data) {
  MCE_DEBUG("********************************:");
  MCE_DEBUG("\t##################tt_data:");
  ShowSearchCacheData(tt_data);
  MCE_DEBUG("\t##################mysql_data:");
  ShowSearchCacheData(mysql_data);
}

void Consumer::ShowSearchCacheData(SearchCacheData& data) {
    const std::string SEARCH_CACHE_TYPE_NETWORK = "network";
    const std::string SEARCH_CACHE_TYPE_JUNIOR_SCHOOL = "junior_school";
    const std::string SEARCH_CACHE_TYPE_HIGH_SCHOOL = "high_school";
    const std::string SEARCH_CACHE_TYPE_ELEMENTARY_SCHOOL = "elementary_school";
    const std::string SEARCH_CACHE_TYPE_COLLEGE = "college";
    const std::string SEARCH_CACHE_TYPE_UNIVERSITY = "university";
    const std::string SEARCH_CACHE_TYPE_WORKPLACE = "workplace";
    const std::string SEARCH_CACHE_TYPE_REGION = "region";

    const std::string SEARCH_CACHE_ID = "user_basic.id";
    const std::string SEARCH_CACHE_NAME = "name";
    const std::string SEARCH_CACHE_SIGNATURE = "signature";
    const std::string SEARCH_CACHE_STAR = "star";
    const std::string SEARCH_CACHE_STATE= "state";
    const std::string SEARCH_CACHE_SELECTED = "selected";
    const std::string SEARCH_CACHE_GENDER = "gender";
    const std::string SEARCH_CACHE_BIRTHDAY_YEAR = "birth_year";
    const std::string SEARCH_CACHE_STATUS = "status";
    const std::string SEARCH_CACHE_TINYURL = "tinyurl";
    const std::string SEARCH_CACHE_HEADURL = "headurl";
    const std::string SEARCH_CACHE_BROWSE_CONFIG = "browse_config";
    const std::string SEARCH_CACHE_STATUS_CONFIG = "status_config";
    const std::string SEARCH_CACHE_BASIC_CONFIG  = "basic_config";
    const std::string SEARCH_CACHE_VIEW_COUNT  = "viewcount";
    const std::string SEARCH_CACHE_HOME_CITY = "home_city";
    const std::string SEARCH_CACHE_HOME_PROVINCE = "home_province";
    const std::string SEARCH_CACHE_NETWORK_ID = "network_id";
    const std::string SEARCH_CACHE_NETWORK_NAME = "network_name";
    const std::string SEARCH_CACHE_NETWORK_STAGE = "network_history.stage";
    const std::string SEARCH_CACHE_ELEMENTARY_SCHOOL_ID = "elementary_school_info.elementary_school_id";
    const std::string SEARCH_CACHE_ELEMENTARY_SCHOOL_YEAR = "elementary_school_info.elementary_school_year";
    const std::string SEARCH_CACHE_ELEMENTARY_SCHOOL_NAME = "elementary_school_info.elementary_school_name";
    const std::string SEARCH_CACHE_JUNIOR_SCHOOL_ID = "junior_high_school_info.junior_high_school_id";
    const std::string SEARCH_CACHE_JUNIOR_SCHOOL_NAME = "junior_high_school_info.junior_high_school_name";
    const std::string SEARCH_CACHE_JUNIOR_SCHOOL_YEAR = "junior_high_school_info.junior_high_school_year";
    const std::string SEARCH_CACHE_HIGH_SCHOOL_YEAR = "enroll_year";
    const std::string SEARCH_CACHE_HIGH_SCHOOL_NAME = "high_school_name";
    const std::string SEARCH_CACHE_HIGH_SCHOOL_ID = "high_school_id";
    const std::string SEARCH_CACHE_COLLEGE_ID = "college_info.college_id";
    const std::string SEARCH_CACHE_COLLEGE_NAME ="college_info.college_name";
    const std::string SEARCH_CACHE_COLLEGE_DEPARTMENT = "college_info.department";
    const std::string SEARCH_CACHE_COLLEGE_YEAR = "college_info.enroll_year";
    const std::string SEARCH_CACHE_UNIVERSITY_ID = "university_id";
    const std::string SEARCH_CACHE_UNIVERSITY_NAME = "university_name";
    const std::string SEARCH_CACHE_UNIVERSITY_YEAR = "enroll_year";
    const std::string SEARCH_CACHE_UNIVERSITY_DEPARTMENT = "department";
    const std::string SEARCH_CACHE_WORKPLACE_ID = "workplace_id";
    const std::string SEARCH_CACHE_WORKPLACE_NAME = "workplace_name";
    const std::string SEARCH_CACHE_REGION_ID = "region_id";
    const std::string SEARCH_CACHE_REGION_PROVINCE = "province_name";
    const std::string SEARCH_CACHE_REGION_CITY = "city_name";

    try{
        MCE_DEBUG(" " << SEARCH_CACHE_ID   << " : " << data._id);
        MCE_DEBUG(" "<< SEARCH_CACHE_STATUS << " : " << data._status);
        MCE_DEBUG(" "<< SEARCH_CACHE_NAME << " : " << data._name );
        MCE_DEBUG(" "<< SEARCH_CACHE_SIGNATURE << " : " << data._signature);
        MCE_DEBUG(" "<< SEARCH_CACHE_HEADURL << " : " << data._headurl );
        MCE_DEBUG(" "<< SEARCH_CACHE_TINYURL << " : " << data._tinyurl);
        MCE_DEBUG(" "<< SEARCH_CACHE_STAR << " : " << data._star);
        MCE_DEBUG(" "<< SEARCH_CACHE_HOME_CITY   << " : " << data._home_city);
        MCE_DEBUG(" "<< SEARCH_CACHE_HOME_PROVINCE << " : " << data._home_province );
        MCE_DEBUG(" "<< SEARCH_CACHE_STATUS_CONFIG << " : " << data._status_config );
        MCE_DEBUG(" "<< SEARCH_CACHE_BROWSE_CONFIG << " : " << data._browse_config);
        MCE_DEBUG(" "<< "birthday_year" << " : " << data._birthday_year);
        MCE_DEBUG(" "<< "birthday_month" << " : " << data._birthday_month);
        MCE_DEBUG(" "<< "birthday_day" << " : " << data._birthday_day);
        MCE_DEBUG(" "<< "state" << " : " << data._state);
        MCE_DEBUG(" "<< "stage" << " : " << data._stage);
        MCE_DEBUG(" "<< "lastlogintime" << " : " << data._lastlogintime);
        MCE_DEBUG(" "<< "astrology" << " : " << data._astrology);
        MCE_DEBUG(" "<< "basic_config" << " : " << data._basic_config);
        MCE_DEBUG(" "<< "gender" << " : " << data._gender);
        MCE_DEBUG(" "<< "interest" << " : " << data._interest);
        MCE_DEBUG(" "<< "movie" << " : " << data._movie);
        MCE_DEBUG(" "<< "sport" << " : " << data._sport);
        MCE_DEBUG(" "<< "music" << " : " << data._music);
        MCE_DEBUG(" "<< "game" << " : " << data._game);
        MCE_DEBUG(" "<< "comic" << " : " << data._comic);
        MCE_DEBUG(" "<< "book" << " : " << data._book);
        MCE_DEBUG(" "<< "society" << " : " << data._society);
        MCE_DEBUG(" "<< "viewcount" << " : " << data._viewcount);


        MCE_DEBUG(" "<< "Network" );
        for (size_t i = 0; i<data._networks.size(); ++i) {
            MCE_DEBUG(" network.id :"<< "\t" << data._networks[i].id);
            MCE_DEBUG(" network.stage:"<< "\t" << data._networks[i].stage);
            MCE_DEBUG(" network.name:"<< "\t" << data._networks[i].name);
        }

        MCE_DEBUG(" "<< "Elementary School" );

        for (size_t i=0; i<data._elementarySchools.size(); ++i) {
            MCE_DEBUG(" "<< "\t" << data._elementarySchools[i].id);
            MCE_DEBUG(" "<< "\t" << data._elementarySchools[i].year);
            MCE_DEBUG(" "<< "\t" << data._elementarySchools[i].name);
        }
     
        MCE_DEBUG(" "<< "Junior School" );
        for (size_t i = 0; i<data._juniorSchools.size(); ++i) {
            MCE_DEBUG(" "<< "\t" << data._juniorSchools[i].id);
            MCE_DEBUG(" "<< "\t" << data._juniorSchools[i].year);
            MCE_DEBUG(" "<< "\t" << data._juniorSchools[i].name);
        }

        MCE_DEBUG(" "<<"High School" );
        
        for (size_t i = 0; i<data._highSchools.size(); ++i) {
            MCE_DEBUG(" "<< "\t" << data._highSchools[i].id);
            MCE_DEBUG(" "<< "\t" << data._highSchools[i].year);
            MCE_DEBUG(" "<< "\t" << data._highSchools[i].name);
            MCE_DEBUG(" "<< "\t" << data._highSchools[i].class1);
            MCE_DEBUG(" "<< "\t" << data._highSchools[i].class2);
            MCE_DEBUG(" "<< "\t" << data._highSchools[i].class3);
        }


        MCE_DEBUG(" "<<"Colege" );
               for (size_t i = 0; i<data._colleges.size(); ++i) {
            MCE_DEBUG(" "<< "\t" << data._colleges[i].id);
            MCE_DEBUG(" "<< "\t" << data._colleges[i].year);
            MCE_DEBUG(" "<< "\t" << data._colleges[i].department);
        }


        MCE_DEBUG(" "<<"University" );
              for (size_t i = 0; i<data._universities.size(); ++i) {
            MCE_DEBUG(" "<< "\t" << data._universities[i].id);
            MCE_DEBUG(" "<< "\t" << data._universities[i].year);
            MCE_DEBUG(" "<< "\t" << data._universities[i].name);
            MCE_DEBUG(" "<< "\t" << data._universities[i].department);
            MCE_DEBUG(" "<< "\t" << data._universities[i].dorm);
        }

        MCE_DEBUG(" "<<"Workplace" );
             for (size_t i = 0; i<data._workplaces.size(); ++i) {
            MCE_DEBUG(" "<< "\t" << data._workplaces[i].id);
            MCE_DEBUG(" "<< "\t" << data._workplaces[i].name);
        }

        MCE_DEBUG(" "<<"Region" );
        for (size_t i = 0; i<data._regions.size(); ++i) {
            MCE_DEBUG(" "<< "\t" << data._regions[i].id);
            MCE_DEBUG(" "<< "\t" << data._regions[i].city);
            MCE_DEBUG(" "<< "\t" << data._regions[i].province);
        }

    } catch (const Ice::Exception& e) {
        MCE_DEBUG(" "<<e );
    } catch (const char* msg) {
        MCE_DEBUG(" "<<msg );
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


