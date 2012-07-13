/*
  Date:         2011-10-25
  Author:       XiongJunWu
  Email:        junwu.xiong@renren-inc.com
  Funtion:      It includes user's blog feature class in recommended blog project.
*/

#ifndef __USER_BLOG_H_
#define __USER_BLOG_H_

#include "socialgraph/recommendblog/util/blogitem.h"

namespace xce {
namespace socialgraph {

//The user's blog feature class definition
class UserBlog {
 public:
  UserBlog(int user_id, int stage, int type, BlogVec& blog_items);
  ~UserBlog() {};

  void setUserId(int user_id);
  int getUserId();

  void setStage(int stage);
  int getStage();
  
  void setType(int type);
  int getType();  

  void setBlogVec(const BlogVec& blog_items);
  BlogVec& getBlogVec();
  
 private:
  int user_id_;
  int stage_;    //indicate the user's stage period   
  int type_;     //indicate the user's blog feature
  
 private:
  BlogVec& blog_items_; 
};

//typedef std::vector<UserBlog> UserBlogVec;

}
}
#endif
