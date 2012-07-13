package com.xiaonei.xce.buddyadapter.buddybyaddtimecache;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import xce.buddy.BuddyByAddTimeCacheManagerPrx;
import xce.buddy.BuddyByAddTimeCacheManagerPrxHelper;
import xce.buddy.BuddyByAddTimeLoaderPrx;
import xce.buddy.BuddyByAddTimeLoaderPrxHelper;
import xce.buddy.BuddyByAddTimeNode;
import xce.clusterstate.ReplicatedClusterAdapter;
import xce.util.tools.MathUtil;
import Ice.ConnectTimeoutException;
import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.XceStat;

/**
 * BuddyByAddTimeCacheAdapter
 * 
 * @author yuyang(yang.yu@opi-corp.com)
 * 
 */

public class BuddyByAddTimeCacheAdapter<T extends BuddyByAddTime> extends
		ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerBuddyByAddTimeCache";
	private static int _interval = 120;
	private final BuddyByAddTimeFactory<T> _factory;

	/**
	 * @param factory
	 */
	private BuddyByAddTimeCacheAdapter(BuddyByAddTimeFactory<T> factory) {
		super(ctr_endpoints, _interval);
		_factory = factory;
	}

	private static BuddyByAddTimeCacheAdapter<DefaultBuddyByAddTime> _instance = new BuddyByAddTimeCacheAdapter<DefaultBuddyByAddTime>(
			new DefaultBuddyByAddTimeFactory());

	/**
	 * @return
	 */
	public static BuddyByAddTimeCacheAdapter<DefaultBuddyByAddTime> getInstance() {
		return _instance;
	}

	/**
	 * @param userId
	 * @return
	 */
	public int getFriendCount(int userId) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "BuddyByAddTimeCache "
				+ userId);
		String prxstr = "NULL Prx";
		try {
			BuddyByAddTimeCacheManagerPrx prx = getBuddyByAddTimeCacheManagerTwoway(userId);
			prxstr = prx.ice_toString();
			return prx.getFriendCount(userId);
		} catch (ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getFriendCount " + userId + " "
							+ "Ice.ConnectTimeoutException " + prxstr);
			stat.endException(e);
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getFriendCount " + userId + " "
							+ "Ice.TimeoutException " + prxstr);
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

	/**
	 * @param userIds
	 * @return
	 */
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
				"BuddyByAddTimeCache getFriendCounts start:" + userIds.get(0)
						+ " size:" + userIds.size());
		for (int i = 0; i < cluster; ++i) {
			List<Integer> subUserIds = splitTargets.get(i);
			if (subUserIds.isEmpty()) {
				continue;
			}
			try {
				Map<Integer, Integer> subTargets = getBuddyByAddTimeCacheManagerTwoway(
						i).getFriendCounts(MathUtil.list2array(subUserIds));
				allResults.putAll(subTargets);
			} catch (TimeoutException e) {
				e.printStackTrace();
				cost.endStep("SubBatch start:" + subUserIds.get(0) + " size:"
						+ subUserIds.size(), e);
			} finally {
				cost.endFinally();
			}
		}
		return allResults;
	}

	/**
	 * @param userId
	 * @param limit
	 * @return
	 */
	public List<Integer> getFriendList(int userId, int limit) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "BuddyByAddTimeCache "
				+ userId);
		List<Integer> result = new ArrayList<Integer>();
		String prxstr = "NULL Prx";
		try {
			BuddyByAddTimeCacheManagerPrx prx = getBuddyByAddTimeCacheManagerTwoway(userId);
			prxstr = prx.ice_toString();
			int[] ret = prx.getFriendList(userId, limit);
			for (int one : ret) {
				result.add(one);
			}
		} catch (ConnectTimeoutException e) {
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

	/**
	 * @param userId
	 * @param begin
	 * @param limit
	 * @return
	 */
	public List<Integer> getFriendList(int userId, int begin, int limit) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "BuddyByAddTimeCache "
				+ userId);
		List<Integer> result = new ArrayList<Integer>();
		String prxstr = "NULL Prx";
		try {
			BuddyByAddTimeCacheManagerPrx prx = getBuddyByAddTimeCacheManagerTwoway(userId);
			prxstr = prx.ice_toString();
			int[] ret = prx.getFriendListN(userId, begin, limit);
			for (int one : ret) {
				result.add(one);
			}
		} catch (ConnectTimeoutException e) {
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

	/**
	 * @param userIds
	 * @return
	 */
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
				"BuddyByAddTimeCache getFriendLists start:" + userIds.get(0) + " size:"
						+ userIds.size());
		for (int i = 0; i < cluster; ++i) {
			List<Integer> subUserIds = splitTargets.get(i);
			if (subUserIds.isEmpty()) {
				continue;
			}
			try {
				Map<Integer, int[]> subTargets = getBuddyByAddTimeCacheManagerTwoway(i)
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
				e.printStackTrace();
			} finally {
				cost.endFinally();
			}
		}
		return allResults;
	}

	/**
	 * @param userId
	 * @param beginTime
	 * @param endTime
	 * @return
	 */
	public List<T> getFriendListWithinTime(int userId, long beginTime,
			long endTime) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "BuddyByAddTimeCache "
				+ userId);
		String prxstr = "NULL Prx";
		try {
			List<T> result = new ArrayList<T>();
			BuddyByAddTimeCacheManagerPrx prx = getBuddyByAddTimeCacheManagerTwoway(userId);
			prxstr = prx.ice_toString();
			BuddyByAddTimeNode[] ret = prx.getFriendListWithinTime(userId, beginTime,
					endTime);
			for (BuddyByAddTimeNode one : ret) {
				T obj = _factory.create();
				obj.setBase(one.userId, one.time);
				result.add(obj);
			}
			return result;
		} catch (Ice.ConnectTimeoutException e) {
			System.err.println("BuddyByAddTimeFriends getFriendListWithinTime( "
					+ userId + ") Ice.ConnectTimeoutException " + prxstr);
			stat.endException(e);
		} catch (Ice.TimeoutException e) {
			System.err.println("BuddyByAddTimeFriends getFriendListWithinTime("
					+ userId + ") Timeout " + prxstr);
			stat.endException(e);
		} catch (Exception e) {
			e.printStackTrace();
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return new ArrayList<T>();
	}

	/**
	 * @param userId
	 * @param limit
	 * @return
	 */
	public List<T> getFriendListAndTime(int userId, int limit) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "BuddyByAddTimeCache "
				+ userId);
		String prxstr = "NULL Prx";
		try {
			List<T> result = new ArrayList<T>();
			BuddyByAddTimeCacheManagerPrx prx = getBuddyByAddTimeCacheManagerTwoway(userId);
			prxstr = prx.ice_toString();
			BuddyByAddTimeNode[] ret = prx.getFriendListAndTime(userId, limit);
			for (BuddyByAddTimeNode one : ret) {
				T obj = _factory.create();
				obj.setBase(one.userId, one.time);
				result.add(obj);
			}
			return result;
		} catch (Ice.ConnectTimeoutException e) {
			System.err.println("BuddyByAddTimeFriends getFriendListAndTime( "
					+ userId + ") Ice.ConnectTimeoutException " + prxstr);
			stat.endException(e);
		} catch (Ice.TimeoutException e) {
			System.err.println("BuddyByAddTimeFriends getFriendListAndTime(" + userId
					+ ") Timeout " + prxstr);
			stat.endException(e);
		} catch (Exception e) {
			e.printStackTrace();
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return new ArrayList<T>();
	}

	/**
	 * @param userId
	 */
	public void reload(int userId) {
		BuddyByAddTimeLoaderAdapter.getInstance().reload(userId);
	}

	/**
	 * @param userId
	 * @return
	 */
	protected BuddyByAddTimeCacheManagerPrx getBuddyByAddTimeCacheManagerTwoway(
			int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			BuddyByAddTimeCacheManagerPrx prx = BuddyByAddTimeCacheManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	/**
	 * @param userId
	 * @return
	 */
	protected BuddyByAddTimeCacheManagerPrx getBuddyByAddTimeCacheManagerOneway(
			int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			BuddyByAddTimeCacheManagerPrx prx = BuddyByAddTimeCacheManagerPrxHelper
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
		List<Integer> listRes1 = BuddyByAddTimeCacheAdapter.getInstance()
				.getFriendList(userId1, -1);
		List<Integer> listRes2 = BuddyByAddTimeCacheAdapter.getInstance()
				.getFriendList(userId2, -1);
		Map<Integer, List<Integer>> listResBatch = BuddyByAddTimeCacheAdapter
				.getInstance().getFriendLists(batch);
		int countRes1 = BuddyByAddTimeCacheAdapter.getInstance().getFriendCount(
				userId1);
		int countRes2 = BuddyByAddTimeCacheAdapter.getInstance().getFriendCount(
				userId2);
		Map<Integer, Integer> countResBatch = BuddyByAddTimeCacheAdapter
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

class BuddyByAddTimeLoaderAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerBuddyByAddTimeLoader";
	private static int _interval = 120;

	private BuddyByAddTimeLoaderAdapter() {
		super(ctr_endpoints, _interval);
	}

	private static BuddyByAddTimeLoaderAdapter _instance = new BuddyByAddTimeLoaderAdapter();

	public static BuddyByAddTimeLoaderAdapter getInstance() {
		return _instance;
	}

	public void reload(int userId) {
		try {
			getBuddyByAddTimeLoaderOneway(userId).reload(userId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error("BuddyCacheByAddTimeLoader timeout ");
		}
	}

	protected BuddyByAddTimeLoaderPrx getBuddyByAddTimeLoaderOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			BuddyByAddTimeLoaderPrx prx = BuddyByAddTimeLoaderPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}
}
