package com.xiaonei.xce.userright;

import java.util.Vector;

import xce.userbase.UserRightManagerPrx;
import xce.userbase.UserRightManagerPrxHelper;

import xce.clusterstate.ReplicatedClusterAdapter;
import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;

import Ice.TimeoutException;


public class UserRightAdapter<T extends UserRightInfo> extends ReplicatedClusterAdapter{
	private static final String ctr_endpoints = "ControllerUserBase";

	private static final String service_name = "URM";
	
	private static int _interval = 120;

	protected UserRightFactory<T> _factory;

	public UserRightAdapter(UserRightFactory<T> factory) {
		super(ctr_endpoints, _interval);
		_factory = factory;
	}

	public void createUserRight(int userId, UserRightInfo uri) {
		TimeCost cost = TimeCost.begin("UserRightAdapter.createUserRight id: "+userId);
		UserRightManagerPrx prx= null;
		try {
			if (userId<=0) {
				return;
			}
			if (userId != uri.getId()) {
				Oce.getLogger().error("com.xiaonei.xce.userright.UserRightAdapter.createUserRight id diff");
			}
			prx = getUserRight10sManager(userId);
			prx.createUserRight(uri.demark());
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".crateUserRight[ "+ prx + " ]id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public T getUserRight(int userId) throws Exception {
		TimeCost cost = TimeCost.begin("UserRightAdapter.getUserRight id: "+userId);
		UserRightManagerPrx prx= null;
		T res = _factory.create();
		
		try {
			if (userId<=0) {
				return res;
			}
			prx= getUserRightManager(userId);
			res.parse(prx.getUserRight(userId));
			return res;
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".getUserRight[ " + prx + " ]id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void setUserRight(int userId, UserRightInfo uri, int timeout) {
		TimeCost cost = TimeCost.begin("UserRightAdapter.setUserRight id: "+userId);
		UserRightManagerPrx prx= null;
		try {
			if (userId<=0) {
				return;
			}
			if (uri.getId()!=0&&userId != uri.getId()) {
				Oce.getLogger().error("com.xiaonei.xce.userright.UserRightAdapter.setUserRight id diff");
				uri.setId(userId);
			}
			prx=getUserRightManager(userId, timeout);
			prx.setUserRight(userId, uri.demark());
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".setUserRight [" + prx + "] id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}
	public void setUserRight(int userId, UserRightInfo uri) {
		TimeCost cost = TimeCost.begin("UserRightAdapter.setUserRight id: "+userId);
		UserRightManagerPrx prx= null;
		try {
			if (userId<=0) {
				return;
			}
			if (uri.getId()!=0&&userId != uri.getId()) {
				Oce.getLogger().error("com.xiaonei.xce.userright.UserRightAdapter.setUserRight id diff");
				uri.setId(userId);
			}
			prx=getUserRightManager(userId);
			prx.setUserRight(userId, uri.demark());
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".setUserRight [" + prx + "] id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	protected UserRightManagerPrx getUserRight10sManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 10*1000);
		if (prx0 != null) {
			UserRightManagerPrx prx = UserRightManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(10*1000).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected UserRightManagerPrx getUserRightManager(int userId, int timeout) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, timeout);
		if (prx0 != null) {
			UserRightManagerPrx prx = UserRightManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(timeout).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected UserRightManagerPrx getUserRightManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 300);
		if (prx0 != null) {
			UserRightManagerPrx prx = UserRightManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected UserRightManagerPrx getUserRightManagerOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 300);
		if (prx0 != null) {
			UserRightManagerPrx prx = UserRightManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}
}
