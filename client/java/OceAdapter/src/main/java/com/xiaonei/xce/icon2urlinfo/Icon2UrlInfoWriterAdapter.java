package com.xiaonei.xce.icon2urlinfo;

import xce.vipinfo.Icon2UrlDataWriterPrx;
import xce.vipinfo.Icon2UrlDataWriterPrxHelper;
import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import xce.clusterstate.ReplicatedClusterAdapter;
public final class Icon2UrlInfoWriterAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerVipInfoWriter";

	private static final String service_name = "IU";
	
	private static int _interval = 120;

	public Icon2UrlInfoWriterAdapter() {
		super(ctr_endpoints, _interval);
	}

	protected Icon2UrlDataWriterPrx getIcon2UrlDataWriter10sPrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 10*1000);
		if (prx0 != null) {
			Icon2UrlDataWriterPrx prx = Icon2UrlDataWriterPrxHelper
					.uncheckedCast(prx0.ice_timeout(10*1000).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected Icon2UrlDataWriterPrx getIcon2UrlInfoWriterPrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name , 300);
		if (prx0 != null) {
			Icon2UrlDataWriterPrx prx = Icon2UrlDataWriterPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	  public void createIcon2UrlInfo(int id, Icon2UrlInfo obj) {
	    TimeCost cost = TimeCost.begin("Icon2UrlInfoWriterAdapter.createIcon2UrlInfo id: " + id);
	    Icon2UrlDataWriterPrx prx = null;
	    try {
		if (id<=0) {
			return;
		}
	      if (id != obj.getIconId()) {
	        Oce.getLogger().error(
	            "com.xiaonei.xce.icon2urlinfo.Icon2UrlInfoWriterAdapter.createIcon2UrlInfo id diff");
	      }
	      prx = getIcon2UrlDataWriter10sPrx(id);
	      prx.createIcon2UrlData(obj.demark());
	    } catch (TimeoutException e) {
	      Oce.getLogger().error(
	          this.getClass().getName() + ".createIcon2UrlInfo[ " + prx + "] id=" + id
	              + " Ice.TimeoutException");
	      throw e;
	    } finally {
	      cost.endFinally();
	    }
	  }

  public void setIcon2UrlInfo(int id, Icon2UrlInfo obj) {
    TimeCost cost = TimeCost.begin("Icon2UrlInfoWriterAdapter.setUserUrl id: " + id);
    Icon2UrlDataWriterPrx prx = null;
    try {
	if (id<0) {
		return;
	}
      if ( id != obj.getIconId()) {
        Oce.getLogger().error(
            "com.xiaonei.xce.icon2urlinfo.Icon2UrlInfoWriterAdapter.setIcon2UrlInfo id diff");
        obj.setIconId(id);
      }
      prx = getIcon2UrlInfoWriterPrx(id);
      prx.setIcon2UrlData(id, obj.demark());
    } catch (TimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".setIcon2UrlInfo[ " + prx + "] id=" + id
              + " Ice.TimeoutException");
      throw e;
    } finally {
      cost.endFinally();
    }
  }

}
