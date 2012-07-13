#include "socialgraph/recommendblog/util/blogdbhelper.h"
#include "socialgraph/socialgraphutil/clogging.h"
#include <boost/lexical_cast.hpp>
//#include "util/cpp/IntSeqSerialize.h"

using namespace std;
using namespace com::xiaonei::xce;
using namespace xce::socialgraph;

//get original blog data sources 
void BlogDbHelper::getBlogInforFromDb(BlogVec& blog_items, int blog_number)
{ 
  CLOG(INFO) << "query shared blog information from hot_share table in share_info_new database instance";
  BlogVec tmp_blog_items;
  tmp_blog_items.reserve(blog_number);
  blog_items.reserve(blog_number);
  Statement sql_blog_infor;
  std::string limit_number( boost::lexical_cast<std::string>(blog_number) );
  const char* kQuery_Blog_Infor = "SELECT share_id, user_id, title, url, summary, url_md5, user_name, thumb_url \
                                    FROM hot_share WHERE type = 1 AND stage = 0 limit ";
  sql_blog_infor << std::string(kQuery_Blog_Infor + limit_number);
  QueryBlogDbHandler blog_db_handler(tmp_blog_items);
  QueryRunner("share_info_new", CDbRServer).query(sql_blog_infor, blog_db_handler);
  
  CLOG(INFO) << "filter useless share blog urls and  query share blog count from url_0~url_99 tables in share_new_0 ~ share_new_9 database instance";
  int share_table_suffix;
  std::string share_table;
  std::string share_pattern;
  int url_table_suffix;
  std::string url_table;
  std::string url_pattern;
  for(BlogVec::iterator it = tmp_blog_items.begin(); it != tmp_blog_items.end(); ++it) {
    //filter useless share blog urls; if the url is invalid, set the share_id to 0
    //std::cout << "share_id" << (*it).share_id << std::endl;
    share_table_suffix = (*it).share_user_id % 100;
    share_table = "share_" + boost::lexical_cast<std::string>(share_table_suffix);
    share_pattern = "share_new_" + boost::lexical_cast<std::string>(share_table_suffix);
    Statement sql_blog_share;
    sql_blog_share << "SELECT id FROM " << share_table << " WHERE id = " << (*it).share_id;
    mysqlpp::StoreQueryResult result_valid = QueryRunner("share_new", CDbRServer, share_pattern).store(sql_blog_share);
    if(result_valid.num_rows() > 0) {     //if the share blog id is valid
      //get original blog id from url_md5
      Statement sql_blog_count;
      (*it).blog_id = strtoul( (*it).url_md5.substr(0,10).c_str(), NULL, 16 );
      //std::cout << "blog_id" << (*it).blog_id << std::endl;
      //get url share count from share url tables 
      url_table_suffix = strtoul( (*it).url_md5.substr(30,2).c_str(), NULL, 16 ) % 100;
      url_table = "url_" + boost::lexical_cast<std::string>(url_table_suffix);
      url_pattern = "share_new_" + boost::lexical_cast<std::string>(url_table_suffix);
      sql_blog_count << "SELECT share_count FROM " << url_table << " WHERE md5 = " << mysqlpp::quote << (*it).url_md5;
      mysqlpp::StoreQueryResult result_blog_count = QueryRunner("share_new", CDbRServer, url_pattern).store(sql_blog_count);

      if(result_blog_count.num_rows() > 0) {
        mysqlpp::Row row = result_blog_count.at(0);
        (*it).share_count = (int)row["share_count"];
      }

      //store prepared valid recommended blog records 
      blog_items.push_back(*it);
    }  else {
      //CLOG(DEBUG) << "useless share id: " << (*it).share_id;
      (*it).share_id = 0;  //indicate the share blog is not valid
    }
  }
}

