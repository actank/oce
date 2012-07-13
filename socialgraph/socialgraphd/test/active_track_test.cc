#include "gtest/gtest.h"

#include <vector>
#include <string>

#include "boost/asio.hpp"

#include "base/logging.h"
#include "OceCxxAdapter/src/ActiveTrackAdapter.h"

namespace xce {
namespace socialgraph {

TEST(ActiveTrack, AccessLog) {
  using namespace xce::log;
  ActiveTrack::instance().initialize();
  int user_id = 286162347;
  const std::string ip = "10.1.1.1";
  const std::string url = "http://test.renren.com/test";
  const std::string decision = "no_decision";
  for (int i = 0; i < 100; ++i) {
    ActiveTrack::instance().access(user_id, ip, url, decision);
  }
}

}}



