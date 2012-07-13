package com.xiaonei.xce.feed;

import xce.feed.FeedAdDispatcherPrx;
import xce.feed.FeedAdDispatcherPrxHelper;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import Ice.ObjectPrx;
import java.util.Map;

public class FeedAdDispatcherAdapter extends AbstractModProxyHolder<Integer, FeedAdDispatcherPrx>{

	private static FeedAdDispatcherAdapter instance;
	private static String endpoints = "@FeedAdDispatcher";

	public static FeedAdDispatcherAdapter getInstance() {
		if (instance == null) {
			instance = new FeedAdDispatcherAdapter();
		}
		return instance;
	}
	

	public FeedAdDispatcherAdapter() {
		super(ChannelFactory.getChannel(ChannelType.XCEFEED),
				"FAD"+endpoints, 1);
	}
	@Override
	protected int mod(Integer key, int size) {
		return key % size;
	}

	@Override
	public FeedAdDispatcherPrx uncheckedCast(ObjectPrx baseProxy) {
		return FeedAdDispatcherPrxHelper.uncheckedCast(baseProxy);
	}

	@Override
	protected ObjectPrx initProxy(ObjectPrx baseProxy) {
		return baseProxy;
	}
	
	
	private FeedAdDispatcherPrx getManagerOneway(int uid) {
		return uncheckedCast(getProxy(uid).ice_oneway());
	}

	private FeedAdDispatcherPrx getManager(int uid) {
		return getProxy(uid);
	}
	
	
	public void dispatch(int ad_id, AdFeedBuilder extra){
		getManager(ad_id).Dispatch(ad_id, extra.getProps());
	}
	
	public void CreateFlashBox(Map<String, String> params){
		if(!params.containsKey("title")){//标题
			return;
		}
		if(!params.containsKey("titleLink")){//标题链接
			return;
		}
		if(!params.containsKey("body")){//新鲜事body
			return;
		}
		if(!params.containsKey("bodyLink")){//body 链接
			return;
		}
		if(!params.containsKey("mediaSrc")){//图片地址
			return;
		}
		if(!params.containsKey("id")){//adid
			return;
		}
		if(!params.containsKey("flashKeyset")){//keyset
			return;
		}
		if(!params.containsKey("flashWeigth")){
			return;
		}
		if(!params.containsKey("flashHeight")){
			return;
		}
		if(!params.containsKey("flashSrc")){//flash url
			return;
		}
		getManager(0).CreateFlashBox(params);
	}

}
