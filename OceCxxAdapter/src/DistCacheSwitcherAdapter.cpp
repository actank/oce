#include "DistCacheSwitcherAdapter.h"

using xce::distcache::monitor::DistCacheSwitcherAdapter;

DistCacheSwitcherAdapter::DistCacheSwitcherAdapter() : MyUtil::ReplicatedClusterAdapterI <DistCacheSwitcherPrx>("ControllerDistCacheSwitcher", 120, 300) {
}

bool DistCacheSwitcherAdapter::delFilter( const std::string& serviceName ) 
{
	try {
		
		return getProxy(0)->delFilter( serviceName );

	} catch (const Ice::Exception& e) {
		MCE_WARN("DistCacheSwitcherAdapter::delFilter Ice::Exception "<<e);
		return false;
	}
}

bool DistCacheSwitcherAdapter::changeNodeStatus( const std::string& serviceName, const std::string& nodeName, const int block, NodeStatus status, bool rwFlag )
{
	try {
		
		return getProxy(0)->changeNodeStatus( serviceName , nodeName, block, status, rwFlag );

	} catch (const Ice::Exception& e) {
		MCE_WARN("DistCacheSwitcherAdapter::changeNodeStatus Ice::Exception "<<e);
		return false;
	}
}

