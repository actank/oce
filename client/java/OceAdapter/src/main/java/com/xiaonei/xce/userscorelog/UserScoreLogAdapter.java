package com.xiaonei.xce.userscorelog;

import xce.userscore.UserScoreLogManagerPrx;
import xce.userscore.UserScoreLogManagerPrxHelper;
import xce.util.tools.MathUtil;

import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.XceStat;

import xce.clusterstate.ReplicatedClusterAdapter;

public class UserScoreLogAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerUserScoreLog";
	private static int _interval = 120;	
	
	private UserScoreLogAdapter() {
		super(ctr_endpoints,_interval);
	}

	private static UserScoreLogAdapter _instance = new UserScoreLogAdapter();
	public static UserScoreLogAdapter getInstance() {
		return _instance;
	}

	public void addLog(int userId,int type,int scoreAdd) {
		try {
			getUserScoreLogOneway(userId).addLog(userId,type,scoreAdd);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error("UserScoreLogAdapter addLog timeout ");
		}
	}

	protected UserScoreLogManagerPrx getUserScoreLogOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			UserScoreLogManagerPrx prx = UserScoreLogManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}

	protected UserScoreLogManagerPrx getUserScoreLogUdp(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			UserScoreLogManagerPrx prx = UserScoreLogManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_datagram());
			return prx;
		} else {
			return null;
		}
	}
	
	public static void main(String[] args) {
		UserScoreLogAdapter.getInstance().addLog(24496255,4,100);
	}
}
