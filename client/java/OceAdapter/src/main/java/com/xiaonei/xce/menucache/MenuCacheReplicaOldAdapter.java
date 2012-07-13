package com.xiaonei.xce.menucache;

import java.util.HashMap;
import java.util.Map;
import java.util.Vector;
import java.util.Map.Entry;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;

import Ice.ConnectTimeoutException;
import Ice.ObjectPrx;
import Ice.TimeoutException;

import xce.serverstate.ServerStateInfo;

import xce.clusterstate.ClientInterface;
import xce.menu.MenuCacheManagerPrx;
import xce.menu.MenuCacheManagerPrxHelper;
import xce.menu.CacheType;
import xce.menu.MenuCacheRecord;

public class MenuCacheReplicaOldAdapter {

	private MenuCacheReplicaOldAdapter() {

	}

	private static final String endpoints = "ControllerMenuCache";
	private static int interval = 120;

	private static MenuCacheReplicaOldAdapter _instance = new MenuCacheReplicaOldAdapter();

	private static ClientInterface _csClient;

	static {
		try{
			_csClient  = new ClientInterface(endpoints,
					interval);
		}catch(Throwable e){
			e.printStackTrace();
		}
	}

	public static MenuCacheReplicaOldAdapter getInstance() {
		return _instance;
	}

	public Map<CacheType, int[]> getAll(int userId) {
		Map<CacheType, int[]> result = new HashMap<CacheType, int[]>();
		MenuCacheManagerPrx prx = null;
		try {
	//		TimeCost cost = TimeCost.begin("getAll[" + userId + "]");
			prx = getMenuCacheManager(userId);
			result = prx.getAll(userId);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".getAll prx = " + prx + " userId = " + userId + " Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".getAll prx = " + prx + " userId = " + userId + " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".getAll " + userId, e);
		}

