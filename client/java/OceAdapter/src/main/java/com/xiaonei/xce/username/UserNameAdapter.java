package com.xiaonei.xce.username;

import java.util.Vector;

import xce.userbase.UserNameManagerPrx;
import xce.userbase.UserNameManagerPrxHelper;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;

import Ice.TimeoutException;
import xce.clusterstate.ReplicatedClusterAdapter;
public class UserNameAdapter<T extends UserNameInfo> extends ReplicatedClusterAdapter{
	private static final String ctr_endpoints = "ControllerUserBase";


	private static final String service_name = "UNM";
	
	private static int _interval = 120;

	protected UserNameFactory<T> _factory;

	public UserNameAdapter(UserNameFactory<T> factory) {
		super(ctr_endpoints, _interval);
		_factory = factory;
	}

	public void createUserName(int userId, UserNameInfo umi) {
		TimeCost cost = TimeCost.begin("UserNameAdapter.createUserName id: "+userId);
		UserNameManagerPrx prx =null;
		try {
			if (userId<=0) {
				return;
			}
			if (userId != umi.getId()) {
				Oce.getLogger().error("com.xiaonei.xce.username.UserNameAdapter.createUserName id diff");
			}
			prx = getUserName10sManager(userId);
			prx.createUserName(umi.demark());
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".createUserName [" + prx +  " ]id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public T getUserName(int userId) throws Exception {
		TimeCost cost = TimeCost.begin("UserNameAdapter.getUserName id: "+userId);
		UserNameManagerPrx prx =null;
		T res = _factory.create();
		if(userId<=0) {
			return res;
		}
		try {
			prx = getUserNameManager(userId) ;
			res.parse(prx.getUserName(userId));
			return res;
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".getUserName [" + prx + " ]id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void setUserName(int userId, UserNameInfo umi) {
		setUserName(userId, umi, 300);
	}

	public void setUserName(int userId, UserNameInfo umi, int timeout) {
		TimeCost cost = TimeCost.begin("UserNameAdapter.setUserName id: "+userId);
		UserNameManagerPrx prx =null;
		try {
			if (userId<=0) {
				return;
			}
			if (umi.getId()!=0&&userId != umi.getId()) {
				Oce.getLogger().error("com.xiaonei.xce.username.UserNameAdapter.setUserName id diff");
				umi.setId(userId);
			}
			prx = getUserNameManager(userId, timeout) ;
			prx.setUserName(userId, umi.demark());
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".setUserName [" + prx + "] id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	protected UserNameManagerPrx getUserName10sManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 10*1000);
		if (prx0 != null) {
			UserNameManagerPrx prx = UserNameManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(10*1000).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected UserNameManagerPrx getUserNameManager(int userId, int timeout) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 300);
		if (prx0 != null) {
			UserNameManagerPrx prx = UserNameManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected UserNameManagerPrx getUserNameManager(int userId) {
		return getUserNameManager(userId, 300);
	}

	protected UserNameManagerPrx getUserNameManagerOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 300);
		if (prx0 != null) {
			UserNameManagerPrx prx = UserNameManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}
}
