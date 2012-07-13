package com.xiaonei.xce.blt;

import com.xiaonei.xce.blt.BulletinRecord.WebSite;
import com.xiaonei.xce.log.Oce;
import xce.blt.Bulletin;
import xce.blt.BulletinManagerPrx;
import xce.blt.BulletinManagerPrxHelper;
import xce.clusterstate.ReplicatedClusterAdapter;

public class BulletinAdapter extends ReplicatedClusterAdapter {

  private static final String ctr_endpoints = "ControllerBulletinN";
  private static int _interval = 120;
  private static BulletinAdapter _adapter = new BulletinAdapter();
  private BulletinAdminAdapter _admin = BulletinAdminAdapter.getInstance();

  private BulletinAdapter() {
    super(ctr_endpoints, _interval);
  }

  public static BulletinAdapter getInstance() {
    return _adapter;
  }

  /*
   * -----------------------------------------------------
   * 
   * 公告读取部分
   * 
   * -----------------------------------------------------
   */
  // 为一个用户推荐一条公告
  public BulletinRecord show(int userid, int website) {
    Bulletin bulletin = null;
    BulletinManagerPrx prx = null;
    try {
      prx = getBulletinManager(userid);
      bulletin = prx.show(userid, website);
    } catch (Ice.TimeoutException e) {
      Oce.getLogger().error(this.getClass().getName() + ".show [" + prx + "] " + userid + " Ice.TimeoutException");
    } catch (Exception e) {
      Oce.getLogger().error(this.getClass().getName() + ".show [" + prx + "] " + userid, e);
    }
    if (bulletin != null) {
      BulletinRecord record = new BulletinRecord(bulletin);
      if (record.getBulletinId() >= 0) {
        return record;
      } else {
        return null;
      }
    } else {
      return null;
    }
  }

  public void renew() {
    BulletinManagerPrx prx = getBulletinManager(0);
    try {
      prx.renew(true);
    } catch (Ice.TimeoutException e) {
      Oce.getLogger().error(this.getClass().getName() + ".renew [" + prx + "] Ice.TimeoutException");
    } catch (Exception e) {
      Oce.getLogger().error(this.getClass().getName() + ".renew [" + prx + "] Exception", e);
    }
  }

  // 为按公告id获取一条公告
  public BulletinRecord get(int bulletinid) {
    Bulletin bulletin = null;
    BulletinManagerPrx prx = null;
    try {
      prx = getBulletinManager(bulletinid);
      bulletin = prx.get(bulletinid);
    } catch (Ice.TimeoutException e) {
      Oce.getLogger().error(this.getClass().getName() + ".get [" + prx + "] " + bulletinid + " Ice.TimeoutException");
    } catch (Exception e) {
      Oce.getLogger().error(this.getClass().getName() + ".get [" + prx + "] " + bulletinid, e);
    }
    if (bulletin != null) {
      BulletinRecord record = new BulletinRecord(bulletin);
      if (record.getBulletinId() >= 0) {
        return record;
      } else {
        return null;
      }
    } else {
      return null;
    }
  }

  // 获取所有公告
  public BulletinRecord[] list() {
    Bulletin[] bulletins = new Bulletin[0];
    BulletinManagerPrx prx = null;
    try {
      prx = getBulletinManager(0);
      bulletins = prx.list();
    } catch (Ice.TimeoutException e) {
      Oce.getLogger().error(this.getClass().getName() + ".list [" + prx + "] Ice.TimeoutException");
    } catch (Exception e) {
      Oce.getLogger().error(this.getClass().getName() + ".list [" + prx + "]", e);
    }

    BulletinRecord[] result = new BulletinRecord[bulletins.length];
    for (int i = 0; i < bulletins.length; ++i) {
      result[i] = new BulletinRecord(bulletins[i]);
    }
    return result;
  }

  /*
   * -----------------------------------------------------
   * 
   * 公告修改部分
   * 
   * -----------------------------------------------------
   */
  // 增加一条公告
  public void addBulletin(BulletinRecord record) {
    _admin.addBulletin(record);
  }

  // 删除一条公告
  public void removeBulletin(int bulletinid) {
    _admin.removeBulletin(bulletinid);
  }

  // 修改一条公告
  public void modifyBulletin(int bulletinid, BulletinRecord record) {
    _admin.modifyBulletin(bulletinid, record);
  }

  // 一条公告上线
  public void onlineBulletin(int bulletinid) {
    _admin.onlineBulletin(bulletinid);
  }

  // 一条公告下线
  public void offlineBulletin(int bulletinid) {
    _admin.offlineBulletin(bulletinid);
  }

