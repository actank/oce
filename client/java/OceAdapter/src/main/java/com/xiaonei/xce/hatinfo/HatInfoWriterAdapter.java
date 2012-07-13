package com.xiaonei.xce.hatinfo;

import xce.vipinfo.HatDataWriterPrx;
import xce.vipinfo.HatDataWriterPrxHelper;
import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import xce.clusterstate.ReplicatedClusterAdapter;
public final class HatInfoWriterAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerVipInfoWriter";

	private static final String service_name = "IH";
	
	private static int _interval = 120;

	public HatInfoWriterAdapter() {
		super(ctr_endpoints, _interval);
	}

	protected HatDataWriterPrx getHatDataWriter10sPrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 10*1000);
		if (prx0 != null) {
			HatDataWriterPrx prx = HatDataWriterPrxHelper
					.uncheckedCast(prx0.ice_timeout(10*1000).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected HatDataWriterPrx getHatInfoWriterPrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name , 300);
		if (prx0 != null) {
			HatDataWriterPrx prx = HatDataWriterPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	  public void createHatInfo(int id, HatInfo obj) {
	    TimeCost cost = TimeCost.begin("HatInfoWriterAdapter.createHatInfo id: " + id);
	    HatDataWriterPrx prx = null;
	    try {
		if (id<=0) {
			return;
		}
	      if (id != obj.getHatId()) {
	        Oce.getLogger().error(
	            "com.xiaonei.xce.hatinfo.HatInfoWriterAdapter.createHatInfo id diff");
	      }
	      prx = getHatDataWriter10sPrx(id);
	      prx.createHatData(obj.demark());
	    } catch (TimeoutException e) {
	      Oce.getLogger().error(
	          this.getClass().getName() + ".createHatInfo[ " + prx + "] id=" + id
	              + " Ice.TimeoutException");
	      throw e;
	    } finally {
	      cost.endFinally();
	    }
	  }

  public void setHatInfo(int id, HatInfo obj) {
    TimeCost cost = TimeCost.begin("HatInfoWriterAdapter.setHatInfo id: " + id);
    HatDataWriterPrx prx = null;
    try {
	if (id<0) {
		return;
	}
      if ( id != obj.getHatId()) {
        Oce.getLogger().error(
            "com.xiaonei.xce.hat2urlinfo.HatInfoWriterAdapter.setHatInfo id diff");
        obj.setHatId(id);
      }
      prx = getHatInfoWriterPrx(id);
      prx.setHatData(id, obj.demark());
    } catch (TimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".setHatInfo[ " + prx + "] id=" + id
              + " Ice.TimeoutException");
      throw e;
    } finally {
      cost.endFinally();
    }
  }

}
