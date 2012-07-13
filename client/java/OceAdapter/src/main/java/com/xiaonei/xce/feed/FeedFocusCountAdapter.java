
package com.xiaonei.xce.feed;

import java.util.ArrayList;
import java.util.List;

import xce.feed.FeedData;
import xce.feed.FocusManagerPrx;
import xce.feed.FocusManagerPrxHelper;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import Ice.ObjectPrx;

public class FeedFocusCountAdapter extends AbstractModProxyHolder<Integer, FocusManagerPrx>{

	private static FeedFocusCountAdapter instance;
	private static String endpoints = "@FeedFocus";

	public static FeedFocusCountAdapter getInstance() {
		if (instance == null) {
			instance = new FeedFocusCountAdapter();
		}
		return instance;
	}
	

	public FeedFocusCountAdapter() {
		super(ChannelFactory.getChannel(ChannelType.XCEFEED),
				"M"+endpoints, 5);
	}
	@Override
	protected int mod(Integer key, int size) {
		return key % size;
	}

	@Override
	public FocusManagerPrx uncheckedCast(ObjectPrx baseProxy) {
		return FocusManagerPrxHelper.uncheckedCast(baseProxy);
	}

	@Override
	protected ObjectPrx initProxy(ObjectPrx baseProxy) {
		return baseProxy;
	}
	
	
	private FocusManagerPrx getManagerOneway(int uid) {
		return uncheckedCast(getProxy(uid).ice_oneway());
	}

	private FocusManagerPrx getManager(int uid) {
		return getProxy(uid);
	}
	
	
	
	
	public int getFocusFeedIncCount(int uid){
		int count = getManager(uid).GetFocusFeedIncCount(uid);		
		return count;
	}

	
	


}

