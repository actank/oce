#ifndef _DISTCACHE_MONITOR_ADAPTER_H_
#define _DISTCACHE_MONITOR_ADAPTER_H_

#include <IceUtil/RWRecMutex.h>
#include <DistCacheMonitor.h>
#include <AdapterI.h>
#include <TaskManager.h>

namespace xce{
namespace distcache{
namespace monitor{

class DistCacheMonitorAdapter : public MyUtil::ReplicatedClusterAdapterI<DistCacheMonitorPrx>, public MyUtil::Singleton<DistCacheMonitorAdapter> {
public:
	DistCacheMonitorAdapter();

	void reloadConfig();
};

}
}
}
#endif 
