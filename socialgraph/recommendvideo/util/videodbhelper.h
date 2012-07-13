/*
  Date:     2011-10-31
  Author:   XiongJunWu
  Email:    junwu.xiong@renren-inc.com
  Function: It includes video database process classes in recommended video project.
*/

#ifndef __VIDEO_DB_HELPER_H_
#define __VIDEO_DB_HELPER_H_

#include "socialgraph/recommendvideo/util/videoitem.h"
#include "socialgraph/recommendvideo/util/uservideo.h"
#include "DbCxxPool/src/QueryRunner.h"
#include <iostream>

namespace xce {
namespace socialgraph{

//video database process class definition
class VideoDbHelper {
 public:
  static void getVideoInforFromDb(VideoVec& video_items, int video_number);
  static void setRcdVideoInforToDb(UserVideo user_video);
  static void getRcdVideoInforFromDb(UserVideo& user_video);
};

class QueryVideoDbHandler : public com::xiaonei::xce::ResultHandler {
 public:
  QueryVideoDbHandler(VideoVec& video_items) : video_items_(video_items) {}
  virtual ~QueryVideoDbHandler() {}
  virtual bool handle(mysqlpp::Row& row) const;

 private:
  VideoVec& video_items_;
};

class QueryRcdVideoDbHandler : public com::xiaonei::xce::ResultHandler {
 public:
  QueryRcdVideoDbHandler(VideoVec& video_items) : video_items_(video_items) {}
  //QueryRcVideoDbHandler(UserVideoVec& user_videos) : user_videos_(user_videos) {}
  virtual ~QueryRcdVideoDbHandler() {}
  virtual bool handle(mysqlpp::Row& row) const;

 private:
  VideoVec& video_items_;
  //UserVideoVec& user_videos_;
};

};
};
#endif
