package com.xiaonei.xce.pagefans;

import java.util.List;

import xce.buddy.PageFansManagerPrx;
import xce.buddy.PageFansManagerPrxHelper;
import xce.clusterstate.ReplicatedClusterAdapter;

public class PageFansCacheAdapter extends ReplicatedClusterAdapter{

	private static final String ctr_endpoints = "ControllerPageFansCache";	
	private static final int _interval = 120;

	public PageFansCacheAdapter() {
		super(ctr_endpoints, _interval);
		
	}

	
	
	protected PageFansManagerPrx getPageFansManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,300);
		if (prx0 != null) {
			PageFansManagerPrx prx = PageFansManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}
  
	protected PageFansManagerPrx getPageFansOneManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,300);
		if (prx0 != null) {
			PageFansManagerPrx prx = PageFansManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}
	


}
