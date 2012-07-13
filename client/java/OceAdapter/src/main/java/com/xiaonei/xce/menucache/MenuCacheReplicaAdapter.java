package com.xiaonei.xce.menucache;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Vector;

import xce.clusterstate.ClientInterface;
import xce.menu.CacheType;
import xce.menu.MenuCacheManagerPrx;
import xce.menu.MenuCacheManagerPrxHelper;
import xce.menu.MenuCacheRecord;
import xce.serverstate.ServerStateInfo;
import Ice.ObjectPrx;

import com.xiaonei.xce.log.Oce;

public class MenuCacheReplicaAdapter {

	private MenuCacheReplicaAdapter() {

	}

	private static final String endpoints = "ControllerMenuCacheN";
	private static int interval = 120;

	private static MenuCacheReplicaAdapter _instance = new MenuCacheReplicaAdapter();

	private static ClientInterface _csClient;

	static {
		try{
			_csClient  = new ClientInterface(endpoints,
					interval);
		}catch(Throwable e){
			e.printStackTrace();
		}
	}

	public static MenuCacheReplicaAdapter getInstance() {
		return _instance;
	}
	
	private MenuCacheRecord getMenuCacheRecord(int userId, CacheType type, int[] appIds) {
	  MenuCacheRecord record = new MenuCacheRecord();
	  record.userId = userId;
	  record.type = type;
	  record.infoArray = appIds;
	  return record;
	}
	
	private MenuCacheRecord[] getMenuCacheRecords(int userId, final Map<CacheType, int[]> menuCaches) {
	  MenuCacheRecord[] records =  new MenuCacheRecord[2];
	  records[0] = getMenuCacheRecord(userId, CacheType.FAVORITE, new int[0]);
	  List<Integer> uniqueAppIds = new ArrayList<Integer>();
	  uniqueAppIds.add(new Integer(166607));
	  uniqueAppIds.add(new Integer(166613));
	  uniqueAppIds.add(new Integer(166614));
	  uniqueAppIds.add(new Integer(166615));
	  uniqueAppIds.add(new Integer(166616));
	  uniqueAppIds.add(new Integer(166617));
	  uniqueAppIds.add(new Integer(166618));
	  CacheType[] cacheTypes = new CacheType[]{CacheType.FAVORITE, CacheType.RECENTLY};
	  for (CacheType type : cacheTypes) {
  	  if (menuCaches.containsKey(type)) {
  	    int[] appIds = menuCaches.get(type);
  	    for (int appId : appIds) {
  	      if (!uniqueAppIds.contains(new Integer(appId))) {
  	        uniqueAppIds.add(new Integer(appId));
  	      }
  	    }
  	  }
	  }
	  int[] recentAppIds = new int[uniqueAppIds.size()];
	  for (int i = 0; i < uniqueAppIds.size(); ++i) {
	    recentAppIds[i] = uniqueAppIds.get(i);
	  }
	  records[1] = getMenuCacheRecord(userId, CacheType.RECENTLY, recentAppIds);
	  return records;
	}
	
	/***
	 * 这个函数是专门给v6版本做的，作用是将原来的Favorite Menu和Recent Menu全部移到Recent Menu中，然后将
	 * 新的ugc的7个appId插到Recent Menu开始的位置
	 * ------->别人不能乱调这个函数
	 */
	public void resetMenuCache(int userId) {
	  Map<CacheType, int[]> menuCaches = getAll(userId);
	  MenuCacheRecord[] records = getMenuCacheRecords(userId, menuCaches);
	  setBatch(records);
  }
	
	public void resetMenuCacheBatch(int[] userIds) {
	  for (int userId : userIds) {
	    resetMenuCache(userId);
	  }
	}

