#include "BuddyCache.h"
int main(){
  xce::buddy::tools::BuddyCache::instance().initialize();
  std::cout << xce::buddy::tools::BuddyCache::instance().getCache()->size() << std::endl;
  std::cout << xce::buddy::tools::BuddyCache::instance().getCache()->has(35640);
  return 0;
}
