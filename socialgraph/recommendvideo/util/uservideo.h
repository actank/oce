/*
  Date:   2011-10-30
  Author: XiongJunWu
  Email:  junwu.xiong@renren-inc.com
  Function: It includes user's video feature class in recommended video project. 
*/

#ifndef __USER_VIDEO_H_
#define __USER_VIDEO_H_

#include "socialgraph/recommendvideo/util/videoitem.h"

namespace xce {
namespace socialgraph {

class UserVideo {
 public:
  UserVideo();
  UserVideo(int user_id, int stage, int type, VideoVec& video_items);
  ~UserVideo() {}

  void setUserId(int user_id);
  int getUserId();

  void setStage(int stage);
  int getStage();

  void setType(int type);
  int getType();

  void setVideoVec(const VideoVec& video_items);
  VideoVec getVideoVec();
   
 private:
  int user_id_;
  int stage_;  //indicate the user's stage period
  int type_;   //indicate the user's video feature
  
 private:
  VideoVec& video_items_;
};

};
};

#endif
