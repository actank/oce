#include "UserCacheReloaderAdapter.h"

xce::usercache::UserCacheReloaderAdapter::UserCacheReloaderAdapter() : MyUtil::ReplicatedClusterAdapterI <xce::usercache::UserCacheReloaderPrx>("ControllerUserCacheReloader", 120, 300) 
{
}
