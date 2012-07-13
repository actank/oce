#ifndef _DAILYACCESSCACHE_ICE_
#define _DAILYACCESSCACHE_ICE_

#include <Util.ice>

module xce {
module dailyaccesscache {

	interface DailyAccessCacheManager {
		void access(int userId);
	};
};
};
#endif
