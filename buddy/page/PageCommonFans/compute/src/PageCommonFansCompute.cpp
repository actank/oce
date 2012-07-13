#include "PageCommonFansCacheAdapter.h"

#include "computetool.h"
#include "multicompute.h"
#include "base/threadpool.h"

int main(int argc, char **argv) {

  if (argc < 2) {
    std::cout << "Usaga: ./PageCommonFansCompute ids.txt" << std::endl;
		exit(1);
	}
  
  /*
  MyUtil::IntSeq common;
	try {
		common = xce::buddy::adapter::PageCommonFansCacheAdapter::instance().GetCommon(299331400, 600098863, 0, -1);
  } catch (Ice::Exception& e) {
		LOG(LERROR) << e.what();
	} catch (...) {
		LOG(LERROR) << "...exeption";
	}
  LOG(INFO) << "common.size():" << common.size();
  for (size_t i = 0; i < common.size(); ++i) {
		LOG(INFO) << "i:" << i << "\t" << common[i];
	}
	return 0;
  */
 	std::vector<int> ids;
	ReadFileGetId(argv[1], ids);
	xce::ThreadPool threadPool(8, 20);
	std::vector<xce::buddy::CMessage*> cmess;
	for (size_t i = 0; i < ids.size(); ++i) {
    int id = ids[i];
	    xce::buddy::CMessage* msg = new xce::buddy::CMessage(id);
 	    cmess.push_back(msg);
			if (i % 1000 == 0) {
				LOG(INFO) << "total:" << ids.size() << "\tcount:" << i;
			}
  }
  threadPool.Post(cmess.begin(), cmess.end());
  sleep(3); 
  threadPool.Stop();
  threadPool.Join();

	return 0;
}




