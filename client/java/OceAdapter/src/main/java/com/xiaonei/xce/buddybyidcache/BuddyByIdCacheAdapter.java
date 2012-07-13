package com.xiaonei.xce.buddybyidcache;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import xce.buddy.BuddyByIdCacheManagerPrx;
import xce.buddy.BuddyByIdCacheManagerPrxHelper;
import xce.buddy.BuddyByIdLoaderPrx;
import xce.buddy.BuddyByIdLoaderPrxHelper;
import xce.util.tools.MathUtil;

import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.XceStat;

import xce.clusterstate.ReplicatedClusterAdapter;

public class BuddyByIdCacheAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerBuddyByIdCache";
	private static int _interval = 120;

	private BuddyByIdCacheAdapter() {
		super(ctr_endpoints, _interval);
	}

	private static BuddyByIdCacheAdapter _instance = new BuddyByIdCacheAdapter();

	public static BuddyByIdCacheAdapter getInstance() {
		return _instance;
	}

	public int getFriendCount(int userId) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "BuddyByIdCache "
				+ userId);
		String prxstr = "NULL Prx";
		try {
			BuddyByIdCacheManagerPrx prx = getBuddyByIdCacheManagerTwoway(userId);
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
				"BuddyByIdCache getFriendCounts start:" + userIds.get(0) + " size:"
						+ userIds.size());
		for (int i = 0; i < cluster; ++i) {
			List<Integer> subUserIds = splitTargets.get(i);
			if (subUserIds.isEmpty()) {
				continue;
			}
			try {
				Map<Integer, Integer> subTargets = getBuddyByIdCacheManagerTwoway(i)
						.getFriendCounts(MathUtil.list2array(subUserIds));
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

	public List<Integer> getFriendListAsc(int userId, int limit) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "BuddyByIdCache "
				+ userId);
		List<Integer> result = new ArrayList<Integer>();
		String prxstr = "NULL Prx";
		try {
			BuddyByIdCacheManagerPrx prx = getBuddyByIdCacheManagerTwoway(userId);
			prxstr = prx.ice_toString();
			int[] ret = prx.getFriendList(userId, limit);
			for (int one : ret) {
				result.add(one);
			}
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getFriendListAsc " + userId
							+ " Ice.ConnectTimeoutException " + prxstr);
			stat.endException(e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getFriendListAsc " + userId
							+ " Ice.TimeoutException " + prxstr);
			stat.endException(e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getFriendListAsc " + userId + " "
							+ prxstr, e);
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return result;
	}

	public List<Integer> getFriendListAsc(int userId, int begin, int limit) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "BuddyByIdCache "
				+ userId);
		List<Integer> result = new ArrayList<Integer>();
		String prxstr = "NULL Prx";
		try {
			BuddyByIdCacheManagerPrx prx = getBuddyByIdCacheManagerTwoway(userId);
			prxstr = prx.ice_toString();
			int[] ret = prx.getFriendListN(userId, begin, limit);
			for (int one : ret) {
				result.add(one);
			}
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getFriendListAsc " + userId
							+ " Ice.ConnectTimeoutException " + prxstr);
			stat.endException(e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getFriendListAsc " + userId
							+ " Ice.TimeoutException " + prxstr);
			stat.endException(e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getFriendListAsc " + userId + " "
							+ prxstr, e);
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return result;
	}

	public Map<Integer, List<Integer>> getFriendListsAsc(List<Integer> userIds) {
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
				"BuddyByIdCache getFriendListsAsc start:" + userIds.get(0) + " size:"
						+ userIds.size());
		for (int i = 0; i < cluster; ++i) {
			List<Integer> subUserIds = splitTargets.get(i);
			if (subUserIds.isEmpty()) {
				continue;
			}
			try {
				Map<Integer, int[]> subTargets = getBuddyByIdCacheManagerTwoway(i)
						.getFriendLists(MathUtil.list2array(subUserIds));
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
		BuddyByIdLoaderAdapter.getInstance().reload(userId);
	}

	public int filter(int userId, List<Integer> buddys) {
		return BuddyByIdLoaderAdapter.getInstance().filter(userId, buddys);
	}

	protected BuddyByIdCacheManagerPrx getBuddyByIdCacheManagerTwoway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			BuddyByIdCacheManagerPrx prx = BuddyByIdCacheManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected BuddyByIdCacheManagerPrx getBuddyByIdCacheManagerOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			BuddyByIdCacheManagerPrx prx = BuddyByIdCacheManagerPrxHelper
					.uncheckedCast(prx0.ice_oneway());
			return prx;
		} else {
			return null;
		}
	}

	public static void main(String[] args) {
		/*
		 * File file = new File("..."); try { BufferedReader input = new
		 * BufferedReader(new FileReader(file)); String text; while ((text =
		 * input.readLine()) != null) { int id = Integer.valueOf(text);
		 * System.out.println( id ); //
		 * BuddyByIdCacheAdapter.getInstance().reload(id); Thread.sleep(100); } }
		 * catch (Exception ex) { ex.printStackTrace(); }
		 */

		/*
		 * int userId1 = 24496255; int userId2 = 68126; //
		 * BuddyByIdCacheAdapter.getInstance().reload(userId1); List<Integer> batch
		 * = new ArrayList<Integer>(); batch.add(userId1); batch.add(userId2);
		 * List<Integer> listRes1 =
		 * BuddyByIdCacheAdapter.getInstance().getFriendListAsc(userId1,-1);
		 * List<Integer> listRes2 =
		 * BuddyByIdCacheAdapter.getInstance().getFriendListAsc(userId2,-1);
		 * Map<Integer, List<Integer>> listResBatch =
		 * BuddyByIdCacheAdapter.getInstance().getFriendListsAsc( batch ); int
		 * countRes1 = BuddyByIdCacheAdapter.getInstance().getFriendCount(userId1);
		 * int countRes2 =
		 * BuddyByIdCacheAdapter.getInstance().getFriendCount(userId2); Map<Integer,
		 * Integer> countResBatch =
		 * BuddyByIdCacheAdapter.getInstance().getFriendCounts( batch );
		 * System.out.println( "--Single:"); System.out.println( userId1 +
		 * ".list.size() = " + listRes1.size() ); System.out.println( userId2 +
		 * ".list.size() = " + listRes2.size() ); System.out.println( userId1 +
		 * ".count = " + countRes1 ); System.out.println( userId2 + ".count = " +
		 * countRes2 ); System.out.println( "--Batch:"); for (
		 * Entry<Integer,List<Integer>> one : listResBatch.entrySet() ) {
		 * System.err.println( one.getKey() + ".list.size() = " +
		 * one.getValue().size() ); } for ( Entry<Integer,Integer> one :
		 * countResBatch.entrySet() ) { System.err.println( one.getKey() +
		 * ".count = " + one.getValue() ); } System.exit(0);
		 */
	}
}

class BuddyByIdLoaderAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerBuddyByIdLoader";
	private static int _interval = 120;

	private BuddyByIdLoaderAdapter() {
		super(ctr_endpoints, _interval);
	}

	private static BuddyByIdLoaderAdapter _instance = new BuddyByIdLoaderAdapter();

	public static BuddyByIdLoaderAdapter getInstance() {
		return _instance;
	}

	public void reload(int userId) {
		getBuddyByIdLoaderOneway(userId).reload(userId);
	}

	public int filter(int userId, List<Integer> buddys) {
		try {
			return getBuddyByIdLoaderTwoway(userId).filter(userId,
					MathUtil.list2array(buddys));
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".filter " + userId + " ");
		}
		return 0;
	}

	protected BuddyByIdLoaderPrx getBuddyByIdLoaderOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			BuddyByIdLoaderPrx prx = BuddyByIdLoaderPrxHelper.uncheckedCast(prx0
					.ice_oneway());
			return prx;
		} else {
			return null;
		}
	}

	protected BuddyByIdLoaderPrx getBuddyByIdLoaderTwoway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			BuddyByIdLoaderPrx prx = BuddyByIdLoaderPrxHelper.uncheckedCast(prx0
					.ice_twoway());
			return prx;
		} else {
			return null;
		}
	}
}
