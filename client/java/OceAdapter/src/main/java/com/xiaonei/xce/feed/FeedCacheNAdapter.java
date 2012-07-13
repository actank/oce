package com.xiaonei.xce.feed;

import java.util.ArrayList;
import java.util.List;

import xce.feed.FeedItem;
import xce.feed.FeedCacheNPrx;
import xce.feed.FeedCacheNPrxHelper;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import Ice.ObjectPrx;

public class FeedCacheNAdapter extends AbstractModProxyHolder<Integer, FeedCacheNPrx>{

	private static FeedCacheNAdapter instance;
	private static String endpoints = "@FeedCacheN";

	public static FeedCacheNAdapter getInstance() {
		if (instance == null) {
			instance = new FeedCacheNAdapter();
		}
		return instance;
	}
	

	public FeedCacheNAdapter() {
		super(ChannelFactory.getChannel(ChannelType.XCEFEED),
				"M"+endpoints, 10);
	}
	@Override
	protected int mod(Integer key, int size) {
		return key % size;
	}

	@Override
	public FeedCacheNPrx uncheckedCast(ObjectPrx baseProxy) {
		return FeedCacheNPrxHelper.uncheckedCast(baseProxy);
	}

	@Override
	protected ObjectPrx initProxy(ObjectPrx baseProxy) {
		return baseProxy;
	}
	
	
	private FeedCacheNPrx getManagerOneway(int uid) {
		return uncheckedCast(getProxy(uid).ice_oneway());
	}

	private FeedCacheNPrx getManager(int uid) {
		return getProxy(uid);
	}
	
/*	
	public List<FeedItem> get(int uid){
		List<FeedItem> list = new ArrayList<FeedItem>();
		FeedItem [] contents = getManager(uid).get(uid);
		for(int i = 0 ; i < contents.length; ++i){
			list.add(contents[i]);
		}
		return list;
	}
*/	
	public int getCount(int uid){
		return getManager(uid).getCount(uid);
	}

//  public static void main(String[] args) {
//    long l = FeedCacheNAdapter.getInstance().getCount(225092473);
//    System.out.println("l:"+l); 
//  }
	
}
