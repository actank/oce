package com.xiaonei.xce.notify;

import java.util.ArrayList;
import java.util.List;

import xce._notify.NotifyGatePrx;
import xce._notify.NotifyGatePrxHelper;
import xce._notify.apns.APNSManagerPrx;
import xce._notify.apns.APNSManagerPrxHelper;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import Ice.ObjectPrx;

public class APNSNotifyAdapter extends AbstractModProxyHolder<Integer, APNSManagerPrx>{

	private static APNSNotifyAdapter instance;
	private static String endpoints = "@APNSNotify";

	private APNSNotifyAdapter() {
		super(ChannelFactory.getChannel(ChannelType.NOTIFY),
				"M"+endpoints, 1);
	}

	public static APNSNotifyAdapter getInstance() {
		if (instance == null) {
			instance = new APNSNotifyAdapter();
		}
		return instance;
	}

	@Override
	protected int mod(Integer key, int size) {
		return key % size;
	}

	@Override
	public APNSManagerPrx uncheckedCast(ObjectPrx baseProxy) {
		return APNSManagerPrxHelper.uncheckedCast(baseProxy);
	}

	@Override
	protected ObjectPrx initProxy(ObjectPrx baseProxy) {
		return baseProxy;
	}
	
	private APNSManagerPrx getManagerOneway(int uid) {
		return uncheckedCast(getProxy(uid).ice_oneway());
	}

	private APNSManagerPrx getManager(int uid) {
		return getProxy(uid);
	}
	
	public void addUser(int userId, String token){
		getManagerOneway(userId).addUser(userId,token);
	}
}