  // 更新一个bulletin的bitset
  public void reloadBulletinInterest(int bltId) {
    BulletinManagerPrx prx = null;
    try {
      prx = getBulletinManager(bltId);
      prx.reloadBulletinInterest(bltId, true);
    } catch (Ice.TimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".reloadBulletinInterest [" + prx + "] " + bltId + " Ice.TimeoutException");
    } catch (Exception e) {
      Oce.getLogger().error(this.getClass().getName() + ".reloadBulletinInterest [" + prx + "] " + bltId, e);
    }
  }

  /*
   * -----------------------------------------------------
   * 
   * 用户操作部分
   * 
   * -----------------------------------------------------
   */
  public void click(int userid, int bulletinid) {
    BulletinManagerPrx prx = null;
    try {
      prx = getBulletinManagerOneway(userid);
      prx.click(userid, bulletinid, false);
    } catch (Ice.TimeoutException e) {
      Oce.getLogger().error(this.getClass().getName() + ".click [" + prx + "] Ice.TimeoutException");
    } catch (Exception e) {
      Oce.getLogger().error(this.getClass().getName() + ".click [" + prx + "]", e);
    }
  }

  public void close(int userid, int bulletinid) {
    BulletinManagerPrx prx = null;
    try {
      prx = getBulletinManager(userid);
      prx.click(userid, bulletinid, true);
    } catch (Ice.TimeoutException e) {
      Oce.getLogger().error(this.getClass().getName() + ".close [" + prx + "] Ice.TimeoutException");
    } catch (Exception e) {
      Oce.getLogger().error(this.getClass().getName() + ".close [" + prx + "]", e);
    }
  }

  public void reloadUserCache(int userid) {
    BulletinManagerPrx prx = null;
    try {
      prx = getBulletinManager(userid);
      prx.relocateUserCache(userid);
    } catch (Ice.TimeoutException e) {
      Oce.getLogger().error(this.getClass().getName() + ".reloadUserCache [" + prx + "] Ice.TimeoutException");
    } catch (Exception e) {
      Oce.getLogger().error(this.getClass().getName() + ".reloadUserCache [" + prx + "]", e);
    }
  }

  /*
   * ----------------------------------------------------- 内部prx等函数
   * -----------------------------------------------------
   */
  public BulletinManagerPrx getBulletinManager(int userid) {
    Ice.ObjectPrx prx0 = getValidProxy(userid, 300);
    if (prx0 != null) {
      BulletinManagerPrx prx = BulletinManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
      return prx;
    } else {
      return null;
    }
  }

  private BulletinManagerPrx getBulletinManagerOneway(int userid) {
    Ice.ObjectPrx prx0 = getValidProxy(userid, 300);
    if (prx0 != null) {
      BulletinManagerPrx prx = BulletinManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
      return prx;
    } else {
      return null;
    }

  }

  public static void main(String[] args) {

    // 删除公告
    // BulletinAdapter.getInstance().removeBulletin(19);

    // 添加公告
    // BulletinAdapter.getInstance().addBulletin(bulletin0);
    // 公告上线
    // BulletinAdapter.getInstance().onlineBulletin(134);
    // BulletinAdapter.getInstance().renew();
    // System.out.println("--------------");
    // BulletinAdapter.getInstance().reloadBulletinInterest(140);
    // BulletinAdapter.getInstance().onlineBulletin(141);

    // 取所有公告
    BulletinRecord[] blts = BulletinAdapter.getInstance().list();
    System.out.println("List all bulletins --");
    for (int i = 0; i < blts.length; ++i) {
      String stage = "";
      for (int j = 0; j < blts[i].getUserStage().length; ++j) {
        stage = stage + blts[i].getUserStage()[j].value + ",";
      }
      ;
      System.out.println(blts[i].getBulletinId() + "/" + blts[i].getTitle()
          // + "/" + blts[i].getText()
          + "/online?" + blts[i].getOnline() + "/maxshow:" + blts[i].getMaxShow() + "/stage:" + stage
          + "/interestTable:" + blts[i].getInterestUserDbTable());
    }
    System.out.println("--------------");

    // 按id取公告
    int bulletinid = 111;
    BulletinRecord bulletin = BulletinAdapter.getInstance().get(bulletinid);
    if (bulletin != null) {
      System.out.println(bulletin.getBulletinId() + "/" + bulletin.getTitle() + "/" + bulletin.getText() + "\n"
          + bulletin.getTimeScope()[0].beginTime.toLocaleString() + ":"
          + bulletin.getTimeScope()[0].endTime.toLocaleString());
    } else {
      System.out.println("bulletin " + bulletinid + " is null");
    }
    // bulletin.setIdmods(new int[]{1});
    // bulletin.setModDiv(200);
    // BulletinAdapter.getInstance().modifyBulletin(bulletinid, bulletin);
    // BulletinAdapter.getInstance().offlineBulletin(bulletinid);
    // BulletinAdapter.getInstance().renew();

    /*
     * BulletinRecord.TimeSeg b = bulletin.new TimeSeg(0, 0); b.beginTime = new
     * Date(110, 7, 23, 0, 0, 0); b.endTime = new Date(110, 7, 23, 23, 59, 59);
     * bulletin.setTimeScope(new TimeSeg[]{b});
     */
    // System.out.println(b.beginTime.toLocaleString() + ":" +
    // b.endTime.toLocaleString());
    // BulletinAdapter.getInstance().modifyBulletin(bulletinid, bulletin);
    // BulletinAdapter.getInstance().offlineBulletin(bulletinid);
    // BulletinAdapter.getInstance().renew();
    System.out.println("--------------");

    // search公告
    int userid = 222525090; // me
    // userid = 280281578;
    // userid = 24496255; // yexiang
    // userid = 222677625; // gaoyanan
    // userid = 279369143; // 非星级用户
    // userid = 272860015;

    // 搜索公告
    WebSite website = WebSite.SITE_REREN;
    BulletinRecord bulletin2 = BulletinAdapter.getInstance().show(userid, website.value);
    if (bulletin2 != null) {
      System.out.println("userid:" + userid + "\t" + bulletin2.getBulletinId() + "/" + bulletin2.getTitle() + "/"
          + bulletin2.getText() + "/Star?" + bulletin2.getStar() + "/Uncompletes?" + bulletin2.getUncompletes().length);
    } else {
      System.out.println("bulletin show for " + userid + " is null");
    }

    // click / close / reload
    // BulletinAdapter.getInstance().click(userid, 39);
    // BulletinAdapter.getInstance().close(userid, 39);
    // BulletinAdapter.getInstance().reloadUserCache(userid);

    System.exit(0);
  }
}
