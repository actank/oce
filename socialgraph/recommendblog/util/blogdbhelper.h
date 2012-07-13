/*
  Date:   	2011-10-25
  Author: 	XiongJunWu
  Email:  	junwu.xiong@renren-inc.com
  Funtion:	It includes blog database process classes in recommended blog project.
*/

#ifndef __BLOG_DB_HELPER_H_
#define __BLOG_DB_HELPER_H_

#include "socialgraph/recommendblog/util/blogitem.h"
#include "socialgraph/recommendblog/util/userblog.h"
#include "DbCxxPool/src/QueryRunner.h"
#include <iostream>
#include <string>


namespace xce {
namespace socialgraph {

//blog db process class definitions
class BlogDbHelper {
 public:
  static void getBlogInforFromDb(BlogVec& blog_items, int blog_number);
  static void setRcdBlogInforToDb(UserBlog user_blog);
  static void getRcdBlogInforFromDb(UserBlog& user_blog);
};


class QueryBlogDbHandler : public com::xiaonei::xce::ResultHandler {
 public: 
  QueryBlogDbHandler(BlogVec& blog_items) : blog_items_(blog_items) { } //std::cout << "In handler initialization" << std::endl;
  virtual ~QueryBlogDbHandler() {}
  virtual bool handle(mysqlpp::Row& row) const;

 private:
  BlogVec& blog_items_;    
};

class QueryRcdBlogDbHandler : public com::xiaonei::xce::ResultHandler {
 public:
  QueryRcdBlogDbHandler(BlogVec& blog_items) : blog_items_(blog_items) {}
  //QueryRcBlogDbHandler(UserBlogVec& user_blogs) : user_blogs_(user_blogs) {}
  virtual ~QueryRcdBlogDbHandler() {}
  virtual bool handle(mysqlpp::Row& row) const;
 
 private:
  BlogVec& blog_items_;    
  //UserBlogVec& user_blogs_;
};
/*
class InsertRcdBlogDbHandler : public com::xiaonei::xce::ResultHandler {
 public:
  InsertRcdBlogDbHandler(UserBlog& user_blog) : user_blog_(user_blog) {}  
  virtual ~InsertRcdBlogDbHandler() {}
  virtual bool handle(mysqlpp::Row& row) const;

 private:
  UserBlog& user_blog_;
};
*/
};
};
#endif
