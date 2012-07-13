package com.xiaonei.xce.compareinfo;
import xce.compareinfo.CompareInfoData;
import xce.compareinfo.CompareInfoManagerPrx;
import xce.compareinfo.CompareInfoManagerPrxHelper;
import xce.compareinfo.CompareInfoReloaderPrx;
import xce.compareinfo.CompareInfoReloaderPrxHelper;
import xce.compareinfo.ELEMENTARYSCHOOL;

import xce.util.channel.Channel;
import xce.util.tools.MathUtil;
import Ice.ConnectTimeoutException;
import Ice.ObjectPrx;
import Ice.TimeoutException;
import xce.util.tools.IPAddress;

import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.XceStat;

import xce.clusterstate.ReplicatedClusterAdapter;


public final class CompareInfoAdapter extends
		ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerCompareInfo";	
	private static int _interval = 120;

	public CompareInfoAdapter() {
		super(ctr_endpoints, _interval);
		
	}
	
		
	public boolean  CompareInfo(int userId1,int userId2) {
		CompareInfoManagerPrx cip=	getCompareInfoManager(userId1);		
        return cip.CompareInfo(userId1, userId2);
	}
	public boolean  CompareInfoByTable(int userId1,int userId2 , CompareInfoType types) {
		CompareInfoManagerPrx cip=	getCompareInfoManager(userId1);		
		return cip.CompareInfoByTable(userId1, userId2 , types.getTypelist());
	}
	
	
	protected CompareInfoManagerPrx getCompareInfoManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			CompareInfoManagerPrx prx = CompareInfoManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}
  
	protected CompareInfoManagerPrx getCompareInfoOneManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			CompareInfoManagerPrx prx = CompareInfoManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}
	
	
	
	
}
