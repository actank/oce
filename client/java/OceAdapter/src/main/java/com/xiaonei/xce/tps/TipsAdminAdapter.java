package com.xiaonei.xce.tps;

import com.xiaonei.xce.log.Oce;
import xce.tps.TipsAdminPrx;
import xce.tps.TipsAdminPrxHelper;
import xce.clusterstate.ReplicatedClusterAdapter;

public class TipsAdminAdapter extends ReplicatedClusterAdapter {
  
  private static final String ctr_endpoints = "ControllerTipsAdmin";
  private static int _interval = 120;   
  private static TipsAdminAdapter _adapter = new TipsAdminAdapter();
  
  private TipsAdminAdapter() {
    super(ctr_endpoints,_interval);
  }
  
  public static TipsAdminAdapter getInstance() {
    return _adapter;
  }
  
  /* -----------------------------------------------------
   * 
   *   提示修改部分
   *   
   -----------------------------------------------------*/  
  // 增加一条提示
  public void addTips(TipsRecord record) {
    TipsAdminPrx prx = getTipsAdmin(0);
    try {
      prx.add(record.getTips());
    } catch (Ice.TimeoutException e) {
      Oce.getLogger().error(this.getClass().getName() + ".addTips [" + prx + "] Ice.TimeoutException");      
    } catch (Exception e) {
      Oce.getLogger().error(this.getClass().getName() + ".addTips [" + prx + "]", e);     
    }
  }
  
  // 删除一条提示
  public void removeTips(int tipsid) {
    TipsAdminPrx prx = getTipsAdmin(0);
    try {
      prx.remove(tipsid);
    } catch (Ice.TimeoutException e) {
      Oce.getLogger().error(this.getClass().getName() + ".removeTips [" + prx + "] Ice.TimeoutException");
    } catch (Exception e) {
      Oce.getLogger().error(this.getClass().getName() + ".removeTips [" + prx + "]", e);
    }
  }
  
  // 修改一条提示
  public void modifyTips(int tipsid, TipsRecord record) {
    TipsAdminPrx prx = getTipsAdmin(0);
    try {
      prx.modify(tipsid, record.getTips());
    } catch (Ice.TimeoutException e) {
      Oce.getLogger().error(this.getClass().getName() + ".modifyTips [" + prx + "] Ice.TimeoutException");
    } catch (Exception e) {
      Oce.getLogger().error(this.getClass().getName() + ".modifyTips [" + prx + "]", e);
    }
  }
  
  // 一条提示上线
  public void onlineTips(int tipsid) {
    TipsAdminPrx prx = getTipsAdmin(0);
    try {
      prx.online(tipsid);
    } catch (Ice.TimeoutException e) {
      Oce.getLogger().error(this.getClass().getName() + ".onlineTips [" + prx + "]" + tipsid + " Ice.TimeoutException");
    } catch (Exception e) {
      Oce.getLogger().error(this.getClass().getName() + ".onlineTips [" + prx + "]" + tipsid, e);
    }
  }
  
  // 一条提示下线
  public void offlineTips(int tipsid) {
    TipsAdminPrx prx = getTipsAdmin(0);
    try {
      prx.offline(tipsid);
    } catch (Ice.TimeoutException e) {
      Oce.getLogger().error(this.getClass().getName() + ".offlineTips [" + prx + "] " + tipsid + " Ice.TimeoutException");
    } catch (Exception e) {
      Oce.getLogger().error(this.getClass().getName() + ".offlineTips [" + prx + "] " + tipsid, e);
    }
  }
  
  /* -----------------------------------------------------
   *   内部prx等函数
   -----------------------------------------------------*/
  public TipsAdminPrx getTipsAdmin(int userid) {
    
    Ice.ObjectPrx prx0 = getValidProxy(userid, 300);
    if ( prx0 != null ) {
      TipsAdminPrx prx = TipsAdminPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
      return prx;
    } else {
      return null;
    }
  }

  private TipsAdminPrx getTipsAdminOneway(int userid) {

    Ice.ObjectPrx prx0 = getValidProxy(userid, 300);
    if ( prx0 != null ) {
      TipsAdminPrx prx = TipsAdminPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
      return prx;
    } else {
      return null;
    } 
  }  
  
  public static void main(String[] args) {    
  }
}
