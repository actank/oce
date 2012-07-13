package com.xiaonei.xce.birthdayrecommendcache;

import xce.clusterstate.ReplicatedClusterAdapter;
import xce.socialgraph.BirthdayRecommendCachePrx;
import xce.socialgraph.BirthdayRecommendCachePrxHelper;

public class BirthdayRecommendCacheAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerBirthdayRecommendCache";
	private static int _interval = 120;	
	
	private BirthdayRecommendCacheAdapter() {
		super(ctr_endpoints,_interval);
	}
	
	private static BirthdayRecommendCacheAdapter _instance = new BirthdayRecommendCacheAdapter();
	public static BirthdayRecommendCacheAdapter getInstance() {
		return _instance;
	}
	
	protected BirthdayRecommendCachePrx getBirthdayRecommendCacheManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			BirthdayRecommendCachePrx prx = BirthdayRecommendCachePrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}
	
	protected BirthdayRecommendCachePrx getBirthdayRecommendCacheManagerOneWay(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			BirthdayRecommendCachePrx prx = BirthdayRecommendCachePrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}
	
	public int getBirthdayRecommendFriendAndRemove(int userId) {
		return getBirthdayRecommendCacheManager(userId).GetAndRemove(userId);
	}
	
	public int getBirthdayRecommendFriend(int userId) {
		return getBirthdayRecommendCacheManager(userId).Get(userId);
	}
	
	public void removeBirthdayRecommendFriend(int userId, int friendId) {
		getBirthdayRecommendCacheManager(userId).Remove(userId, friendId);
	}
	
	public static void main(String[] args) {
		int userId = 238111132;
		
//		int friendId = BirthdayRecommendCacheAdapter.getInstance().getBirthdayRecommendFriendAndRemove(userId);
		int friendId = BirthdayRecommendCacheAdapter.getInstance().getBirthdayRecommendFriend(userId);
		System.out.println(friendId);
		
//		BirthdayRecommendCacheAdapter.getInstance().removeBirthdayRecommendFriend(userId, friendId);
		System.exit(0);
	}
}
