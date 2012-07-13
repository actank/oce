package com.xiaonei.xce.usercontact;

import xce.userdesc.UserContactWriterPrx;
import xce.userdesc.UserContactWriterPrxHelper;

import xce.clusterstate.ReplicatedClusterAdapter;
import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;

import Ice.TimeoutException;

public final class UserContactWriterAdapter extends ReplicatedClusterAdapter {

	private static final String ctr_endpoints = "ControllerUserDescWriter";

	private static final String service_name = "C";

	private static int _interval = 120;

	public UserContactWriterAdapter() {
		super(ctr_endpoints, _interval);
	}

	protected UserContactWriterPrx getUserContact10sManagerPrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, service_name, 10 * 1000);
		if (prx0 != null) {
			UserContactWriterPrx prx = UserContactWriterPrxHelper
					.uncheckedCast(prx0.ice_timeout(10 * 1000).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected UserContactWriterPrx getUserContactWriterPrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, service_name, 300);
		if (prx0 != null) {
			UserContactWriterPrx prx = UserContactWriterPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	public void setUserContact(int id, UserContactInfo obj) {
		TimeCost cost = TimeCost
				.begin("UserContactWriterAdapter.setUserContact id: " + id);
		UserContactWriterPrx prx = null;
		try {
			if (id <= 0) {
				return;
			}
			if (obj.getId() != 0 && id != obj.getId()) {
				Oce.getLogger()
						.info("com.xiaonei.xce.usercontact.UserContactWriterAdapter.setUserContact id diff");
			}
			prx = getUserContactWriterPrx(id);
			prx.setUserContact(id, obj.demark());
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".setUserContact [" + prx
							+ " ]id=" + id + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void createUserContact(int id, UserContactInfo obj) {
		TimeCost cost = TimeCost
				.begin("UserContactWriterAdapter.createUserContact id: " + id);
		UserContactWriterPrx prx = null;
		try {
			if (id <= 0) {
				return;
			}
			if (id != obj.getId()) {
				Oce.getLogger()
						.info("com.xiaonei.xce.usercontact.UserContactWriterAdapter.setUserContact id diff");
			}
			prx = getUserContact10sManagerPrx(id);
			prx.createUserContact(obj.demark());
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".createUserContact[ " + prx
							+ "] id=" + id + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}
};
