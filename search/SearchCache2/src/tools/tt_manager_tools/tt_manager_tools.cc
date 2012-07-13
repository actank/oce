//#include <gtest/gtest.h>
#include <mysql/mysql.h>
#include <iostream>
#include "OceCxxAdapter/src/TTManagerAdapter.h"
#include "search/SearchCache2/src/Util/SearchCacheData.h"
#include "search/SearchCache2/src/Util/IDbCache.h"
#include "search/SearchCache2/src/Util/DatabaseCacheFactory.h"

using std::cout;
using std::endl;
using namespace xce::searchcache;
//typedef std::map<int, SearchCacheDataPtr> UserId2SearchCacheDataPtrMap;

void Usage(const char* cmd) {
  fprintf(stderr, "%s <unit_test | show_user_info_in_tt user_id | insert_user_info_into_tt user_id | remove_user_info_from_tt user_id\n\t| show_mem_id_num_in_tt_manager | get_mem_id_state_in_tt_manager user_id | traversal start_pos | traversal_key start_pos| traversal_all\n\t assess_the_integrity_of_user_information_for_special_user_id userid | \n\tassess_the_integrity_of_user_information_for_user_id_list_and_store_in_sql id_list_file date(\"2010-04-24\")>\n", cmd);
}

void ShowSearchCacheData(SearchCacheData& s) {
  cout <<"id=" << s._id << endl; 
  cout << "name=" << s._name << endl; 
  cout << "gender=" << s._gender<< endl; 
  cout << "signature=" << s._signature << endl; 
  cout << "view_count=" << s._viewcount<< endl; 
  cout << "star=" << s._star << endl; 
  cout << "status=" << s._status << endl; 
  cout << "stage=" << s._stage<< endl; 
  cout << "birthday_year=" << s._birthday_year<< endl; 
  cout << "birthday_month=" << s._birthday_month<< endl; 
  cout << "birthday_day=" << s._birthday_day<< endl; 
  cout << "tinyurl=" << s._tinyurl << endl; 
  cout << "headurl=" << s._headurl << endl; 
  cout << "astrology=" << s._astrology << endl; 
  cout << "browse_config=" << s._browse_config << endl; 
  cout << "status_config=" << s._status_config << endl; 
  cout << "home_city=" << s._home_city << endl; 
  cout << "home_province=" << s._home_province << endl; 
  cout << "intrest=" << s._interest<< endl; 
  cout << "music=" << s._music<< endl; 
  cout << "movie=" << s._movie<< endl; 
  cout << "game=" << s._game<< endl; 
  cout << "comic=" << s._comic<< endl; 
  cout << "sport=" << s._sport<< endl; 
  cout << "book=" << s._book<< endl; 
  cout << "society=" << s._society<< endl; 
  cout << "lastlogintime=" << s._lastlogintime << endl; 
  cout << "networks:" << endl; 
  for (unsigned int i=0; i< s._networks.size(); ++i) { 
    cout << "\ttid=" << s._networks[i].id << endl; 
    cout << "\ttstage=" << s._networks[i].stage << endl; 
    cout << "\ttname=" << s._networks[i].name<< endl; 
  } 
  cout << "regions" << endl; 
  for (unsigned i=0; i< s._regions.size(); ++i) { 
    cout << "\ttid=" << s._regions[i].id << endl; 
    cout << "\ttcity=" << s._regions[i].city<< endl; 
    cout << "\ttprovince=" << s._regions[i].province<< endl; 
  } 
  cout << "workplace" << endl; 
  for (unsigned i=0; i< s._workplaces.size(); ++i) { 
    cout << "\ttid=" << s._workplaces[i].id << endl; 
    cout << "\ttname=" << s._workplaces[i].name<< endl; 
  } 
  cout << "university" << endl; 
  for (unsigned i=0; i< s._universities.size(); ++i) { 
    cout << "\ttid=" << s._universities[i].id << endl; 
    cout << "\ttyear=" << s._universities[i].year << endl; 
    cout << "\ttname=" << s._universities[i].name<< endl; 
    cout << "\ttdepartment=" << s._universities[i].department << endl; 
    cout << "\ttdorm=" << s._universities[i].dorm<< endl; 
  } 
  cout << "college" << endl; 
  for (unsigned i=0; i< s._colleges.size(); ++i) { 
    cout << "\ttid=" << s._colleges[i].id << endl; 
    cout << "\ttyear=" << s._colleges[i].year << endl; 
    cout << "\ttname=" << s._colleges[i].name<< endl; 
    cout << "\ttdepartment=" << s._colleges[i].department << endl; 
  } 
  cout << "highschool" << endl; 
  for (unsigned i=0; i< s._highSchools.size(); ++i) { 
    cout << "\ttid=" << s._highSchools[i].id << endl; 
    cout << "\ttyear=" << s._highSchools[i].year << endl; 
    cout << "\ttname=" << s._highSchools[i].name<< endl; 
    cout << "\ttclass1=" << s._highSchools[i].class1<< endl; 
    cout << "\ttclass2=" << s._highSchools[i].class2<< endl; 
    cout << "\ttclass3=" << s._highSchools[i].class3<< endl; 
  } 
  cout << "juniorschool" << endl; 
  for (unsigned i=0; i< s._juniorSchools.size(); ++i) { 
    cout << "\ttid=" << s._juniorSchools[i].id << endl; 
    cout << "\ttyear=" << s._juniorSchools[i].year << endl; 
    cout << "\ttname=" << s._juniorSchools[i].name<< endl; 
  } 
  cout << "elmentaryschool" << endl; 
  for (unsigned i=0; i< s._elementarySchools.size(); ++i) { 
    cout << "\ttid=" << s._elementarySchools[i].id << endl; 
    cout << "\ttyear=" << s._elementarySchools[i].year << endl; 
    cout << "\ttname=" << s._elementarySchools[i].name<< endl; 
  }
}

