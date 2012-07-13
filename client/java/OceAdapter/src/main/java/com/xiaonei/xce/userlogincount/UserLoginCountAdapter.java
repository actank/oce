package com.xiaonei.xce.userlogincount;

import java.util.Vector;
import java.util.Map;
import java.util.HashMap;

import xce.userbase.UserLoginCountManagerPrx;
import xce.userbase.UserLoginCountManagerPrxHelper;
import com.xiaonei.xce.log.TimeCost;

import com.xiaonei.xce.log.Oce;
import Ice.TimeoutException;
import xce.clusterstate.ReplicatedClusterAdapter;
public class UserLoginCountAdapter extends ReplicatedClusterAdapter{
	private static final String ctr_endpoints = "ControllerUserBase";


	private static final String service_name = "ULCM";
	
	private static int _interval = 120;

	public UserLoginCountAdapter() {
		super(ctr_endpoints, _interval);
	}

	public void createUserLoginCount(int userid) {
		TimeCost cost = TimeCost.begin("UserLoginCountAdapter.createUserLoginCount id: "+userid);
		UserLoginCountManagerPrx prx=null;
		try {
			if (userid<=0) {
				return;
			}
			Map<String, String> map = new HashMap<String, String>();
			map.put("ID", String.valueOf(userid));
			prx=getUserLoginCount10sManager(userid);
			prx.createUserLoginCount(map);
		} catch (TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".createUserLoginCount [ " + prx + " ]  id="+ userid + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public int getUserLoginCount(int userId) throws Exception {
		TimeCost cost = TimeCost.begin("UserLoginCountAdapter.getUserLoginCount id: "+userId);
		UserLoginCountManagerPrx prx=null;
		try {
			if (userId<=0) {
				return 0;
			}
			prx=getUserLoginCountManager(userId);
			return prx.getUserLoginCount(userId);
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".getUserLoginCount [ " + prx + " ]  id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void incUserLoginCount(int userId) {
		TimeCost cost = TimeCost.begin("UserLoginCountAdapter.incUserLoginCount id: "+userId);
		try {
			if (userId<=0) {
				return;
			}
			getUserLoginCountManagerOneway(userId).incUserLoginCount(userId);
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".incUserLoginCount " + " id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	protected UserLoginCountManagerPrx getUserLoginCount10sManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 10*1000);
		if (prx0 != null) {
			UserLoginCountManagerPrx prx = UserLoginCountManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(10 * 1000).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected UserLoginCountManagerPrx getUserLoginCountManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 300);
		if (prx0 != null) {
			UserLoginCountManagerPrx prx = UserLoginCountManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected UserLoginCountManagerPrx getUserLoginCountManagerOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 300);
		if (prx0 != null) {
			UserLoginCountManagerPrx prx = UserLoginCountManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}
}
