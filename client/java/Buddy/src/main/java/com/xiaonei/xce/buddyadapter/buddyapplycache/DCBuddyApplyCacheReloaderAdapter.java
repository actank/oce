package com.xiaonei.xce.buddyadapter.buddyapplycache;

import xce.clusterstate.ReplicatedClusterAdapter;
import xce.buddyapplycache.DistBuddyApplyCacheReloaderPrx;
import xce.buddyapplycache.DistBuddyApplyCacheReloaderPrxHelper;

public class DCBuddyApplyCacheReloaderAdapter extends ReplicatedClusterAdapter{
	private final static String ctr_endpoints = "ControllerDistBuddyApplyCacheReloader";
	public DCBuddyApplyCacheReloaderAdapter() {
		super(ctr_endpoints, 120);
	}
	
	public void reload(int[] userids) {
		getProxy(0).reload(userids);
	}
	
	public DistBuddyApplyCacheReloaderPrx getProxy(int userId){
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			DistBuddyApplyCacheReloaderPrx prx = DistBuddyApplyCacheReloaderPrxHelper.uncheckedCast(prx0.ice_twoway());
			return prx;
		} else {
			return null;
		}
	}
}