		return result;
	}

	public Map<CacheType, int[]> get(int userId, CacheType[] typeArray) {
		Map<CacheType, int[]> result = new HashMap<CacheType, int[]>();
		MenuCacheManagerPrx prx = null;
		try {
	//		TimeCost cost = TimeCost.begin("get[" + userId + "]");
			prx = getMenuCacheManager(userId);
			result = prx.get(userId, typeArray);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".get prx = " + prx + " userId = " + userId + " Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".get prx = " + prx + " userId = " + userId + " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".get " + userId, e);
		}
		return result;
	}

	public void set(int userId, CacheType type, int[] appIds) {

		ServerStateInfo[] states = getMenuCacheManagerAllOneWay(userId);
		if(states==null)
			return;
		for (int i = 0; i < states.length; ++i) {
			if (states[i].prx != null) {

				try {
	//				TimeCost cost = TimeCost.begin("set[" + userId + "]");
					MenuCacheManagerPrxHelper.uncheckedCast(
							states[i].prx.ice_timeout(300).ice_oneway()).set(
							userId, type, appIds);
				} catch (ConnectTimeoutException e) {
					Oce.getLogger().error(
							this.getClass().getName() + ".set " + e);
				} catch (TimeoutException e) {
					Oce.getLogger().error(
							this.getClass().getName() + ".set " + e);
				}

			}
		}
	}

	public void setBatch(MenuCacheRecord[] records) {

		int cluster = _csClient.getCluster();
		if (cluster <= 0) {
			return;
		}
		Map<Integer, Vector<MenuCacheRecord>> recordGroup = new HashMap<Integer, Vector<MenuCacheRecord>>();
		for (int i = 0; i < records.length; ++i) {
			int mod = records[i].userId % cluster;
			if (recordGroup.containsKey(mod)) {
				recordGroup.get(mod).add(records[i]);
			} else {
				Vector<MenuCacheRecord> group = new Vector<MenuCacheRecord>();
				group.add(records[i]);
				recordGroup.put(mod, group);
			}
		}

		for (int mod = 0; mod < cluster; ++mod) {

			ServerStateInfo[] states = getMenuCacheManagerAllOneWay(mod);
			if (recordGroup.containsKey(mod)) {
				MenuCacheRecord[] themod = new MenuCacheRecord[recordGroup.get(
						mod).size()];
				themod = recordGroup.get(mod).toArray(themod);

				for (int i = 0; i < states.length; ++i) {
					if (states[i].prx != null) {

						try {
	//						TimeCost cost = TimeCost.begin("setBatch");
							MenuCacheManagerPrxHelper
									.uncheckedCast(
											states[i].prx.ice_timeout(300)
													.ice_oneway()).setBatch(
											themod);
						} catch (ConnectTimeoutException e) {
							Oce.getLogger().error(
									this.getClass().getName() + ".setBatch "
											+ e);
						} catch (TimeoutException e) {
							Oce.getLogger().error(
									this.getClass().getName() + ".setBatch "
											+ e);
						}

					}
				}
			}
		}
	}

	// 获取一个适应userid的服务的prx
	protected MenuCacheManagerPrx getMenuCacheManager(int userId) {

		Ice.ObjectPrx prx0 = _csClient.getValidProxy(userId);
		if (prx0 != null) {
			MenuCacheManagerPrx prx = MenuCacheManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	// 获取所有适应userid的服务的prx
	protected ServerStateInfo[] getMenuCacheManagerAllOneWay(int userid) {
		int cluster = _csClient.getCluster();
		if (cluster <= 0) {
			ServerStateInfo[] states = new ServerStateInfo[0];
			return states;
		} else {
			int mod = userid % cluster;
			return _csClient.getAvailableObjProxies().get(mod);
		}
	}

	protected boolean verifyProxy(ObjectPrx proxy) {
		return MenuCacheManagerPrxHelper.uncheckedCast(proxy).isValid();
	}

	public static void main(String[] args) {
		int i = 0;
		while (true) {

			try {
				Map<Integer, ServerStateInfo[]> nowPrx = _csClient
						.getAvailableObjProxies();// clusterState.getValidState().get(0);
				for (Entry<Integer, ServerStateInfo[]> entry : nowPrx
						.entrySet()) {
					ServerStateInfo[] infos = entry.getValue();
					for (ServerStateInfo info : infos) {
						MenuCacheManagerPrx prx = MenuCacheManagerPrxHelper
								.uncheckedCast(info.prx);
						System.out.println(prx.ice_toString());
						System.out.println(prx.isValid());
						if (prx.ice_toString().contains("10.3.20.194"))
							prx.setValid(true);
					}
				}
				int count = 10;
				while (count-- > 0) {
					ObjectPrx prx0 = _csClient.getValidProxy(0);
					System.out.println(prx0.ice_toString());
				}
				count = 10;
				while (count-- > 0) {
					ObjectPrx prx0 = _csClient.getValidProxy(1);
					System.out.println(prx0.ice_toString());
				}
				/*
				 * int userId = 487435686; // userId = 222525090;
				 * 
				 * // get Ice.ObjectPrx prx0 = _csClient.getProxy(userId); if (
				 * prx0 == null) { System.out.println("Prx is null"); } else {
				 * Map<CacheType, int[]> results =
				 * MenuCacheReplicaAdapter.getInstance().getAll(userId); for
				 * (CacheType key : results.keySet()) {
				 * System.out.println("CacheType : " + key.name() );
				 * System.out.println("\t" + results.get(key).length); for ( int
				 * j = 0; j < results.get(key).length; ++j ) {
				 * System.out.println(key.name() + " " + j + ":" +
				 * results.get(key)[j]); } } }
				 * 
				 * // set /* int[] appIds = new int[]{1,2,3,4,5,7,8,9,10,11,12};
				 * CacheType type = CacheType.FAVORITE;
				 * MenuCacheReplicaAdapter.getInstance().set(userId, type,
				 * appIds);
				 */

			} catch (Exception e) {
				e.printStackTrace();
			}

			System.exit(0);

			try {
				System.out.println("Loop : " + String.valueOf(i));
				i++;
				Thread.sleep(10 * 1000);
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}

}
