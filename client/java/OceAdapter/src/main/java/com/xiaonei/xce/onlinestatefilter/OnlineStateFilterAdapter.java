package com.xiaonei.xce.onlinestatefilter;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import xce.onlinestatefilter.OnlineStateFilterManagerPrx;
import xce.onlinestatefilter.OnlineStateFilterManagerPrxHelper;
import xce.util.tools.MathUtil;

import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.XceStat;

import xce.clusterstate.ReplicatedClusterAdapter;

public class OnlineStateFilterAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerOnlineStateFilter";
	private static int _interval = 120;	
	
	private OnlineStateFilterAdapter() {
		super(ctr_endpoints,_interval);
	}

	private static OnlineStateFilterAdapter _instance = new OnlineStateFilterAdapter();
	public static OnlineStateFilterAdapter getInstance() {
		return _instance;
	}

	public Boolean isOnlineStateTrue(int userId) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "OnlineStateFilter userId="
				+ userId );
		OnlineStateFilterManagerPrx prx = null;
		try {
			prx = getOnlineStateFilterTwoway(userId);
			return prx.isOnlineStateTrue(userId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error("OnlineStateFilter isOnlineStateTrue timeout prx = " + prx + " userId = " + userId );
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return false;
	}

	public Map<Integer,Boolean> getOnlineStateBatch(int[] userIds) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "OnlineStateFilter userIds.length="
				+ userIds.length );
		Map<Integer,Boolean> result = new HashMap<Integer,Boolean>();
		OnlineStateFilterManagerPrx prx = null;
		try {
			prx = getOnlineStateFilterTwoway(0);
			result = prx.getOnlineStateBatch(userIds);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error("OnlineStateFilter getOnlineStateBatch timeout prx = " + prx + " userIds.length=" + userIds.length );
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return result;
	}

	public List<Integer> getOnlineIdsFrom(int[] userIds) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "OnlineStateFilter userIds.length="
				+ userIds.length );
		List<Integer> result = new ArrayList<Integer>();
		OnlineStateFilterManagerPrx prx = null;
		try {
			prx = getOnlineStateFilterTwoway(0);
			int[] ret = prx.getOnlineIdsFrom(userIds);
                        for( int one: ret ){
                                result.add( one );
                        }
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error("OnlineStateFilter getOnlineIdsFrom timeout prx = " + prx + " userIds.length=" + userIds.length );
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return result;
	}

	protected OnlineStateFilterManagerPrx getOnlineStateFilterTwoway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			OnlineStateFilterManagerPrx prx = OnlineStateFilterManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected OnlineStateFilterManagerPrx getOnlineStateFilterOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			OnlineStateFilterManagerPrx prx = OnlineStateFilterManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}
	
//	public static void main(String[] args) {
//		int userId = 238111132;
//		System.out.println( OnlineStateFilterAdapter.getInstance().getOnlineState(userId) );
//		System.exit(0);
//	}
}

