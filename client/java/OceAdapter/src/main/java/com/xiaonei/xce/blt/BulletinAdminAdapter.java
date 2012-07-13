package com.xiaonei.xce.blt;

import com.xiaonei.xce.log.Oce;
import xce.blt.BulletinAdminPrx;
import xce.blt.BulletinAdminPrxHelper;
import xce.clusterstate.ReplicatedClusterAdapter;

public class BulletinAdminAdapter extends ReplicatedClusterAdapter {
  
  private static final String ctr_endpoints = "ControllerBulletinAdmin";
  private static int _interval = 120;   
  private static BulletinAdminAdapter _adapter = new BulletinAdminAdapter();
  
  private BulletinAdminAdapter() {
    super(ctr_endpoints,_interval);
  }
  
  public static BulletinAdminAdapter getInstance() {
    return _adapter;
  }
  
  /* -----------------------------------------------------
   * 
   *   公告修改部分
   *   
   -----------------------------------------------------*/  
  // 增加一条公告
  public void addBulletin(BulletinRecord record) {
    BulletinAdminPrx prx = getBulletinAdmin(0);
    try {
      prx.add(record.getBulletin());
    } catch (Ice.TimeoutException e) {
      Oce.getLogger().error(this.getClass().getName() + ".addBulletin [" + prx + "] Ice.TimeoutException");      
    } catch (Exception e) {
      Oce.getLogger().error(this.getClass().getName() + ".addBulletin [" + prx + "]", e);     
    }
  }
  
  // 删除一条公告
  public void removeBulletin(int bulletinid) {
    BulletinAdminPrx prx = getBulletinAdmin(0);
    try {
      prx.remove(bulletinid);
    } catch (Ice.TimeoutException e) {
      Oce.getLogger().error(this.getClass().getName() + ".removeBulletin [" + prx + "] Ice.TimeoutException");
    } catch (Exception e) {
      Oce.getLogger().error(this.getClass().getName() + ".removeBulletin [" + prx + "]", e);
    }
  }
  
  // 修改一条公告
  public void modifyBulletin(int bulletinid, BulletinRecord record) {
    BulletinAdminPrx prx = getBulletinAdmin(0);
    try {
      prx.modify(bulletinid, record.getBulletin());
    } catch (Ice.TimeoutException e) {
      Oce.getLogger().error(this.getClass().getName() + ".modifyBulletin [" + prx + "] Ice.TimeoutException");
    } catch (Exception e) {
      Oce.getLogger().error(this.getClass().getName() + ".modifyBulletin [" + prx + "]", e);
    }
  }
  
  // 一条公告上线
  public void onlineBulletin(int bulletinid) {
    BulletinAdminPrx prx = getBulletinAdmin(0);
    try {
      prx.online(bulletinid);
    } catch (Ice.TimeoutException e) {
      Oce.getLogger().error(this.getClass().getName() + ".onlineBulletin [" + prx + "]" + bulletinid + " Ice.TimeoutException");
    } catch (Exception e) {
      Oce.getLogger().error(this.getClass().getName() + ".onlineBulletin [" + prx + "]" + bulletinid, e);
    }
  }
  
  // 一条公告下线
  public void offlineBulletin(int bulletinid) {
    BulletinAdminPrx prx = getBulletinAdmin(0);
    try {
      prx.offline(bulletinid);
    } catch (Ice.TimeoutException e) {
      Oce.getLogger().error(this.getClass().getName() + ".offlineBulletin [" + prx + "] " + bulletinid + " Ice.TimeoutException");
    } catch (Exception e) {
      Oce.getLogger().error(this.getClass().getName() + ".offlineBulletin [" + prx + "] " + bulletinid, e);
    }
  }
  
  /* -----------------------------------------------------
   *   内部prx等函数
   -----------------------------------------------------*/
  public BulletinAdminPrx getBulletinAdmin(int userid) {
    
    Ice.ObjectPrx prx0 = getValidProxy(userid, 300);
    if ( prx0 != null ) {
      BulletinAdminPrx prx = BulletinAdminPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
      return prx;
    } else {
      return null;
    }
  }

  private BulletinAdminPrx getBulletinAdminOneway(int userid) {

    Ice.ObjectPrx prx0 = getValidProxy(userid, 300);
    if ( prx0 != null ) {
      BulletinAdminPrx prx = BulletinAdminPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
      return prx;
    } else {
      return null;
    } 
  }  
  
  public static void main(String[] args) {    
  }
}
