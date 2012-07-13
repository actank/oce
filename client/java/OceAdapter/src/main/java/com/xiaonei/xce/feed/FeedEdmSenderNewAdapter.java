package com.xiaonei.xce.feed;

import java.util.Map;

import xce.edm.FeedEdmSenderPrx;
import xce.edm.FeedEdmSenderPrxHelper;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import Ice.ObjectPrx;

public class FeedEdmSenderNewAdapter extends
		AbstractModProxyHolder<Integer, FeedEdmSenderPrx> {
	private static FeedEdmSenderNewAdapter instance;
	private static String endpoints = "@FeedEdmSender";

	public FeedEdmSenderNewAdapter() {
		super(ChannelFactory.getChannel(ChannelType.AD), "FEDM"
				+ endpoints, 2);
	}

	public static FeedEdmSenderNewAdapter getInstance() {
		if (instance == null) {
			instance = new FeedEdmSenderNewAdapter();
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
