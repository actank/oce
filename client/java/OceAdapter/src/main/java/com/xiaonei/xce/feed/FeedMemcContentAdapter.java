package com.xiaonei.xce.feed;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.HashMap;

import xce._notify.ObserverPrx;
import xce.feed.FeedData;
import xce.feed.FeedContent;
import xce.feed.FeedDataRes;
import xce.feed.FeedItem;
import xce.feed.FeedMemcContentPrx;
import xce.feed.FeedMemcContentPrxHelper;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import Ice.ObjectPrx;

public class FeedMemcContentAdapter extends AbstractModProxyHolder<Integer, FeedMemcContentPrx>{

	private static FeedMemcContentAdapter instance;
	private static String endpoints = "@FeedMemcContent";

	public static FeedMemcContentAdapter getInstance() {
		if (instance == null) {
			instance = new FeedMemcContentAdapter();
		}
		return instance;
	}
	

	public FeedMemcContentAdapter() {
		super(ChannelFactory.getChannel(ChannelType.MCPROXY),
				"M"+endpoints, 1);
	}
	
	@Override
	protected int mod(Integer key, int size) {
		return key % size;
	}

	@Override
	public FeedMemcContentPrx uncheckedCast(ObjectPrx baseProxy) {
		return FeedMemcContentPrxHelper.uncheckedCast(baseProxy);
	}

	@Override
	protected ObjectPrx initProxy(ObjectPrx baseProxy) {
		return baseProxy;
	}
	
	private FeedMemcContentPrx getManagerOneway(int uid) {
		return uncheckedCast(getProxy(uid).ice_oneway());
	}

	private FeedMemcContentPrx getManager(int uid) {
		return getProxy(uid);
	}
	
	public FeedData[] GetFeedDataByIds(int uid,long[] feedIds){
		return getManager((int)(uid % 10000)).GetFeedDataByIds(uid,feedIds);
	}
	public static void main(String[] args) throws Exception {
		System.out.println("start testing ...");
		long[] fids={13438669773L,13429490941L};
		
		FeedData[] data = FeedMemcContentAdapter.getInstance().GetFeedDataByIds(1,fids);
		System.out.println(data.length);
		for(FeedData d:data){
			System.out.println(d.xml);
		}
//		NotifyStormObserver observer = new NotifyStormObserverExample();
//		NotifyStormAdapter.getInstance().subscribe("PLATFORM_REPLY", observer);
//
//
//		Map<String, ObserverPrx[]> pool = NotifyStormAdapter.getInstance().query("PLATFORM_REPLY");
//		for (ObserverPrx o : pool.get("Test")) {
//			System.out.println("AAA:"+o);
//		}
		
		while(true){
			Thread.sleep(1* 1000);
		}

	}
}
