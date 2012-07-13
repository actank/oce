package com.xiaonei.xce.userstatecache;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.usercache.DefaultUserCache;
import com.xiaonei.xce.usercache.DefaultUserCacheFactory;
import com.xiaonei.xce.usercache.UserCacheAdapter;

public class UserStateCacheAdapter {

  private static UserStateCacheAdapter _instance = new UserStateCacheAdapter();
  private UserCacheAdapter<DefaultUserCache> _usercacheinstance;

  private UserStateCacheAdapter() {
    _usercacheinstance = new UserCacheAdapter<DefaultUserCache>(
        new DefaultUserCacheFactory());
  }

  public static UserStateCacheAdapter getInstance() {
    return _instance;
  }

  public Map<java.lang.Integer, UserStateCache> getUserStateCacheList(
      List<Integer> userids) {

    try {
      Map<java.lang.Integer, UserStateCache> res = new HashMap<Integer, UserStateCache>();
      Map<java.lang.Integer, DefaultUserCache> tres = _usercacheinstance
          .getUserCacheMap(userids);
      for (Entry<java.lang.Integer, DefaultUserCache> e : tres.entrySet()) {
        res.put(e.getKey(), new UserStateCache(e.getValue()));
      }
      return res;
    } catch (TimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".getUserStateCacheList " + " idSize="
              + userids.size() + " Ice.TimeoutException");
      throw e;
    }
  }

  public static void main(String[] args) {
    try {
      List<Integer> ids = new ArrayList<Integer>();
      ids.add(68126);
      Map<Integer, UserStateCache> map = UserStateCacheAdapter.getInstance()
          .getUserStateCacheList(ids);
      System.out.println("Size: " + map.size());
      for (Entry<Integer, UserStateCache> entry : map.entrySet()) {
        System.out.println("Id: " + entry.getValue().getId());
        System.out.println("Name: " + entry.getValue().getName());
        System.out.println("State: " + entry.getValue().getState());
        System.out.println("Vip: " + entry.getValue().isVipMember());
        System.out.println("Level: " + entry.getValue().getLevel());
      }
    } catch (Exception e) {
      e.printStackTrace();
    }
    System.exit(0);
  }
}
