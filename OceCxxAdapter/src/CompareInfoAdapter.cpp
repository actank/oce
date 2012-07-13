#include "CompareInfoAdapter.h"

xce::compareinfo::CompareInfoAdapter::CompareInfoAdapter() 
                : MyUtil::ReplicatedClusterAdapterI <xce::compareinfo::CompareInfoManagerPrx>("ControllerCompareInfo", 120, 300) {
}

