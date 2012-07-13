package com.xiaonei.xce.feed;

import java.util.ArrayList;
import java.util.List;

import xce.feed.FeedItem;
import xce.feed.FeedCachePrx;
import xce.feed.FeedCachePrxHelper;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import Ice.ObjectPrx;

public class FeedCacheAdapter extends AbstractModProxyHolder<Integer, FeedCachePrx>{

	private static FeedCacheAdapter instance;
	private static String endpoints = "@FeedCache";

	public static FeedCacheAdapter getInstance() {
		if (instance == null) {
			instance = new FeedCacheAdapter();
		}
		return instance;
	}
	

	public FeedCacheAdapter() {
		super(ChannelFactory.getChannel(ChannelType.XCEFEED),
				"M"+endpoints, 10);
	}
	@Override
	protected int mod(Integer key, int size) {
		return key % size;
	}

	@Override
	public FeedCachePrx uncheckedCast(ObjectPrx baseProxy) {
		return FeedCachePrxHelper.uncheckedCast(baseProxy);
	}

	@Override
	protected ObjectPrx initProxy(ObjectPrx baseProxy) {
		return baseProxy;
	}
	
	
	private FeedCachePrx getManagerOneway(int uid) {
		return uncheckedCast(getProxy(uid).ice_oneway());
	}

	private FeedCachePrx getManager(int uid) {
		return getProxy(uid);
	}
	
	
	public List<FeedItem> get(int uid){
		List<FeedItem> list = new ArrayList<FeedItem>();
		FeedItem [] contents = getManager(uid).get(uid);
		for(int i = 0 ; i < contents.length; ++i){
			list.add(contents[i]);
		}
		return list;
	}
	
	public int getCount(int uid){
		return getManager(uid).getCount(uid);
	}
	
}
