package com.xiaonei.xce.iprecord;

import java.util.Vector;

import Ice.ObjectPrx;
import java.net.Inet4Address;
import xce.iprecord.IPRecordManagerPrx;
import xce.iprecord.IPRecordManagerPrxHelper;
import xce.util.service.ReplicatedServiceAdapter;

import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.XceStat;

import xce.clusterstate.ReplicatedClusterAdapter;

public class IPRecordAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerIPRecord";
	private static int _interval = 120;	
	
	private IPRecordAdapter() {
		super(ctr_endpoints,_interval);
	}

	private static IPRecordAdapter _instance = new IPRecordAdapter();
	public static IPRecordAdapter getInstance() {
		return _instance;
	}

	public int[] getUsers(String ip,int limit) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "IPRecord "
				+ ip);
		long ipLong = ipStr2Long(ip);
		if( ipLong == -1 ){
			return new int[] {};
		}
		IPRecordManagerPrx prx = null;
		try {
			prx = getIPRecordManagerTwoway((int)(ipLong%10));
			return prx.getUsers(ipLong,limit);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error("IPRecord getUsers timeout prx = " + prx + " ip = " + ip + "(" + ipLong + ")" );
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return new int[] {};
	}

	public int getCount(String ip) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "IPRecord "
				+ ip);
		long ipLong = ipStr2Long(ip);
		if( ipLong == -1 ){
			return 0;
		}
		IPRecordManagerPrx prx = null;
		try {
			prx = getIPRecordManagerTwoway((int)(ipLong%10));
			return prx.getCount(ipLong);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error("IPRecord getCount timeout prx = " + prx + " ip = " + ip + "(" + ipLong + ")" );
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return 0;
	}

	public int[] getUsers(long ip,int limit) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "IPRecord "
				+ ip);
		IPRecordManagerPrx prx = null;
		try {
			prx = getIPRecordManagerTwoway((int)(ip%10));
			return prx.getUsers(ip,limit);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error("IPRecord getUsers timeout prx = " + prx + " ip = " + "(" + ip + ")" );
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return new int[] {};
	}

	public int getCount(long ip) {
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "IPRecord "
				+ ip);
		IPRecordManagerPrx prx = null;
		try {
			prx = getIPRecordManagerTwoway((int)(ip%10));
			return prx.getCount(ip);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error("IPRecord getCount timeout prx = " + prx + " ip = " + "(" + ip + ")" );
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return 0;
	}

	public void reload(String ip) {
		long ipLong = ipStr2Long(ip);
		if( ipLong == -1 ){
			return;
		}
		getIPRecordManagerOneway((int)(ipLong%10)).reload(ipLong);
	}

	public void updateToDB(long ip) {
		getIPRecordManagerOneway((int)(ip%10)).updateToDB(ip);
	}

	protected IPRecordManagerPrx getIPRecordManagerTwoway(int ip) {
		Ice.ObjectPrx prx0 = getValidProxy(ip, 300);
		if ( prx0 != null ) {
			IPRecordManagerPrx prx = IPRecordManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected IPRecordManagerPrx getIPRecordManagerOneway(int ip) {
		Ice.ObjectPrx prx0 = getValidProxy(ip, 300);
		if ( prx0 != null ) {
			IPRecordManagerPrx prx = IPRecordManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}

	private long ipStr2Long(String ip) {
		try{
			byte[] ips = Inet4Address.getByName(ip).getAddress();
			long res = 0;
			for (int i = 0; i < ips.length; i++) {
				res = (res<<8) + (ips[i]&0xff);
			}
			return res;
		} catch (Exception e) {
			Oce.getLogger().error("[ipStr2Long] error translating ip:\"" + ip + "\"");
			return -1;
		}
	}
	
	public static void main(String[] args) {
		Long res = IPRecordAdapter.getInstance().ipStr2Long("123.125.40.251");
		System.out.println(res);
//		int[] res = IPRecordAdapter.getInstance().getUsers("123.125.40.251",100);
//		System.out.println(res.length);
//		for( int o : res ){
//			System.out.println(o);
//		}
//		int[] res = IPRecordAdapter.getInstance().getUsers(2071800061,100);
//		System.out.println(res.length);
//		for( int o : res ){
//			System.out.println(o);
//		}
//		IPRecordAdapter.getInstance().updateToDB(2071800061);
	}
}

