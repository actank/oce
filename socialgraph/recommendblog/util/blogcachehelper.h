/*
  Date:   2011-12-05
  Author: junwu.xiong
  Email:  junwu.xiong@renren-inc.com  
  Function: build page recommendation information to cache  
*/

#ifndef __BLOG_CACHE_HELPER_H_
#define __BLOG_CACHE_HELPER_H_

#include "socialgraph/recommendblog/util/blogitem.h"
#include "socialgraph/recommendblog/util/userblog.h"
#include "item.pb.h"
#include "socialgraph/socialgraphutil/mc_adapter.h"
#include "socialgraph/socialgraphutil/dbutil.h"
#include "socialgraph/socialgraphutil/clogging.h"
#include "socialgraph/socialgraphutil/timecost.h"

namespace xce {
namespace socialgraph {
class BlogCacheHelper {
 public: 
  static void setRcdBlogInforToCache(UserBlog user_blog);
//static void getRcdPageInforFromCache(user_id, page_items);
};

};
};

#endif