void ShowUserInfoInTT(int user_id) {
  cout << "user_id is " << user_id << endl;

  SearchCacheData s;
  if (TTManagerAdapter::instance().Query(user_id, &s)) {
    ShowSearchCacheData(s);
  } else {
    cout << "Query " << user_id << " Failed!" << endl;
  }
}

/// @brief AssessUserInfoIntegrity 评估指定用户的资料完整度, 用户资料主要参考以下几项: 
///                                network, region, workplace, university, college, high school, 
///                                junior high school, elementary school, home province(city), 
///                                计算完整度得分的方法是：以上9项，哪一项有内容，则分数相应加一，最低分为0分，最高分为9分.
/// 
/// @param user_id 
/// 
/// @return 返回用户信息的完整度得分， -1表示在TT中查不到该用户. 
int AssessUserInfoIntegrity(int user_id) {
  /// @brief 用户信息的完整度，最低为0分, 目前最高分为9分
  int integrity = 0;
  SearchCacheData s;

  if (TTManagerAdapter::instance().Query(user_id, &s)) {
    //ShowSearchCacheData(s);

    if (!s._networks.empty()) {
      MCE_INFO(user_id << " has networks");
      ++integrity;
    }

    if (!s._regions.empty()) {
      MCE_INFO(user_id << " has regions");
      ++integrity;
    }

    if (!s._workplaces.empty()) {
      MCE_INFO(user_id << " has workplaces" );
      ++integrity;
    }

    if (!s._universities.empty()) {
      MCE_INFO(user_id << " has universities" );
      ++integrity;
    }

  if (!s._colleges.empty()) {
      MCE_INFO(user_id << " has colleges" );
      ++integrity;
    }

  if (!s._juniorSchools.empty()) {
      MCE_INFO(user_id << " has juniorschools" );
      ++integrity;
    }

  if (!s._highSchools.empty()) {
      MCE_INFO(user_id << " has highschools" );
      ++integrity;
    }

  if (!s._elementarySchools.empty()) {
      MCE_INFO(user_id << " has elementarySchools" );
      ++integrity;
    }

  if (0 != s._home_city.compare("") || 0 != s._home_province.compare("")) {
      MCE_INFO(user_id << " has home info" );
      ++integrity;
    }

  } else {
    cout << "Query " << user_id << " Failed!" << endl;
    integrity = -1;
  }

  return integrity;

}

