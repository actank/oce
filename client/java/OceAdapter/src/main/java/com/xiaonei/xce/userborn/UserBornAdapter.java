package com.xiaonei.xce.userborn;

import xce.clusterstate.ReplicatedClusterAdapter;

import xce.userbase.UserBornManagerPrx;
import xce.userbase.UserBornManagerPrxHelper;



import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;

import Ice.TimeoutException;

public class UserBornAdapter<T extends UserBornInfo> extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerUserBase";

	private static final String service_name = "UBOM";
	
	private static int _interval = 120;
	

	protected UserBornFactory<T> _factory;

	public UserBornAdapter(UserBornFactory<T> factory) {
		super(ctr_endpoints, _interval);
		_factory = factory;
	}

	public void createUserBorn(int userId, UserBornInfo umi) {
		TimeCost cost = TimeCost.begin("UserBornAdapter.createUserBorn id: "+userId);
		UserBornManagerPrx prx = null; 
		try {
			if (userId <= 0) {
				return;
			}
			if (userId != umi.getId()) {
				Oce.getLogger().error("com.xiaonei.xce.username.UserBornAdapter.createUserBorn id diff");
			}
			prx = getUserBorn10sManager(userId);
			prx.createUserBorn(umi.demark());
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".createUserBorn [" + prx + "] id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public T getUserBorn(int userId) throws Exception {
		TimeCost cost = TimeCost.begin("UserBornAdapter.getUserBorn id: "+userId);
		UserBornManagerPrx prx=null;
		try {
			T res = _factory.create();
			if (userId<=0) {
				return res;
			}
			prx = getUserBornManager(userId) ;
			res.parse(prx.getUserBorn(userId));
			return res;
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".getUserBorn [" + prx + "] id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void setUserBorn(int userId, UserBornInfo umi) {
		TimeCost cost = TimeCost.begin("UserBornAdapter.setUserBorn id: "+userId);
		UserBornManagerPrx prx=null;
		try {
			if (userId<=0) {
				return;
			}
			if (umi.getId()!=0&&userId != umi.getId()) {
				Oce.getLogger().error("com.xiaonei.xce.username.UserBornAdapter.setUserBorn id diff");
				umi.setId(userId);
			}
			prx = getUserBornManager(userId) ;
			prx.setUserBorn(userId, umi.demark());
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".setUserBorn[ " + prx+ "]id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	protected UserBornManagerPrx getUserBorn10sManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 10*1000);
		if (prx0 != null) {
			UserBornManagerPrx prx = UserBornManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(10*1000).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected UserBornManagerPrx getUserBornManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 300);
		if (prx0 != null) {
			UserBornManagerPrx prx = UserBornManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected UserBornManagerPrx getUserBornManagerOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 300);
		if (prx0 != null) {
			UserBornManagerPrx prx = UserBornManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}
}
