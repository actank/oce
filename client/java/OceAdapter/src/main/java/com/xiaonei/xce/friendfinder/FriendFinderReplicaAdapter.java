/*package com.xiaonei.xce.friendfinder;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import xce.util.tools.MathUtil;

import mop.hi.oce.domain.factory.FriendFinderFactory;
import mop.hi.oce.domain.model.FriendCommon;
import mop.hi.oce.domain.model.FriendShare;
import mop.hi.oce.domain.model.FriendShares;

import com.xiaonei.service.CommonFriend;
import com.xiaonei.service.FriendFinderManagerPrx;
import com.xiaonei.service.FriendFinderManagerPrxHelper;
import com.xiaonei.service.ShareFriend;
import com.xiaonei.xce.log.Oce;

import xce.clusterstate.ReplicatedClusterAdapter;

public class FriendFinderReplicaAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerFriendFinderReplica";
	private static int _interval = 120;	
	private final FriendFinderFactory _factory;
	
	public FriendFinderReplicaAdapter(FriendFinderFactory factory) {
		super(ctr_endpoints,_interval);
		_factory = factory;
	}

	public List<FriendCommon> getCommonFriends(int userId, int begin, int limit) {
		try {
			List<FriendCommon> result;

			CommonFriend[] ret = getFriendFinderManagerTwoway(userId).getCommonFriend(
					userId, begin, limit);
			result = new ArrayList<FriendCommon>();
			for (CommonFriend one : ret) {
				FriendCommon obj = _factory.createFriendCommon();
				obj.setUserId(one.userId);
				obj.setWeight(one.weight);
				obj.setShares(one.shares);
				result.add(obj);
			}
			return result;
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + " getCommonFriends " + userId + e);
			//System.err.println("getCommonFriends " + userId
					//+ " Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + " getCommonFriends " + userId + e);
			//System.err.println("getCommonFriends " + userId
					//+ " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + " getCommonFriends " + e);
			//e.printStackTrace();
		}
		return null;
	}

	private static Map<String, String> randomContext = new HashMap<String, String>();
	static {
		randomContext.put("RANDOM_COMMON_FRIEND", "1");
	}

	public List<FriendCommon> getCommonFriendsRandom(int userId, int limit) {
		try {
			List<FriendCommon> result;

			CommonFriend[] ret = getFriendFinderManagerTwoway(userId).getCommonFriend(
					userId, 0, limit, randomContext);
			result = new ArrayList<FriendCommon>();
			for (CommonFriend one : ret) {
				FriendCommon obj = _factory.createFriendCommon();
				obj.setUserId(one.userId);
				obj.setWeight(one.weight);
				obj.setShares(one.shares);
				result.add(obj);
			}
			return result;
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + " getCommonFriendsRandom " + userId + e);
			//System.err.println("getCommonFriendsRandom " + userId
					//+ " Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + " getCommonFriendsRandom " + userId + e);
			//System.err.println("getCommonFriendsRandom " + userId
					//+ " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + " getCommonFriendsRandom " + e);
			//e.printStackTrace();
		}
		return null;
	}

	private static Map<String, String> _removeWithBlock = new HashMap<String, String>();
	static {
		_removeWithBlock.put("REMOVE_WITH_BLOCK", "1");
	}

	public void removeCommonFriendWithBlock(int userId, int blockId) {
		getFriendFinderManagerOneway(userId).removeCommonFriend(userId, blockId,
				_removeWithBlock);
	}

	public List<FriendShare> getSharedFriends(int userId1, int userId2,
			int begin, int limit) {
		List<FriendShare> result = new ArrayList<FriendShare>();
		try {
			int[] ret = getFriendFinderManagerTwoway(userId1).getSharedFriend(userId1,
					userId2, begin, limit);
			for (int one : ret) {
				FriendShare obj = _factory.createFriendShare();
				obj.setUserId(one);
				result.add(obj);
			}
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + " getSharedFriends " + e);
			//System.err.println("getSharedFriends " + userId1 + " " + userId2
					//+ " Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + " getSharedFriends " + e);
			//System.err.println("getSharedFriends " + userId1 + " " + userId2
					//+ " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + " getSharedFriends " + e);
			//e.printStackTrace();
		}
		return result;
	}

	public Map<Integer, FriendShares> getSharedFriends(int userId,
			List<Integer> friendIds, int limit) {
		Map<Integer, FriendShares> result = null;
		try {
			Map<Integer, ShareFriend> ret = getFriendFinderManagerTwoway(userId)
					.getSharedFriends(userId, MathUtil.list2array(friendIds), limit);
			result = new HashMap<Integer, FriendShares>();
			for (Entry<Integer, ShareFriend> one : ret.entrySet()) {
				FriendShares obj = new FriendShares();
				obj.count = one.getValue().count;
				for (int os : one.getValue().shares) {
					FriendShare o = _factory.createFriendShare();
					o.setUserId(os);
					obj.shares.add(o);
				}
				result.put(one.getKey(), obj);
			}
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + " getSharedFriends " + userId + e);
			//System.err
					//.println("getSharedFriends " + userId + " and "
							//+ friendIds.size()
							//+ " friends Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + " getSharedFriends " + userId + e);
			//System.err.println("getSharedFriends " + userId + " "
					//+ friendIds.size() + " friends Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + " getSharedFriends " + userId + e);
			//e.printStackTrace();
		}
		return result;
	}

	public int getSharedFriendsCount(int userId1, int userId2) {
		return getFriendFinderManagerTwoway(userId1).getSharedFriendCount(userId1,
				userId2);
	}

	protected FriendFinderManagerPrx getFriendFinderManagerTwoway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			FriendFinderManagerPrx prx = FriendFinderManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	protected FriendFinderManagerPrx getFriendFinderManagerOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			FriendFinderManagerPrx prx = FriendFinderManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}
	
//	public static void main(String[] args) {
//		FriendFinderReplicaAdapter _adapter = new FriendFinderReplicaAdapter(new MyFriendFinderFactory());
//		List<FriendCommon> tmp = _adapter.getCommonFriendsRandom(24496250, 100);// 随机
//		System.out.println(tmp.size());
//	}
}

//class MyFriendCommon extends FriendCommon {
//}
//class MyFriendShare extends FriendShare {
//}
//
//class MyFriendFinderFactory implements FriendFinderFactory {
//
//	public FriendCommon createFriendCommon() {
//		// TODO Auto-generated method stub
//		return new MyFriendCommon();
//	}
//
//	public FriendShare createFriendShare() {
//		// TODO Auto-generated method stub
//		return new MyFriendShare();
//	}
//}

*/
