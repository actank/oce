#ifndef _HOTSHAREINFO_H_
#define _HOTSHAREINFO_H_

#include <string>
#include <Ice/Ice.h>

struct HotShareInfo {
  Ice::Long id;
  Ice::Long share_id;
  size_t user_id;
  std::string title;
  std::string url;
  std::string summary;
  std::string creation_date;
  std::string release_date;
  std::string user_name;
  std::string thumb_url;
  std::string catagory;
};
 

#endif
