#ifndef _DISTCACHE_SWITCHER_ADAPTER_H_
#define _DISTCACHE_SWITCHER_ADAPTER_H_

#include <IceUtil/RWRecMutex.h>
#include <DistCacheMonitor.h>
#include <AdapterI.h>
#include <TaskManager.h>

namespace xce{
namespace distcache{
namespace monitor{

class DistCacheSwitcherAdapter : public MyUtil::ReplicatedClusterAdapterI<DistCacheSwitcherPrx>, public MyUtil::Singleton<DistCacheSwitcherAdapter> {
public:
	DistCacheSwitcherAdapter();

	bool delFilter( const std::string& serviceName );

	bool changeNodeStatus( const std::string& serviceName, const std::string& nodeName, const int block, NodeStatus status, bool rwFlag );
};

}
}
}
#endif 