void AssessUserInfoIntegrityForIdList(string& id_list_file, string& date_string) {

  ifstream input_file(id_list_file.c_str());
  string line;
  std::map<int, int> integrity_2_user_id_count; //资料完整度 -> 用户id数

  int user_id;
  int integrity;

  while (getline(input_file, line)) {
    stringstream ss(line);
    ss >> user_id;
    integrity = AssessUserInfoIntegrity(user_id);
    //当integrity=-1时，确认数据库里的status是否真正大于等于三，即是否真的是无效用户
    if (-1 == integrity) {
      MYSQL my_connection;
      mysql_init(&my_connection);

      if (mysql_real_connect(&my_connection, "user47.db.d.xiaonei.com", "xndev", "rebornlOM", "fb", 0, NULL, 0)) {
        boost::shared_ptr<char> query_string(new char[1024]);
        sprintf(query_string.get(), "SELECT status from user_passport where id=%d", user_id);
        int res = mysql_query(&my_connection, query_string.get());

        if (0 == res) {
          MYSQL_RES* res_ptr = mysql_use_result(&my_connection);
          if (res_ptr) {
              MYSQL_ROW row = mysql_fetch_row(res_ptr);
              if (NULL != row) {
                stringstream ss(row[0]);
                int status; 
                ss >> status;
                MCE_INFO("user_id status=" << status);
                if (status < 3) {
                  MCE_INFO("Error! integrity=" << integrity <<", user_id status=" << status );
                } else {
                  if (integrity_2_user_id_count.end() != integrity_2_user_id_count.find(integrity)) {
                    ++integrity_2_user_id_count[integrity];
                  } else {
                    integrity_2_user_id_count[integrity] = 1;
                  }

                }

              }
          }
        } else {
          MCE_WARN("ERROR while mysql_query, sql=" << query_string.get());
        }

      }

      mysql_close(&my_connection);

    } else {
      if (integrity_2_user_id_count.end() != integrity_2_user_id_count.find(integrity)) {
        ++integrity_2_user_id_count[integrity];
      } else {
        integrity_2_user_id_count[integrity] = 1;
      }
    }
  }

  //初始化mysql相关
  MYSQL my_connection;
  mysql_init(&my_connection);

  if (mysql_real_connect(&my_connection, "127.0.0.1", "root", "", "search_engine_log", 0, NULL, 0)) {
    boost::shared_ptr<char> query_string(new char[1024]);

    for (std::map<int, int>::const_iterator iter=integrity_2_user_id_count.begin(); 
        iter!=integrity_2_user_id_count.end(); ++iter) {
      MCE_INFO("integrity=" << iter->first << ", count=" << iter->second);
      cout << "integrity=" << iter->first << ", count=" << iter->second << endl;

      sprintf(query_string.get(), "INSERT INTO active_search_user_info_integrity(date,integrity_score,count) VALUES ('%s', %d, %d)", date_string.c_str(), iter->first, iter->second);
      int res = mysql_query(&my_connection, query_string.get());
      if (0 == res) {
      } else {
        MCE_WARN("ERROR while mysql_query, sql=" << query_string.get());
      }
    }

  } else {
    MCE_WARN("ERROR! mysql_real_connent " << mysql_error(&my_connection));
  }

  mysql_close(&my_connection);


  
}

void RemoveFromTT(int user_id) {
  cout << "user_id is " << user_id << endl;

  TTManagerAdapter::instance().Remove(user_id);
}

void TestTraversal(int start_pos) {
  UserId2SearchCacheDataPtrMap id_2_search_cache_data_ptr_map;
  int travel_length = 0;
  TTManagerAdapter::instance().Traversal(start_pos, travel_length, id_2_search_cache_data_ptr_map);
  cout << "travel_length is " << travel_length << endl;
  for (UserId2SearchCacheDataPtrMap::const_iterator iter=id_2_search_cache_data_ptr_map.begin();
      iter!=id_2_search_cache_data_ptr_map.end(); ++iter) {
    ShowSearchCacheData(*(iter->second.get()));
  }
}

void TestTraversalKey(int start_pos) {
  IntSeq valid_user_ids;
  int travel_length = 1000;
  valid_user_ids = TTManagerAdapter::instance().TraversalKey(start_pos, travel_length);
  cout << "travel_length is " << travel_length << endl;
  stringstream ss;
  for (IntSeq::const_iterator iter=valid_user_ids.begin();
      iter!=valid_user_ids.end(); ++iter) {
    ss << " " << *iter;
  }

  cout << " valid user ids is " << ss.str() << endl;
}

void TestTraversalAll() {
  int start_pos = 0;
  cout << "start_pos=" << start_pos  << endl;
  int travel_length = 0;
  int total_num = TTManagerAdapter::instance().GetMemIdsListSize();
  cout << "total_num=" << total_num << endl;
  int idx = 0;
  int valid_user_id_num = 0;
  while (true) {
    UserId2SearchCacheDataPtrMap id_2_search_cache_data_ptr_map;
    TTManagerAdapter::instance().Traversal(start_pos, travel_length, id_2_search_cache_data_ptr_map);
    valid_user_id_num += id_2_search_cache_data_ptr_map.size();
    cout << "idx=" << idx << ", start_pos=" << start_pos << ", travel_length=" << travel_length << ", result.size()=" << id_2_search_cache_data_ptr_map.size() << ", total_valid_user_id_num=" << valid_user_id_num << endl;
    if (start_pos + (travel_length - 1) == total_num) break;
    start_pos += travel_length;
    ++idx;
  }
}

