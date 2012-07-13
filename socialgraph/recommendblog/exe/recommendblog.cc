/*
  Funtion:      Process the user's recommended blog information 
  Author:       XiongJunWu
  Email:        junwu.xiong@renren-inc.com
  Date:         2011-10-25
  2011-11-17:   Add original blog id to database; and move out process of fetching original blog information from for circle 
*/

/******************************************************************************
1 fetch orignal blog information from databases
2 build blog information that will be recommended for each user
******************************************************************************/

#include "socialgraph/recommendblog/util/blogdbhelper.h"
#include "socialgraph/recommendblog/util/blogcachehelper.h"
#include "socialgraph/socialgraphutil/clogging.h"
#include <iostream>

using namespace std;
using namespace com::xiaonei::xce;
using namespace xce::socialgraph;


int main(int argc, char *argv[])
{
  InitLogger("recommendblog");
  int user_number = 100;
  CLOG(INFO) << "connect to the database pool manager";
  ConnectionPoolManager::instance().initialize();

  CLOG(INFO) << "get original blog information from hot share db";
  int blog_number = 30;
  BlogVec blog_items;
  BlogDbHelper::getBlogInforFromDb(blog_items, blog_number);
  //check the results
  /*
  for(BlogVec::iterator it = blog_items.begin(); it != blog_items.end(); ++it) {
    std::cout << "blog_id: " << (*it).blog_id  << std::endl
              << "share_user_id: " << (*it).share_user_id << std::endl
              << "title: " << (*it).title << std::endl
              << "url: " << (*it).url << std::endl 
              << "summary: " << (*it).summary << std::endl
              << "url_md5: " << (*it).url_md5 << std::endl
              << "share_user_name: " << (*it).share_user_name << std::endl
              << "share_count: " << (*it).share_count << std::endl;   
  }
  */
  for (int user_id = 0; user_id < user_number; ++user_id) {     
    //get user blog feature data initialization
    int stage = 0;
    int type = 0;   
    UserBlog user_blog(user_id, stage, type, blog_items);
    //set user blog data to cache
    BlogCacheHelper::setRcdBlogInforToCache(user_blog);
    //set user blog data to new recommended database
    BlogDbHelper::setRcdBlogInforToDb(user_blog);
    /*
    //check user new recommend blog data
    BlogVec rcd_blog_items;
    UserBlog user_rcd_blog(user_id, -1, -1, rcd_blog_items);
    BlogDbHelper::getRcdBlogInforFromDb(user_rcd_blog);
    std::cout << "user_id: " << user_rcd_blog.getUserId() << endl
              << "stage: " << user_rcd_blog.getStage() << endl
              << "type: " << user_rcd_blog.getType() << endl;  
    rcd_blog_items=user_rcd_blog.getBlogVec();
    for(BlogVec::iterator it = rcd_blog_items.begin(); it != rcd_blog_items.end(); ++it) {
      std::cout << "blog_id: " << (*it).blog_id  << std::endl
                << "share_id: " << (*it).share_id << std::endl
                << "share_user_id: " << (*it).share_user_id << std::endl
                << "title: " << (*it).title << std::endl
                << "url: " << (*it).url << std::endl 
                << "summary: " << (*it).summary << std::endl
                << "url_md5: " << (*it).url_md5 << std::endl
                << "share_user_name: " << (*it).share_user_name << std::endl
                << "share_count: " << (*it).share_count << std::endl;   
    }
   */
  }
}
