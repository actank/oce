package com.xiaonei.xce.tps;

import com.xiaonei.xce.tps.TipsRecord.WebSite;
import com.xiaonei.xce.log.Oce;
import xce.tps.Tips;
import xce.tps.TipsManagerPrx;
import xce.tps.TipsManagerPrxHelper;
import xce.clusterstate.ReplicatedClusterAdapter;

public class TipsAdapter extends ReplicatedClusterAdapter {

  private static final String ctr_endpoints = "ControllerTips";
  private static int _interval = 120;
  private static TipsAdapter _adapter = new TipsAdapter();
  private TipsAdminAdapter _admin = TipsAdminAdapter.getInstance();

  private TipsAdapter() {
    super(ctr_endpoints, _interval);
  }

  public static TipsAdapter getInstance() {
    return _adapter;
  }

  /*
   * -----------------------------------------------------
   * 
   * 提示读取部分
   * 
   * -----------------------------------------------------
   */
  // 为一个用户推荐一条提示
  public TipsRecord show(int userid, int website) {
    Tips tips = null;
    TipsManagerPrx prx = null;
    try {
      prx = getTipsManager(userid);
      tips = prx.show(userid, website);
    } catch (Ice.TimeoutException e) {
      Oce.getLogger().error(this.getClass().getName() + ".show [" + prx + "] " + userid + " Ice.TimeoutException");
    } catch (Exception e) {
      Oce.getLogger().error(this.getClass().getName() + ".show [" + prx + "] " + userid, e);
    }
    if (tips != null) {
      TipsRecord record = new TipsRecord(tips);
      if (record.getTipsId() >= 0) {
        return record;
      } else {
        return null;
      }
    } else {
      return null;
    }
  }

  public void renew() {
    TipsManagerPrx prx = getTipsManager(0);
    try {
      prx.renew(true);
    } catch (Ice.TimeoutException e) {
      Oce.getLogger().error(this.getClass().getName() + ".renew [" + prx + "] Ice.TimeoutException");
    } catch (Exception e) {
      Oce.getLogger().error(this.getClass().getName() + ".renew [" + prx + "] Exception", e);
    }
  }

  // 为按提示id获取一条提示
  public TipsRecord get(int tipsid) {
    Tips tips = null;
    TipsManagerPrx prx = null;
    try {
      prx = getTipsManager(tipsid);
      tips = prx.get(tipsid);
    } catch (Ice.TimeoutException e) {
      Oce.getLogger().error(this.getClass().getName() + ".get [" + prx + "] " + tipsid + " Ice.TimeoutException");
    } catch (Exception e) {
      Oce.getLogger().error(this.getClass().getName() + ".get [" + prx + "] " + tipsid, e);
    }
    if (tips != null) {
      TipsRecord record = new TipsRecord(tips);
      if (record.getTipsId() >= 0) {
        return record;
      } else {
        return null;
      }
    } else {
      return null;
    }
  }

  // 获取所有提示
  public TipsRecord[] list() {
    Tips[] tipss = new Tips[0];
    TipsManagerPrx prx = null;
    try {
      prx = getTipsManager(0);
      tipss = prx.list();
    } catch (Ice.TimeoutException e) {
      Oce.getLogger().error(this.getClass().getName() + ".list [" + prx + "] Ice.TimeoutException");
    } catch (Exception e) {
      Oce.getLogger().error(this.getClass().getName() + ".list [" + prx + "]", e);
    }

    TipsRecord[] result = new TipsRecord[tipss.length];
    for (int i = 0; i < tipss.length; ++i) {
      result[i] = new TipsRecord(tipss[i]);
    }
    return result;
  }

  /*
   * -----------------------------------------------------
   * 
   * 提示修改部分
   * 
   * -----------------------------------------------------
   */
  // 增加一条提示
  public void addTips(TipsRecord record) {
    _admin.addTips(record);
  }

  // 删除一条提示
  public void removeTips(int tipsid) {
    _admin.removeTips(tipsid);
  }

  // 修改一条提示
  public void modifyTips(int tipsid, TipsRecord record) {
    _admin.modifyTips(tipsid, record);
  }

  // 一条提示上线
  public void onlineTips(int tipsid) {
    _admin.onlineTips(tipsid);
  }

  // 一条提示下线
  public void offlineTips(int tipsid) {
    _admin.offlineTips(tipsid);
  }

