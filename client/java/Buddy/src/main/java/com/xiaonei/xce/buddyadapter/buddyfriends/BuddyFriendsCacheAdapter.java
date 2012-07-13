package com.xiaonei.xce.buddyadapter.buddyfriends;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.PriorityQueue;
import java.util.Map.Entry;

import xce.buddy.BuddyFriendsCacheManagerPrx;
import xce.buddy.BuddyFriendsCacheManagerPrxHelper;
import xce.clusterstate.ReplicatedClusterAdapter;
import xce.util.tools.MathUtil;
import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.XceStat;

/**
 * BuddyFriendsCacheAdapter
 * 
 * @author yuyang(yang.yu@opi-corp.com)
 * 
 */

public class BuddyFriendsCacheAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerBuddyFriendsCache";
	private static int _interval = 120;
	private static int timeout = 550;

	private BuddyFriendsCacheAdapter() {
		super(ctr_endpoints, _interval);
	}

	private static BuddyFriendsCacheAdapter _instance = new BuddyFriendsCacheAdapter();

	/**
	 * @return
	 */
	public static BuddyFriendsCacheAdapter getInstance() {
		return _instance;
	}

	/**
	 * @param userId
	 * @return
	 */
	public int getFriendCount(int userId) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(),
				"BuddyFriendsCache.getFriendCount " + userId);
		String prxstr = "NULL Prx";
		try {
			BuddyFriendsCacheManagerPrx prx = getBuddyFriendsCacheManagerTwoway(userId);
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
				"BuddyFriendsCache getFriendCounts start:" + userIds.get(0) + " size:"
						+ userIds.size());
		for (int i = 0; i < cluster; ++i) {
			List<Integer> subUserIds = splitTargets.get(i);
			if (subUserIds.isEmpty()) {
				continue;
			}
			try {
				Map<Integer, Integer> subTargets = getBuddyFriendsCacheManagerTwoway(i)
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

	/**
	 * @param userId
	 * @param begin
	 * @param limit
	 * @return
	 */
	public List<Integer> getFriendListAsc(int userId, int begin, int limit) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(),
				"BuddyFriendsCache.getFriendListAsc " + userId);
		List<Integer> result = new ArrayList<Integer>();
		String prxstr = "NULL Prx";
		try {
			BuddyFriendsCacheManagerPrx prx = getBuddyFriendsCacheManagerTwoway(userId);
			prxstr = prx.ice_toString();
			int[] ret = prx.getFriendList(userId, begin, limit);
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

	/**
	 * @param userIds
	 * @return
	 */
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
				"BuddyFriendsCache getFriendListsAsc start:" + userIds.get(0)
						+ " size:" + userIds.size());
		for (int i = 0; i < cluster; ++i) {
			List<Integer> subUserIds = splitTargets.get(i);
			if (subUserIds.isEmpty()) {
				continue;
			}
			try {
				Map<Integer, int[]> subTargets = getBuddyFriendsCacheManagerTwoway(i)
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

	/**
	 * @param hostId
	 * @param guestId
	 * @return
	 */
	public boolean isFriendsFriend(int hostId, int guestId) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(),
				"BuddyFriendsCache.isFriendsFriend " + hostId + " " + guestId);
		String prxstr = "NULL Prx";
		boolean result = false;
		try {
			BuddyFriendsCacheManagerPrx prx = getBuddyFriendsCacheManagerTwoway(hostId);
			prxstr = prx.ice_toString();
			result = prx.isFriendsFriend(hostId, guestId);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".isFriendsFriend " + hostId + " "
							+ guestId + " Ice.ConnectTimeoutException " + prxstr);
			stat.endException(e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".isFriendsFriend " + hostId + " "
							+ guestId + " Ice.TimeoutException " + prxstr);
			stat.endException(e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".isFriendsFriend " + hostId + " "
							+ guestId + " " + prxstr, e);
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return result;
	}

	/***
	 * test which users in ids are friends' friends of hostId
	 * 
	 * @param hostId
	 * @param ids
	 * @return Map<Integer, Integer> result, if an id in ids is friends' friend,
	 *         result[id] = 1 else result[id] = 0
	 */
	public Map<Integer, Integer> isFriendsFriendBatch(int hostId,
			List<Integer> ids) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(),
				"BuddyFriendsCache.isFriendsFriends " + hostId
						+ " potentialFriends size = " + ids.size());
		String prxstr = "NULL Prx";
		Map<Integer, Integer> result = new HashMap<Integer, Integer>();
		try {
			BuddyFriendsCacheManagerPrx prx = getBuddyFriendsCacheManagerTwoway(hostId);
			prxstr = prx.ice_toString();
			int[] int_ids = new int[ids.size()];
			for (int i = 0, size = ids.size(); i < size; ++i) {
				int_ids[i] = ids.get(i);
			}
			result = prx.isFriendsFriendBatch(hostId, int_ids);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".isFriendsFriends " + hostId + " "
							+ ids.size() + " Ice.ConnectTimeoutException " + prxstr);
			stat.endException(e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".isFriendsFriends " + hostId + " "
							+ ids.size() + " Ice.TimeoutException " + prxstr);
			stat.endException(e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".isFriendsFriends " + hostId + " "
							+ ids.size() + " " + prxstr, e);
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return result;
	}

	/**
	 * @param hostId
	 * @param guestId
	 * @return
	 */
	public List<Integer> getCommonFriends(int hostId, int guestId) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(),
				"BuddyFriendsCache.getCommonFriends " + hostId + " " + guestId);
		String prxstr = "NULL Prx";
		List<Integer> result = new ArrayList<Integer>();

		try {
			BuddyFriendsCacheManagerPrx prx = getBuddyFriendsCacheManagerTwoway(hostId);
			prxstr = prx.ice_toString();
			int[] ret = prx.getCommonFriends(hostId, guestId);
			for (int one : ret) {
				result.add(one);
			}
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getCommonFriends " + hostId + " "
							+ guestId + " Ice.ConnectTimeoutException " + prxstr);
			stat.endException(e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getCommonFriends " + hostId + " "
							+ guestId + " Ice.TimeoutException " + prxstr);
			stat.endException(e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getCommonFriends " + hostId + " "
							+ guestId + " " + prxstr, e);
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return result;
	}

	/**
	 * @param idpairs
	 * @return
	 */
	public Map<String, List<Integer>> getCommonFriendsBatch(
			Map<Integer, Integer> idpairs) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(),
				"BuddyFriendsCache.getCommonFriendsBatch " + idpairs.size());
		String prxstr = "NULL Prx";
		Map<String, int[]> resultTmp = new HashMap<String, int[]>();
		Map<String, List<Integer>> result = new HashMap<String, List<Integer>>();
		try {
			BuddyFriendsCacheManagerPrx prx = getBuddyFriendsCacheManagerTwoway(0);
			prxstr = prx.ice_toString();
			resultTmp = prx.getCommonFriendsBatch(idpairs);
			for (Entry<String, int[]> pair : resultTmp.entrySet()) {
				List<Integer> list = new ArrayList<Integer>();
				for (int one : pair.getValue()) {
					list.add(one);
				}
				result.put(pair.getKey(), list);
			}
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getCommonFriendsBatch "
							+ idpairs.size() + " Ice.ConnectTimeoutException " + prxstr);
			stat.endException(e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getCommonFriendsBatch "
							+ idpairs.size() + " Ice.TimeoutException " + prxstr);
			stat.endException(e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getCommonFriendsBatch "
							+ idpairs.size() + " " + prxstr, e);
			stat.endException(e);
		} finally {
			stat.endFinally();
		}

		return result;
	}

	/**
	 * @param userId
	 * @return
	 */
	protected BuddyFriendsCacheManagerPrx getBuddyFriendsCacheManagerTwoway(
			int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, timeout);
		if (prx0 != null) {
			BuddyFriendsCacheManagerPrx prx = BuddyFriendsCacheManagerPrxHelper
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
	protected BuddyFriendsCacheManagerPrx getBuddyFriendsCacheManagerOneway(
			int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, timeout);
		if (prx0 != null) {
			BuddyFriendsCacheManagerPrx prx = BuddyFriendsCacheManagerPrxHelper
					.uncheckedCast(prx0.ice_oneway());
			return prx;
		} else {
			return null;
		}
	}

	public static void main(String[] args) {
		// 用一万组数据测试Buddy.isFriendsFriendBatch接口的性能
		try {
			PriorityQueue<Integer> biggest = new PriorityQueue<Integer>();
			String dataPath = "/home/shangmin/test/user_friends";
			BufferedReader br = new BufferedReader(new FileReader(new File(dataPath)));
			long timecost = 0;
			long totalLen = 0;
			long totalTimes = 0;
			String line = "";
			List<Integer> friendsfriends = new ArrayList<Integer>();
			while ((line = br.readLine()) != null) {
				String[] nums = line.split(" ");
				int userId = Integer.valueOf(nums[0]);
				if (friendsfriends.size() > 0) {
					totalTimes++;
					totalLen += friendsfriends.size();
					long start = System.currentTimeMillis();
					Map<Integer, Integer> result = BuddyFriendsCacheAdapter.getInstance()
							.isFriendsFriendBatch(userId, friendsfriends);
					long end = System.currentTimeMillis();
					int costTime = (int) (end - start);
					if (biggest.size() < 10) {
						biggest.add(costTime);
					} else if (costTime > biggest.peek()) {
						biggest.poll();
						biggest.add(costTime);
					}
					timecost += (end - start);
					int count = 0;
					for (Integer key : result.keySet()) {
						if (result.get(key) == 1) {
							count++;
						}
					}
					// if(count > 0) {
					// System.out.println("total tried : " + result.size() + " " + count +
					// " friends in friendsfriends");
					// }
					if (totalTimes == 10000) {
						break;
					}
				}
				friendsfriends.clear();
				for (int i = 2; i < nums.length; ++i) {
					friendsfriends.add(Integer.valueOf(nums[i]));
				}
			}
			br.close();
			while (biggest.size() > 0) {
				System.out.println(biggest.poll());
			}
			System.out
					.println("average cost of invoking isFriendsFriendBatch function is "
							+ timecost * 1.0 / totalTimes + " ms and total lenth is "
							+ totalLen * 1.0 / totalTimes + " tried " + totalTimes);
			System.exit(0);
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