	public Map<CacheType, int[]> getAll(int userId) {
		Map<CacheType, int[]> result = new HashMap<CacheType, int[]>();
		MenuCacheManagerPrx prx= null;
		try {
	//		TimeCost cost = TimeCost.begin("getAll[" + userId + "]");
			prx=getMenuCacheManager(userId);
			result = prx.getAll(userId);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".getAll   [ " + prx + " ] id="   + userId + " Ice.ConnectTimeoutExceptio");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".getAll   [ " + prx + " ] id="   + userId + " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".getAll " + userId, e);
		}
		return result;
	}

	public Map<CacheType, int[]> get(int userId, CacheType[] typeArray) {
		Map<CacheType, int[]> result = new HashMap<CacheType, int[]>();
		MenuCacheManagerPrx prx= null;
		try {
	//		TimeCost cost = TimeCost.begin("get[" + userId + "]");
			prx=getMenuCacheManager(userId);
			result = prx.get(userId, typeArray);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".get   [ " + prx + " ] id="   + userId + " Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".get   [ " + prx + " ] id="   + userId + " Ice.TimeoutException");
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
				} catch (Ice.ConnectTimeoutException e) {
					Oce.getLogger().error(this.getClass().getName() + ".set "   + userId + " Ice.ConnectTimeoutException");
				} catch (Ice.TimeoutException e) {
					Oce.getLogger().error(this.getClass().getName() + ".set "   + userId + " Ice.TimeoutException");
				} catch (Exception e) {
					Oce.getLogger().error(this.getClass().getName() + ".set "   + userId, e);				
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
						} catch (Ice.ConnectTimeoutException e) {
							Oce.getLogger().error(this.getClass().getName() + ".setBatch" + " Ice.ConnectTimeoutException");											
						} catch (Ice.TimeoutException e) {
							Oce.getLogger().error(this.getClass().getName() + ".setBatch" + " Ice.TimeoutException");											
						} catch (Exception e) {
							Oce.getLogger().error(this.getClass().getName() + ".setBatch", e);				
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
	
	//判断尾号为mod的服务是否是valid的
	public void isValid(int mod) {
	   ServerStateInfo[] states = getMenuCacheManagerAllOneWay(mod);
	    if(states==null)
	      return;
	    for (int i = 0; i < states.length; ++i) {
	      if (states[i].prx != null) {      
	        try {	          
	          System.out.println(states[i].prx.ice_getAdapterId() + " " + MenuCacheManagerPrxHelper.uncheckedCast(
	              states[i].prx.ice_timeout(300)).isValid());
	        } catch (Exception ex) {
	          System.out.println(ex);
	        }
	      }
	    }
	}
	
	/***
	 * 将尾号为mod的服务全部setValid
	 * @param mod
	 */
	public void setAllValid(int mod, boolean valid) {
    ServerStateInfo[] states = getMenuCacheManagerAllOneWay(mod);
    if(states==null)
      return;
    for (int i = 0; i < states.length; ++i) {
      if (states[i].prx != null) {      
        try {           
          MenuCacheManagerPrx prx = MenuCacheManagerPrxHelper.uncheckedCast(
              states[i].prx.ice_timeout(300));
          if(prx.isValid() != valid) {
            prx.setValid(valid);
          }
        } catch (Exception ex) {
          System.out.println(ex);
        }
      }
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
	  //MenuCacheReplicaAdapter.getInstance().isValid(0);
	  //MenuCacheReplicaAdapter.getInstance().setAllValid(0, true);
	  //MenuCacheReplicaAdapter.getInstance().resetMenuCache(35178);
	  CacheType[] cacheTypes = new CacheType[]{CacheType.FAVORITE, CacheType.RECENTLY};
	  Map<CacheType, int[]> recentMenus = MenuCacheReplicaAdapter.getInstance().get(35178, cacheTypes);
	  for (Map.Entry<CacheType, int[]> menus : recentMenus.entrySet()) {
	    System.out.println(menus.getKey() + ":");
	    for (int menuId : menus.getValue()) {
	      System.out.println(menuId);
	    }
	  }
	  System.exit(0);
	}
}
