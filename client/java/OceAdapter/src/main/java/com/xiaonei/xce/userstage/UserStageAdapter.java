package com.xiaonei.xce.userstage;

import java.util.Vector;

import xce.userbase.UserStageManagerPrx;
import xce.userbase.UserStageManagerPrxHelper;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;

import xce.clusterstate.ReplicatedClusterAdapter;
import Ice.TimeoutException;

public class UserStageAdapter<T extends UserStageInfo> extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerUserBase";


	private static final String service_name = "USGM";
	
	private static int _interval = 120;

	protected UserStageFactory<T> _factory;

	public UserStageAdapter(UserStageFactory<T> factory) {
		super(ctr_endpoints, _interval);
		_factory = factory;
	}

	public void createUserStage(int userId, UserStageInfo umi) {
		TimeCost cost = TimeCost.begin("UserStageAdapter.createUserStage id: "+userId);
		UserStageManagerPrx prx = null;
		try {
			if (userId<=0) {
				return;
			}
			if (userId != umi.getId()) {
				Oce.getLogger().error("com.xiaonei.xce.username.userstageadapter.createUserStage id diff");
			}
			prx = getUserStage10sManager(userId);
			prx.createUserStage(umi.demark());
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".createUserStage " + " prx = " + prx + " id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public T getUserStage(int userId) throws Exception {
		TimeCost cost = TimeCost.begin("UserStageAdapter.getUserStage id: "+userId);
		UserStageManagerPrx prx = null;
		T res = _factory.create();
		
		try {
			if (userId<=0) {
				return res;
			}
			prx = getUserStageManager(userId);
			res.parse(prx.getUserStage(userId));
			return res;
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".getUserStage " + " prx = " + prx + " id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public void setUserStage(int userId, UserStageInfo umi) {
		TimeCost cost = TimeCost.begin("UserStageAdapter.setUserStage id: "+userId);
		UserStageManagerPrx prx = null;
		try {
			if (userId<=0){
				return;
			}
			if (umi.getId()!=0&&userId != umi.getId()) {
				Oce.getLogger().error("com.xiaonei.xce.username.userstageadapter.setUserStage id diff");
				umi.setId(userId);
			}
			prx = getUserStageManager(userId);
			prx.setUserStage(userId, umi.demark());
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".setUserStage " + " prx = " + prx + " id=" + userId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	protected UserStageManagerPrx getUserStage10sManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 10*1000);
		if (prx0 != null) {
			UserStageManagerPrx prx = UserStageManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(10*1000).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected UserStageManagerPrx getUserStageManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 300);
		if (prx0 != null) {
			UserStageManagerPrx prx = UserStageManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected UserStageManagerPrx getUserStageManagerOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 300);
		if (prx0 != null) {
			UserStageManagerPrx prx = UserStageManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}
}
