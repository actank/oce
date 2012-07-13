#include "gtest/gtest.h"

#include <vector>
#include <string>

#include "OceCxxAdapter/src/XceLoggerAdapter.h"

namespace xce {
namespace socialgraph {

TEST(XceLogger, SG_RECONNECT_STAT_LOG) {
  using namespace com::xiaonei::service;
  using namespace xce::log;
  
  XceLoggerPtr xce_logger = XceLoggerAdapter::instance().getLogger();
  xce_logger->initialize();

  int user_id = 286162347;

  std::vector<std::string> paths;
  paths.push_back("socialgraph");
  paths.push_back("reconnect");
  paths.push_back("homeReconnect");
 
  std::string info("test_test_test"); 

  while (true) {
    xce_logger->addLog(paths, info);
    sleep(1);
  }
  sleep(20);

}

}}


