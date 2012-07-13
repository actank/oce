package com.xiaonei.xce.pukfeedcache;

import java.util.ArrayList;
import java.util.List;

import xce.clusterstate.ReplicatedClusterAdapter;
import xce.socialgraph.PUKFeed;
import xce.socialgraph.PUKFeedCachePrx;
import xce.socialgraph.PUKFeedCachePrxHelper;

public class PUKFeedCacheAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerPUKFeedCache";
	private static int _interval = 120;	
	
	private PUKFeedCacheAdapter() {
		super(ctr_endpoints,_interval);
	}
	
	private static PUKFeedCacheAdapter _instance = new PUKFeedCacheAdapter();
	public static PUKFeedCacheAdapter getInstance() {
		return _instance;
	}
	
	protected PUKFeedCachePrx getPUKFeedCacheManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			PUKFeedCachePrx prx = PUKFeedCachePrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}
	
	protected PUKFeedCachePrx getPUKFeedCacheManagerOneWay(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			PUKFeedCachePrx prx = PUKFeedCachePrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}
	
	public List<PUKFeedData> getPUKFeedData(int userId, int limit) {
		PUKFeed[] rawFeedList = getPUKFeedCacheManager(userId).Get(userId, limit);
		List<PUKFeedData> result = new ArrayList<PUKFeedData>();
		for (PUKFeed f : rawFeedList) {
			PUKFeedData one = new PUKFeedData(f.friendId, f.commonId);
			result.add(one);
		}
		return result;
	}
	
	public List<PUKFeedData> getPUKFeedDataAndRemove(int userId, int limit) {
		PUKFeed[] rawFeedList = getPUKFeedCacheManager(userId).GetAndRemove(userId, limit);
		List<PUKFeedData> result = new ArrayList<PUKFeedData>();
		for (PUKFeed f : rawFeedList) {
			PUKFeedData one = new PUKFeedData(f.friendId, f.commonId);
			result.add(one);
		}
		return result;
	}
	
	public void removePUKFeedData(int userId, List<PUKFeedData> feedList) {
		PUKFeed[] rawFeedList = new PUKFeed[feedList.size()];
		int count = 0;
		for (PUKFeedData fd : feedList) {
			PUKFeed tmp = new PUKFeed();
			tmp.friendId = fd.getFriendId();
			tmp.commonId = fd.getCommonId();
			rawFeedList[count++] = tmp;
		}
		
		getPUKFeedCacheManagerOneWay(userId).Remove(userId, rawFeedList);
	}
	
	public static void main(String[] args) {
//		int userId = 24496255;  //叶湘
//		int userId = 2055206602;
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

		List<PUKFeedData> result = PUKFeedCacheAdapter.getInstance().getPUKFeedDataAndRemove(userId, 1);
//		List<PUKFeedData> result;
//		for( int i=0; i<10; ++i ){
//			System.out.println("hehe " + i);
//			try{
//			result = PUKFeedCacheAdapter.getInstance().getPUKFeedData(i, -1);
//			}catch(Exception e){
//				System.out.println("shit " +i );
//			}
//		}
		
		for (PUKFeedData p : result) {
			System.out.println(p.getFriendId() + " : " + p.getCommonId());
		}
//		PUKFeedCacheAdapter.getInstance().removePUKFeedData(userId, result);
		
		System.exit(0);
	}
}
