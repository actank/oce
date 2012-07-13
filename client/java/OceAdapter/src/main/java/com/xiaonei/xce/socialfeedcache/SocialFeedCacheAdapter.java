package com.xiaonei.xce.socialfeedcache;

import java.util.ArrayList;
import java.util.List;
import xce.clusterstate.ReplicatedClusterAdapter;
import xce.socialgraph.Feed;
import xce.socialgraph.SocialFeedCachePrx;
import xce.socialgraph.SocialFeedCachePrxHelper;

public class SocialFeedCacheAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerSocialFeedCache";
	private static int _interval = 120;	
	
	private SocialFeedCacheAdapter() {
		super(ctr_endpoints,_interval);
	}
	
	private static SocialFeedCacheAdapter _instance = new SocialFeedCacheAdapter();
	public static SocialFeedCacheAdapter getInstance() {
		return _instance;
	}
	
	protected SocialFeedCachePrx getSocialFeedCacheManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			SocialFeedCachePrx prx = SocialFeedCachePrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}
	
	protected SocialFeedCachePrx getSocialFeedCacheManagerOneWay(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			SocialFeedCachePrx prx = SocialFeedCachePrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}
	
	public List<FeedData> getSocialFeed(int userId, int limit) {
		Feed[] tmp = getSocialFeedCacheManager(userId).get(userId, limit);
		List<FeedData> result = new ArrayList<FeedData>();
		for(Feed f : tmp) {
			result.add( new FeedData(f) );
		}
		return result;
	}
	
	public void removeSocialFeed(int userId, int[] friendIds, int[] newCommonIds) {
		getSocialFeedCacheManagerOneWay(userId).remove(userId, friendIds, newCommonIds);
	}
	
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
//		int userId = 24496255;  //叶湘
//		int userId = 1002;
		int userId = 238111132;  //我
//		int userId = 1331;  // 翟禹 
//		int userId = 239737004;  // 蔡建山
//		int userId = 222677625;   // 高亚楠 
//		int userId = 249386140;  //袁景瑞
//		int userId = 233940269;   //葛广鹤 
//		int userId = 190136739;    //曾哥
//		int userId = 237767538;     //小奥
//		int userId = 285340707;         //黄毅
		
		List<FeedData> result = SocialFeedCacheAdapter.getInstance().getSocialFeed(userId, -1);
		System.out.println("result size : " + result.size());
		
		for(FeedData d : result) {
			for(Integer o : d.getFriendList()) {
				System.out.print(o + "  ");
			}
			System.out.print("=>  ");
			for(Integer o : d.getNewCommonList()) {
				System.out.print(o + "  ");
			}
			System.out.println();
			System.out.println();
		}
		
		System.exit(0);
	}
}
