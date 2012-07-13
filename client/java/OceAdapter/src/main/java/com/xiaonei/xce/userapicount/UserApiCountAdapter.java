package com.xiaonei.xce.userapicount;

import java.util.Vector;

import xce.userapicount.UserApiCountManagerPrx;
import xce.userapicount.UserApiCountManagerPrxHelper;
import xce.userapicount.UserApiCounter;
import xce.util.service.ServiceAdapter;

import xce.clusterstate.ReplicatedClusterAdapter;
import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;

public final class UserApiCountAdapter extends ReplicatedClusterAdapter {

	private static UserApiCountAdapter _adapter = new UserApiCountAdapter();

	private static final String ctr_endpoints = "ControllerUserApiCount";

	private static int _interval = 120;

	protected UserApiCountAdapter() {
		super(ctr_endpoints, _interval);
	}

	public static UserApiCountAdapter getInstance() {
		return _adapter;
	}

	protected UserApiCountManagerPrx getUserApiCountManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			UserApiCountManagerPrx prx = UserApiCountManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected UserApiCountManagerPrx getUserApiCountManagerOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if (prx0 != null) {
			UserApiCountManagerPrx prx = UserApiCountManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}

	public void inc(int userId, int type, int step) {
		TimeCost cost = TimeCost.begin("UserApiCountAdapter.inc id: "+userId+" type: "+type+" step: "+step);
		try {
			if (userId <= 0) {
				return;
			}
			getUserApiCountManagerOneway(userId).inc(userId, type, step);
		} finally {
			cost.endFinally();
		}
	}

	public void dec(int userId, int type, int step) {
		TimeCost cost = TimeCost.begin("UserApiCountAdapter.dec id: "+userId+" type: "+type+" step: "+step);
		try {
			if (userId <= 0) {
				return;
			}
			getUserApiCountManagerOneway(userId).dec(userId, type, step);
		} finally {
			cost.endFinally();
		}
	}

	public void set(int userId, int type, int step) {
		TimeCost cost = TimeCost.begin("UserApiCountAdapter.set id: "+userId+" type: "+type+" step: "+step);
		try {
			if (userId <= 0) {
				return;
			}
			getUserApiCountManagerOneway(userId).set(userId, type, step);
		} finally {
			cost.endFinally();
		}
	}

	public int get(int userId, int type) {
		TimeCost cost = TimeCost.begin("UserApiCountAdapter.get id: "+userId+" type: "+type);
		UserApiCountManagerPrx prx = null;
		try {
			if (userId <= 0) {
				return 0;
			}
			int ret = 0;
			try {
				prx = getUserApiCountManager(userId);
				ret = prx.get(userId, type);
			} catch (Ice.TimeoutException e) {
				Oce.getLogger().error(this.getClass().getName() + ".get " + " prx = " + prx +  " id=" + userId + " type=" + type + " Ice.TimeoutException");
			} catch (Throwable e) {
				Oce.getLogger().error(this.getClass().getName() + ".get " + " prx = " + prx + " id=" + userId + " type=" + type , e);
			}
			return ret;
		} finally {
			cost.endFinally();
		}
	}

	public UserApiCountData getAll(int userId) {
		TimeCost cost = TimeCost.begin("UserApiCountAdapter.getAll id: "+userId);
		UserApiCountManagerPrx prx = null;
		try {
			if (userId <= 0) {
				return new UserApiCountData();
			}
			try {
				prx = getUserApiCountManager(userId);
				UserApiCounter counter = prx.getAll(userId);
				return new UserApiCountData(counter);
			} catch (Ice.TimeoutException e) {
				Oce.getLogger().error(this.getClass().getName() + ".getAll " + " prx = " + prx + " id=" + userId + " Ice.TimeoutException");
			} catch (Throwable e) {
				Oce.getLogger().error(this.getClass().getName() + ".getAll " + " prx = " + prx + " id=" + userId , e);
			}
			return new UserApiCountData();
		} finally {
			cost.endFinally();
		}
	}

	public void reload(int userId) {
		TimeCost cost = TimeCost.begin("UserApiCountAdapter.reload id: "+userId);
		try {
			if (userId <= 0) {
				return;
			}
			getUserApiCountManager(userId).reload(userId);
		} finally {
			cost.endFinally();
		}
	}
	
	public static void main(String[] args){
		try{
			UserApiCountData data=UserApiCountAdapter.getInstance().getAll(700043939);
			for(int i=0;i<data.types.length;i++){
				System.out.println("type: "+data.types[i]+" value: "+data.values[i]);
			}
			System.out.println(data.types.length);
		}catch(Exception e){
			e.printStackTrace();
		}
		System.exit(0);
	}
}
