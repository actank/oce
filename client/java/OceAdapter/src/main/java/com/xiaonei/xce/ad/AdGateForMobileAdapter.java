package com.xiaonei.xce.ad;

import xce.ad.AdGateMPrx;
import xce.ad.AdGateMPrxHelper;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import Ice.ObjectPrx;

public class AdGateForMobileAdapter extends
		AbstractModProxyHolder<Integer, AdGateMPrx> {

	private static AdGateForMobileAdapter instance;
	private static String endpoints = "@AdGateM";

	public AdGateForMobileAdapter() {
		super(ChannelFactory.getChannel(ChannelType.AD), "M" + endpoints, 6);
	}

	public static AdGateForMobileAdapter getInstance() {
		if (instance == null) {
			instance = new AdGateForMobileAdapter();
		}
		return instance;
	}

	@Override
	protected int mod(Integer key, int size) {
		return key % size;
	}

	@Override
	public AdGateMPrx uncheckedCast(ObjectPrx baseProxy) {
		return AdGateMPrxHelper.uncheckedCast(baseProxy);
	}

	@Override
	protected ObjectPrx initProxy(ObjectPrx baseProxy) {
		return baseProxy;
	}

	private AdGateMPrx getManagerOneway(int uid) {
		return uncheckedCast(getProxy(uid).ice_timeout(100).ice_oneway());
	}

	private AdGateMPrx getManager(int uid) {
		return uncheckedCast(getProxy(uid).ice_timeout(100));
	}

	public String getAds(int userid, long zoneid, String ip, int rotate_id){
		return getManager(userid).GetAds(userid,zoneid,ip);
	}
}
