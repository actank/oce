package com.xiaonei.xce.vipmemberinfo;

import xce.vipinfo.VipMemberDataWriterPrx;
import xce.vipinfo.VipMemberDataWriterPrxHelper;
import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import xce.clusterstate.ReplicatedClusterAdapter;
public final class VipMemberInfoWriterAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerVipInfoWriter";

	private static final String service_name = "VM";
	
	private static int _interval = 120;

	public VipMemberInfoWriterAdapter() {
		super(ctr_endpoints, _interval);
	}

	protected VipMemberDataWriterPrx getVipMemberDataWriter10sPrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 10*1000);
		if (prx0 != null) {
			VipMemberDataWriterPrx prx = VipMemberDataWriterPrxHelper
					.uncheckedCast(prx0.ice_timeout(10*1000).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected VipMemberDataWriterPrx getVipMemberInfoWriterPrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name , 300);
		if (prx0 != null) {
			VipMemberDataWriterPrx prx = VipMemberDataWriterPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	  public void createVipMemberInfo(int id, VipMemberInfo obj) throws Exception {
	    TimeCost cost = TimeCost.begin("VipMemberInfoWriterAdapter.createVipMemberInfo id: " + id);
	    VipMemberDataWriterPrx prx = null;
	    try {
		if (id<=0) {
			return;
		}
	      if (id != obj.getId()) {
	        Oce.getLogger().error(
	            "com.xiaonei.xce.vipbasicinfo.VipMemberInfoWriterAdapter.createVipMemberInfo id diff");
	        return;
	      }
	      if (!obj.isTypeMarked()) {
		        Oce.getLogger().error(
		            "com.xiaonei.xce.vipbasicinfo.VipMemberInfoWriterAdapter.createVipMemberInfo no type is create");
		        throw new Exception("VipMemberInfoWriterAdapter.createVipMemberInfo No type is set!");
		   }
	      prx = getVipMemberDataWriter10sPrx(id);
	      prx.createVipMemberData(obj.demark());
	    } catch (TimeoutException e) {
	      Oce.getLogger().error(
	          this.getClass().getName() + ".createVipMemberInfo[ " + prx + "] id=" + id
	              + " Ice.TimeoutException");
	      throw e;
	    } finally {
	      cost.endFinally();
	    }
	  }

  public void setVipMemberInfo(int id, VipMemberInfo obj) throws Exception {
    TimeCost cost = TimeCost.begin("VipMemberInfoWriterAdapter.setUserUrl id: " + id);
    VipMemberDataWriterPrx prx = null;
    try {
	if (id<=0) {
		return;
	}
      if (obj.getId() != 0 && id != obj.getId()) {
        Oce.getLogger().error(
            "com.xiaonei.xce.vipbasicinfo.VipMemberInfoWriterAdapter.setVipMemberInfo id diff");
        obj.setId(id);
      }
      if (!obj.isTypeMarked()) {
	        Oce.getLogger().error(
	            "com.xiaonei.xce.vipbasicinfo.VipMemberInfoWriterAdapter.setVipMemberInfo no type is create");
	        throw new Exception("VipMemberInfoWriterAdapter.createVipMemberInfo No type is set!");
	  }
      prx = getVipMemberInfoWriterPrx(id);
      prx.setVipMemberData(id, obj.demark());
    } catch (TimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".setVipMemberInfo[ " + prx + "] id=" + id
              + " Ice.TimeoutException");
      throw e;
    } finally {
      cost.endFinally();
    }
  }

}
