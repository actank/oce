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

class FeedItemAdapter extends AbstractModProxyHolder<Integer, FeedItemManagerPrx>{

	private static FeedItemAdapter instance;
	private static String endpoints = "@FeedItem";

	public static FeedItemAdapter getInstance() {
		if (instance == null) {
			instance = new FeedItemAdapter();
		}
		return instance;
	}
	

	public FeedItemAdapter() {
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
	
	
	
	
	public List<FeedData> getNewsByStype(int uid, int[] stypes, int begin, int limit){
		List<FeedData> list = new ArrayList<FeedData>();
		FeedData [] contents = getManager(uid).getFeedDataByStype(uid, stypes, begin, limit);
		for(int i = 0 ; i < contents.length; ++i){
			list.add(contents[i]);
		}
		return list;
	}
	public List<FeedData> getNewsByType(int uid, int[] types, int begin, int limit){
		List<FeedData> list = new ArrayList<FeedData>();
		FeedData [] contents = getManager(uid).getFeedDataByType(uid, types, begin, limit);
		for(int i = 0 ; i < contents.length; ++i){
			list.add(contents[i]);
		}
		return list;
	}
	public List<FeedData> getNews(int uid, int begin, int limit){
		List<FeedData> list = new ArrayList<FeedData>();
		FeedData [] contents = getManager(uid).getFeedData(uid,  begin, limit);
		for(int i = 0 ; i < contents.length; ++i){
			list.add(contents[i]);
		}
		return list;
	}
	public List<FeedData> getNews2(int uid, int begin, int limit){
		List<FeedData> list = new ArrayList<FeedData>();
		FeedData [] contents = getManager(uid).getFeedData2(uid,  begin, limit);
		for(int i = 0 ; i < contents.length; ++i){
			list.add(contents[i]);
		}
		return list;
	}
	
	public List<FeedData> getFeedDataByIds(long[] feedIds){
		List<FeedData> list = new ArrayList<FeedData>();
		if(feedIds.length == 0){
			return list;
		}
		FeedData [] contents  = getManager((int)(feedIds[0]% 10000)).getFeedDataByIds(feedIds);
		for(int i = 0 ; i < contents.length; ++i){
			list.add(contents[i]);
		}
		return list;
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
