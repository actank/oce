#ifndef _DISTCACHE_MONITOR_ICE
#define _DISTCACHE_MONITOR_ICE

module xce
{
module distcache
{
module monitor
{
	class DistCacheMonitor
	{
		void reloadConfig();

		void setFlag( string nameStr, bool flag );
	};

	enum NodeStatus
	{
		ONLINE,
		UPGRADING,
		OFFLINE
	};
	
	class DistCacheSwitcher
	{
		bool changeNodeStatus(string serviceName, string nodeName, int block, NodeStatus status, bool rwFlag);

		bool delFilter( string serviceName );
	};
};  
};  
};  
#endif
    
    
    
    
    
    
    
    
    
    
    
