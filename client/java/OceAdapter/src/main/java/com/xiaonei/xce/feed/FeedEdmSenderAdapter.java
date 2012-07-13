package com.xiaonei.xce.feed;

import java.util.Map;

import xce.edm.FeedEdmSenderPrx;
import xce.edm.FeedEdmSenderPrxHelper;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import Ice.ObjectPrx;

public class FeedEdmSenderAdapter extends
		AbstractModProxyHolder<Integer, FeedEdmSenderPrx> {
	private static FeedEdmSenderAdapter instance;
	private static String endpoints = "@FeedEdmSender";

	public FeedEdmSenderAdapter() {
		super(ChannelFactory.getChannel(ChannelType.XCEFEED), "FEDM"
				+ endpoints, 2);
	}

	public static FeedEdmSenderAdapter getInstance() {
		if (instance == null) {
			instance = new FeedEdmSenderAdapter();
		}
		return instance;
	}

	@Override
	protected int mod(Integer key, int size) {
		return key % size;
	}

	@Override
	public FeedEdmSenderPrx uncheckedCast(ObjectPrx baseProxy) {
		return FeedEdmSenderPrxHelper.uncheckedCast(baseProxy);
	}

	@Override
	protected ObjectPrx initProxy(ObjectPrx baseProxy) {
		return baseProxy;
	}

	private FeedEdmSenderPrx getManagerOneway(int uid) {
		return uncheckedCast(getProxy(uid).ice_oneway());
	}

	private FeedEdmSenderPrx getManager(int uid) {
		return getProxy(uid);
	}

	public void Receive(int user_id, Map<String, String> params)
			throws Exception {
		//getManagerOneway(user_id).Receive(user_id, params);
		getManagerOneway(user_id).Receive(user_id, params);
	}
}
