package com.xiaonei.xce.feed;

import java.util.Map;


import xce.feed.FeedGroupCountPrx;
import xce.feed.FeedGroupCountPrxHelper;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import Ice.ObjectPrx;

public class FeedGroupCountAdapter  extends AbstractModProxyHolder<Integer, FeedGroupCountPrx>{

	private static FeedGroupCountAdapter instance;
	private static String endpoints = "@FeedGroupCount";

	public static FeedGroupCountAdapter getInstance() {
		if (instance == null) {
			instance = new FeedGroupCountAdapter();
		}
		return instance;
	}
	

	public FeedGroupCountAdapter() {
		super(ChannelFactory.getChannel(ChannelType.XCEFEED),
				"M"+endpoints, 5);
	}
	@Override
	protected int mod(Integer key, int size) {
		return key % size;
	}

	@Override
	public FeedGroupCountPrx uncheckedCast(ObjectPrx baseProxy) {
		return FeedGroupCountPrxHelper.uncheckedCast(baseProxy);
	}

	@Override
	protected ObjectPrx initProxy(ObjectPrx baseProxy) {
		return baseProxy;
	}
	
	
	private FeedGroupCountPrx getManagerOneway(int uid) {
		return uncheckedCast(getProxy(uid).ice_oneway());
	}

	private FeedGroupCountPrx getManager(int uid) {
		return getProxy(uid);
	}
	
	public Map<Integer,Integer> getUnread(int uid, int[] groups){
		return getManager(uid).GetUnread(uid, groups);
	}
	

}
