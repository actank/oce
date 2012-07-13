#include "search/tripodsearchcache/TripodSearchUpdateAdapter.h"
#include <boost/lexical_cast.hpp>
#include "DbCxxPool/src/ConnectionPoolManager.h"

using namespace search::tripodupdate;
using namespace com::renren::tripod;


int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cout << "Usage: TestTripodSearchLoad logPath userId1 [userId2, ...]" << std::endl;
    return 1;
  }
  std::cout << "argc:" << argc << std::endl;
  ostringstream oss;
  com::xiaonei::xce::ConnectionPoolManager::instance();
  oss << argv[1] << "/TripodSearchLoad.log";
  std::string logPath = oss.str();
  MyUtil::init_logger("Mce", logPath, "DEBUG");
  for (int i = 2; i < argc; i++) {
    int userId = boost::lexical_cast<int>(argv[i]);
    MCE_INFO("logPath:" << logPath << " userId:" << userId << " i:" << i);
    TripodSearchUpdateAdapter::instance().invoke(userId);
  }
  while(true) {
    sleep(10);
    MCE_INFO("sleep 10s");
  }
  return 0;

}
