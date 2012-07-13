#include "DistCacheMonitorAdapter.h"

using xce::distcache::monitor::DistCacheMonitorAdapter;

DistCacheMonitorAdapter::DistCacheMonitorAdapter() : MyUtil::ReplicatedClusterAdapterI <DistCacheMonitorPrx>("ControllerDistCacheMonitor", 120, 300) {
}

void DistCacheMonitorAdapter::reloadConfig() 
{
	try {
		std::vector<DistCacheMonitorPrx> prxs = getProxySeq(0);

		for( std::vector<DistCacheMonitorPrx>::const_iterator it = prxs.begin(); it != prxs.end(); ++it )
		{
			(*it)->reloadConfig();
		}
	} catch (const Ice::Exception& e) {
		MCE_WARN("DistCacheMonitorAdapter::reloadConfig Ice::Exception "<<e);
	}
}

