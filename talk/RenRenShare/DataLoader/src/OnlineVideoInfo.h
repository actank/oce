#ifndef _ONLINEVIDEOINFO_H_
#define _ONLINEVIDEOINFO_H_

#include <string>
#include <Ice/Ice.h>

struct OnlineVideoInfo {
  Ice::Long id;
  std::string title;
  std::string thumb_url;
  std::string page_url;
  std::string swf_url;
  std::string catagory;
  std::string creation_date;
  std::string release_date;
};

#endif
