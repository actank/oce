package com.xiaonei.xce.socialgraph.friendcluster.impl;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Random;
import java.util.Map.Entry;

import com.xiaonei.xce.socialgraph.SocialGraphAdapter;
import com.xiaonei.xce.socialgraph.friendcluster.FriendClusterCacheAdapter;

import xce.clusterstate.ReplicatedClusterAdapter;
import xce.socialgraph.FriendClusterCachePrx;
import xce.socialgraph.FriendClusterCachePrxHelper;
import xce.socialgraph.FriendClusterData;

public class FriendClusterCacheAdapterImpl extends ReplicatedClusterAdapter
		implements FriendClusterCacheAdapter {

	private static final String endpoints = "ControllerFriendClusterCache";

	private static int interval = 120;

	private static FriendClusterCacheAdapterImpl instance = null;

	private static final int TIME_OUT = 2000;

	private static Random random = new Random(47);

	private static final int RANDOM_INT_RANGE = 10;

	public static FriendClusterCacheAdapterImpl getInstance() {
		if (instance == null) {
			synchronized (FriendClusterCacheAdapterImpl.class) {
				if (instance == null) {
					try {
						instance = new FriendClusterCacheAdapterImpl();
					} catch (Exception e) {
						e.printStackTrace();
					}
				}

			}
		}
		return instance;
	}

	private FriendClusterCacheAdapterImpl() {
		super(endpoints, interval);
	}

	private FriendClusterCachePrx getFriendClusterCachePrxTwoWay() {
		Ice.ObjectPrx prx0 = getValidProxy(random.nextInt(RANDOM_INT_RANGE),
				TIME_OUT);
		if (prx0 != null) {
			FriendClusterCachePrx prx = FriendClusterCachePrxHelper
					.uncheckedCast(prx0.ice_timeout(TIME_OUT).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	private FriendClusterCachePrx getFriendClusterCachePrxOneWay() {
		Ice.ObjectPrx prx0 = getValidProxy(random.nextInt(RANDOM_INT_RANGE),
				TIME_OUT);
		if (prx0 != null) {
			FriendClusterCachePrx prx = FriendClusterCachePrxHelper
					.uncheckedCast(prx0.ice_timeout(TIME_OUT).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}

	private FriendClusterData getFriendClusterData(List<Integer> idList) {
		FriendClusterCachePrx proxy = getFriendClusterCachePrxTwoWay();
		FriendClusterData friendClusterData = null;
		try {
			if (proxy != null) {
				friendClusterData = proxy.GetFriendClusterData(idList);
			} else {
				System.err.println("proxy is null");
			}

		} catch (Exception e) {
			e.printStackTrace();
		}

		if (friendClusterData == null) {
			return new FriendClusterData();
		}
		return friendClusterData;

	}

	public List<Integer> getFriendClusterPeopleForGuide(
			List<Integer> userIdList, int limit) {
		if (limit <= 0 || userIdList == null || userIdList.isEmpty()) {
			return new ArrayList<Integer>();
		}

		FriendClusterData friendClsuterData = getFriendClusterData(userIdList);
		Map<String, CountData> clusterCountMap = new HashMap<String, CountData>();

		for (List<String> cnameList : friendClsuterData.data1.values()) {
			for (String cname : cnameList) {
				if (clusterCountMap.containsKey(cname)) {
					clusterCountMap.get(cname).increase();
				} else {
					clusterCountMap.put(cname, new CountData(cname));
				}
			}
		}

		List<CountData> sortCountData = new ArrayList<CountData>(
				clusterCountMap.values());
		Collections.sort(sortCountData);

		// 开始准备返回的数据了
		List<Integer> ret = new ArrayList<Integer>();
		int count = 0;
		for (CountData c : sortCountData) {
			if (friendClsuterData.data2.containsKey(c.name)) {
				List<Integer> cidList = friendClsuterData.data2.get(c.name);
				for (int i = 0; i < cidList.size() && count != limit; ++i, ++count) {
					ret.add(cidList.get(i));
				}
				if (count >= limit) {
					break;
				}
			}
		}
		ret.removeAll(userIdList);
		return ret;
	}

	@Override
	public List<String> getClusterNames(int uid) {
		List<String> ret = new ArrayList<String>();
		FriendClusterCachePrx proxy = getFriendClusterCachePrxTwoWay();
		if (proxy == null) {
			System.err.println("proxy is null");
		} else {
			String[] strs = null;
			try {
				strs = proxy.GetC(uid);
				if (strs != null && strs.length != 0) {
					for (String s : strs) {
						ret.add(s);
					}
				}
			} catch (Exception e) {
				e.printStackTrace();
			}

		}
		return ret;
	}

	@Override
	public List<Integer> getClusterUsers(String clusterName) {
		List<Integer> ret = new ArrayList<Integer>();
		FriendClusterCachePrx proxy = getFriendClusterCachePrxTwoWay();
		if (proxy == null) {
			System.err.println("proxy is null");
		} else {
			int[] ids = null;
			try {
				ids = proxy.GetU(clusterName);
				if (ids != null && ids.length != 0) {
					for (int i = 0; i < ids.length; ++i) {
						ret.add(ids[i]);
					}
				}
			} catch (Exception e) {
				e.printStackTrace();
			}

		}
		return ret;
	}

	/*
	 * public static void main(String[] args) { List<Integer> userIdList = new
	 * ArrayList<Integer>(); userIdList.add(276403690);
	 * System.out.println(SocialGraphAdapter.getFriendClusterCacheAdapter()
	 * .getFriendClusterPeopleForGuide(userIdList, 2)); System.exit(0); }
	 */

	private static class CountData implements Comparable<CountData> {

		private String name;

		private int count;

		public CountData(String name) {
			this.name = name;
			this.count = 0;
		}

		public void increase() {
			++count;
		}

		public int getCount() {
			return count;
		}

		@Override
		public int compareTo(CountData o) {
			return o.count - this.count;
		}

	}

}
