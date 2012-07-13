#include "socialgraph/recommendpage/util/pagedbhelper.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <boost/lexical_cast.hpp>
#include "util/cpp/IntSeqSerialize.h"
#include "socialgraph/socialgraphutil/clogging.h" 

using namespace std;
using namespace com::xiaonei::xce;
using namespace xce::socialgraph;
//using namespace com::xiaonei::xce::socialgraph;

void PageDbHelper::getPageInforFromDb(Page& page_item)
{
  CLOG(INFO) << "QUERY DB: get common page's page_name, fans_counter and description from the database";
  Statement sql_page;
  string page_table("page");
  sql_page << "SELECT name, description, fans_count FROM " << page_table << " WHERE id = " << page_item.page_id;
  mysqlpp::StoreQueryResult result_page = QueryRunner("page", CDbRServer).store(sql_page);
  if (result_page.num_rows() > 0) {
    mysqlpp::Row row = result_page.at(0);
    page_item.page_name = std::string(row["name"]);
    page_item.description = std::string(row["description"]);
    page_item.fans_counter = (int)row["fans_count"];
    
    //std::cout << "page_id = " << page_item.page_id << "," << endl  
    //          << "page_value = " << page_item.page_value << "," << endl
    //          << "page_name = " << page_item.page_name << "," << endl
    //      << "description = " << page_item.description << "," << endl
    //      << "fans_counter = " << page_item.fans_counter << "," << endl; 
   
  } else {
    //std::cout<<"the page is not existed" << std::endl;
  }
}

void PageDbHelper::getPageFansFromDb(Page& page_item)
{
  CLOG(INFO) << "QUERY DB: get page's fans from db";
  int page_id = page_item.page_id;
  std::string page_fans_table_idx( boost::lexical_cast<std::string>(page_id % 100) );
  std::string page_fans_table( "page_fans_" + page_fans_table_idx);
  Statement sql_fans;
  sql_fans << "SELECT user_id FROM " << page_fans_table << " WHERE page_id = " << page_id;
  const unsigned int kfans_ids_num = 100000;  //usual fans number of the page
  page_item.fans_ids.reserve(kfans_ids_num);
  PageFansIdsHandler page_fans_ids_handler(page_item.fans_ids);
  QueryRunner("page_fans", CDbRServer, page_fans_table).query(sql_fans, page_fans_ids_handler);
  //if( page_item.fans_counter == page_item.fans_ids.size() ) {
  //  std::cout << "ok " << std::endl;
  //} else {
  //  std::cout <<page_item.fans_counter << ": " <<page_item.fans_ids.size() << std::endl;
  //}
}

void PageDbHelper::getUserFriendFromDb(int user_id, IntVec& friend_ids)
{
  CLOG(INFO) << "QUERY DB: Get user's friends from database";
  FriendIdsHandler friend_ids_handler(friend_ids);
  std::string relation_table = "relation_" + boost::lexical_cast<std::string>(user_id % 100);
  Statement sql_friend_ids;
  sql_friend_ids << "SELECT guest FROM " << relation_table << " WHERE host = " << user_id;
  QueryRunner("relation_friendrank", CDbRServer, relation_table).query(sql_friend_ids, friend_ids_handler);
  
  /*
  std::string ids_test;
  for(IntVec::iterator idx = friend_ids.begin(); idx != friend_ids.end(); ++idx) {
    std::string friend_idx = boost::lexical_cast<std::string> (*idx);
    ids_test = ids_test + friend_idx;
    if((idx + 1) != friend_ids.end() )
      ids_test = ids_test + ",";
  } 
 */    
  //std::cout << "user_id: " << user_id << "'friends size: " << friend_ids.size() << endl; 
}

void PageDbHelper::getUserPageFromDb(int user_id, IntSet& page_ids)
{
  CLOG(INFO) << "QUERY DB: Get user's already focused pages"; 
  Statement sql_page_ids;
  std::string fans_pages_table = "fans_pages_" + boost::lexical_cast<std::string>(user_id % 100); 
  sql_page_ids << "SELECT page_id FROM " << fans_pages_table << " WHERE user_id = " << user_id;
  PageIdsHandler page_ids_handler(page_ids);
  QueryRunner("fans_pages", CDbRServer, fans_pages_table).query(sql_page_ids, page_ids_handler);   
}

