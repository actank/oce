#ifndef CELL_FEED_HEADURL_H__
#define CELL_FEED_HEADURL_H__

#include <string>

namespace xce {

struct HeadUrl {
  static std::string FromPart(const std::string & pattern);
};

}
#endif // CELL_FEED_HEADURL_H__
