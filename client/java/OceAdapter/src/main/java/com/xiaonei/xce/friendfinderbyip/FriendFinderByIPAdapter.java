package com.xiaonei.xce.friendfinderbyip;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Vector;
import java.util.Map.Entry;

import Ice.ObjectPrx;
import java.net.Inet4Address;
import xce.friendfinder.IPNote;
import xce.friendfinder.FriendFinderByIPManagerPrx;
import xce.friendfinder.FriendFinderByIPManagerPrxHelper;
import xce.util.service.ReplicatedServiceAdapter;

import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.XceStat;

import xce.clusterstate.ReplicatedClusterAdapter;

public class FriendFinderByIPAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerFriendFinderByIP";
	private static int _interval = 120;	
	
	private FriendFinderByIPAdapter() {
		super(ctr_endpoints,_interval);
	}

	private static FriendFinderByIPAdapter _instance = new FriendFinderByIPAdapter();
	public static FriendFinderByIPAdapter getInstance() {
		return _instance;
	}

	public List<IPData> getFreqIP(int userId,int limit) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FriendFinderByIP "
				+ userId);

                List<IPData> result = new ArrayList<IPData>();

                IPNote[] ret = null;
                FriendFinderByIPManagerPrx prx= null;
                try {
                	prx=getFriendFinderByIPManagerTwoway(userId);
                	ret = prx.getFreqIP(userId,limit);
                } catch (Ice.TimeoutException e) {
                	Oce.getLogger().error(this.getClass().getName() + " getFreqIP  [ " + prx + " ] id="  + userId + "(" + limit + ")" + e);
                	//System.err.println("FriendFinderByIP getFreqIP timeout " + userId + "(" + limit + ")" );
                	stat.endException(e);
		} finally {
			stat.endFinally();
                }

                if (ret == null){
                        return result;
		}

                for (IPNote one : ret) {
                        IPData obj = new IPData();
                        obj.setBase(one);
                        result.add(obj);
                }
                return result;
	}

	public List<IPData> getRecIP(int userId,int limit) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FriendFinderByIP "
				+ userId);

                List<IPData> result = new ArrayList<IPData>();

                IPNote[] ret = null;
                FriendFinderByIPManagerPrx prx= null;
                try {
                	prx=getFriendFinderByIPManagerTwoway(userId);
                	ret = prx.getRecIP(userId,limit);
                } catch (Ice.TimeoutException e) {
                	Oce.getLogger().error(this.getClass().getName() + " getRecIP  [ " + prx + " ] id="  + userId + "(" + limit + ")" + e);
			//System.err.println("FriendFinderByIP getRecIP timeout " + userId + "(" + limit + ")" );
			stat.endException(e);
		} finally {
			stat.endFinally();
                }

                if (ret == null){
                        return result;
		}

                for (IPNote one : ret) {
                        IPData obj = new IPData();
                        obj.setBase(one);
                        result.add(obj);
                }
                return result;
	}

	public void reload(int userId) {
		getFriendFinderByIPManagerOneway(userId).reload(userId);
	}

	public void accessIP(int userId, long ip) {
		getFriendFinderByIPManagerOneway(userId).accessIP(userId,ip);
	}

	public void accessIPStr(int userId, String ip) {
		getFriendFinderByIPManagerOneway(userId).accessIPStr(userId,ip);
	}

	public void accessIPs(int userId, long[] ip) {
		getFriendFinderByIPManagerOneway(userId).accessIPs(userId,ip);
	}

	public void accessIPsStr(int userId, String[] ips) {
		getFriendFinderByIPManagerOneway(userId).accessIPsStr(userId,ips);
	}

	protected FriendFinderByIPManagerPrx getFriendFinderByIPManagerTwoway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			FriendFinderByIPManagerPrx prx = FriendFinderByIPManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected FriendFinderByIPManagerPrx getFriendFinderByIPManagerOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			FriendFinderByIPManagerPrx prx = FriendFinderByIPManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}

}

