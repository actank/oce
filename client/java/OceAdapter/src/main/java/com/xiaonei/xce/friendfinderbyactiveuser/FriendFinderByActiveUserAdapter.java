package com.xiaonei.xce.friendfinderbyactiveuser;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.xiaonei.xce.log.Oce;

import xce.clusterstate.ReplicatedClusterAdapter;
import xce.friendfinder.FriendFinderByActiveUserPrx;
import xce.friendfinder.FriendFinderByActiveUserPrxHelper;

public class FriendFinderByActiveUserAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerFriendFinderByActiveUser";
	private static int _interval = 120;	
	
	private FriendFinderByActiveUserAdapter() {
		super(ctr_endpoints,_interval);
	}
	
	private static FriendFinderByActiveUserAdapter _instance = new FriendFinderByActiveUserAdapter();
	public static FriendFinderByActiveUserAdapter getInstance() {
		return _instance;
	}
	
	protected FriendFinderByActiveUserPrx getFriendFinderByActiveUserManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			FriendFinderByActiveUserPrx prx =  FriendFinderByActiveUserPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}
	
	protected FriendFinderByActiveUserPrx getFriendFinderByActiveUserManagerOneWay(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			FriendFinderByActiveUserPrx prx =  FriendFinderByActiveUserPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}
	
	public Map<Integer, List<Integer>> getRecommendFriends(int userId, int limit){
		FriendFinderByActiveUserPrx prx=null;
		try {
			prx=getFriendFinderByActiveUserManager(userId);
			Map<Integer, int[]> result = prx.getRecommend(userId, limit);
			RecommendData obj = new RecommendData();
			obj.set(result);
			return obj.get();
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + " getRecommendFriends [ " + prx + " ] id="  + userId + e);
			//System.err.println("getCommonFriends " + userId
					//+ " Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + " getRecommendFriends [ " + prx + " ] id="  + userId + e);
			//System.err.println("getCommonFriends " + userId
					//+ " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + " getRecommendFriends " + e);
			//e.printStackTrace();
		}
		return null;
	}
	
//	public Map<Integer, List<Integer>> getRecommendFriendsRandom(int userId, int limit) {
//		Map<Integer, int[]> result = getFriendFinderByActiveUserManager(userId).getRecommendRandom(userId, limit);
//		RecommendData obj = new RecommendData();
//		obj.set(result);
//		return obj.get();
//	}
	
	public int[] getWriteRecommendRandom(int userId, int limit) {
		FriendFinderByActiveUserPrx prx=null;
		try {
			prx=getFriendFinderByActiveUserManager(userId);
			return prx.getWriteRecommendRandom(userId, limit);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + " getWriteRecommendRandom [ " + prx + " ] id="  + userId + e);
			//System.err.println("getCommonFriends " + userId
					//+ " Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + " getWriteRecommendRandom [ " + prx + " ] id="  + userId + e);
			//System.err.println("getCommonFriends " + userId
					//+ " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + " getWriteRecommendRandom " + e);
			//e.printStackTrace();
		}
		return null;
	}
	
	public int[] getFriendsRecommendRandom(int userId, int limit) {
		FriendFinderByActiveUserPrx prx=null;
		try {
			prx=getFriendFinderByActiveUserManager(userId);
			return prx.getFriendsRecommendRandom(userId, limit);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + " getFriendsRecommendRandom [ " + prx + " ] id="  + userId + e);
			//System.err.println("getCommonFriends " + userId
					//+ " Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + " getFriendsRecommendRandom [ " + prx + " ] id="  + userId + e);
			//System.err.println("getCommonFriends " + userId
					//+ " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + " getFriendsRecommendRandom [ " + prx + " ] id="  + e);
			//e.printStackTrace();
		}
		return null;
	}
	
	public int[] getRecommendFriendList(int userId, int friendId, int begin, int limit) {
		FriendFinderByActiveUserPrx prx=null;
		try {
			prx=getFriendFinderByActiveUserManager(userId);
			return prx.getRecommendFriendList(userId, friendId, begin, limit);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + " getRecommendFriendList [ " + prx + " ] id="  + userId + e);
			//System.err.println("getCommonFriends " + userId
					//+ " Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + " getRecommendFriendList [ " + prx + " ] id="  + userId + e);
			//System.err.println("getCommonFriends " + userId
					//+ " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + " getRecommendFriendList " + e);
			//e.printStackTrace();
		}
		return null;
	}
	
	private static Map<String, String> _removeWithBlock = new HashMap<String, String>();
	static {
		_removeWithBlock.put("REMOVE_WITH_BLOCK", "1");
	}
	
	public void removeRecommendFriends(int userId, int friendId, int[] blockIds) {
		getFriendFinderByActiveUserManagerOneWay(userId).removeRecommendFriends(userId, friendId, blockIds, _removeWithBlock);
	}
	
	public static void main(String[] args) {
//		int userId = 24496255;  //叶湘
//		int userId = 121569904;
		int userId = 238111132;  //我
//		int userId = 1331;  // 翟禹 
//		int userId = 239737004;  // 蔡建山
//		int userId = 222677625;   // 高亚楠 
//		int userId = 249386140;  //袁景瑞
		
		int[] result =  FriendFinderByActiveUserAdapter.getInstance().getFriendsRecommendRandom(userId, 50);
		System.out.println("result size : " + result.length);
	}
}
