package com.xiaonei.xce.usercache.impl;

import xce.clusterstate.ReplicatedClusterAdapter;
import xce.usercache.DistUserCacheReloaderPrx;
import xce.usercache.DistUserCacheReloaderPrxHelper;

public class DCUserCacheReloaderAdapter extends ReplicatedClusterAdapter{
	private final static String ctr_endpoints = "ControllerDistUserCacheReloader";
	public DCUserCacheReloaderAdapter() {
		super(ctr_endpoints, 120);
	}
	
	public void reload(int[] userids) {
		getProxy(0).reload(userids);
	}
	
	public DistUserCacheReloaderPrx getProxy(int userId){
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			DistUserCacheReloaderPrx prx = DistUserCacheReloaderPrxHelper.uncheckedCast(prx0.ice_twoway());
			return prx;
		} else {
			return null;
		}
	}
}
