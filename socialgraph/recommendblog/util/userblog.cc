/*
  the user's blog feature class implementation
*/

//#include "socialgraph/recommendblog/util/userblog.h"
#include "socialgraph/recommendblog/util/userblog.h"

//using namespace com::xiaonei::xce;
using namespace xce::socialgraph;

UserBlog::UserBlog(int user_id, int stage, int type, BlogVec& blog_items) :
user_id_(user_id), stage_(stage), type_(type), blog_items_(blog_items)
{
}

void UserBlog::setUserId(int user_id)
{
  user_id_ = user_id;
}

int UserBlog::getUserId()
{
  return user_id_;
}

void UserBlog::setStage(int stage)
{
  stage_ = stage;
}

int UserBlog::getStage()
{
  return stage_;
}

void UserBlog::setType(int type)
{
  type_ = type;
}

int UserBlog::getType()
{
  return type_;
}

void UserBlog::setBlogVec(const BlogVec& blog_items)
{
  blog_items_ = blog_items;
}

BlogVec& UserBlog::getBlogVec()
{
  return blog_items_;
}
