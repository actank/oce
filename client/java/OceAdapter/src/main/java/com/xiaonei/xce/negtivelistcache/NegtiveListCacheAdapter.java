package com.xiaonei.xce.negtivelistcache;

import xce.clusterstate.ReplicatedClusterAdapter;
import xce.socialgraph.NegtiveListCachePrx;
import xce.socialgraph.NegtiveListCachePrxHelper;

public class NegtiveListCacheAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerNegtiveListCache";
	private static int _interval = 120;	
	
	private NegtiveListCacheAdapter() {
		super(ctr_endpoints,_interval);
	}
	
	private static NegtiveListCacheAdapter _instance = new NegtiveListCacheAdapter();
	
	public static NegtiveListCacheAdapter getInstance() {
		return _instance;
	}
	
	protected NegtiveListCachePrx getNegtiveListCacheManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			NegtiveListCachePrx prx = NegtiveListCachePrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}
	
	protected NegtiveListCachePrx getNegtiveListCacheManagerOneWay(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			NegtiveListCachePrx prx = NegtiveListCachePrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}
	
	public int[] getNegtiveFriendsList(int userId, int limit) {
		return getNegtiveListCacheManager(userId).Get(userId, limit);
	}
	
	public void removeNegtiveFriendsList(int userId, int[] negtiveList) {
		getNegtiveListCacheManagerOneWay(userId).Remove(userId, negtiveList);
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
		
		int limit = -1;
		
		int[] result = NegtiveListCacheAdapter.getInstance().getNegtiveFriendsList(userId, limit);
		
		for (int r : result) {
			System.out.print(r + " ");
		}
		System.out.println();
		
		System.exit(0);
	}
}
