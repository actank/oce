#include "Loader.h"

#include <IceUtil/IceUtil.h>
#include <SearchCache.h>
#include <XceCacheAdapter.h>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include "search/SearchCache2/src/Util/SearchMemCacheData.h"
#include <IceUtil/IceUtil.h>
#include "SearchCache.h"

using namespace xce;
using namespace xce::searchcache;

static void usage(const char *cmd)
{
	fprintf(stderr, "%s <load|get_all_status|set_all_false|set_all_true>\r\n", cmd);
}

SearchCacheLoader* SearchCacheLoader::__instance = 0;
IceUtil::Mutex SearchCacheLoader::__mutex;

bool SearchCacheLoader::Initialize() {

	//get properties
	//__cluster = getProperties("cluster");
	
	__vecDSN.resize(4);
	//__vecDSN[0] = TOKYOTYRANT_DSN_PREFIX"10.3.17.70:1981";
	__vecDSN[0] = TOKYOTYRANT_DSN_PREFIX"10.3.21.12:1982";
	__vecDSN[1] = TOKYOTYRANT_DSN_PREFIX"10.3.21.12:1983";
	__vecDSN[2] = TOKYOTYRANT_DSN_PREFIX"10.3.21.16:1982";
	__vecDSN[3] = TOKYOTYRANT_DSN_PREFIX"10.3.21.16:1983";

	__cluster = 4;
	for (int i=0;i<__cluster;i++) {
    IceUtil::ThreadPtr t = new LoadThread(i);
		__vecLoadThread.push_back(t->start(128*1024));
	}

	return true;
}

void SearchCacheLoader::join() {
	for (LoadThreadVec::iterator it = __vecLoadThread.begin();
			it != __vecLoadThread.end(); it++) {
		it->join();
	}
}

int main(int argc, char *argv[])
{
  MyUtil::init_logger("Mce", "SearchCacheLoader.log", "INFO");
	if (argc != 2) {
		usage(argv[0]);
		return 1;
	}

  if (0 == strcmp("set_all_false", argv[1])){
    SearchMemCacheAdapter::instance().SetValid(0, 0);
    SearchMemCacheAdapter::instance().SetValid(1, 0);
    SearchMemCacheAdapter::instance().SetValid(2, 0);
    SearchMemCacheAdapter::instance().SetValid(3, 0);
  } else if (0 == strcmp("set_all_true", argv[1])){
    SearchMemCacheAdapter::instance().SetValid(0, 1);
    SearchMemCacheAdapter::instance().SetValid(1, 1);
    SearchMemCacheAdapter::instance().SetValid(2, 1);
    SearchMemCacheAdapter::instance().SetValid(3, 1);
  } else if (0 == strcmp("get_all_status", argv[1])) {
    std::cout<<"valid status for mod 0 is "<< SearchMemCacheAdapter::instance().IsValid(0)<<std::endl;
    std::cout<<"valid status for mod 1 is "<< SearchMemCacheAdapter::instance().IsValid(1)<<std::endl;
    std::cout<<"valid status for mod 2 is "<< SearchMemCacheAdapter::instance().IsValid(2)<<std::endl;
    std::cout<<"valid status for mod 3 is "<< SearchMemCacheAdapter::instance().IsValid(3)<<std::endl;

  } else if (0 == strcmp("load", argv[1])) {
    if (!SearchCacheLoader::instance().Initialize()) {
      MCE_FATAL("SearchCacheLoader initialize failed");
    }

    SearchCacheLoader::instance().join();
  }

  return 0;
}
