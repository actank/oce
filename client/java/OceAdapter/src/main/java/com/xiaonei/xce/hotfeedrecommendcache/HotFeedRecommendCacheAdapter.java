package com.xiaonei.xce.hotfeedrecommendcache;

import java.util.ArrayList;
import java.util.List;

import xce.clusterstate.ReplicatedClusterAdapter;
import xce.socialgraph.HotFeed;
import xce.socialgraph.HotFeedRecommendCachePrx;
import xce.socialgraph.HotFeedRecommendCachePrxHelper;

public class HotFeedRecommendCacheAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerHotFeedRecommendCache";
	private static int _interval = 120;	
	
	private HotFeedRecommendCacheAdapter() {
		super(ctr_endpoints,_interval);
	}
	
	private static HotFeedRecommendCacheAdapter _instance = new HotFeedRecommendCacheAdapter();
	public static HotFeedRecommendCacheAdapter getInstance() {
		return _instance;
	}
	
	protected HotFeedRecommendCachePrx getHotFeedRecommendCacheManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			HotFeedRecommendCachePrx prx = HotFeedRecommendCachePrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}
	
	protected HotFeedRecommendCachePrx getHotFeedRecommendCacheManagerOneWay(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			HotFeedRecommendCachePrx prx = HotFeedRecommendCachePrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}
	
	public List<FeedRecommendData> getHotFeedRecommend(int userId, int limit) {
		HotFeed[] feed = getHotFeedRecommendCacheManager(userId).Get(userId, limit);
		List<FeedRecommendData> result = new ArrayList<FeedRecommendData>();
		
		for (HotFeed f : feed) {
			result.add(new FeedRecommendData(f));
		}
		
		return result;
	}
	
	public List<FeedRecommendData> getHotFeedRecommendAndRemove(int userId, int limit) {
		HotFeed[] feed = getHotFeedRecommendCacheManager(userId).GetAndRemove(userId, limit);
		List<FeedRecommendData> result = new ArrayList<FeedRecommendData>();
		
		for (HotFeed f : feed) {
			result.add(new FeedRecommendData(f));
		}
		
		return result;
	}
	
	public void removeHotFeedRecommend(int userId, List<FeedRecommendData> result) {
		HotFeed[] feedList = new HotFeed[result.size()];
		int count = 0;
		for (FeedRecommendData f : result) {
			HotFeed tmp = new HotFeed();
			tmp.userId = f.getUserId();
			tmp.feedId = f.getFeedId();
			tmp.feedType = f.getFeedType();
			feedList[count++] = tmp;
		}
		
		getHotFeedRecommendCacheManagerOneWay(userId).Remove(userId, feedList);
	}

	public static void main(String[] args) {
//		int userId = 24496255;  //叶湘
//		int userId = 11111;
		int userId = 238111132;  //我
//		int userId = 1331;  // 翟禹 
//		int userId = 239737004;  // 蔡建山
//		int userId = 222677625;   // 高亚楠 
//		int userId = 249386140;  //袁景瑞
//		int userId = 233940269;   //葛广鹤 
//		int userId = 190136739;    //曾哥
//		int userId = 237767538;     //小奥
//		int userId = 285340707;         //黄毅
//		int userId = 202911262;  //白伯纯
		
		List<FeedRecommendData> result = HotFeedRecommendCacheAdapter.getInstance().getHotFeedRecommend(userId, 1);
		System.out.println(result.size());
		
		for (FeedRecommendData f : result) {
			System.out.println(f.getUserId() + " " + f.getFeedId() + " " + f.getFeedType());
		}
		
//		HotFeedRecommendCacheAdapter.getInstance().removeHotFeedRecommend(userId, result);
		
		System.exit(0);
	}
}
