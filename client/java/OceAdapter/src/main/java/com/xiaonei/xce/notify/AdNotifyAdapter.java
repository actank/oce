package com.xiaonei.xce.notify;

import xce.ad.AdNotifierPrx;
import xce.ad.AdNotifierPrxHelper;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import Ice.ObjectPrx;

public class AdNotifyAdapter extends AbstractModProxyHolder<Integer, AdNotifierPrx>{

	private static AdNotifyAdapter instance;
	private static String endpoints = "@AdNotify";

	private AdNotifyAdapter() {
		super(ChannelFactory.getChannel(ChannelType.NOTIFY),
				"M"+endpoints, 1);
	}

	public static AdNotifyAdapter getInstance() {
		if (instance == null) {
			instance = new AdNotifyAdapter();
		}
		return instance;
	}

	@Override
	protected int mod(Integer key, int size) {
		return key % size;
	}

	@Override
	public AdNotifierPrx uncheckedCast(ObjectPrx baseProxy) {
		return AdNotifierPrxHelper.uncheckedCast(baseProxy);
	}

	@Override
	protected ObjectPrx initProxy(ObjectPrx baseProxy) {
		return baseProxy;
	}
	
	private AdNotifierPrx getManagerOneway(int uid) {
		return uncheckedCast(getProxy(uid).ice_oneway());
	}

	private AdNotifierPrx getManager(int uid) {
		return getProxy(uid);
	}
	
	public void Login(int uid){
		getManagerOneway(uid).Login(uid);
	}

}
