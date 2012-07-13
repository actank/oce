/*
  Date:   2011-10-30
  Author: XiongJunWu
  Email:  junwu.xiong@renren-inc.com
  Function: It includes video information structure in recommended video project. 
*/

#ifndef __VIDEO_ITEM_H_
#define __VIDEO_ITEM_H_

#include <string>
#include <vector>

typedef struct Video {
  long video_id;
  unsigned long share_id;
  unsigned int share_user_id;
  std::string title;
  std::string url;
  std::string summary;
  std::string url_md5;
  std::string share_user_name;
  std::string thumb_url;
  int share_count;
}Video;

typedef std::vector<Video> VideoVec; 
 
#endif