void TestTraversalAllKey() {
  int start_pos = 0;
  cout << "start_pos=" << start_pos  << endl;
  //请求返回的结果数
  int request_num = 100;
  int total_num = TTManagerAdapter::instance().GetMemIdsListSize();
  cout << "total_num=" << total_num << endl;
  int idx = 0;
  int valid_user_id_num = 0;
  IntSeq user_ids;
  while (true) {
    UserId2SearchCacheDataPtrMap id_2_search_cache_data_ptr_map;
    user_ids = TTManagerAdapter::instance().TraversalKey(start_pos, request_num);
    valid_user_id_num += user_ids.size();
    cout << "idx=" << idx << ", start_pos=" << start_pos << ", request_num =" << request_num << ", result.size()=" << user_ids.size() << ", total_valid_user_id_num=" << valid_user_id_num << endl;
    stringstream ss; 
    for (IntSeq::const_iterator iter=user_ids.begin(); iter!=user_ids.end(); ++iter) {
      ss << " " << *iter;
    }
    cout << ss.str() << endl;
    if (user_ids.size() < request_num) break;
    start_pos = user_ids.at(user_ids.size() - 1) + 1;
    ++idx;
  }
}



void InsertIntoTT(int id) {
  cout << "user_id is " << id << endl;

  boost::shared_ptr<IDbCache> db_cache;
  if (0 == id % 4) {
    db_cache = DatabaseCacheFactory::instance().getDbCache("tokyotryant://10.3.17.70:1981");
  } else if (1 == id % 4 ) {
    db_cache = DatabaseCacheFactory::instance().getDbCache("tokyotryant://10.3.17.71:1981");
  } else if (2 == id % 4 ) {
    db_cache = DatabaseCacheFactory::instance().getDbCache("tokyotryant://10.3.17.72:1981");
  } else if (3 == id % 4 ) {
    db_cache = DatabaseCacheFactory::instance().getDbCache("tokyotryant://10.3.17.73:1981");
  }
  if (db_cache->Initialize() == false) {
    cout<<"Db initialize failed"<<endl;
  } else {
    SearchCacheData old_data;
    if (db_cache->Query(id, &old_data)) {
      TTManagerAdapter::instance().Insert(id, old_data);
    } else {
      cout << "error userid = " << id << endl;
    }
    db_cache->Close();
  }
}


