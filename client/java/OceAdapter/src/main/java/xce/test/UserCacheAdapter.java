package xce.test;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import xce.usercache.UserCacheData3;
import xce.usercache.UserCacheManagerPrx;
import xce.usercache.UserCacheManagerPrxHelper;
import xce.util.tools.MathUtil;
import Ice.ConnectTimeoutException;
import Ice.TimeoutException;
import xce.util.tools.IPAddress;

import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.XceStat;

import xce.clusterstate.ReplicatedClusterAdapter;

public final class UserCacheAdapter<T extends UserCache> extends
		ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerUserCacheN";
	private UserCacheFactory<T> _factory;
	private static int _interval = 120;

	public UserCacheAdapter(UserCacheFactory<T> factory) {
		super(ctr_endpoints, _interval);
		_factory = factory;
	}

	public Map<Integer, T> getUserCacheMap(List<Integer> userIds) {
		/* split targets */
		Map<Integer, List<Integer>> splitTargets = new HashMap<Integer, List<Integer>>();
		int cluster = 1;
		for (int i = 0; i < cluster; ++i) {
			splitTargets.put(i, new ArrayList<Integer>());
		}
		for (int i : userIds) {
			splitTargets.get(i % cluster).add(i);
		}

		String ip = IPAddress.getLocalAddress();
		/* collect results */
		Map<Integer, T> allResults = new HashMap<Integer, T>();
		if (userIds.isEmpty())
			return allResults;
		TimeCost total = TimeCost.begin("GetUserCacheMap");
		try {
			for (int i = 0; i < cluster; ++i) {
				List<Integer> subUserIds = splitTargets.get(i);
				if (subUserIds.isEmpty())
					continue;
				try {
					Map<String, String> ctx = new HashMap<String,String>();
					ctx.put("IP", ip);
					TimeCost cost = TimeCost.begin("GetUserCacheList3[" + i + "]");
					UserCacheData3[] subTargets = getUserCacheManager(i)
							.getUserCacheList3(MathUtil.list2array(subUserIds),ctx);
					cost.endFinally();
					for (UserCacheData3 obj : subTargets) {
						T o = _factory.createUserCache();
						o.setBase(obj);
						allResults.put(o.getId(), o);
					}
				} catch(ConnectTimeoutException e){
					e.printStackTrace();
				} catch (TimeoutException e) {
					e.printStackTrace();
				} 
			}
		} finally {
			total.endFinally();
		}
		return allResults;
	}

	@Deprecated
	public List<T> getUserCacheList(List<Integer> userIds) {
		/* re-order the result from map */
		TimeCost cost = TimeCost.begin(XceStat.getLogger(),
				"GetUserCacheList");
		Map<Integer, T> allResults = getUserCacheMap(userIds);
		List<T> result = new ArrayList<T>();
		for (int userId : userIds) {
			T obj = allResults.get(userId);
			if (obj == null)
				continue;
			result.add(obj);
		}
		cost.endFinally();
		return result;
	}

	public void load(int id) {
		int userIds[] = new int[1];
		userIds[0] = id;
		getUserCacheManager(id).load(userIds);
	}

	protected UserCacheManagerPrx getUserCacheManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			UserCacheManagerPrx prx = UserCacheManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected UserCacheManagerPrx getUserCacheManagerOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			UserCacheManagerPrx prx = UserCacheManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}

	public static class MyUserCache extends UserCache {
	}

	public static class MyUserCacheFactory implements
			UserCacheFactory<MyUserCache> {
		public MyUserCache createUserCache() {
			return new MyUserCache();
		}
	}

	public static void main(String[] args) {
		try {
			UserCacheAdapter<MyUserCache> adapter = new UserCacheAdapter<MyUserCache>(
					new MyUserCacheFactory());
			List<Integer> ids=new ArrayList<Integer>();
			ids.add(30010);
			Map<Integer, MyUserCache> map=adapter.getUserCacheMap(ids);
			for(Entry<Integer,MyUserCache> entry:map.entrySet()){
				System.out.println(entry.getKey()+" "+entry.getValue().getName());
				System.out.println("gender: " + entry.getValue().getGender());
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		System.exit(0);
	}
}
