#include "ClientFactory.h"
#include "CacheClient.h"
#include "CacheManagerClient.h"
#include "ProducerManagerClient.h"
#include "ConfigData.h"
#include <util/cpp/TimeCost.h>

#include "LogWrapper.h"

using namespace com::renren::tripod;

int main() {
  MCE_DEFAULT_INIT("WARN");
  MCE_INFO("interclient main....");
//  ClientFactory::getInstance()->GetRedisCacheClient("fuck zk adress");
  return 0;
}
