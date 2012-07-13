package com.xiaonei.xce.usercontact;

import xce.userdesc.UserContactDataN;
import xce.userdesc.UserContactReaderPrx;
import xce.userdesc.UserContactReaderPrxHelper;

import xce.clusterstate.ReplicatedClusterAdapter;
import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;

import Ice.TimeoutException;

public final class UserContactReaderAdapter<T extends UserContactInfo> extends
		ReplicatedClusterAdapter {

	private static final String ctr_endpoints = "ControllerUserDescReader";

	private static final String service_name = "C";

	private static int _interval = 120;

	public UserContactReaderAdapter(UserContactFactory<T> factory) {
		super(ctr_endpoints, _interval);
		_factory = factory;
	}

	protected UserContactFactory<T> _factory;

	protected UserContactReaderPrx getUserContactReaderPrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, service_name, 300);
		if (prx0 != null) {
			UserContactReaderPrx prx = UserContactReaderPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	public T getUserContact(int id) {
		TimeCost cost = TimeCost
				.begin("UserContactReaderAdapter.getUserContact id: " + id);
		UserContactReaderPrx prx = null;
		try {
			T result = _factory.create();
			if (id <= 0) {
				return result;
			}
			prx = getUserContactReaderPrx(id);
			UserContactDataN data = prx.getUserContactN(id);
			result.parse(data);
			return result;
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getUserContact[ " + prx
							+ " ]id=" + id + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

};
