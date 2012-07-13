package com.xiaonei.xce.feed;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import xce.feed.FeedData;
import xce.feed.FeedDataRes;
import xce.feed.FeedItem;
import xce.feed.FeedNewsManagerPrx;
import xce.feed.FeedNewsManagerPrxHelper;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import Ice.ObjectPrx;


public class FeedNewsAdapter extends AbstractModProxyHolder<Integer, FeedNewsManagerPrx>{

	private static FeedNewsAdapter instance;
	private static String endpoints = "@FeedNews";

	public static FeedNewsAdapter getInstance() {
		if (instance == null) {
			instance = new FeedNewsAdapter();
		}
		return instance;
	}
	

	public FeedNewsAdapter() {
		super(ChannelFactory.getChannel(ChannelType.XCEFEED),
				"FIM"+endpoints, 20);
	}
	@Override
	protected int mod(Integer key, int size) {
		return key % size;
	}

	@Override
	public FeedNewsManagerPrx uncheckedCast(ObjectPrx baseProxy) {
		return FeedNewsManagerPrxHelper.uncheckedCast(baseProxy);
	}

	@Override
	protected ObjectPrx initProxy(ObjectPrx baseProxy) {
		return baseProxy;
	}
	
	
	private FeedNewsManagerPrx getManagerOneway(int uid) {
		return uncheckedCast(getProxy(uid).ice_oneway());
	}

	private FeedNewsManagerPrx getManager(int uid) {
		return getProxy(uid);
	}
	
	
	
	
	public List<FeedData> getLiveFeedByStype(int uid, int[] stypes, int begin, int limit){
		List<FeedData> list = new ArrayList<FeedData>();
		FeedDataRes feed_data_res = getManager(uid).GetLiveFeedDataByStype(uid, stypes,  begin, limit);
		FeedData [] contents = feed_data_res.data;
		for(int i = 0 ; i < contents.length; ++i){
			list.add(contents[i]);
		}
		return list;
	}
	
	public List<FeedData> getHotFeedByStype(int uid, int[] stypes, int begin, int limit){
		List<FeedData> list = new ArrayList<FeedData>();
		FeedDataRes feed_data_res = getManager(uid).GetHotFeedDataByStype(uid, stypes,  begin, limit);
		FeedData [] contents = feed_data_res.data;
		for(int i = 0 ; i < contents.length; ++i){
			list.add(contents[i]);
		}
		return list;
	}
	
	public List<FeedData> getLiveFeedByType(int uid, int[] types, int begin, int limit){
		List<FeedData> list = new ArrayList<FeedData>();
		FeedDataRes feed_data_res = getManager(uid).GetLiveFeedDataByType(uid, types,  begin, limit);
		FeedData [] contents = feed_data_res.data;
		for(int i = 0 ; i < contents.length; ++i){
			list.add(contents[i]);
		}
		return list;
	}
	

	public List<FeedData> getHotFeedByType(int uid, int[] types, int begin, int limit){
		List<FeedData> list = new ArrayList<FeedData>();
		FeedDataRes feed_data_res = getManager(uid).GetHotFeedDataByType(uid, types,  begin, limit);
		FeedData [] contents = feed_data_res.data;
		for(int i = 0 ; i < contents.length; ++i){
			list.add(contents[i]);
		}
		return list;
	}
	
	public List<FeedData> getLiveFeed(int uid, int begin, int limit){
		
		List<FeedData> list = new ArrayList<FeedData>();
		FeedDataRes feed_data_res = getManager(uid).GetLiveFeedData(uid,  begin, limit);
		FeedData [] contents = feed_data_res.data;
		for(int i = 0 ; i < contents.length; ++i){
			list.add(contents[i]);
		}
		return list;
	}
	

	public List<FeedData> getHotFeed(int uid, int begin, int limit) {

		List<FeedData> list = new ArrayList<FeedData>();
		FeedDataRes feed_data_res = getManager(uid).GetHotFeedData(uid, begin,
				limit);
		FeedData[] contents = feed_data_res.data;
		for (int i = 0; i < contents.length; ++i) {
			list.add(contents[i]);
		}
		return list;
	}

	
	public List<FeedData> getFeedDataByIds(long[] feedIds){
		List<FeedData> list = new ArrayList<FeedData>();
		if(feedIds.length == 0){
			return list;
		}
		FeedData [] contents  = getManager((int)(feedIds[0]% 10000)).GetFeedDataByIds(feedIds);
		for(int i = 0 ; i < contents.length; ++i){
			list.add(contents[i]);
		}
		return list;
	}
	
	public List<FeedItem>  getOriginalFeedData(int uid) {
		List<FeedItem> list = new ArrayList<FeedItem>();
		
		FeedItem [] items = getManager(uid).GetOriginalFeedData(uid);
		for(int i = 0 ; i < items.length; ++i){
			list.add(items[i]);
		}
		return list;
	}
	
	public int  getUnviewedCount(int uid, int type) {
			
		return  getManager(uid).GetUnviewedCount(uid, type);
	
	}

	

}
