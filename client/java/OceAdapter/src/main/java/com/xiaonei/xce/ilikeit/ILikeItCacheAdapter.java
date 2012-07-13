package com.xiaonei.xce.ilikeit;

import java.util.HashMap;
import java.util.Vector;

import xce.clusterstate.ClientInterface;
import xce.clusterstate.ReplicatedClusterAdapter;
import xce.ilikeit.GlobalId;
import xce.ilikeit.ILikeItCachePrx;
import xce.ilikeit.ILikeItCachePrxHelper;
import xce.ilikeit.LikeInfo;
import xce.ilikeit.LikeInfo2;
import xce.ilikeit.LikeInfoSimple;
import xce.ilikeit.UserDetail;
import xce.ilikeit.UserDetail2;
import xce.serverstate.ServerStateInfo;
import xce.util.channel.Channel;

import java.util.Map;
import Ice.ConnectTimeoutException;
import Ice.TimeoutException;
import MyUtil.ObjectCachePrx;
import MyUtil.ObjectCachePrxHelper;

import com.xiaonei.xce.log.Oce;

public final class ILikeItCacheAdapter extends ReplicatedClusterAdapter {

  private static int _interval = 300;
  private static final String _ctlEndpoints = "ControllerILikeItCacheM";
  private static int _cluster = 1;

  private static ILikeItCacheAdapter _adapter = new ILikeItCacheAdapter();

  private ILikeItCachePrx[] prxs = new ILikeItCachePrx[_cluster];
  private ILikeItCachePrx[] prxsOneway = new ILikeItCachePrx[_cluster];

  public static ILikeItCacheAdapter instance() {
    return _adapter;
  }

  private ILikeItCacheAdapter() {
    super(_ctlEndpoints,_interval);
    for (int i = 0; i < _cluster; ++i) {
      prxs[i] = null;
      prxsOneway[i] = null;
    }
  }

  LikeInfo emptyInfo() {
    LikeInfo result = new LikeInfo();
    result.totalCount = 0;
    result.friendCount = 0;
    result.selfCount = 0;
    result.showStrangers = true;
    result.withFriendList = false;
    result.detailLimit = 0;
    result.detailList = new UserDetail[0];
    return result;
  }

