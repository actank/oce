package com.xiaonei.xce.pagefans;

import xce.buddy.PageFansReloaderPrx;
import xce.buddy.PageFansReloaderPrxHelper;
import xce.clusterstate.ReplicatedClusterAdapter;

public class PageFansReloaderAdapter extends ReplicatedClusterAdapter{


	private static final String ctr_endpoints = "ControllerPageFansReloader";	
	private static final int _interval = 120;

	public PageFansReloaderAdapter() {
		super(ctr_endpoints, _interval);
		
	}
			
	protected PageFansReloaderPrx getReloaderPrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,300);
		if (prx0 != null) {
			PageFansReloaderPrx prx = PageFansReloaderPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}
  
	protected PageFansReloaderPrx getReloaderOnePrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,300);
		if (prx0 != null) {
			PageFansReloaderPrx prx = PageFansReloaderPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}
	

	

}
