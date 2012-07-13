package com.xiaonei.xce.feed;

import java.util.ArrayList;
import java.util.List;

import xce.feed.FeedData;
import xce.feed.FeedItemManagerPrx;
import xce.feed.FocusManagerPrx;
import xce.feed.FocusManagerPrxHelper;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import Ice.ObjectPrx;

public class FeedFocusAdapter extends
		AbstractModProxyHolder<Integer, FocusManagerPrx> {

	private static FeedFocusAdapter instance;
	private static String endpoints = "@FeedFocus";

	public static FeedFocusAdapter getInstance() {
		if (instance == null) {
			instance = new FeedFocusAdapter();
		}
		return instance;
	}

	public FeedFocusAdapter() {
		super(ChannelFactory.getChannel(ChannelType.XCEFEED), "M" + endpoints,
				5);
	}

	@Override
	protected int mod(Integer key, int size) {
		return key % size;
	}

	@Override
	public FocusManagerPrx uncheckedCast(ObjectPrx baseProxy) {
		return FocusManagerPrxHelper.uncheckedCast(baseProxy);
	}

	@Override
	protected ObjectPrx initProxy(ObjectPrx baseProxy) {
		return baseProxy;
	}

	private FocusManagerPrx getManagerOneway(int uid) {
		return uncheckedCast(getProxy(uid).ice_oneway());
	}

	private FocusManagerPrx getManager(int uid) {
		return getProxy(uid);
	}

	public List<FeedData> getFocusFeedData(int user, int begin, int limit) {
		FeedData[] list = getManager(user).GetFocusFeedData(user, begin, limit);
		List<FeedData> res = new ArrayList<FeedData>();
		for(int i = 0; i < list.length; ++i){
			res.add(list[i]);
		}
		return res;
	}

	public int addFocuses(int user, int target) {
		try{
			return getManager(user).AddFocuses(user, target) == true ? 1: 0;
		}catch(Exception e){
			return -1;
		}
	}
	
	public int addFocusWithCheck(int user, int target) {
		return getManager(user).AddFocusWithCheck(user, target);
	}

	public void delFocuses(int user, int target) {
		getManagerOneway(user).DelFocuses(user, target);
	}
	
	public List<Integer> getFocuses(int user){
		int [] l = getManager(user).GetFocuses(user);
		List<Integer> res = new ArrayList<Integer>();
		for(int i = 0;i < l.length; ++i){
			res.add(l[i]);
		}
		return res;
	}
//	public static void main(String[] args) {
//		// TODO Auto-generated method stub
//		int res = FeedFocusAdapter.getInstance().addFocusesWithCheck(347348947, 347349486);
//		System.out.println("res:" + res );
//	}
}
