package com.xiaonei.xce.buddyadapter.buddybyidcache;

import xce.buddy.DistBuddyByIdCacheReloaderPrx;
import xce.buddy.DistBuddyByIdCacheReloaderPrxHelper;
import xce.clusterstate.ReplicatedClusterAdapter;

public class DCBuddyByIdCacheReloaderAdapter extends ReplicatedClusterAdapter {

    private final static String ctr_endpoints = "ControllerDistBuddyByIdCacheReloader";

    public DCBuddyByIdCacheReloaderAdapter() {
        super(ctr_endpoints, 120);
    }

    public void reload(int[] userids) {
        getProxy(0).reload(userids);
    }

    public DistBuddyByIdCacheReloaderPrx getProxy(int userId) {
        Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
        if (prx0 != null) {
            DistBuddyByIdCacheReloaderPrx prx = DistBuddyByIdCacheReloaderPrxHelper
                    .uncheckedCast(prx0.ice_twoway());
            return prx;
        } else {
            return null;
        }
    }

}
