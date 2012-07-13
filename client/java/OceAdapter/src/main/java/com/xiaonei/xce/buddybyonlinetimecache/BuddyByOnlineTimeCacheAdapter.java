package com.xiaonei.xce.buddybyonlinetimecache;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Vector;
import java.util.Map.Entry;

import Ice.ObjectPrx;
import xce.buddy.BuddyByOnlineTimeCacheManagerPrx;
import xce.buddy.BuddyByOnlineTimeCacheManagerPrxHelper;
import xce.buddy.BuddyByOnlineTimeLoaderPrx;
import xce.buddy.BuddyByOnlineTimeLoaderPrxHelper;
import xce.util.service.ReplicatedServiceAdapter;
import xce.util.tools.MathUtil;

import Ice.TimeoutException;

import com.xiaonei.xce.buddybyonlinetimecache.BuddyByOnlineTimeCacheAdapter;
import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.XceStat;

import xce.clusterstate.ReplicatedClusterAdapter;

public class BuddyByOnlineTimeCacheAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerBuddyByOnlineTimeCache";
	private static int _interval = 120;

	private BuddyByOnlineTimeCacheAdapter() {
		super(ctr_endpoints, _interval);
	}

	private static BuddyByOnlineTimeCacheAdapter _instance = new BuddyByOnlineTimeCacheAdapter();

	public static BuddyByOnlineTimeCacheAdapter getInstance() {
		return _instance;
	}

	public int getFriendCount(int userId) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(),
				"BuddyByOnlineTimeCache " + userId);
		String prxstr = "NULL Prx";
		try {
			BuddyByOnlineTimeCacheManagerPrx prx = getBuddyByOnlineTimeCacheManagerTwoway(userId);
			prxstr = prx.ice_toString();
			return prx.getFriendCount(userId);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getFriendCount " + userId
							+ " Ice.ConnectTimeoutException " + prxstr);
			stat.endException(e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getFriendCount " + userId
							+ " Ice.TimeoutException " + prxstr);
			stat.endException(e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getFriendCount " + userId + " "
							+ prxstr, e);
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return 0;
	}

	public Map<Integer, Integer> getFriendCounts(List<Integer> userIds) {
		Map<Integer, List<Integer>> splitTargets = new HashMap<Integer, List<Integer>>();
		int cluster = getCluster();
		for (int i = 0; i < cluster; ++i) {
			splitTargets.put(i, new ArrayList<Integer>());
		}
		for (int i : userIds) {
			splitTargets.get(i % cluster).add(i);
		}

		/* collect results */
		Map<Integer, Integer> allResults = new HashMap<Integer, Integer>();
		if (userIds.isEmpty()) {
			return allResults;
		}
		TimeCost cost = TimeCost.begin(XceStat.getLogger(),
				"BuddyByOnlineTimeCache getFriendCounts start:" + userIds.get(0)
						+ " size:" + userIds.size());
		for (int i = 0; i < cluster; ++i) {
			List<Integer> subUserIds = splitTargets.get(i);
			if (subUserIds.isEmpty()) {
				continue;
			}
			try {
				Map<Integer, Integer> subTargets = getBuddyByOnlineTimeCacheManagerTwoway(
						i).getFriendCounts(MathUtil.list2array(subUserIds));
				allResults.putAll(subTargets);
			} catch (TimeoutException e) {
				cost.endStep("SubBatch start:" + subUserIds.get(0) + " size:"
						+ subUserIds.size(), e);
			} finally {
				cost.endFinally();
			}
		}
		return allResults;
	}

	public List<Integer> getFriendList(int userId, int limit) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(),
				"BuddyByOnlineTimeCache " + userId);
		List<Integer> result = new ArrayList<Integer>();
		String prxstr = "NULL Prx";
		try {
			BuddyByOnlineTimeCacheManagerPrx prx = getBuddyByOnlineTimeCacheManagerTwoway(userId);
			prxstr = prx.ice_toString();
			int[] ret = prx.getFriendList(userId, limit);
			for (int one : ret) {
				result.add(one);
			}
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getFriendList " + userId
							+ " Ice.ConnectTimeoutException " + prxstr);
			stat.endException(e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getFriendList " + userId
							+ " Ice.TimeoutException " + prxstr);
			stat.endException(e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger()
					.error(
							this.getClass().getName() + ".getFriendList " + userId + " "
									+ prxstr, e);
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return result;
	}

	public List<Integer> getFriendList(int userId, int begin, int limit) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(),
				"BuddyByOnlineTimeCache " + userId);
		List<Integer> result = new ArrayList<Integer>();
		String prxstr = "NULL Prx";
		try {
			BuddyByOnlineTimeCacheManagerPrx prx = getBuddyByOnlineTimeCacheManagerTwoway(userId);
			prxstr = prx.ice_toString();
			int[] ret = prx.getFriendListN(userId, begin, limit);
			for (int one : ret) {
				result.add(one);
			}
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getFriendList " + userId
							+ " Ice.ConnectTimeoutException " + prxstr);
			stat.endException(e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getFriendList " + userId
							+ " Ice.TimeoutException " + prxstr);
			stat.endException(e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger()
					.error(
							this.getClass().getName() + ".getFriendList " + userId + " "
									+ prxstr, e);
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return result;
	}

	public Map<Integer, List<Integer>> getFriendLists(List<Integer> userIds) {
		Map<Integer, List<Integer>> splitTargets = new HashMap<Integer, List<Integer>>();
		int cluster = getCluster();
		for (int i = 0; i < cluster; ++i) {
			splitTargets.put(i, new ArrayList<Integer>());
		}
		for (int i : userIds) {
			splitTargets.get(i % cluster).add(i);
		}

		/* collect results */
		Map<Integer, List<Integer>> allResults = new HashMap<Integer, List<Integer>>();
		if (userIds.isEmpty()) {
			return allResults;
		}
		TimeCost cost = TimeCost.begin(XceStat.getLogger(),
				"BuddyByOnlineTimeCache getFriendLists start:" + userIds.get(0)
						+ " size:" + userIds.size());
		for (int i = 0; i < cluster; ++i) {
			List<Integer> subUserIds = splitTargets.get(i);
			if (subUserIds.isEmpty()) {
				continue;
			}
			try {
				Map<Integer, int[]> subTargets = getBuddyByOnlineTimeCacheManagerTwoway(
						i).getFriendLists(MathUtil.list2array(subUserIds));
				for (Entry<Integer, int[]> one : subTargets.entrySet()) {
					int key = one.getKey();
					List<Integer> value = new ArrayList<Integer>();
					for (int two : one.getValue()) {
						value.add(two);
					}
					allResults.put(key, value);
				}
			} catch (TimeoutException e) {
				cost.endStep("SubBatch start:" + subUserIds.get(0) + " size:"
						+ subUserIds.size(), e);
			} finally {
				cost.endFinally();
			}
		}
		return allResults;
	}

	public void reload(int userId) {
		BuddyByOnlineTimeLoaderAdapter.getInstance().reload(userId);
	}

	protected BuddyByOnlineTimeCacheManagerPrx getBuddyByOnlineTimeCacheManagerTwoway(
			int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			BuddyByOnlineTimeCacheManagerPrx prx = BuddyByOnlineTimeCacheManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected BuddyByOnlineTimeCacheManagerPrx getBuddyByOnlineTimeCacheManagerOneway(
			int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			BuddyByOnlineTimeCacheManagerPrx prx = BuddyByOnlineTimeCacheManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
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
		List<Integer> listRes1 = BuddyByOnlineTimeCacheAdapter.getInstance()
				.getFriendList(userId1, -1);
		List<Integer> listRes2 = BuddyByOnlineTimeCacheAdapter.getInstance()
				.getFriendList(userId2, -1);
		Map<Integer, List<Integer>> listResBatch = BuddyByOnlineTimeCacheAdapter
				.getInstance().getFriendLists(batch);
		int countRes1 = BuddyByOnlineTimeCacheAdapter.getInstance().getFriendCount(
				userId1);
		int countRes2 = BuddyByOnlineTimeCacheAdapter.getInstance().getFriendCount(
				userId2);
		Map<Integer, Integer> countResBatch = BuddyByOnlineTimeCacheAdapter
				.getInstance().getFriendCounts(batch);
		System.out.println("--Single:");
		System.out.println(userId1 + ".list.size() = " + listRes1.size());
		System.out.println(userId2 + ".list.size() = " + listRes2.size());
		System.out.println(userId1 + ".count = " + countRes1);
		System.out.println(userId2 + ".count = " + countRes2);
		System.out.println("--Batch:");
		for (Entry<Integer, List<Integer>> one : listResBatch.entrySet()) {
			System.err.println(one.getKey() + ".list.size() = "
					+ one.getValue().size());
		}
		for (Entry<Integer, Integer> one : countResBatch.entrySet()) {
			System.err.println(one.getKey() + ".count = " + one.getValue());
		}
		System.exit(0);
		/*
		*/
	}
}

class BuddyByOnlineTimeLoaderAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerBuddyByOnlineTimeLoader";
	private static int _interval = 120;

	private BuddyByOnlineTimeLoaderAdapter() {
		super(ctr_endpoints, _interval);
	}

	private static BuddyByOnlineTimeLoaderAdapter _instance = new BuddyByOnlineTimeLoaderAdapter();

	public static BuddyByOnlineTimeLoaderAdapter getInstance() {
		return _instance;
	}

	public void reload(int userId) {
		try {
			getBuddyByOnlineTimeLoaderOneway(userId).reload(userId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error("BuddyCacheByOnlineTimeLoader timeout ");
		}
	}

	protected BuddyByOnlineTimeLoaderPrx getBuddyByOnlineTimeLoaderOneway(
			int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			BuddyByOnlineTimeLoaderPrx prx = BuddyByOnlineTimeLoaderPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}
}
