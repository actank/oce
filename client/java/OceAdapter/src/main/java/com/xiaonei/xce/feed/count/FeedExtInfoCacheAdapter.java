package com.xiaonei.xce.feed.count;

import xce.feed.count.FeedExtInfoCachePrx;
import xce.feed.count.FeedExtInfoCachePrxHelper;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import Ice.ObjectPrx;
public class FeedExtInfoCacheAdapter  extends AbstractModProxyHolder<Integer, FeedExtInfoCachePrx> {

	


	private static FeedExtInfoCacheAdapter instance;
	private static String endpoints = "@FeedExtInfoCache";

	public static FeedExtInfoCacheAdapter getInstance() {
		if (instance == null) {
			instance = new FeedExtInfoCacheAdapter();
		}
		return instance;
	}
	

	public FeedExtInfoCacheAdapter() {
		super(ChannelFactory.getChannel(ChannelType.XCEFEED),
				"M"+endpoints, 10);
	}
	@Override
	protected int mod(Integer key, int size) {
		return key % size;
	}

	@Override
	public FeedExtInfoCachePrx uncheckedCast(ObjectPrx baseProxy) {
		return FeedExtInfoCachePrxHelper.uncheckedCast(baseProxy);
	}

	@Override
	protected ObjectPrx initProxy(ObjectPrx baseProxy) {
		return baseProxy;
	}
	
	
	private FeedExtInfoCachePrx getManagerOneway(int source) {
		return uncheckedCast(getProxy(source).ice_oneway());
	}

	private FeedExtInfoCachePrx getManager(int source) {
		return getProxy(source);
	}
	
	
	public int GetCount(long source, int stype, int actor, byte index) {
		int s = (int)(source %1000);
		return  getManager(s).GetCount(source, stype, actor, index);
	}
	


	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		FeedExtInfoCacheAdapter adapter = FeedExtInfoCacheAdapter.getInstance();
		System.out.println(adapter);
		long source = 100;
		int stype = 110;
		int actor = 10;
		byte index = 0;
		adapter.GetCount(source,stype,actor, index);
		//System.out.println(n);
	}

}
