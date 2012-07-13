//the user's video feature implementation

#include "socialgraph/recommendvideo/util/uservideo.h"

using namespace xce::socialgraph;

UserVideo::UserVideo(int user_id, int stage, int type, VideoVec& video_items) :
user_id_(user_id), stage_(stage), type_(type), video_items_(video_items)
{
}

void UserVideo::setUserId(int user_id)
{
  user_id_ = user_id;
}

int UserVideo::getUserId()
{
  return user_id_;
}

void UserVideo::setStage(int stage)
{
  stage_ = stage;
}

int UserVideo::getStage()
{
  return stage_;
}

void UserVideo::setType(int type)
{
  type_ = type;
}

int UserVideo::getType()
{
  return type_;
}

void UserVideo::setVideoVec(const VideoVec& video_items)
{
  video_items_ = video_items; 
}

VideoVec UserVideo::getVideoVec()
{
  return video_items_;  
}
