package com.xiaonei.xce.usercache.impl;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Map.Entry;

import xce.clusterstate.ReplicatedClusterAdapter;
import xce.usercache.UserCacheData3;
import xce.usercache.UserCacheManagerPrx;
import xce.usercache.UserCacheManagerPrxHelper;
import xce.util.tools.MathUtil;
import Ice.ConnectTimeoutException;
import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.XceStat;
import com.xiaonei.xce.usercache.UserCache;
import com.xiaonei.xce.usercache.UserCacheFactory;
import com.xiaonei.xce.usercache.UserCacheInvoker;
import com.xiaonei.xce.vipiconinfo.DefaultVipIconInfo;
import com.xiaonei.xce.vipiconinfo.DefaultVipIconInfoFactory;
import com.xiaonei.xce.vipiconinfo.VipIconInfo;
import com.xiaonei.xce.vipiconinfo.VipIconInfoAdapter;

public final class IceUserCacheAdapter<T extends UserCache> extends
    ReplicatedClusterAdapter implements UserCacheInvoker<T> {
  private static final String ctr_endpoints = "ControllerUserCacheN";
  private UserCacheFactory<T> _factory;
  private static int _interval = 120;

  private static final int MAX_GET_SIZE = 1000;
  
  private VipIconInfoAdapter<DefaultVipIconInfo> _vipIconInfoAdapter;
 
  private VipIconInfoAdapter<DefaultVipIconInfo> getVipIconInfoAdapter()
  {
	  if( _vipIconInfoAdapter == null )
	  {
		  synchronized( this )
		  {
			  if( _vipIconInfoAdapter == null )
			  {
				  _vipIconInfoAdapter  = new VipIconInfoAdapter<DefaultVipIconInfo>(new DefaultVipIconInfoFactory());
				  return _vipIconInfoAdapter;
			  }
		  }
	  }
	  return _vipIconInfoAdapter;
  }


  public IceUserCacheAdapter(UserCacheFactory<T> factory) {
    super(ctr_endpoints, _interval);
    _factory = factory;
  }

  public Map<Integer, T> getUserCacheMap(List<Integer> userIds) {
    Map<Integer, T> allResults = new HashMap<Integer, T>();

    // 为空直接返回
    if (userIds.isEmpty()) {
      XceStat.getLogger().warn(
          this.getClass().getName() + ".getUserCacheMap " + " idSize=0");
      return allResults;
    }

    // 过滤重复id
    Set<Integer> idsSet = new HashSet<Integer>(userIds);
    List<Integer> finalIdsList = new ArrayList<Integer>();
    finalIdsList.addAll(idsSet);
    if (userIds.size() != finalIdsList.size()) {
      XceStat.getLogger().warn(
          this.getClass().getName() + ".getUserCacheMap "
              + " there are duplicate ids inIdsSize = " + userIds.size()
              + "outIdsSize = " + finalIdsList.size());
    }

    // 按cluster进行散id
    int cluster = getCluster();
    Map<Integer, List<Integer>> splitIdsLists = new HashMap<Integer, List<Integer>>();
    for (int i = 0; i < cluster; ++i) {
      splitIdsLists.put(i, new ArrayList<Integer>());
    }
    for (Integer i : finalIdsList) {
      splitIdsLists.get(i % cluster).add(i);
    }

    // 对每个mod取UserCache
    for (Entry<Integer, List<Integer>> entry : splitIdsLists.entrySet()) {
      if (entry.getValue().size() > MAX_GET_SIZE) {
        XceStat.getLogger().error(
            this.getClass().getName() + ".getUserCacheMap "
                + " there are too many ids in one get idSize = "
                + entry.getValue().size());
      }
      int times = (int) Math.ceil((double) entry.getValue().size()
          / (double) MAX_GET_SIZE);
      int idsSize = entry.getValue().size();
      for (int i = 0; i < times; ++i) {
        int startIndex = i * MAX_GET_SIZE;
        int endIndex = startIndex + MAX_GET_SIZE;
        if (endIndex > idsSize) {
          endIndex = idsSize;
        }
        List<Integer> tempList = entry.getValue().subList(startIndex, endIndex);
        UserCacheManagerPrx prx = null;
        try {
          prx = getUserCacheManager(entry.getKey());
          Map<Integer, UserCacheData3> subTargets = prx
              .getUserCacheMap3(MathUtil.list2array(tempList));
          for (Entry<Integer, UserCacheData3> obj : subTargets.entrySet()) {
            T o = _factory.createUserCache();
            o.setBase(obj.getValue());
            allResults.put(o.getId(), o);
          }
        } catch (ConnectTimeoutException e) {
          Oce.getLogger().error(
              this.getClass().getName() + ".getUserCacheMap [" + prx
                  + "] idSize=" + tempList.size()
                  + " Ice.ConnectTimeoutException");
        } catch (TimeoutException e) {
          Oce.getLogger().error(
              this.getClass().getName() + ".getUserCacheMap [" + prx
                  + "] idSize=" + tempList.size() + " Ice.TimeoutException");
        } catch (java.lang.Throwable e) {
          Oce.getLogger().error(
              this.getClass().getName() + ".getUserCacheMap [" + prx
                  + "] idSize=" + tempList.size(), e);
        }
      }
    }
    return allResults;
  }

  @Deprecated
  public List<T> getUserCacheList(List<Integer> userIds) {
    /* re-order the result from map */
    TimeCost cost = TimeCost.begin(XceStat.getLogger(), "GetUserCacheList");
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
      UserCacheManagerPrx prx = UserCacheManagerPrxHelper.uncheckedCast(prx0
          .ice_timeout(300).ice_twoway());
      return prx;
    } else {
      return null;
    }
  }

  protected UserCacheManagerPrx getUserCacheManagerOneway(int userId) {
    Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
    if (prx0 != null) {
      UserCacheManagerPrx prx = UserCacheManagerPrxHelper.uncheckedCast(prx0
          .ice_timeout(300).ice_oneway());
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
      IceUserCacheAdapter<MyUserCache> adapter = new IceUserCacheAdapter<MyUserCache>(
          new MyUserCacheFactory());
      List<Integer> ids = new ArrayList<Integer>();
      for (String arg : args) {
        ids.add(Integer.valueOf(arg));
      }
      Map<Integer, MyUserCache> map = adapter.getUserCacheMap(ids);
      for (Entry<Integer, MyUserCache> entry : map.entrySet()) {
        System.out.println(entry.getKey() + " " + entry.getValue().getName());
        System.out.println("gender: " + entry.getValue().getGender());
      }
    } catch (Exception e) {
      Oce.getLogger().error(e);
    }
    System.exit(0);
  }

@Override
public Map<Integer, T> getUserCacheMap(List<Integer> userIds,
		List<Integer> types) {
	// TODO Auto-generated method stub
	Map<Integer, T> res = getUserCacheMap(userIds);
	List<Integer> vipIds = new ArrayList<Integer>();
	for(Entry<Integer, T> ent : res.entrySet()) {
		if(ent.getValue().isVipMember()) {
			vipIds.add(ent.getKey());
		}
	}
  
  if(vipIds.isEmpty()) {
    return res;
  }
	
	Map<Integer, List<DefaultVipIconInfo>> vipIconRes = getVipIconInfoAdapter()
			.getVipIconInfoSeqMap(vipIds, types);
	for (Entry<Integer, List<DefaultVipIconInfo>> ent : vipIconRes
			.entrySet()) {
		List<VipIconInfo> vList = new ArrayList<VipIconInfo>();
		vList.addAll(ent.getValue());
		res.get(ent.getKey()).setVipIconInfoList(vList);
	}
	return res;
}
}
