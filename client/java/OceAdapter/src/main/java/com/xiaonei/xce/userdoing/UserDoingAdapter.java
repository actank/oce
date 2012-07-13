package com.xiaonei.xce.userdoing;

import java.util.Vector;

import xce.userbase.UserDoingManagerPrx;
import xce.userbase.UserDoingManagerPrxHelper;
import xce.util.service.ServiceAdapter;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;

import xce.clusterstate.ReplicatedClusterAdapter;
import Ice.TimeoutException;

public class UserDoingAdapter<T extends UserDoingInfo> extends ReplicatedClusterAdapter {

	private static final String ctr_endpoints = "ControllerUserBase";


	private static final String service_name = "UDM";
	
	private static int _interval = 120;
	
	protected UserDoingFactory<T> _factory;

	public UserDoingAdapter(UserDoingFactory<T> factory) {
		super(ctr_endpoints, _interval);
		_factory = factory;
	}

	public void createUserDoing(int userId, UserDoingInfo umi) {
		TimeCost cost = TimeCost.begin("UserDoingAdapter.createUserDoing id: "+userId);
		UserDoingManagerPrx prx=null;
		try {
			if (userId<=0) {
				return;
			}
			if (userId != umi.getId()) {
				Oce.getLogger().error("com.xiaonei.xce.username.UserDoingAdapter.createUserDoing id diff");
			}
			prx=getUserDoing10sManager(userId);
			prx.createUserDoing(umi.demark());
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".createUserDoing  [ " + prx + " ]  id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public T getUserDoing(int userId) throws Exception {
		TimeCost cost = TimeCost.begin("UserDoingAdapter.getUserDoing id: "+userId);
		UserDoingManagerPrx prx=null;
		try {
			T res = _factory.create();
			if (userId<=0) {
				return res;
			}
			prx=getUserDoingManager(userId);
			res.parse(prx.getUserDoing(userId));
			return res;
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".getUserDoing  [ " + prx + " ]  id="  + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void setUserDoing(int userId, UserDoingInfo umi) {
		TimeCost cost = TimeCost.begin("UserDoingAdapter.setUserDoing id: "+userId);
		UserDoingManagerPrx prx=null;
		try {
			if (userId<=0) {
				return;
			}
			if (umi.getId()!=0&&userId != umi.getId()) {
				Oce.getLogger().error("com.xiaonei.xce.username.UserDoingAdapter.setUserDoing id diff");
				umi.setId(userId);
			}
			prx=getUserDoingManager(userId);
			prx.setUserDoing(userId, umi.demark());
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".setUserDoing  [ " + prx + " ]  id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	protected UserDoingManagerPrx getUserDoing10sManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 10*1000);
		if (prx0 != null) {
			UserDoingManagerPrx prx = UserDoingManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(10*1000).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected UserDoingManagerPrx getUserDoingManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 300);
		if (prx0 != null) {
			UserDoingManagerPrx prx = UserDoingManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected UserDoingManagerPrx getUserDoingManagerOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 300);
		if (prx0 != null) {
			UserDoingManagerPrx prx = UserDoingManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}
}