/////void CompareSearchCacheData(SearchCacheData& old_data, SearchCacheData& new_data) {
/////  EXPECT_EQ(old_data._id, new_data._id);
/////  EXPECT_STREQ(old_data._name.c_str(), new_data._name.c_str());
/////  EXPECT_STREQ(old_data._gender.c_str(), new_data._gender.c_str());
/////  EXPECT_STREQ(old_data._signature.c_str(), new_data._signature.c_str());
/////  EXPECT_EQ(old_data._viewcount, new_data._viewcount);
/////  EXPECT_EQ(old_data._star, new_data._star);
/////  EXPECT_EQ(old_data._status, new_data._status);
/////  EXPECT_EQ(old_data._birthday_year, new_data._birthday_year);
/////  EXPECT_EQ(old_data._birthday_month, new_data._birthday_month);
/////  EXPECT_EQ(old_data._birthday_day, new_data._birthday_day);
/////  EXPECT_STREQ(old_data._tinyurl.c_str(), new_data._tinyurl.c_str());
/////  EXPECT_STREQ(old_data._headurl.c_str(), new_data._headurl.c_str());
/////  EXPECT_STREQ(old_data._astrology.c_str(), new_data._astrology.c_str());
/////  EXPECT_EQ(old_data._browse_config, new_data._browse_config);
/////  EXPECT_EQ(old_data._status_config, new_data._status_config);
/////  EXPECT_EQ(old_data._basic_config, new_data._basic_config);
/////  EXPECT_STREQ(old_data._home_city.c_str(), new_data._home_city.c_str());
/////  EXPECT_STREQ(old_data._home_province.c_str(), new_data._home_province.c_str());
/////  EXPECT_STREQ(old_data._interest.c_str(), new_data._interest.c_str());
/////  EXPECT_STREQ(old_data._music.c_str(), new_data._music.c_str());
/////  EXPECT_STREQ(old_data._movie.c_str(), new_data._movie.c_str());
/////  EXPECT_STREQ(old_data._game.c_str(), new_data._game.c_str());
/////  EXPECT_STREQ(old_data._comic.c_str(), new_data._comic.c_str());
/////  EXPECT_STREQ(old_data._sport.c_str(), new_data._sport.c_str());
/////  EXPECT_STREQ(old_data._book.c_str(), new_data._book.c_str());
/////  EXPECT_STREQ(old_data._society.c_str(), new_data._society.c_str());
/////  EXPECT_STREQ(old_data._lastlogintime.c_str(), new_data._lastlogintime.c_str());
/////  ASSERT_EQ(old_data._networks.size(), new_data._networks.size());
/////  for (unsigned int i=0; i< old_data._networks.size(); ++i) { 
/////     EXPECT_EQ(old_data._networks[i].id, new_data._networks[i].id);
/////     EXPECT_EQ(old_data._networks[i].stage, new_data._networks[i].stage);
/////     EXPECT_STREQ(old_data._networks[i].name.c_str(), new_data._networks[i].name.c_str());
/////  } 
/////
/////  ASSERT_EQ(old_data._regions.size(), new_data._regions.size());
/////  for (unsigned i=0; i< old_data._regions.size(); ++i) { 
/////     EXPECT_EQ(old_data._regions[i].id, new_data._regions[i].id);
/////     EXPECT_STREQ(old_data._regions[i].city.c_str(), new_data._regions[i].city.c_str());
/////     EXPECT_STREQ(old_data._regions[i].province.c_str(), new_data._regions[i].province.c_str());
/////  } 
/////
/////  ASSERT_EQ(old_data._workplaces.size(), new_data._workplaces.size());
/////  for (unsigned i=0; i< old_data._workplaces.size(); ++i) { 
/////     EXPECT_EQ(old_data._workplaces[i].id, new_data._workplaces[i].id);
/////     EXPECT_STREQ(old_data._workplaces[i].name.c_str(), new_data._workplaces[i].name.c_str());
/////  } 
/////
/////  ASSERT_EQ(old_data._universities.size(), new_data._universities.size());
/////  for (unsigned i=0; i< old_data._universities.size(); ++i) { 
/////     EXPECT_EQ(old_data._universities[i].id, new_data._universities[i].id);
/////     EXPECT_EQ(old_data._universities[i].year, new_data._universities[i].year);
/////     EXPECT_STREQ(old_data._universities[i].name.c_str(), new_data._universities[i].name.c_str());
/////     EXPECT_STREQ(old_data._universities[i].department.c_str(), new_data._universities[i].department.c_str());
/////     EXPECT_STREQ(old_data._universities[i].dorm.c_str(), new_data._universities[i].dorm.c_str());
/////  } 
/////
/////  //TODO:测中小学
/////}

boost::shared_ptr<IDbCache> GetDbCache(int id) {
  boost::shared_ptr<IDbCache> db_cache;
  if (0 == id % 4) {
    db_cache = DatabaseCacheFactory::instance().getDbCache("tokyotryant://10.3.17.70:1981");
  } else if (1 == id % 4 ) {
    db_cache = DatabaseCacheFactory::instance().getDbCache("tokyotryant://10.3.17.71:1981");
  } else if (2 == id % 4 ) {
    db_cache = DatabaseCacheFactory::instance().getDbCache("tokyotryant://10.3.17.72:1981");
  } else if (3 == id % 4 ) {
    db_cache = DatabaseCacheFactory::instance().getDbCache("tokyotryant://10.3.17.73:1981");
  }

  return db_cache;
}

