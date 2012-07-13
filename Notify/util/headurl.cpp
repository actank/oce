#include "headurl.h"

//#include "base/hashmap.h"

//#include <boost/thread/mutex.hpp>

//#include "base/common.h"

//#include "dbpool/queryrunner.h"
namespace xce {
namespace notify {


std::string GetHeadUrl(const std::string & pattern) {
  // 针对合并后的开心
  bool kaixin = false;
  std::string pattern1(pattern);
  if (0 == pattern1.find("kx/")) {
    pattern1 = pattern1.substr(3);
    kaixin = true;		
  }

  if (pattern1.empty())
    return std::string();

  //std::string url;
  std::string url = HeadStorage::instance().Locate(pattern1, HeadStorage::NODE_TYPE_HEAD);

  MCE_DEBUG("head url : " << url); 

  if (url.empty())	{
    static const char * URL_HEAD = "http://head.xiaonei.com";
    url  = URL_HEAD;

    if (pattern1.find("head.jpg") != std::string::npos)
      url += "/photos/0/0/men_head.gif";
    else if (pattern1.find("main.jpg") != std::string::npos)
      url += "/photos/0/0/men_main.gif";
    else if (pattern1.find("tiny.jpg") != std::string::npos)
      url += "/photos/0/0/men_tiny.gif";
    else
      url += "/photos/" + pattern1;
    // url = URL_HEAD + "/photos/0/0/men_tiny.gif";
  }

  // kaixin
  if (kaixin) {
    // 针对kaixin的，需要把域名改为 rrimg.com
    size_t pos = url.find("xnimg.cn");
    if (pos != std::string::npos) {
      std::string tmp = url.substr(0, pos);
      tmp += "rrimg.com";
      tmp += url.substr(pos + 8);
      url = tmp;
      // url.insert(pos, "rrimg.com");
    }
  }
  return url;
}

}
}
