/*
  Date:   2011-12-05
  Author: junwu.xiong
  Email:  junwu.xiong@renren-inc.com  
  Function: build page recommendation information to cache  
*/

#ifndef _PAGE_CACHE_HELPER_H_
#define _PAGE_CACHE_HELPER_H_

#include "socialgraph/recommendpage/util/pageitem.h"
#include "socialgraph/recommendpage/util/userpage.h"
#include "item.pb.h"
#include "socialgraph/socialgraphutil/mc_adapter.h"
#include "socialgraph/socialgraphutil/dbutil.h"
#include "socialgraph/socialgraphutil/clogging.h"
#include "socialgraph/socialgraphutil/timecost.h"

namespace xce {
namespace socialgraph {

class PageCacheHelper {
 public: 
  static void setRcdPageInforToCache(int user_id, const PageVec& page_items);
//static void getRcdPageInforFromCache(user_id, page_items);
};

};
};

#endif
