#include "DailyAccessCacheAdapter.h"

namespace xce {
namespace dailyaccesscache {
namespace adapter {

void DailyAccessCacheAdapter::access(int userId) {
        getProxy(0)->access(userId);
}

}
}
}
