/*
  Date:   2011-12-05
  Author: junwu.xiong
  Email:  junwu.xiong@renren-inc.com  
  Function: build page recommendation information to cache  
*/

#ifndef __VIDEO_CACHE_HELPER_H_
#define __VIDEO_CACHE_HELPER_H_

#include "socialgraph/recommendvideo/util/videoitem.h"
#include "socialgraph/recommendvideo/util/uservideo.h"
#include "item.pb.h"
#include "socialgraph/socialgraphutil/mc_adapter.h"
#include "socialgraph/socialgraphutil/dbutil.h"
#include "socialgraph/socialgraphutil/clogging.h"
#include "socialgraph/socialgraphutil/timecost.h"

namespace xce {
namespace socialgraph {
class VideoCacheHelper {
 public: 
  static void setRcdVideoInforToCache(UserVideo user_video);
//static void getRcdPageInforFromCache(user_id, page_items);
};

};
};

#endif
