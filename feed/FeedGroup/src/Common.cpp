#include "Common.h"
#include <sstream>

namespace xce {
namespace feed {

const char * FeedDBInstance = "feed_minigroup";
const int kUserClusterSize = 20;
const int kDBClusterSize = 100;

std::string GetFeedTableName(int id) {
  std::ostringstream os;
  os << "feed_group_" << id % kDBClusterSize;
  return os.str();
}

}
}
