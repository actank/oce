package com.xiaonei.xce.friendcountcache;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import xce.buddy.FriendCountCacheManagerPrx;
import xce.buddy.FriendCountCacheManagerPrxHelper;
import xce.util.tools.MathUtil;

import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.XceStat;

import xce.clusterstate.ReplicatedClusterAdapter;

public class FriendCountCacheAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerFriendCountCache";
	private static int _interval = 120;	
	
	private FriendCountCacheAdapter() {
		super(ctr_endpoints,_interval);
	}

	private static FriendCountCacheAdapter _instance = new FriendCountCacheAdapter();
	public static FriendCountCacheAdapter getInstance() {
		return _instance;
	}

	public int getFriendCount(int userId) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FriendCountCache "
				+ userId);
		FriendCountCacheManagerPrx prx = null;
		try {
			prx = getFriendCountCacheManagerTwoway(userId);
			return prx.getFriendCount(userId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error("FriendCountCache getFriendCount timeout prx = " + prx + " userId = " + userId);
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return -1;
	}

	public Map<Integer,Integer> getFriendCounts(List<Integer> userIds) {
		Map<Integer, List<Integer>> splitTargets = new HashMap<Integer, List<Integer>>();
		int cluster = getCluster();
		for (int i = 0; i < cluster; ++i) {
			splitTargets.put(i, new ArrayList<Integer>());
		}
		for (int i : userIds) {
			splitTargets.get(i % cluster).add(i);
		}

		/* collect results */
		Map<Integer,Integer> allResults = new HashMap<Integer,Integer>();
		if (userIds.isEmpty()){
			return allResults;
		}
		TimeCost cost = TimeCost.begin(XceStat.getLogger(),
				"FriendCountCache getFriendCounts start:" + userIds.get(0) + " size:"
				+ userIds.size());
		for (int i = 0; i < cluster; ++i) {
			List<Integer> subUserIds = splitTargets.get(i);
			if ( subUserIds.isEmpty() ){
				continue;
			}
			FriendCountCacheManagerPrx prx = null;
			try {
				prx = getFriendCountCacheManagerTwoway(i);
				Map<Integer, Integer> subTargets = prx.getFriendCounts(MathUtil.list2array(subUserIds));
				allResults.putAll( subTargets );
			} catch (TimeoutException e) {
				cost.endStep("SubBatch start: prx = " + prx + " " + subUserIds.get(0)
						+ " size:" + subUserIds.size(), e);
			} finally {
				cost.endFinally();
			}
		}
		return allResults;
	}

	protected FriendCountCacheManagerPrx getFriendCountCacheManagerTwoway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			FriendCountCacheManagerPrx prx = FriendCountCacheManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected FriendCountCacheManagerPrx getFriendCountCacheManagerOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			FriendCountCacheManagerPrx prx = FriendCountCacheManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}

	public static void main(String[] args) {
		int userId1 = 24496255;
		int userId2 = 68126;
		List<Integer> batch = new ArrayList<Integer>();
		batch.add(userId1);
		batch.add(userId2);
		int countRes1 = FriendCountCacheAdapter.getInstance().getFriendCount(userId1);
		int countRes2 = FriendCountCacheAdapter.getInstance().getFriendCount(userId2);
		Map<Integer, Integer> countResBatch = FriendCountCacheAdapter.getInstance().getFriendCounts( batch );
		System.out.println( "--Single:");
		System.out.println( userId1 + ".count = " + countRes1 );
		System.out.println( userId2 + ".count = " + countRes2 );
		System.out.println( "--Batch:");
		for ( Entry<Integer,Integer> one : countResBatch.entrySet() ) {
			System.err.println( one.getKey() + ".count = " + one.getValue() );
		}
		System.exit(0);
		/*
		*/
	}
}

