package com.xiaonei.xce.userfav;

import java.util.Vector;

import xce.userdesc.UserFavManagerPrx;
import xce.userdesc.UserFavManagerPrxHelper;
import com.xiaonei.xce.log.TimeCost;

import Ice.TimeoutException;
import xce.clusterstate.ReplicatedClusterAdapter;
public final class UserFavAdapter<T extends UserFavInfo> extends ReplicatedClusterAdapter{


	private static final String ctr_endpoints = "ControllerUserDesc";

	private static final String service_name = "F";
	
	private static int _interval = 120;

	public UserFavAdapter(UserFavFactory<T> factory) {
		super(ctr_endpoints, _interval);
		_factory = factory;
	}


	protected UserFavFactory<T> _factory;

	protected UserFavManagerPrx getUserFav10sManagerPrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 10*1000);
		if (prx0 != null) {
			UserFavManagerPrx prx = UserFavManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(10*1000).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}
	protected UserFavManagerPrx getUserFavManagerPrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 300);
		if (prx0 != null) {
			UserFavManagerPrx prx = UserFavManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected UserFavManagerPrx getUserFavManagerOnewayPrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 300);
		if (prx0 != null) {
			UserFavManagerPrx prx = UserFavManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}

	public void createUserFav(int id, UserFavInfo obj) {
		TimeCost cost = TimeCost.begin("UserFavAdapter.createUserFav id: "+id);
		cost.endFinally();
	}
}
