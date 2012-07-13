package com.xiaonei.xce.feed;

import java.util.ArrayList;
import java.util.List;

import xce.feed.FeedData;
import xce.feed.FeedItemManagerPrx;
import xce.feed.FeedItemManagerPrxHelper;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import Ice.ObjectPrx;

class FeedMiniAdapter extends AbstractModProxyHolder<Integer, FeedItemManagerPrx>{

	private static FeedMiniAdapter instance;
	private static String endpoints = "@FeedMini";

	public static FeedMiniAdapter getInstance() {
		if (instance == null) {
			instance = new FeedMiniAdapter();
		}
		return instance;
	}
	

	public FeedMiniAdapter() {
		super(ChannelFactory.getChannel(ChannelType.XCEFEED),
				"FIM"+endpoints, 20);
	}
	@Override
	protected int mod(Integer key, int size) {
		return key % size;
	}

	@Override
	public FeedItemManagerPrx uncheckedCast(ObjectPrx baseProxy) {
		return FeedItemManagerPrxHelper.uncheckedCast(baseProxy);
	}

	@Override
	protected ObjectPrx initProxy(ObjectPrx baseProxy) {
		return baseProxy;
	}
	
	
	private FeedItemManagerPrx getManagerOneway(int uid) {
		return uncheckedCast(getProxy(uid).ice_oneway());
	}

	private FeedItemManagerPrx getManager(int uid) {
		return getProxy(uid);
	}
	
	
	
	
	public List<FeedData> getMiniByStype(int uid, int[] stypes, int begin, int limit){
		List<FeedData> list = new ArrayList<FeedData>();
		FeedData [] contents = getManager(uid).getFeedDataByStype(uid, stypes, begin, limit);
		for(int i = 0 ; i < contents.length; ++i){
			list.add(contents[i]);
		}
		return list;
	}
	public List<FeedData> getMiniByType(int uid, int[] types, int begin, int limit){
		List<FeedData> list = new ArrayList<FeedData>();
		FeedData [] contents = getManager(uid).getFeedDataByType(uid, types, begin, limit);
		for(int i = 0 ; i < contents.length; ++i){
			list.add(contents[i]);
		}
		return list;
	}
	public List<FeedData> getMini(int uid, int begin, int limit){
		List<FeedData> list = new ArrayList<FeedData>();
		FeedData [] contents = getManager(uid).getFeedData(uid,  begin, limit);
		for(int i = 0 ; i < contents.length; ++i){
			list.add(contents[i]);
		}
		return list;
	}
	
	public boolean hasFeed(int uid){
		return getManager(uid).hasFeed(uid);
	}
	
//	public List<FeedData> getMiniByStype(int uid, int[] stypes, int begin, int limit){
//		
//	}
//	public List<FeedData> getMiniByType(int uid, int[] types, int begin, int limit){
//		
//	}
//	public List<FeedData> getMini(int uid, int begin, int limit){
//		
//	}
}