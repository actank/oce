#ifndef __DISTCACHE_MONITOR_H__
#define __DISTCACHE_MONITOR_H__

#include <ServiceI.h>

#include "DistCacheMonitor.h"
#include "ServiceMonitor.h"
#include "ZooKeeperMonitor.h"

namespace xce
{
namespace distcache
{
namespace monitor
{
	const string DISTCACHE_MONITOR = "DM";
	
	class DistCacheMonitorI : public DistCacheMonitor , public IceUtil::Thread, public MyUtil::Singleton<DistCacheMonitorI>
	{
		public:

			void reloadConfig(const Ice::Current& current);
			
			void setFlag( const std::string& nameStr, bool flag, const Ice::Current& current);

		protected:

			void run();

		public:

			int m_idx;

			int m_repNums;
	};
}
}
}

#endif
