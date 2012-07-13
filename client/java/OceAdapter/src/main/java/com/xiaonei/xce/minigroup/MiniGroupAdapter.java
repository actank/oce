package com.xiaonei.xce.minigroup;

import java.util.ArrayList;
import java.util.List;

import xce.clusterstate.ClientInterface;
import xce.clusterstate.ReplicatedClusterAdapter;
import xce.mngp.MiniGroupManagerPrx;
import xce.mngp.MiniGroupManagerPrxHelper;
import xce.mngp.MiniGroupWriterPrx;
import xce.mngp.MiniGroupWriterPrxHelper;
import Ice.ConnectTimeoutException;
import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;

public class MiniGroupAdapter extends ReplicatedClusterAdapter {
  private MiniGroupAdapter() {
    super(miniGroupManagerEndpoints, interval);
  }

  private static final String miniGroupManagerEndpoints = "ControllerMiniGroupManager";
  private static final String MiniGroupWriterEndpoints = "ControllerMiniGroupWriter";
  private static int interval = 120;

  private static MiniGroupAdapter _instance = new MiniGroupAdapter();
  
  private static ClientInterface _miniGroupWriterClient;
  
  static {
    try{
      _miniGroupWriterClient = new ClientInterface(MiniGroupWriterEndpoints,
          interval);
    }catch(Throwable e){
      e.printStackTrace();
    }
  }

  public static MiniGroupAdapter getInstance() {
    return _instance;
  }
  
  private List<Long> toLongList(long[] nums) {
    List<Long> results = new ArrayList<Long>();
    if (nums == null || nums.length == 0) {
      return results;
    }
    for (long num : nums) {
      results.add(new Long(num));
    }
    return results;
  }
  
  private int[] toIntArray(final List<Integer> integers) {
    if (integers == null || integers.isEmpty()) {
      return new int[0];
    } else {
      int[] results = new int[integers.size()];
      for (int i = 0; i < results.length; ++i) {
        results[i] = integers.get(i).intValue();
      }
      return results;
    }
  }
  
  private long[] toLongArray(final List<Long> longs) {
    if (longs == null || longs.isEmpty()) {
      return new long[0];
    } else {
      long[] results = new long[longs.size()];
      for (int i = 0; i < results.length; ++i) {
        results[i] = longs.get(i).longValue();
      }
      return results;
    }
  }
  
