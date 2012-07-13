package com.xiaonei.xce.feed;

import java.util.ArrayList;
import java.util.List;


import xce.feed.FeedDispatcherPrx;
import xce.feed.FeedDispatcherPrxHelper;
import xce.feed.LoadType;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import Ice.ObjectPrx;

class FeedDispatcherAdapter  extends AbstractModProxyHolder<Integer, FeedDispatcherPrx>{

	private static FeedDispatcherAdapter instance;
	private static String endpoints = "@FeedDispatcher";

	public static FeedDispatcherAdapter getInstance() {
		if (instance == null) {
			instance = new FeedDispatcherAdapter();
		}
		return instance;
	}
	

	public FeedDispatcherAdapter() {
		super(ChannelFactory.getChannel(ChannelType.XCEFEED),
				"FD"+endpoints, 10);
	}
	@Override
	protected int mod(Integer key, int size) {
		return key % size;
	}

	@Override
	public FeedDispatcherPrx uncheckedCast(ObjectPrx baseProxy) {
		return FeedDispatcherPrxHelper.uncheckedCast(baseProxy);
	}

	@Override
	protected ObjectPrx initProxy(ObjectPrx baseProxy) {
		return baseProxy;
	}
	
	
	private FeedDispatcherPrx getManagerOneway(int uid) {
		return uncheckedCast(getProxy(uid).ice_oneway());
	}

	private FeedDispatcherPrx getManager(int uid) {
		return getProxy(uid);
	}
	
	
	public void joinPageNotify(int pageId, int fansId){
		getManagerOneway(pageId).joinPageNotify(pageId,fansId);
	}
	
	public void guideBuddyReplyNotify(int newUser, int friend){
		getManagerOneway(friend).guideBuddyReplyNotify(newUser,friend);
	}
	
	public void reload(LoadType  type , int id){
		for(int i = 0; i < 10; ++i){
			getManagerOneway(i).Reload(type, id);
		}
	}
	
	

}
