package com.xiaonei.xce.photostatefilter;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import xce.photostatefilter.PhotoStateFilterManagerPrx;
import xce.photostatefilter.PhotoStateFilterManagerPrxHelper;
import xce.util.tools.MathUtil;

import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.XceStat;

import xce.clusterstate.ReplicatedClusterAdapter;

public class PhotoStateFilterAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerPhotoStateFilter";
	private static int _interval = 120;	
	
	private PhotoStateFilterAdapter() {
		super(ctr_endpoints,_interval);
	}

	private static PhotoStateFilterAdapter _instance = new PhotoStateFilterAdapter();
	public static PhotoStateFilterAdapter getInstance() {
		return _instance;
	}

	public Boolean isPhotoStateTrue(int userId) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "PhotoStateFilter userId="
				+ userId );
    PhotoStateFilterManagerPrx prx = null;
		try {
      prx = getPhotoStateFilterTwoway(userId);
			return prx.isPhotoStateTrue(userId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error("PhotoStateFilter isPhotoStateTrue timeout userId=" + userId +" "+prx);
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return false;
	}

	public Map<Integer,Boolean> getPhotoStateBatch(int[] userIds) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "PhotoStateFilter userIds.length="
				+ userIds.length );
		Map<Integer,Boolean> result = new HashMap<Integer,Boolean>();
    PhotoStateFilterManagerPrx prx = null;
		try {
      prx = getPhotoStateFilterTwoway(0);
			result = prx.getPhotoStateBatch(userIds);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error("PhotoStateFilter getPhotoStateBatch timeout userIds.length=" + userIds.length +" "+prx);
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return result;
	}

	public List<Integer> getPhotoIdsFrom(int[] userIds) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "PhotoStateFilter userIds.length="
				+ userIds.length );
		List<Integer> result = new ArrayList<Integer>();
    PhotoStateFilterManagerPrx prx = null;
		try {
      prx = getPhotoStateFilterTwoway(0);
			int[] ret = prx.getPhotoIdsFrom(userIds);
                        for( int one: ret ){
                                result.add( one );
                        }
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error("PhotoStateFilter getPhotoIdsFrom timeout userIds.length=" + userIds.length+" " +prx );
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return result;
	}

	protected PhotoStateFilterManagerPrx getPhotoStateFilterTwoway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			PhotoStateFilterManagerPrx prx = PhotoStateFilterManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected PhotoStateFilterManagerPrx getPhotoStateFilterOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			PhotoStateFilterManagerPrx prx = PhotoStateFilterManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}
	
//	public static void main(String[] args) {
//		int userId = 238111132;
//		System.out.println( PhotoStateFilterAdapter.getInstance().getPhotoState(userId) );
//		System.exit(0);
//	}
}

