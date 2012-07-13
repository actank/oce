/*
  Date:         2011-10-25
  Author:       XiongJunWu
  Email:        junwu.xiong@renren-inc.com
  Funtion:      It includes blog infomation structure in recommended blog project.
*/

#ifndef __BLOG_ITEM_H_
#define __BLOG_ITEM_H_

#include <string>
#include <vector>

namespace xce {
namespace socialgraph {

//Recommended blog information items structure
typedef struct Blog {
  long blog_id;
  unsigned long share_id;
  unsigned int share_user_id;
  std::string title;
  std::string url;
  std::string summary;
  std::string url_md5;
  std::string share_user_name;
  int share_count;
}Blog;

typedef std::vector<Blog> BlogVec;
};
};

#endif