  public List<MiniGroupInfo> getJoinedMiniGroups(int userId) {
    MiniGroupManagerPrx prx = null;
    try {
      prx = getMiniGroupManagerPrx();
      return MiniGroupInfo.formMiniGroupInfos(prx.getMiniGroups(userId));
    } catch (ConnectTimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".getMiniGroups [" + prx
              + "] userId = " + userId + " " + e);
      e.printStackTrace();
    } catch (TimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".getMiniGroups [" + prx
              + "] userId = " + userId + " " + e);
      e.printStackTrace();
    } catch (Exception e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".getMiniGroups [" + prx
              + "] userId= " + userId + " " + e);
      //e.printStackTrace();
    }
    return new ArrayList<MiniGroupInfo>();
  }
  
  public List<MiniGroupInfo> getMiniGroups(final List<Long> miniGroupIds) {
    MiniGroupManagerPrx prx = null;
    try {
      prx = getMiniGroupManagerPrx();
      return MiniGroupInfo.formMiniGroupInfos(prx.getMiniGroupsWithIds(toLongArray(miniGroupIds)));
    } catch (ConnectTimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".getMiniGroupsWithIds [" + prx
              + "] size  = " + miniGroupIds.size() + " " + e);
    } catch (TimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".getMiniGroupsWithIds [" + prx
              + "] size = " + miniGroupIds.size() + " " + e);
    } catch (Exception e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".getMiniGroupsWithIds [" + prx
              + "] size= " + miniGroupIds.size() + " " + e);
    }
    return new ArrayList<MiniGroupInfo>();
  }
  
  public void addMiniGroups(final List<MiniGroupInfo> miniGroups) {
    MiniGroupWriterPrx prx = null;
    try {
      prx = getMiniGroupWriterPrx();
      prx.addMiniGroups(MiniGroupInfo.formMiniGroups(miniGroups));
    } catch (ConnectTimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".addMiniGroups [" + prx
              + "] size  = " + miniGroups.size() + " " + e);
    } catch (TimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".addMiniGroups [" + prx
              + "] size = " + miniGroups.size() + " " + e);
    } catch (Exception e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".addMiniGroups [" + prx
              + "] size= " + miniGroups.size() + " " + e);
    }
  }
  
  public void addJoinedMiniGroups(int userId, final List<Long> miniGroupIds) {
    MiniGroupWriterPrx prx = null;
    try {
      prx = getMiniGroupWriterPrx();
      prx.addJoinedMiniGroups(userId, toLongArray(miniGroupIds));
    } catch (ConnectTimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".addJoinedMiniGroups [" + prx
              + "] size  = " + miniGroupIds.size() + " " + e);
    } catch (TimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".addJoinedMiniGroups [" + prx
              + "] size = " + miniGroupIds.size() + " " + e);
    } catch (Exception e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".addJoinedMiniGroups [" + prx
              + "] size= " + miniGroupIds.size() + " " + e);
    }
  }
  
  public void setJoinedMiniGroups(int userId, final List<Long> miniGroupIds) {
    MiniGroupWriterPrx prx = null;
    try {
      prx = getMiniGroupWriterPrx();
      prx.setJoinedMiniGroups(userId, toLongArray(miniGroupIds));
    } catch (ConnectTimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".setJoinedMiniGroups [" + prx
              + "] size  = " + miniGroupIds.size() + " " + e);
    } catch (TimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".setJoinedMiniGroups [" + prx
              + "] size = " + miniGroupIds.size() + " " + e);
    } catch (Exception e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".setJoinedMiniGroups [" + prx
              + "] size= " + miniGroupIds.size() + " " + e);
    }

  }
  
  public void removeJoinedMiniGroups(int userId, final List<Long> miniGroupIds) {
    MiniGroupWriterPrx prx = null;
    try {
      prx = getMiniGroupWriterPrx();
      prx.removeJoinedMiniGroups(userId, toLongArray(miniGroupIds));
    } catch (ConnectTimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".removeJoinedMiniGroups [" + prx
              + "] size  = " + miniGroupIds.size() + " " + e);
    } catch (TimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".removeJoinedMiniGroups [" + prx
              + "] size = " + miniGroupIds.size() + " " + e);
    } catch (Exception e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".removeJoinedMiniGroups [" + prx
              + "] size= " + miniGroupIds.size() + " " + e);
    }

  }
  
  protected MiniGroupManagerPrx getMiniGroupManagerPrx() {
    Ice.ObjectPrx prx0 = getValidProxy(0, "MNGPM");
    if (prx0 != null) {
      MiniGroupManagerPrx prx = MiniGroupManagerPrxHelper
          .uncheckedCast(prx0.ice_timeout(300).ice_twoway());
      return prx;
    } else {
      return null;
    }
  }
  
  public void reloadUsers(final List<Integer> userIds) {
    MiniGroupWriterPrx prx = null;
    try {
      prx = getMiniGroupWriterPrx();
      prx.reloadUsers(toIntArray(userIds));
    } catch (ConnectTimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".reloadUsers [" + prx
              + "] size  = " + userIds.size() + " " + e);
    } catch (TimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".reloadUsers [" + prx
              + "] size = " + userIds.size() + " " + e);
    } catch (Exception e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".reloadUsers [" + prx
              + "] size= " + userIds.size() + " " + e);
    }
  }
  
  public void reloadMiniGroups(final List<Long> miniGroupIds) {
    MiniGroupWriterPrx prx = null;
    try {
      prx = getMiniGroupWriterPrx();
      prx.reloadMiniGroups(toLongArray(miniGroupIds));
    } catch (ConnectTimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".reloadUsers [" + prx
              + "] size  = " + miniGroupIds.size() + " " + e);
    } catch (TimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".reloadUsers [" + prx
              + "] size = " + miniGroupIds.size() + " " + e);
    } catch (Exception e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".reloadUsers [" + prx
              + "] size= " + miniGroupIds.size() + " " + e);
    }
  }
  
  protected MiniGroupWriterPrx getMiniGroupWriterPrx() {
    Ice.ObjectPrx prx0 = _miniGroupWriterClient.getValidProxy(0);
    if (prx0 != null) {
      MiniGroupWriterPrx prx = MiniGroupWriterPrxHelper
          .uncheckedCast(prx0.ice_timeout(300).ice_twoway());
      return prx;
    } else {
      return null;
    }
  }
  
  public static void main(String args[]) {
    System.out.println("------------------------------------------------------");
    System.out.println(MiniGroupAdapter.getInstance().getJoinedMiniGroups(1045));
    System.out.println(MiniGroupAdapter.getInstance().getJoinedMiniGroups(381309829));
    System.out.println(MiniGroupAdapter.getInstance().getJoinedMiniGroups(67913));
//    List<Long> miniGroupIds = new ArrayList<Long>();
//    miniGroupIds.add(new Long(352261900));
//    miniGroupIds.add(new Long(363037600));
//    System.out.println(MiniGroupAdapter.getInstance().getMiniGroups(miniGroupIds));
    System.exit(0);
  }
}
