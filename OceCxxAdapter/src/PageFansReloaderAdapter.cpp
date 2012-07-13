#include "PageFansReloaderAdapter.h"
#include "PageFans.h"
 
using namespace xce::buddy;

PageFansReloaderAdapter::PageFansReloaderAdapter() : MyUtil::ReplicatedClusterAdapterI <PageFansReloaderPrx>("ControllerPageFansReloader", 120, 300){
}

void PageFansReloaderAdapter::reload(int id)
{
		try {
				getProxy(0)->reload(id);
		} catch (const Ice::Exception& e) {
				MCE_WARN("[PageFansReloaderAdapter::addFan] Ice::Exception " << e);
		}
		return ;


}


