package com.xiaonei.xce.highschoolfilter;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import xce.highschoolfilter.HighSchoolFilterManagerPrx;
import xce.highschoolfilter.HighSchoolFilterManagerPrxHelper;
import xce.util.tools.MathUtil;

import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.XceStat;

import xce.clusterstate.ReplicatedClusterAdapter;

public class HighSchoolFilterAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerHighSchoolFilter";
	private static int _interval = 120;	
	
	private HighSchoolFilterAdapter() {
		super(ctr_endpoints,_interval);
	}

	private static HighSchoolFilterAdapter _instance = new HighSchoolFilterAdapter();
	public static HighSchoolFilterAdapter getInstance() {
		return _instance;
	}

	public Boolean isStageHighSchoolTrue(int userId) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "HighSchoolFilter userId="
				+ userId );
    HighSchoolFilterManagerPrx prx=null;
		try {
      prx = getHighSchoolFilterTwoway(userId);
			return prx.isStageHighSchoolTrue(userId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error("HighSchoolFilter isStageHighSchoolTrue timeout userId=" + userId +" "+prx);
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return false;
	}

	public Map<Integer,Boolean> getStageHighSchoolBatch(int[] userIds) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "HighSchoolFilter userIds.length="
				+ userIds.length );
		Map<Integer,Boolean> result = new HashMap<Integer,Boolean>();
    HighSchoolFilterManagerPrx prx =null;
		try {
      prx=getHighSchoolFilterTwoway(0);
			result = prx.getStageHighSchoolBatch(userIds);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error("HighSchoolFilter getStageHighSchoolBatch timeout userIds.length=" + userIds.length + " "+prx);
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return result;
	}

	public List<Integer> getStageHighSchoolIdsFrom(int[] userIds) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "HighSchoolFilter userIds.length="
				+ userIds.length );
		List<Integer> result = new ArrayList<Integer>();
    HighSchoolFilterManagerPrx prx =null;
		try {
      prx = getHighSchoolFilterTwoway(0);
			int[] ret = prx.getStageHighSchoolIdsFrom(userIds);
                        for( int one: ret ){
                                result.add( one );
                        }
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error("HighSchoolFilter getStageHighSchoolIdsFrom timeout userIds.length=" + userIds.length +" "+prx);
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return result;
	}

	protected HighSchoolFilterManagerPrx getHighSchoolFilterTwoway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			HighSchoolFilterManagerPrx prx = HighSchoolFilterManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected HighSchoolFilterManagerPrx getHighSchoolFilterOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			HighSchoolFilterManagerPrx prx = HighSchoolFilterManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}
	
//	public static void main(String[] args) {
//		int userId = 238111132;
//		System.out.println( HighSchoolFilterAdapter.getInstance().getStageHighSchool(userId) );
//		System.exit(0);
//	}
}