  public LikeInfo getLikeDetail(String gidStr, int userid, int limit, boolean showStrangers) {

    LikeInfo result = emptyInfo();
    ILikeItCachePrx prx = null;
    try {
      GlobalId gidSlice = new GlobalId();
      com.xiaonei.xce.globalid.GlobalId gid = com.xiaonei.xce.globalid.GlobalId.fromString(gidStr);

      gidSlice.id = gid.getId();
      gidSlice.type = gid.getType();
      prx = getILikeItCache(gid.getId());
      if (prx != null) {
        LikeInfo tmp = prx.getLikeDetail(gidSlice, userid, limit, showStrangers);
        if (tmp != null) {
          result = tmp;
        }
      }
    } catch (Ice.ConnectTimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".getLikeDetail [" + prx + "] userid=" + userid + " Ice.ConnectTimeoutException");
    } catch (Ice.TimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".getLikeDetail [" + prx + "] userid=" + userid + " Ice.TimeoutException");
    } catch (Exception e) {
      Oce.getLogger().error(this.getClass().getName() + ".getLikeDetail [" + prx + "] userid=" + userid, e);
    }
    return result;
  };

  public LikeInfo getLikeDetail(com.xiaonei.xce.globalid.GlobalId gid, int userid, int limit, boolean showStrangers) {

    LikeInfo result = emptyInfo();
    ILikeItCachePrx prx = null;
    try {
      GlobalId gidSlice = new GlobalId();
      gidSlice.id = gid.getId();
      gidSlice.type = gid.getType();
      prx = getILikeItCache(gid.getId());
      if (prx != null) {
        LikeInfo tmp = prx.getLikeDetail(gidSlice, userid, limit, showStrangers);
        if (tmp != null) {
          result = tmp;
        }
      }
    } catch (Ice.ConnectTimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".getLikeDetail [" + prx + "] userid=" + userid + " Ice.ConnectTimeoutException");
    } catch (Ice.TimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".getLikeDetail [" + prx + "] userid=" + userid + " Ice.TimeoutException");
    } catch (Exception e) {
      Oce.getLogger().error(this.getClass().getName() + ".getLikeDetail [" + prx + "] userid=" + userid, e);
    }
    return result;
  };

  public Map<String, LikeInfo> getLikeDetailMapBatch(String[] gidStrs, int userid, int limit, int detailNum) {

    Map<String, LikeInfo> results = new HashMap<String, LikeInfo>();

    // 如果只散1份则不需分散
    if (_cluster == 1) {
      GlobalId[] gids = new GlobalId[gidStrs.length];
      for (int i = 0; i < gidStrs.length; ++i) {
        com.xiaonei.xce.globalid.GlobalId tmp = com.xiaonei.xce.globalid.GlobalId.fromString(gidStrs[i]);
        gids[i] = new GlobalId();
        if (tmp != null) {
          gids[i].id = tmp.getId();
          gids[i].type = tmp.getType();
        } else {
          gids[i].id = 0;
          gids[i].type = 0;
        }
      }

      ILikeItCachePrx prx = null;
      try {
        prx = getILikeItCache(0);
        if (prx != null) {
          LikeInfo[] resultSeq = prx.getLikeDetailBatch(gids, userid, limit, detailNum);

          if (resultSeq.length == gidStrs.length) {
            for (int i = 0; i < gidStrs.length; ++i) {
              results.put(gidStrs[i], resultSeq[i]);
            }
          } else {
            Oce.getLogger().error(
                this.getClass().getName() + ".getLikeDetailMapBatch [" + prx
                    + "] the length of result doesn't match that of gids");
          }
        }
      } catch (Ice.ConnectTimeoutException e) {
        Oce.getLogger().error(
            this.getClass().getName() + ".getLikeDetailMapBatch [" + prx + "] Ice.ConnectTimeoutException");
      } catch (Ice.TimeoutException e) {
        Oce.getLogger().error(this.getClass().getName() + ".getLikeDetailMapBatch [" + prx + "] Ice.TimeoutException");
      } catch (Exception e) {
        Oce.getLogger().error(this.getClass().getName() + ".getLikeDetailMapBatch [" + prx + "]", e);
      }
      return results;
    }

    // 如果散多份, 则需要分散
    Vector<int[]> vecOrder = new Vector<int[]>(); // 存储顺序
    Map<Integer, Vector<GlobalId>> gidGroups = new HashMap<Integer, Vector<GlobalId>>();
    Map<Integer, LikeInfo[]> mapResult = new HashMap<Integer, LikeInfo[]>();

    // 按照mod分组
    for (String key : gidStrs) {
      com.xiaonei.xce.globalid.GlobalId gid = com.xiaonei.xce.globalid.GlobalId.fromString(key);
      GlobalId gidSlice = new GlobalId();
      if (gid != null) {
        gidSlice.id = gid.getId();
        gidSlice.type = gid.getType();
      } else {
        gidSlice.id = 0;
        gidSlice.type = 0;
      }
      Long modL = gidSlice.id % _cluster;
      int mod = modL.intValue();
      if (gidGroups.containsKey(mod)) {
        gidGroups.get(mod).add(gidSlice);
      } else {
        Vector<GlobalId> group = new Vector<GlobalId>();
        group.add(gidSlice);
        gidGroups.put(mod, group);
      }
      int[] order = new int[2];
      order[0] = mod;
      order[1] = gidGroups.get(mod).size() - 1;
      vecOrder.add(order);
    }

    // 查询服务
    for (int mod : gidGroups.keySet()) {
      ILikeItCachePrx prx = null;
      try {
        GlobalId[] gidSlices = new GlobalId[gidGroups.get(mod).size()];
        prx = getILikeItCache(mod);
        LikeInfo[] resultSeq = prx.getLikeDetailBatch(gidGroups.get(mod).toArray(gidSlices), userid, limit, detailNum);
        mapResult.put(mod, resultSeq);
      } catch (Ice.ConnectTimeoutException e) {
        Oce.getLogger().error(
            this.getClass().getName() + ".getLikeDetailMapBatch [" + prx + "] Ice.ConnectTimeoutException");
      } catch (Ice.TimeoutException e) {
        Oce.getLogger().error(this.getClass().getName() + ".getLikeDetailMapBatch [" + prx + "] Ice.TimeoutException");
      } catch (Exception e) {
        Oce.getLogger().error(this.getClass().getName() + ".getLikeDetailMapBatch [" + prx + "]", e);
      }
    }

    // 组合结果
    for (int order = 0; order < vecOrder.size(); ++order) {
      int mod = vecOrder.get(order)[0];
      int pos = vecOrder.get(order)[1];
      LikeInfo result = null;
      if (mapResult.containsKey(mod)) {
        if (mapResult.get(mod).length > pos) {
          result = mapResult.get(mod)[pos];
          results.put(gidStrs[order], result);
        }
      }
    }
    return results;
  };

  public LikeInfo[] getLikeDetailBatch(String[] gidStrs, int userid, int limit, int detailNum) {

    Vector<int[]> vecOrder = new Vector<int[]>(); // 存储顺序
    Map<Integer, Vector<GlobalId>> gidGroups = new HashMap<Integer, Vector<GlobalId>>();
    Map<Integer, LikeInfo[]> mapResult = new HashMap<Integer, LikeInfo[]>();

    // 按照mod分组
    for (String key : gidStrs) {
      com.xiaonei.xce.globalid.GlobalId gid = com.xiaonei.xce.globalid.GlobalId.fromString(key);
      GlobalId gidSlice = new GlobalId();
      gidSlice.id = gid.getId();
      gidSlice.type = gid.getType();
      Long modL = gidSlice.id % _cluster;
      int mod = modL.intValue();
      if (gidGroups.containsKey(mod)) {
        gidGroups.get(mod).add(gidSlice);
      } else {
        Vector<GlobalId> group = new Vector<GlobalId>();
        group.add(gidSlice);
        gidGroups.put(mod, group);
      }
      int[] order = new int[2];
      order[0] = mod;
      order[1] = gidGroups.get(mod).size() - 1;
      vecOrder.add(order);
    }

    // 查询服务
    for (int mod : gidGroups.keySet()) {
      ILikeItCachePrx prx = null;
      try {
        GlobalId[] gidSlices = new GlobalId[gidGroups.get(mod).size()];
        prx = getILikeItCache(mod);
        LikeInfo[] results = prx.getLikeDetailBatch(gidGroups.get(mod).toArray(gidSlices), userid, limit, detailNum);
        mapResult.put(mod, results);
      } catch (Ice.ConnectTimeoutException e) {
        Oce.getLogger().error(
            this.getClass().getName() + ".getLikeDetailBatch [" + prx + "] Ice.ConnectTimeoutException");
      } catch (Ice.TimeoutException e) {
        Oce.getLogger().error(this.getClass().getName() + ".getLikeDetailBatch [" + prx + "] Ice.TimeoutException");
      } catch (Exception e) {
        Oce.getLogger().error(this.getClass().getName() + ".getLikeDetailBatch [" + prx + "] ", e);
      }
    }

    // 组合结果
    Vector<LikeInfo> vecResult = new Vector<LikeInfo>();
    for (int order = 0; order < vecOrder.size(); ++order) {
      int mod = vecOrder.get(order)[0];
      int pos = vecOrder.get(order)[1];
      LikeInfo result = null;
      if (mapResult.containsKey(mod)) {
        if (mapResult.get(mod).length > pos) {
          result = mapResult.get(mod)[pos];
        }
      }
      vecResult.add(result);
    }

    LikeInfo[] arrResult = new LikeInfo[vecResult.size()];
    return vecResult.toArray(arrResult);
  };

  public LikeInfoSimple getLikeInfoSimple(String gidStr, int limit) {

    ILikeItCachePrx prx = null;
    try {
      GlobalId gidSlice = new GlobalId();
      com.xiaonei.xce.globalid.GlobalId gid = com.xiaonei.xce.globalid.GlobalId.fromString(gidStr);

      gidSlice.id = gid.getId();
      gidSlice.type = gid.getType();
      prx = getILikeItCache(gid.getId());
      if (prx != null) {
        return prx.getLikeInfoSimple(gidSlice, limit);
      }
    } catch (Ice.ConnectTimeoutException e) {
      Oce.getLogger().error(this.getClass().getName() + ".getLikeInfoSimple [" + prx + "] Ice.ConnectTimeoutException");
    } catch (Ice.TimeoutException e) {
      Oce.getLogger().error(this.getClass().getName() + " [" + prx + "] Ice.TimeoutException");
    } catch (Exception e) {
      Oce.getLogger().error(this.getClass().getName() + " getLikeInfoSimple [" + prx + "] ", e);
    }
    return null;
  }

  public Map<String, LikeInfoSimple> getLikeInfoSimpleBatch(String[] gidStrs, int limit) {
    Map<String, LikeInfoSimple> result = new HashMap<String, LikeInfoSimple>();
    Map<Integer, Vector<String>> modMap = divideByMod(gidStrs);
    for (int mod : modMap.keySet()) {
      ILikeItCachePrx prx = null;
      try {
        prx = getILikeItCache(mod);
        Vector<String> gidStrsMod = modMap.get(mod);
        GlobalId[] gidSlices = strVector2gidArray(gidStrsMod);
        LikeInfoSimple[] infoArray = prx.getLikeInfoSimpleBatch(gidSlices, limit);
        if (infoArray.length == gidStrsMod.size()) {
          for (int i = 0; i < infoArray.length; ++i) {
            result.put(gidStrsMod.get(i), infoArray[i]);
          }
        }
      } catch (Ice.ConnectTimeoutException e) {
        Oce.getLogger().error(
            this.getClass().getName() + ".getLikeInfoSimpleBatch [" + prx + "] Ice.ConnectTimeoutException");
      } catch (Ice.TimeoutException e) {
        Oce.getLogger().error(this.getClass().getName() + " getLikeInfoSimpleBatch [" + prx + "] Ice.TimeoutException");
      } catch (Exception e) {
        Oce.getLogger().error(this.getClass().getName() + " getLikeInfoSimpleBatch [" + prx + "] ", e);
      }
    }
    return result;
  }

  public int isLiked(String gidStr, int uid) {
    ILikeItCachePrx prx = null;
    try {
      GlobalId gidSlice = new GlobalId();
      com.xiaonei.xce.globalid.GlobalId gid = com.xiaonei.xce.globalid.GlobalId.fromString(gidStr);

      gidSlice.id = gid.getId();
      gidSlice.type = gid.getType();
      prx = getILikeItCache(gid.getId());

      if (prx != null) {
        return prx.isLiked(gidSlice, uid);
      }
    } catch (Ice.ConnectTimeoutException e) {
      Oce.getLogger().error(this.getClass().getName() + ".getLikeInfoSimple [" + prx + "] Ice.ConnectTimeoutException");
    } catch (Ice.TimeoutException e) {
      Oce.getLogger().error(this.getClass().getName() + " [" + prx + "] Ice.TimeoutException");
    } catch (Exception e) {
      Oce.getLogger().error(this.getClass().getName() + " getLikeInfoSimple [" + prx + "] ", e);
    }
    return 0;
  }

  public Map<String, Integer> isLikedBatch(String[] gidStrs, int uid) {

    Map<String, Integer> result = new HashMap<String, Integer>();
    Map<Integer, Vector<String>> modMap = divideByMod(gidStrs);
    for (int mod : modMap.keySet()) {
      ILikeItCachePrx prx = null;
      try {
        prx = getILikeItCache(mod);
        Vector<String> gidStrsMod = modMap.get(mod);
        GlobalId[] gidSlices = strVector2gidArray(gidStrsMod);
        int[] likedArray = prx.isLikedBatch(gidSlices, uid);
        if (likedArray.length == gidStrsMod.size()) {
          for (int i = 0; i < likedArray.length; ++i) {
            result.put(gidStrsMod.get(i), likedArray[i]);
          }
        }
      } catch (Ice.ConnectTimeoutException e) {
        Oce.getLogger().error(this.getClass().getName() + ".isLikedBatch [" + prx + "] Ice.ConnectTimeoutException");
      } catch (Ice.TimeoutException e) {
        Oce.getLogger().error(this.getClass().getName() + " [" + prx + "] Ice.TimeoutException");
      } catch (Exception e) {
        Oce.getLogger().error(this.getClass().getName() + " isLikedBatch [" + prx + "] ", e);
      }
    }
    return result;
  }

  /**
   * @brief 按mod分拆Global Id，从而分配到各个服务上
   */
  private Map<Integer, Vector<String>> divideByMod(String[] gidStrs) {
    Map<Integer, Vector<String>> modMap = new HashMap<Integer, Vector<String>>();
    for (String gidStr : gidStrs) {
      GlobalId gidSlice = str2gidSlice(gidStr);
      int mod = (int) (gidSlice.id % (long) _cluster);
      if (modMap.containsKey(mod)) {
        modMap.get(mod).add(gidStr);
      } else {
        Vector<String> gidStrsMod = new Vector<String>();
        gidStrsMod.add(gidStr);
        modMap.put(mod, gidStrsMod);
      }
    }
    return modMap;
  }

  private GlobalId str2gidSlice(String gidStr) {
    GlobalId gidSlice = new GlobalId();
    com.xiaonei.xce.globalid.GlobalId gid = com.xiaonei.xce.globalid.GlobalId.fromString(gidStr);
    gidSlice.id = gid.getId();
    gidSlice.type = gid.getType();
    return gidSlice;
  }

  private GlobalId[] strVector2gidArray(Vector<String> gidStrsMod) {
    GlobalId[] gidSlices = new GlobalId[gidStrsMod.size()];
    for (int i = 0; i < gidStrsMod.size(); ++i) {
      gidSlices[i] = str2gidSlice(gidStrsMod.get(i));
    }
    return gidSlices;
  }

  protected ILikeItCachePrx getILikeItCache(long objectid) {
    int mod = (int) (objectid % (long) _cluster);
    Ice.ObjectPrx prx0 = getValidProxy(mod);
    if (prx0 != null) {
      if (prxs[mod] == null) {
        ILikeItCachePrx prx = ILikeItCachePrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
        prxs[mod] = prx;
      } else if (!prxs[mod].ice_getIdentity().equals(prx0.ice_getIdentity())) {
        ILikeItCachePrx prx = ILikeItCachePrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
        prxs[mod] = prx;
      }
    }
    return prxs[mod];
  }

  protected ILikeItCachePrx getILikeItCacheOneWay(int objectid) {
    Ice.ObjectPrx prx0 = getProxy(objectid);
    if (prx0 != null) {
      ILikeItCachePrx prx = ILikeItCachePrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
      return prx;
    } else {
      return null;
    }
  }

  public void setValid(boolean isValid) {
    Map<Integer, ServerStateInfo[]> prxMap = _csClient.getAvailableObjProxies();

    for (Map.Entry<Integer, ServerStateInfo[]> entry : prxMap.entrySet()) {
      // int key = entry.getKey();
      ServerStateInfo[] value = entry.getValue();
      for (int i = 0; i < value.length; ++i) {
        ServerStateInfo serverStateInfo = value[i];
        Ice.ObjectPrx prx0 = serverStateInfo.prx;
        if (prx0 != null) {
          ILikeItCachePrx prx = null;
          try {
            prx = ILikeItCachePrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
            prx.setValid(isValid);
          } catch (Ice.ConnectTimeoutException e) {
            Oce.getLogger().error(this.getClass().getName() + ".setValid [" + prx + "] Ice.ConnectTimeoutException");
          } catch (Ice.TimeoutException e) {
            Oce.getLogger().error(this.getClass().getName() + ".setValid [" + prx + "] Ice.TimeoutException");
          } catch (Exception e) {
            Oce.getLogger().error(this.getClass().getName() + ".setValid [" + prx + "] ", e);
          }
        }
      }
    }
  }

  public void reloadCache(long uid) {
    int category = 0;
    Channel channel = Channel.newLikeChannel("Like");
    for (int index = 4; index <= 9; ++index) {
      try {
        ObjectCachePrx objPrx = channel.locate("SC", "@ILikeItCacheM" + index, Channel.Invoke.Twoway,
            ObjectCachePrxHelper.class, 300);
        objPrx.reloadObject(category, uid);
      } catch (Exception e) {
        e.printStackTrace();
      }
    }
  }

  // 测试
  public static void main(String[] args) {

    int uid = 70000001;
    int uid2 = 221125077;
    String gidStr = "album_342193355";
    gidStr = "photo_2519243394";
    String[] gidStrs = new String[] { "photo_2519243394", "photo_2080251673" };

    ILikeItCacheAdapter _adapter = ILikeItCacheAdapter.instance();
    System.out.println(_adapter.getLikeDetail(gidStr, uid, 10, true).totalCount);
    for ( UserDetail d : _adapter.getLikeDetail(gidStr, uid, 10, true).detailList ) {
      System.out.println("\t" + d.name);
    }
    
    // _adapter.reloadCache(16004300903081L);    

    System.exit(1);
  }
};
