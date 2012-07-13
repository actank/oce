package com.xiaonei.xce.usertime;

import xce.userdesc.UserTimeWriterPrx;
import xce.userdesc.UserTimeWriterPrxHelper;

import xce.clusterstate.ReplicatedClusterAdapter;
import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;

import Ice.TimeoutException;

public final class UserTimeWriterAdapter extends ReplicatedClusterAdapter {

	private static final String ctr_endpoints = "ControllerUserDescWriter";
	private static final String service_name = "T";

	private static int _interval = 120;

	public UserTimeWriterAdapter() {
		super(ctr_endpoints, _interval);
	}

	protected UserTimeWriterPrx getUserTimeWriter10sPrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, service_name, 10 * 1000);
		if (prx0 != null) {
			UserTimeWriterPrx prx = UserTimeWriterPrxHelper.uncheckedCast(prx0
					.ice_timeout(10 * 1000).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected UserTimeWriterPrx getUserTimeWriterPrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, service_name, 300);
		if (prx0 != null) {
			UserTimeWriterPrx prx = UserTimeWriterPrxHelper.uncheckedCast(prx0
					.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	public void setUserTime(int id, UserTimeInfo obj) {
		TimeCost cost = TimeCost.begin("UserTimeWriterAdapter.setUserTime id: "
				+ id);
		UserTimeWriterPrx prx = null;
		try {
			if (id <= 0) {
				return;
			}
			if (obj.getId() != 0 && id != obj.getId()) {
				Oce.getLogger()
						.error("com.xiaonei.xce.usertime.UserTimeWriterAdapter.setUserTime id diff");
				obj.setId(id);
			}
			prx = getUserTimeWriterPrx(id);
			prx.setUserTime(id, obj.demark());
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".setUserTime [" + prx
							+ " ]id=" + id + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void createUserTime(int id, UserTimeInfo obj) {
		TimeCost cost = TimeCost
				.begin("UserTimeWriterAdapter.createUserTime id: " + id);
		UserTimeWriterPrx prx = null;
		try {
			if (id <= 0) {
				return;
			}
			if (id != obj.getId()) {
				Oce.getLogger()
						.error("com.xiaonei.xce.usertime.UserTimeWriterAdapter.createUserTime id diff");
			}
			prx = getUserTimeWriter10sPrx(id);
			prx.createUserTime(obj.demark());
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".createUserTime [" + prx
							+ "] id=" + id + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

}
