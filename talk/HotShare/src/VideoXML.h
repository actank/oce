#ifndef _VIDEOXML_H_
#define _VIDEOXML_H_

#include <string>
#include "HotShareInfo.h"

namespace xce {
namespace hotshare {

class VideoXML
{
public:
  static std::string OutputXML (const HotShareInfo &hsInfo, const size_t version);
  static std::string OutputXML (const std::vector<std::string> &xmlVec, const std::vector<Ice::Long> &lowIdVec, const std::vector<Ice::Long> &highIdVec);

private:
  static std::string XMLEncode(const std::string &source);
  static int UTF8Length(const char *s, int _len);
  static bool ValidCheck(const std::string &xml);
  static const std::string GetCurrentTime();
  static std::string CutUtf8(const std::string &src, size_t limitlen);
  static std::string DeleteInvalidChar(const std::string &src);
};

}
}
#endif
