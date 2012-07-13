package com.xiaonei.xce.feed;

import java.util.ArrayList;
import java.util.List;

import xce.feed.FeedData;

import xce.feed.FeedGroupManagerPrx;
import xce.feed.FeedGroupManagerPrxHelper;

import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import Ice.ObjectPrx;

public class FeedGroupAdapter extends
		AbstractModProxyHolder<Integer, FeedGroupManagerPrx> {

	private static FeedGroupAdapter instance;
	private static String endpoints = "@FeedGroup";

	public static FeedGroupAdapter getInstance() {
		if (instance == null) {
			instance = new FeedGroupAdapter();
		}
		return instance;
	}

	public FeedGroupAdapter() {
		super(ChannelFactory.getChannel(ChannelType.XCEFEED), "M"
				+ endpoints, 10);
	}

	@Override
	protected int mod(Integer key, int size) {
		return key % size;
	}

	@Override
	public FeedGroupManagerPrx uncheckedCast(ObjectPrx baseProxy) {
		return FeedGroupManagerPrxHelper.uncheckedCast(baseProxy);
	}

	@Override
	protected ObjectPrx initProxy(ObjectPrx baseProxy) {
		return baseProxy;
	}

	private FeedGroupManagerPrx getManagerOneway(int gid) {
		return uncheckedCast(getProxy(gid).ice_oneway());
	}

	private FeedGroupManagerPrx getManager(int gid) {
		return getProxy(gid);
	}
	
	
	public List<FeedData> GetFeedData(int gid, int memberid, int begin, int limit) {

		List<FeedData> list = new ArrayList<FeedData>();
		FeedData[] listArray = getManager(gid).GetFeedData(gid, memberid, begin, limit);
		
		for (int i = 0; i < listArray.length; ++i) {
			list.add(listArray[i]);
		}
		return list;
	}
	
	public List<FeedData> GetFeedDataByType(int gid, int memberid, int[] types, int begin, int limit) {

		List<FeedData> list = new ArrayList<FeedData>();
		FeedData[] listArray = getManager(gid).GetFeedDataByType(gid, memberid, types, begin, limit);
		
		for (int i = 0; i < listArray.length; ++i) {
			list.add(listArray[i]);
		}
		return list;
	}
	
	public List<FeedData> GetFeedDataByStype(int gid, int memberid, int[] stypes, int begin, int limit) {

		List<FeedData> list = new ArrayList<FeedData>();
		FeedData[] listArray = getManager(gid).GetFeedDataByStype(gid, memberid, stypes,begin, limit);
		
		for (int i = 0; i < listArray.length; ++i) {
			list.add(listArray[i]);
		}
		return list;
	}

	
			
			
}