//////*****//测试查询接口
//////*****TEST(TTManagerTestCase, TestInsertAndQuery) {
//////*****  int id = 58742;
//////*****  boost::shared_ptr<IDbCache> db_cache;
//////*****  if (0 == id % 4) {
//////*****    db_cache = DatabaseCacheFactory::instance().getDbCache("tokyotryant://10.3.17.70:1981");
//////*****  } else if (1 == id % 4 ) {
//////*****    db_cache = DatabaseCacheFactory::instance().getDbCache("tokyotryant://10.3.17.71:1981");
//////*****  } else if (2 == id % 4 ) {
//////*****    db_cache = DatabaseCacheFactory::instance().getDbCache("tokyotryant://10.3.17.72:1981");
//////*****  } else if (3 == id % 4 ) {
//////*****    db_cache = DatabaseCacheFactory::instance().getDbCache("tokyotryant://10.3.17.73:1981");
//////*****  }
//////*****  if (db_cache->Initialize() == false) {
//////*****    cout<<"Db initialize failed"<<endl;
//////*****  } else {
//////*****    SearchCacheData old_data;
//////*****    SearchCacheData new_data;
//////*****    if (db_cache->Query(id, &old_data)) {
//////*****      TTManagerAdapter::instance().Insert(id, old_data);
//////*****      sleep(1);
//////*****      int state = TTManagerAdapter::instance().GetMemIdState(id);
//////*****      EXPECT_TRUE(TTManagerAdapter::instance().Query(id, &new_data));
//////*****      EXPECT_EQ(state, 1);
//////*****      CompareSearchCacheData(old_data, new_data);
//////*****    } else {
//////*****      cout << "error userid = " << id << endl;
//////*****    }
//////*****
//////*****    db_cache->Close();
//////*****  }
//////*****}
//////*****
//////*****
//////*****TEST(TTManagerTestCase, BatchInsert) {
//////*****  int id1 = 58742; //武志刚
//////*****  int id2 = 25701953; //李洁
//////*****
//////*****  vector<int> uids;
//////*****  vector<SearchCacheDataPtr> query_data_ptr_array;
//////*****  uids.push_back(id1);
//////*****  uids.push_back(id2);
//////*****
//////*****
//////*****  SearchCacheDataPtr direct_tt_data_ptr1 = new SearchCacheData; 
//////*****  SearchCacheDataPtr direct_tt_data_ptr2 = new SearchCacheData;
//////*****  SearchCacheData tt_manager_data1, tt_manager_data2;
//////*****
//////*****  //1.从TT中逐个读取两个用户的信息
//////*****  boost::shared_ptr<IDbCache>db_cache1 = GetDbCache(id1);
//////*****  boost::shared_ptr<IDbCache>db_cache2 = GetDbCache(id2);
//////*****
//////*****  if (db_cache1->Initialize() == false) {
//////*****    cout<<"Db initialize failed"<<endl;
//////*****  } else {
//////*****    if (!db_cache1->Query(id1, direct_tt_data_ptr1.get())) {
//////*****      MCE_ERROR("cannot find user id " << id1 << " in tt");
//////*****    }
//////*****  }
//////*****
//////*****  if (db_cache2->Initialize() == false) {
//////*****    cout<<"Db initialize failed"<<endl;
//////*****  } else {
//////*****    if (!db_cache2->Query(id2, direct_tt_data_ptr2.get())) {
//////*****      MCE_ERROR("cannot find user id " << id2 << " in tt");
//////*****    }
//////*****  }
//////*****
//////*****  //2. 删除TT中的两个用户信息
//////*****  TTManagerAdapter::instance().Remove(id1);
//////*****  TTManagerAdapter::instance().Remove(id2);
//////*****  //3. 调用批量插入接口向TT中插入两个用户的信息
//////*****  vector<SearchCacheDataPtr> insert_data_ptr_array;
//////*****  insert_data_ptr_array.push_back(direct_tt_data_ptr1);
//////*****  insert_data_ptr_array.push_back(direct_tt_data_ptr2);
//////*****
//////*****  EXPECT_TRUE(TTManagerAdapter::instance().Insert(insert_data_ptr_array));
//////*****  
//////*****  //3.逐个查询并比较
//////*****  TTManagerAdapter::instance().Query(id1, &tt_manager_data1);
//////*****  CompareSearchCacheData(tt_manager_data1, *direct_tt_data_ptr1.get());
//////*****  TTManagerAdapter::instance().Query(id2, &tt_manager_data2);
//////*****  CompareSearchCacheData(tt_manager_data2, *direct_tt_data_ptr2.get());
//////*****
//////*****  
//////*****  db_cache1->Close();
//////*****  db_cache2->Close();
//////*****
//////*****}
//////*****
//////*****TEST(TTManagerTestCase, BatchQuery) {
//////*****  //测试思路，先直接从TT里读两个用户的信息，然后调用Query查这两个用户的信息，比较之， 
//////*****  //remove 掉， 然后 Insert， 然后 逐个 Query， 比较之
//////*****  int id1 = 58742; //武志刚
//////*****  int id2 = 25701953; //李洁
//////*****
//////*****  vector<int> uids;
//////*****  vector<SearchCacheDataPtr> query_data_ptr_array;
//////*****  uids.push_back(id1);
//////*****  uids.push_back(id2);
//////*****
//////*****
//////*****  SearchCacheDataPtr direct_tt_data_ptr1 = new SearchCacheData; 
//////*****  SearchCacheDataPtr direct_tt_data_ptr2 = new SearchCacheData;
//////*****  SearchCacheData tt_manager_data1, tt_manager_data2;
//////*****
//////*****  //1.从TT中逐个读取两个用户的信息
//////*****  boost::shared_ptr<IDbCache>db_cache1 = GetDbCache(id1);
//////*****  boost::shared_ptr<IDbCache>db_cache2 = GetDbCache(id2);
//////*****
//////*****  if (db_cache1->Initialize() == false) {
//////*****    cout<<"Db initialize failed"<<endl;
//////*****  } else {
//////*****    if (!db_cache1->Query(id1, direct_tt_data_ptr1.get())) {
//////*****      MCE_ERROR("cannot find user id " << id1 << " in tt");
//////*****    }
//////*****  }
//////*****
//////*****  if (db_cache2->Initialize() == false) {
//////*****    cout<<"Db initialize failed"<<endl;
//////*****  } else {
//////*****    if (!db_cache2->Query(id2, direct_tt_data_ptr2.get())) {
//////*****      MCE_ERROR("cannot find user id " << id2 << " in tt");
//////*****    }
//////*****  }
//////*****
//////***** 
//////*****  //2. 调用Query批量查询接口查询这两个用户的信息
//////*****  if (!TTManagerAdapter::instance().Query(uids, &query_data_ptr_array)) {
//////*****    MCE_ERROR("TTManagerAdapter::instance().Query(uids) ERROR!");
//////*****  }
//////*****
//////*****  EXPECT_EQ(query_data_ptr_array.size(), 2);
//////*****  //3. 比较1和2中的结果
//////*****  if (58742 == query_data_ptr_array[0]->_id) {
//////*****    CompareSearchCacheData(*direct_tt_data_ptr1.get(), *query_data_ptr_array[0].get());
//////*****    CompareSearchCacheData(*direct_tt_data_ptr2.get(), *query_data_ptr_array[1].get());
//////*****  } else {
//////*****    CompareSearchCacheData(*direct_tt_data_ptr1.get(), *query_data_ptr_array[1].get());
//////*****    CompareSearchCacheData(*direct_tt_data_ptr2.get(), *query_data_ptr_array[0].get());
//////*****  }
//////*****    
//////*****  db_cache1->Close();
//////*****  db_cache2->Close();
//////*****}
//////*****
//////*****
//////*****
//////*****//测试Update接口, 测试方法：从指定的tt中取出自己的数据(id=58742), 手工修改姓名字段，
//////*****//然后调用update接口存储更改，再调用Query接口比对修改是否生效
//////*****//
//////*****TEST(TTManagerTestCase, TestUpdate) {
//////*****  int id = 58742;
//////*****  boost::shared_ptr<IDbCache> db_cache;
//////*****  if (0 == id % 4) {
//////*****    db_cache = DatabaseCacheFactory::instance().getDbCache("tokyotryant://10.3.17.70:1981");
//////*****  } else if (1 == id % 4 ) {
//////*****    db_cache = DatabaseCacheFactory::instance().getDbCache("tokyotryant://10.3.17.71:1981");
//////*****  } else if (2 == id % 4 ) {
//////*****    db_cache = DatabaseCacheFactory::instance().getDbCache("tokyotryant://10.3.17.72:1981");
//////*****  } else if (3 == id % 4 ) {
//////*****    db_cache = DatabaseCacheFactory::instance().getDbCache("tokyotryant://10.3.17.73:1981");
//////*****  }
//////*****  if (db_cache->Initialize() == false) {
//////*****    cout<<"Db initialize failed"<<endl;
//////*****  } else {
//////*****    SearchCacheData old_data;
//////*****    SearchCacheData new_data;
//////*****    if (db_cache->Query(id, &old_data)) {
//////*****      old_data._name = "wuzhigang0408";
//////*****      TTManagerAdapter::instance().Update(id, old_data);
//////*****      EXPECT_TRUE(TTManagerAdapter::instance().Query(id, &new_data));
//////*****      EXPECT_STREQ(new_data._name.c_str(), "wuzhigang0408");
//////*****    } else {
//////*****      cout << "error userid = " << id << endl;
//////*****    }
//////*****
//////*****    db_cache->Close();
//////*****  }
//////*****}
//////*****
//////*****TEST(TTManagerTestCase, TestRemoveMemIdAndInsertMemId) {
//////*****  int id = 58742;
//////*****  TTManagerAdapter::instance().RemoveMemId(id);
//////*****  sleep(1);
//////*****  EXPECT_EQ(TTManagerAdapter::instance().GetMemIdState(id), 0);
//////*****  TTManagerAdapter::instance().InsertMemId(id);
//////*****  sleep(1);
//////*****  EXPECT_EQ(TTManagerAdapter::instance().GetMemIdState(id), 1);
//////*****  TTManagerAdapter::instance().RemoveMemId(id);
//////*****  sleep(1);
//////*****  EXPECT_EQ(TTManagerAdapter::instance().GetMemIdState(id), 0);
//////*****}
//////*****
//////*****TEST(TTManagerTestCase, TestRemove) {
//////*****  int id = 58742;
//////*****  SearchCacheData new_data;
//////*****  EXPECT_TRUE(TTManagerAdapter::instance().Query(id, &new_data));
//////*****  TTManagerAdapter::instance().Remove(id);
//////*****  EXPECT_EQ(TTManagerAdapter::instance().GetMemIdState(id), 0);
//////*****  EXPECT_FALSE(TTManagerAdapter::instance().Query(id, &new_data));
//////*****}
//测试
int main(int argc, char *argv[]) {
  //初始化MCE_形式的log
  MyUtil::init_logger("Mce", "TT_manager_tools.log", "DEBUG");
  if (1 == argc || 0 == strcmp(argv[1], "--help")) {
    Usage(argv[0]);
    return -1;
  } else if (0 == strcmp(argv[1], "--help")) {

  } /*else if (0 == strcmp(argv[1], "unit_test")) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
  } */else if (0 == strcmp(argv[1], "show_user_info_in_tt")) {
    stringstream ss;
    int user_id;
    ss << argv[2];
    ss >> user_id;
    ShowUserInfoInTT(user_id);
  } else if (0 == strcmp(argv[1], "insert_user_info_into_tt")) {
    stringstream ss;
    int user_id;
    ss << argv[2];
    ss >> user_id;
    InsertIntoTT(user_id);
   } else if (0 == strcmp(argv[1], "remove_user_info_from_tt")) {
    stringstream ss;
    int user_id;
    ss << argv[2];
    ss >> user_id;
    RemoveFromTT(user_id);
  } else if (0 == strcmp(argv[1], "show_mem_id_num_in_tt_manager")) {
    cout << " Id list size in mem is " << TTManagerAdapter::instance().GetMemIdsCount() << endl;  
  } else if (0 == strcmp(argv[1], "get_mem_id_state_in_tt_manager")) {
    stringstream ss;
    int user_id;
    ss << argv[2];
    ss >> user_id;
    cout << " user id " << user_id  << " state in mem is " << TTManagerAdapter::instance().GetMemIdState(user_id) << endl;  
  } else if (0 == strcmp(argv[1], "traversal")) {
    stringstream ss;
    int start_pos;
    ss << argv[2];
    ss >> start_pos;
    cout << " start pos is " << start_pos << endl;  
    TestTraversal(start_pos);
  } else if (0 == strcmp(argv[1], "traversal_key")) {
    stringstream ss;
    int start_pos;
    ss << argv[2];
    ss >> start_pos;
    cout << " start pos is " << start_pos << endl;  
    TestTraversalKey(start_pos);
  } else if (0 == strcmp(argv[1], "traversal_all")) {
    TestTraversalAll();
  } else if (0 == strcmp(argv[1], "traversal_all_key")) {
    TestTraversalAllKey();
  } else if (0 == strcmp(argv[1], "assess_the_integrity_of_user_information_for_user_id_list_and_store_in_sql")) {
    string id_list_file(argv[2]);
    string date_string(argv[3]);

    AssessUserInfoIntegrityForIdList(id_list_file, date_string);
  } else if (0 == strcmp(argv[1], "assess_the_integrity_of_user_information_for_special_user_id")) {
    stringstream ss;
    int user_id;
    ss << argv[2];
    ss >> user_id;
    MCE_INFO("input user_id is " << user_id);
    MCE_INFO("integrity for " << user_id << " is " << AssessUserInfoIntegrity(user_id));
  }



  return 0;
}
