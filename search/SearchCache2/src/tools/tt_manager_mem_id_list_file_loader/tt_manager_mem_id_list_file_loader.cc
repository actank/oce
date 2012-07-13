#include "tt_manager_mem_id_list_file_loader.h"
#include <SearchCache.h>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
//#include <glog/logging.h>

using namespace xce;
using namespace xce::searchcache;

static void usage(const char *cmd)
{
	fprintf(stderr, "%s <load>\r\n", cmd);
}

TTManagerMemIdListFileLoader* TTManagerMemIdListFileLoader::__instance = 0;
IceUtil::Mutex TTManagerMemIdListFileLoader::__mutex;

bool TTManagerMemIdListFileLoader::Initialize() {

	__vecDSN.resize(4);
	__vecDSN[0] = TOKYOTYRANT_DSN_PREFIX"10.3.21.193:1980";
	__vecDSN[1] = TOKYOTYRANT_DSN_PREFIX"10.3.21.193:1981";
	__vecDSN[2] = TOKYOTYRANT_DSN_PREFIX"10.3.21.194:1982";
	__vecDSN[3] = TOKYOTYRANT_DSN_PREFIX"10.3.21.194:1983";

	__cluster = 4;
	for (int i=0;i<__cluster;i++) {
    IceUtil::ThreadPtr t = new LoadThread(i);
		__vecLoadThread.push_back(t->start(128*1024));
	}


	return true;
}

void TTManagerMemIdListFileLoader::join() {
	for (LoadThreadVec::iterator it = __vecLoadThread.begin();
			it != __vecLoadThread.end(); it++) {
		it->join();
	}
}

int main(int argc, char *argv[])
{
  MyUtil::init_logger("Mce", "TTManagerMemIdListFileLoader.log", "DEBUG");
	if (argc != 2) {
		usage(argv[0]);
		return 1;
	}

  //google::InstallFailureSignalHandler();

  if (0 == strcmp("load", argv[1])) {
    if (!TTManagerMemIdListFileLoader::instance().Initialize()) {
      MCE_FATAL("TTManagerMemIdListFileLoader initialize failed");
    }
    TTManagerMemIdListFileLoader::instance().join();

    TTManagerMemIdListFileLoader::instance().get_bit_vector_().Save("./user_id_list_file.bin");
    cout << "TTManagerMemIdListFileLoader::instance().get_bit_vector_().Save()" << endl;
  }

  return 0;
}
