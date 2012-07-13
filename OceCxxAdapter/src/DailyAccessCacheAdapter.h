#ifndef DAILYACCESS_CACHE_H
#define DAILYACCESS_CACHE_H

#include <DailyAccessCache.h>
#include <AdapterI.h>
#include <bitset>
namespace xce {
namespace dailyaccesscache {
namespace adapter {

const int DAILYACCESS_CACHE_SIZE = 2147483647;

class DailyAccessCacheAdapter :
        public MyUtil::ReplicatedClusterAdapterI<DailyAccessCacheManagerPrx>,
                public MyUtil::Singleton<DailyAccessCacheAdapter> {
public:
	 DailyAccessCacheAdapter() : MyUtil::ReplicatedClusterAdapterI<DailyAccessCacheManagerPrx>("ControllerDailyAccessCache", 120, 3000) {}
	 void access(int UserId);

};

}
}
}
#endif
