#include "CompareInfoReloaderAdapter.h"

xce::compareinfo::CompareInfoReloaderAdapter::CompareInfoReloaderAdapter() 
                : MyUtil::ReplicatedClusterAdapterI <xce::compareinfo::CompareInfoReloaderPrx>("ControllerCompareInfoReloader", 120, 300) {
}
    
void xce::compareinfo::CompareInfoReloaderAdapter::update(const xce::compareinfo::CompareInfoUpdateOldDataPtr& data)
{
	getProxy(0)->update(data);
}

