package com.xiaonei.xce.userapicache;


import xce.clusterstate.ReplicatedClusterAdapter;
import xce.userapicache.UserApiCacheReloaderPrx;
import xce.userapicache.UserApiCacheReloaderPrxHelper;

public class DCUserApiCacheReloaderAdapter extends ReplicatedClusterAdapter{
	private final static String ctr_endpoints = "ControllerDistUserApiCacheReloader";
	public DCUserApiCacheReloaderAdapter() {
		super(ctr_endpoints, 120);
	}
	
	public void reload(int[] userids) {
		getProxy(0).reload(userids);
	}
	
	public UserApiCacheReloaderPrx getProxy(int userId){
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			UserApiCacheReloaderPrx prx = UserApiCacheReloaderPrxHelper.uncheckedCast(prx0.ice_twoway());
			return prx;
		} else {
			return null;
		}
	}
}
