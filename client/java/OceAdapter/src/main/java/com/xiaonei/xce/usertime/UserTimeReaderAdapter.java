package com.xiaonei.xce.usertime;

import xce.userdesc.UserTimeData;
import xce.userdesc.UserTimeReaderPrx;
import xce.userdesc.UserTimeReaderPrxHelper;

import xce.clusterstate.ReplicatedClusterAdapter;
import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;

import Ice.TimeoutException;

public final class UserTimeReaderAdapter<T extends UserTimeInfo> extends
		ReplicatedClusterAdapter {

	private static final String ctr_endpoints = "ControllerUserDescReader";
	private static final String service_name = "T";

	private static int _interval = 120;

	public UserTimeReaderAdapter(UserTimeFactory<T> factory) {
		super(ctr_endpoints, _interval);
		_factory = factory;
	}

	protected UserTimeFactory<T> _factory;

	protected UserTimeReaderPrx getUserTimeReaderPrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, service_name, 300);

		if (prx0 != null) {
			UserTimeReaderPrx prx = UserTimeReaderPrxHelper.uncheckedCast(prx0
					.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	public T getUserTime(int id) {
		TimeCost cost = TimeCost.begin("UserTimeReaderAdapter.getUserTime id: "
				+ id);
		UserTimeReaderPrx prx = null;
		T result = _factory.create();
		if (id <= 0) {
			return result;
		}
		try {
			prx = getUserTimeReaderPrx(id);
			UserTimeData data = prx.getUserTime(id);
			result.parse(data);
			return result;
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getUserTime [" + prx
							+ "] id=" + id + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

}
