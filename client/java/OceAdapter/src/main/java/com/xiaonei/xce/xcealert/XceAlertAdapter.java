package com.xiaonei.xce.xcealert;

import java.util.List;
import java.util.Vector;

import com.xiaonei.xce.XceAlertPrx;
import com.xiaonei.xce.XceAlertPrxHelper;
import xce.clusterstate.ReplicatedClusterAdapter;
import com.xiaonei.xce.log.Oce;

public class XceAlertAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerXceAlert";

	private static int _interval = 1;

	private static XceAlertAdapter alert_ = new XceAlertAdapter();

	public static XceAlertAdapter getInstance() {
		return alert_;
	}

	private XceAlertAdapter() {
		super(ctr_endpoints, _interval);
	}

	public void notifyAlert(List<String> ids, String msg) {
		XceAlertPrx prx = null;
		try {
			prx = getXceAlert();
			prx.notifyAlert(ids.toArray(new String[ids.size()]), msg);
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".notifyAlert, prx = " + prx, e);
		}
	}
	
	public void notifyAlertN(List<String> subjects, String msg, int sendType) {
		XceAlertPrx prx = null;
		try {
			prx = getXceAlert();
			//sendType must be 1, 2, or 3.
			//1 send msg to mobile; 2 send msg to email, 3 send msg to both
			prx.notifyAlertN(subjects.toArray(new String[subjects.size()]), msg, sendType);
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".notifyAlert, prx = " + prx, e);
		}
	}

	protected XceAlertPrx getXceAlert() {
		Ice.ObjectPrx prx0 = getValidProxy(0, 300);
		if (prx0 != null) {
			XceAlertPrx prx = XceAlertPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected XceAlertPrx getXceAlertOneway() {
		Ice.ObjectPrx prx0 = getValidProxy(0, 300);
		if (prx0 != null) {
			XceAlertPrx prx = XceAlertPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}
}