  // 更新一个tips的bitset
  public void reloadTipsInterest(int tpsId) {
    TipsManagerPrx prx = null;
    try {
      prx = getTipsManager(tpsId);
      prx.reloadTipsInterest(tpsId, true);
    } catch (Ice.TimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".reloadTipsInterest [" + prx + "] " + tpsId + " Ice.TimeoutException");
    } catch (Exception e) {
      Oce.getLogger().error(this.getClass().getName() + ".reloadTipsInterest [" + prx + "] " + tpsId, e);
    }
  }

  /*
   * -----------------------------------------------------
   * 
   * 用户操作部分
   * 
   * -----------------------------------------------------
   */
  public void click(int userid, int tipsid) {
    TipsManagerPrx prx = null;
    try {
      prx = getTipsManagerOneway(userid);
      prx.click(userid, tipsid, false);
    } catch (Ice.TimeoutException e) {
      Oce.getLogger().error(this.getClass().getName() + ".click [" + prx + "] Ice.TimeoutException");
    } catch (Exception e) {
      Oce.getLogger().error(this.getClass().getName() + ".click [" + prx + "]", e);
    }
  }

  public void close(int userid, int tipsid) {
    TipsManagerPrx prx = null;
    try {
      prx = getTipsManager(userid);
      prx.click(userid, tipsid, true);
    } catch (Ice.TimeoutException e) {
      Oce.getLogger().error(this.getClass().getName() + ".close [" + prx + "] Ice.TimeoutException");
    } catch (Exception e) {
      Oce.getLogger().error(this.getClass().getName() + ".close [" + prx + "]", e);
    }
  }

  public void reloadUserCache(int userid) {
    TipsManagerPrx prx = null;
    try {
      prx = getTipsManager(userid);
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
  public TipsManagerPrx getTipsManager(int userid) {
    Ice.ObjectPrx prx0 = getValidProxy(userid, 300);
    if (prx0 != null) {
      TipsManagerPrx prx = TipsManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
      return prx;
    } else {
      return null;
    }
  }

  private TipsManagerPrx getTipsManagerOneway(int userid) {
    Ice.ObjectPrx prx0 = getValidProxy(userid, 300);
    if (prx0 != null) {
      TipsManagerPrx prx = TipsManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
      return prx;
    } else {
      return null;
    }

  }

  public static void main(String[] args) {

    // 删除提示
    // TipsAdapter.getInstance().removeTips(19);

    // 添加提示
    // TipsAdapter.getInstance().addTips(tips0);
    // 提示上线
    // TipsAdapter.getInstance().onlineTips(134);
    // TipsAdapter.getInstance().renew();
    // System.out.println("--------------");
    // TipsAdapter.getInstance().reloadTipsInterest(140);
    // TipsAdapter.getInstance().onlineTips(141);

    // 取所有提示
    TipsRecord[] tpss = TipsAdapter.getInstance().list();
    System.out.println("List all tipss --");
    for (int i = 0; i < tpss.length; ++i) {
      String stage = "";
      for (int j = 0; j < tpss[i].getUserStage().length; ++j) {
        stage = stage + tpss[i].getUserStage()[j].value + ",";
      }
      ;
      System.out.println(tpss[i].getTipsId() + "/" + tpss[i].getTitle()
          // + "/" + tpss[i].getText()
          + "/online?" + tpss[i].getOnline() + "/maxshow:" + tpss[i].getMaxShow() + "/stage:" + stage
          + "/interestTable:" + tpss[i].getInterestUserDbTable());
    }
    System.out.println("--------------");

    // 按id取提示
    int tipsid = 111;
    TipsRecord tips = TipsAdapter.getInstance().get(tipsid);
    if (tips != null) {
      System.out.println(tips.getTipsId() + "/" + tips.getTitle() + "/" + tips.getText() + "\n"
          + tips.getTimeScope()[0].beginTime.toLocaleString() + ":"
          + tips.getTimeScope()[0].endTime.toLocaleString());
    } else {
      System.out.println("tips " + tipsid + " is null");
    }
    // tips.setIdmods(new int[]{1});
    // tips.setModDiv(200);
    // TipsAdapter.getInstance().modifyTips(tipsid, tips);
    // TipsAdapter.getInstance().offlineTips(tipsid);
    // TipsAdapter.getInstance().renew();

    /*
     * TipsRecord.TimeSeg b = tips.new TimeSeg(0, 0); b.beginTime = new
     * Date(110, 7, 23, 0, 0, 0); b.endTime = new Date(110, 7, 23, 23, 59, 59);
     * tips.setTimeScope(new TimeSeg[]{b});
     */
    // System.out.println(b.beginTime.toLocaleString() + ":" +
    // b.endTime.toLocaleString());
    // TipsAdapter.getInstance().modifyTips(tipsid, tips);
    // TipsAdapter.getInstance().offlineTips(tipsid);
    // TipsAdapter.getInstance().renew();
    System.out.println("--------------");

    // search提示
    int userid = 222525090; // me
    // userid = 280281578;
    // userid = 24496255; // yexiang
    // userid = 222677625; // gaoyanan
    // userid = 279369143; // 非星级用户
    // userid = 272860015;

    // 搜索提示
    WebSite website = WebSite.SITE_RENREN;
    TipsRecord tips2 = TipsAdapter.getInstance().show(userid, website.value);
    if (tips2 != null) {
      System.out.println("userid:" + userid + "\t" + tips2.getTipsId() + "/" + tips2.getTitle() + "/"
          + tips2.getText() + "/Star?" + tips2.getStar() + "/Uncompletes?" + tips2.getUncompletes().length);
    } else {
      System.out.println("tips show for " + userid + " is null");
    }

    // click / close / reload
    // TipsAdapter.getInstance().click(userid, 39);
    // TipsAdapter.getInstance().close(userid, 39);
    // TipsAdapter.getInstance().reloadUserCache(userid);

    System.exit(0);
  }
}