// store the user's recommended page infomation to the dabase 
void PageDbHelper::setRcdPageInforToDb(int user_id, PageVec page_items)
{
  CLOG(INFO) << "WRITE DB: store the user's recommended page infomation to the dabase, user_id:" << user_id;
  //int user_id = userpages_item.getUserId();
  //std::cout << "user_id: " << user_id << std::endl;
  //PageVec page_items=userpages_item.getPages();
  //PageVec page_items.swap(userpages_item.getPages());

  // delete old recommended page data before create the user's recommended page 
  Statement sql_delete_userpage;
  sql_delete_userpage << "DELETE FROM recommend_by_page WHERE user_id = " << user_id;
  QueryRunner("socialgraph_recommend_page", CDbWServer).execute(sql_delete_userpage);

  Statement sql_create_userpage;
  sql_create_userpage << "INSERT INTO recommend_by_page (user_id, page_id, page_value, page_name, description, ";
  sql_create_userpage << "fans_counter, common_friend_number, common_friend_ids) VALUES ";
  for(PageVec::iterator it=page_items.begin(); it != page_items.end(); ++it) {
    sql_create_userpage << "(" << user_id << ", "
               		<< it->page_id << ", "
               		<< it->page_value << ", " << mysqlpp::quote
               		<< it->page_name << ", " << mysqlpp::quote
               		<< it->description << ", "
               		<< it->fans_counter << ", "
               		<< it->common_friend_ids.size() << ", " << mysqlpp::quote
               		<< MyUtil::IntSeqSerialize::serialize(it->common_friend_ids) << ")";
    //sql_create_userpage << " ON DUPLICATE KEY UPDATE "
               		//<< "page_value = VALUES(page_value), description = VALUES(description), "
               		//<< "fans_counter = VALUES(fans_counter), common_friend_number = VALUES(common_friend_number), "
               		//<< "common_friend_ids = VALUES(common_friend_ids)";
    if((it+1) != page_items.end()) {
      sql_create_userpage << ", ";
    }
  }
  /*
  std::ostringstream oss2;
  oss2 << "REPLACE INTO recommend_by_page " << " (user_id, page_id, page_value, page_name, description, "
       << "fans_counter, common_friend_number, common_friend_ids) VALUES "; 
  for(PageVec::iterator it=page_items.begin(); it != page_items.end(); ++it) {
    oss2 << "(" << user_id << ", "
                        << it->page_id << ", "
                        << it->page_value << ", " << mysqlpp::quote
                        << it->page_name << ", " << mysqlpp::quote
                        << it->description << ", "
                        << it->fans_counter << ", "
                        << it->common_friend_ids.size() << ", " << mysqlpp::quote
                        << MyUtil::IntSeqSerialize::serialize(it->common_friend_ids) << ")";
    //sql_create_userpage << " ON DUPLICATE KEY UPDATE "
                        //<< "page_value = VALUES(page_value), description = VALUES(description), "
                        //<< "fans_counter = VALUES(fans_counter), common_friend_number = VALUES(common_friend_number), "
                        //<< "common_friend_ids = VALUES(common_friend_ids)";
    if((it+1) != page_items.end()) {
      oss2 << ", ";
    }
  }
  std::cout << oss2.str() << std::endl; 
  */
  QueryRunner("socialgraph_recommend_page", CDbWServer).execute(sql_create_userpage);
}

// query the user's recommended pages infomation 
void PageDbHelper::getRcdPageInforFromDb(int user_id, PageVec& page_item_vector)
{
  Statement sql_result;
  sql_result << "SELECT * FROM recommend_by_page WHERE user_id = " << user_id;
  RcdPagesHandler pageitem_handler( page_item_vector );
  QueryRunner("socialgraph_recommend_page", CDbRServer).query(sql_result,  pageitem_handler);
}

bool PageFansIdsHandler::handle(mysqlpp::Row& row) const
{
  int user_id = (int)row["user_id"];
  result_.push_back(user_id);
  return true;
}

bool FriendIdsHandler::handle(mysqlpp::Row& row) const
{
  //std::cout << "user_id :" << (int)row["guest"] << std::endl;
  int guest = (int)row["guest"];
  if(guest) {
    result_.push_back(guest);
  } else {
    //std::cout << "there is no data" << std::endl;
  } 
  return true;
}

bool PageIdsHandler::handle(mysqlpp::Row& row) const
{
  //std::cout << "query page_id " << std::endl;
  int page_id = (int)row["page_id"];
  if(page_id) {
    //std::cout << "page_id" << page_id << std::endl;
    result_.insert(page_id);
  } else {
    //std::cout << "there is no data" << std::endl;
  }
  return true;
}

bool RcdPagesHandler::handle(mysqlpp::Row& row) const
{
  //std::cout << "RcdPagesHandler::handle " << std::endl;
  std::string init_str("");
  IntVec init_fans;
  IntVec init_ids;
  //Page page_item = {0, 0.0, init_str, init_str, 0, init_fans, 0, init_ids};
  Page page_item;

  page_item.page_id = (int) row["page_id"];
  page_item.page_value = (double) row["page_value"];
  page_item.page_name = std::string(row["page_name"]);
  page_item.description = std::string( row["description"] );
  page_item.fans_counter = (int) row["fans_counter"];

  int number = (int)row["common_friend_number"];
  page_item.common_friend_number = number;
  init_ids = MyUtil::IntSeqSerialize::unserialize(row["common_friend_ids"]);
  page_item.common_friend_ids = init_ids;
  /* 
  std::cout << "number " << number << std::endl;
  if( !init_ids.empty() ) {
     for(IntVec::iterator idx = init_ids.begin(); idx != init_ids.end(); ++idx) {
       std::cout << *idx << ", ";
     }
  } else {
     std::cout << "no common friends" <<  std::endl;
  }
  
  std::cout << std::endl;
  */
  
  result_.push_back(page_item);
  return true;
}
