package com.xiaonei.xce.feed;

import java.util.Map;


import xce.feed.FeedCreatorPrx;
import xce.feed.FeedCreatorPrxHelper;
import xce.feed.FeedReply;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import Ice.ObjectPrx;
import MyUtil.MceException;

public class FeedCreatorAdapter extends AbstractModProxyHolder<Integer, FeedCreatorPrx>{

	private static FeedCreatorAdapter instance;
	private static String endpoints = "@FeedAssistant";

	public FeedCreatorAdapter() {
		super(ChannelFactory.getChannel(ChannelType.XCEFEED),
				"FA"+endpoints, 1);
	}

	public static FeedCreatorAdapter getInstance() {
		if (instance == null) {
			instance = new FeedCreatorAdapter();
		}
		return instance;
	}

	@Override
	protected int mod(Integer key, int size) {
		return key % size;
	}

	@Override
	public FeedCreatorPrx uncheckedCast(ObjectPrx baseProxy) {
		return FeedCreatorPrxHelper.uncheckedCast(baseProxy);
	}

	@Override
	protected ObjectPrx initProxy(ObjectPrx baseProxy) {
		return baseProxy;
	}
	
	private FeedCreatorPrx getManagerOneway(int uid) {
		return uncheckedCast(getProxy(uid).ice_oneway());
	}

	private FeedCreatorPrx getManager(int uid) {
		return getProxy(uid);
	}
	
	public boolean Create(int stype, int version, Map<String,String> props) throws MceException{
		return getManager(0).Create(stype, version, props);
	}
	public boolean CreateWithReply(int stype, int version, Map<String,String> props, FeedReply reply) throws MceException{
		return getManager(0).CreateWithReply(stype, version, props, reply);
	}
	

}