//set new recommend blog data to database
void BlogDbHelper::setRcdBlogInforToDb(UserBlog user_blog)
{
  //delete user's old recommended blog information 
  Statement sql_delete_user_blog;
  sql_delete_user_blog << "DELETE FROM recommend_by_blog WHERE user_id = " << user_blog.getUserId();
  QueryRunner("socialgraph_recommend_blog", CDbWServer).execute(sql_delete_user_blog);

  //build user's recommended new blog information and blog feature data
  Statement sql_insert_user_blog;
  const char* kInsert_Rcd_Blog_Item = "INSERT INTO recommend_by_blog (user_id, blog_id, share_id, share_user_id, stage, type, \
                                            title, url, summary, url_md5, share_user_name, share_count) VALUES ";
  sql_insert_user_blog  << std::string(kInsert_Rcd_Blog_Item);
  BlogVec tmp_blog_items = user_blog.getBlogVec();   
  //BlogVec::iterator it = tmp_blog_items.begin();
  for(BlogVec::iterator it = tmp_blog_items.begin(); it != tmp_blog_items.end(); ++it ) {    
    sql_insert_user_blog  << "(" << user_blog.getUserId() << " , ";
    sql_insert_user_blog  << it->blog_id << ", " 
                          << it->share_id << ", "
                          << it->share_user_id << ", "
                          << user_blog.getStage() << ", "
                          << user_blog.getType() << ", " << mysqlpp::quote 
			  << it->title << ", " << mysqlpp::quote 
			  << it->url << ", " << mysqlpp::quote   
			  << it->summary << ", " << mysqlpp::quote 
			  << it->url_md5 << ", " << mysqlpp::quote  
			  << it->share_user_name << ", "
			  << it->share_count;
    sql_insert_user_blog << ")";
    if( (it + 1) != tmp_blog_items.end() ) {
      sql_insert_user_blog << ", ";
    }
  }
  QueryRunner("socialgraph_recommend_blog", CDbWServer).execute(sql_insert_user_blog);
}

//get user blog data from new recommend blog database
void BlogDbHelper::getRcdBlogInforFromDb(UserBlog& user_blog)
{
  //get user blog feature data
  Statement sql_user_blog;
  sql_user_blog << "SELECT * FROM recommend_by_blog WHERE user_id = " << user_blog.getUserId() << " limit 1";
  mysqlpp::StoreQueryResult result_user_blog = QueryRunner("socialgraph_recommend_blog").store(sql_user_blog);
  if(result_user_blog.num_rows() > 0) {
    mysqlpp::Row row = result_user_blog.at(0);
    user_blog.setUserId( (int)row["user_id"] );
    user_blog.setStage( (int)row["stage"] );
    user_blog.setType( (int)row["type"] );
  }
   
  //get user blog data  
  Statement sql_rcd_blog_infor;
  sql_rcd_blog_infor << "SELECT * FROM recommend_by_blog WHERE user_id = " << user_blog.getUserId();
  BlogVec tmp_blog_items;
  QueryRcdBlogDbHandler rcd_blog_db_handler( tmp_blog_items );
  QueryRunner("socialgraph_recommend_blog", CDbRServer).query(sql_rcd_blog_infor, rcd_blog_db_handler);
  user_blog.setBlogVec(tmp_blog_items);
}

bool QueryBlogDbHandler::handle(mysqlpp::Row& row) const
{
  //std::cout << "I am here now " << std::endl;
  Blog blog_item;
  blog_item.share_id = (long)row["share_id"];
  blog_item.share_user_id = (int)row["user_id"];
  blog_item.title = std::string(row["title"]);
  blog_item.url = std::string(row["url"]);
  blog_item.summary = std::string(row["summary"]);
  blog_item.url_md5 = std::string(row["url_md5"]);
  blog_item.share_user_name = std::string(row["user_name"]);
  blog_item.share_count = 0;

  blog_items_.push_back(blog_item); 
  return true;
}

bool QueryRcdBlogDbHandler::handle(mysqlpp::Row& row) const
{
  //std::cout << "I am here now " << std::endl;
  Blog blog_item;
  blog_item.blog_id = (long)row["blog_id"];
  blog_item.share_id = (long)row["share_id"];
  blog_item.share_user_id = (int)row["share_user_id"];
  blog_item.title = std::string(row["title"]);
  blog_item.url = std::string(row["url"]);
  blog_item.summary = std::string(row["summary"]);
  blog_item.url_md5 = std::string(row["url_md5"]);
  blog_item.share_user_name = std::string(row["share_user_name"]);
  blog_item.share_count = (int)row["share_count"];

  blog_items_.push_back(blog_item);
  return true;
}


/*
bool InsertRcdBlogDbHandler::handle(mysqlpp::Row& row) const
{

}
*/
