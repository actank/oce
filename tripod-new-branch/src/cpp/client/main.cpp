#include <TripodClient.h>
#include "LogWrapper.h"


#include <boost/foreach.hpp>

using namespace com::renren::tripod;

int main() {
  MCE_DEFAULT_INIT("DEBUG");
  MCE_INFO("tripodclient main....");
  TripodClient tripod_client("10.22.206.32:2181/Tripod", "test", "LUCK");

  std::string key1 = "liyong";
  StrList value1;
  tripod_client.GetList(key1, value1, 1, 100);
  MCE_DEBUG("key1:" << key1);
  BOOST_FOREACH(const std::string v, value1) {
    MCE_DEBUG("\t" << v);
  }
  
  std::string key2 = "wuhao";
  IntList value2;
  MCE_DEBUG("key2:" << key2);
  tripod_client.GetIntList(key2, value2, 1, -1);
  BOOST_FOREACH(int v, value2) {
    MCE_DEBUG("\t" << v);
  }

  sleep(3);
  MCE_DEBUG("main done...");

  return 0;
}
