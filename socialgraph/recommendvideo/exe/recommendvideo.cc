/*
  Funtion:      Process the user's recommended video information 
  Author:       XiongJunWu
  Email:        junwu.xiong@renren-inc.com
  Date:         2011-10-31
  2011-11-17:   Add original video id to database; and move out process of fetching original video information from for circle 
*/

#include "socialgraph/recommendvideo/util/videodbhelper.h"
#include "socialgraph/recommendvideo/util/videocachehelper.h"
#include "socialgraph/socialgraphutil/clogging.h"
#include <iostream>

using namespace std;
using namespace com::xiaonei::xce;
using namespace xce::socialgraph;

/******************************************************************************
1 fetch orignal video information from databases
2 build video information that will be recommended for each user
******************************************************************************/

int main(int argc, char *argv[])
{

  InitLogger("recommendvideo");
  int user_number = 100;

  //connect to the database pool manager
  ConnectionPoolManager::instance().initialize();

  CLOG(INFO) << "get original video information from hot share db";
  int video_number = 30;
  VideoVec video_items;
  //video_items.reserve(video_number);
  VideoDbHelper::getVideoInforFromDb(video_items, video_number);  
  //check the video url source information
  /* 
  for(VideoVec::iterator it = video_items.begin(); it != video_items.end(); ++it) {
    std::cout << "video_id: " << (*it).video_id  << std::endl
              << "share_user_id: " << (*it).share_user_id << std::endl
              << "title: " << (*it).title << std::endl
              << "url: " << (*it).url << std::endl 
              << "summary: " << (*it).summary << std::endl
              << "url_md5: " << (*it).url_md5 << std::endl
              << "share_user_name: " << (*it).share_user_name << std::endl
              << "thumb_url: " << (*it).thumb_url << std::endl
              << "share_count: " << (*it).share_count << std::endl;   
  }
  */
  CLOG(INFO) << "set user video data to new recommended database";
  for (int user_id = 0; user_id < user_number; ++user_id) {     
    //user video feature data initialization
    int stage = 0;
    int type = 0;   
    UserVideo user_video(user_id, stage, type, video_items);
    //set user video data to cache
    VideoCacheHelper::setRcdVideoInforToCache(user_video);
    //set user video data to new recommended database
    VideoDbHelper::setRcdVideoInforToDb(user_video);
    /*
    //check user new recommend video data
    VideoVec rcd_video_items;
    UserVideo user_rcd_video(user_id, -1, -1, rcd_video_items);
    VideoDbHelper::getRcdVideoInforFromDb(user_rcd_video);
    std::cout << "user_id: " << user_rcd_video.getUserId() << endl
              << "stage: " << user_rcd_video.getStage() << endl
              << "type: " << user_rcd_video.getType() << endl;  
    rcd_video_items=user_rcd_video.getVideoVec();
    for(VideoVec::iterator it = rcd_video_items.begin(); it != rcd_video_items.end(); ++it) {
      std::cout << "video_id: " << (*it).video_id  << std::endl
                << "share_id: " << (*it).share_id << std::endl
                << "share_user_id: " << (*it).share_user_id << std::endl
                << "title: " << (*it).title << std::endl
                << "url: " << (*it).url << std::endl 
                << "summary: " << (*it).summary << std::endl
                << "url_md5: " << (*it).url_md5 << std::endl
                << "share_user_name: " << (*it).share_user_name << std::endl
                << "share_count: " << (*it).share_count << std::endl;   
    }
   */
  }
}
