package com.xiaonei.xce.buddy;

import java.util.List;

import xce.buddy.Operation;
import Ice.Current;

import com.xiaonei.icode.verify.CodeUtil;
import com.xiaonei.xce.buddy.model.Constants;
import com.xiaonei.xce.buddy.model.FriendApply;
import com.xiaonei.xce.buddy.model.Result;

public class BuddySOAClient {

	private static BuddySOAClient instance;

	private BuddySOAClient() {

	}

	public static BuddySOAClient getInstance() {
		if (instance == null) {
			synchronized (BuddySOAClient.class) {
				if (instance == null) {
					instance = new BuddySOAClient();
				}
			}
		}
		return instance;
	}

	/**
	 * 返回true:需要弹验证码<br/>
	 * 返回false:不需要弹验证码<br/>
	 * 
	 * @param userId
	 * @return
	 */
	public boolean needCheckCodeBeforeAddFriendApply(int userId) {
		try {
			return BuddySOAAdapter.getInstance().getBuddySOAManager(userId)
					.needAuthCode(userId, Operation.AddFriendApply);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return false;
	}

	/**
	 * 发送好友申请前的验证码操作
	 * 
	 * @param userId
	 * @param checkCode
	 * @return
	 */
	public boolean checkCodeBeforeAddFriendApply(int userId, String checkCode) {
		return CodeUtil.getInstance().validCode(checkCode, "requestfriend_",
				userId);
	}

	/**
	 * 发送好友申请的主要接口<br/>
	 * 注:此接口中不包含验证码相关操作,所以调用前最好先走一下验证码逻辑,依次调用验证码相关方法 @See
	 * needCheckCodeBeforeAddFriendApply方法与checkCodeBeforeAddFriendApply方法
	 * 逻辑是这样的: 判断是否需要用户输入验证码,如果需要则先验证用户的输入,再调用addFriendApply方法. 判断的这个步骤必须得有!!
	 * 
	 * 
	 * @param friendApply
	 * @return
	 */
	public Result addFriendApply(FriendApply friendApply) {
		xce.buddy.FriendApplyResult rpcResult = null;
		try {
			rpcResult = BuddySOAAdapter.getInstance()
					.getBuddySOAManager(friendApply.getApplicant())
					.addFriendApply(friendApply.getRpcData());
		} catch (Exception e) {
			e.printStackTrace();
		}
		Result result = new Result(rpcResult);
		return result;
	}

	/**
	 * 接受好友申请的主要接口<br/>
	 * 
	 * @param friendApply
	 * @return
	 */
	public Result acceptFriendApply(FriendApply friendApply) {
		xce.buddy.FriendApplyResult rpcResult = null;
		try {
			rpcResult = BuddySOAAdapter.getInstance()
					.getBuddySOAManager(friendApply.getApplicant())
					.acceptFriendApply(friendApply.getRpcData());
		} catch (Exception e) {
			e.printStackTrace();
		}
		Result result = new Result(rpcResult);
		return result;
	}

	/**
	 * 返回true:需要弹验证码<br/>
	 * 返回false:不需要弹验证码<br/>
	 * 
	 * @param userId
	 * @return
	 */
	public boolean needCheckCodeBeforeDeleteFriendApply(int userId) {
		try {
			return BuddySOAAdapter.getInstance().getBuddySOAManager(userId)
					.needAuthCode(userId, Operation.DeleteFriend);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return false;
	}

	/**
	 * 删除好友前的验证码操作
	 * 
	 * @param userId
	 * @param checkCode
	 * @return
	 */
	public boolean checkCodeBeforeDeleteFriend(int userId, String checkCode) {
		return CodeUtil.getInstance()
				.validCode(checkCode, "delfriend_", userId);
	}

	/**
	 * 加黑名单
	 * 
	 * @param host
	 * @param guest
	 * @param current
	 * @return
	 */
	public int addBlock(int host, int guest, Current current) {
		try {
			return BuddySOAAdapter.getInstance().getBuddySOAManager(host)
					.addBlock(host, guest);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return Constants.ERROR;
	}

	/**
	 * 移出黑名单
	 * 
	 * @param host
	 * @param guest
	 * @param current
	 * @return
	 */
	public int removeBlock(int host, int guest, Current current) {
		try {
			return BuddySOAAdapter.getInstance().getBuddySOAManager(host)
					.removeBlock(host, guest);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return Constants.ERROR;
	}

	/**
	 * 忽略所有好友申请
	 * 
	 * @param host
	 * @param current
	 * @return
	 */
	public int denyAllFriendApply(int host, Current current) {
		try {
			return BuddySOAAdapter.getInstance().getBuddySOAManager(host)
					.denyAllFriendApply(host);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return Constants.ERROR;
	}

	/**
	 * 忽略好友申请
	 * 
	 * @param fa
	 * @param current
	 * @return
	 */
	public int denyFriendApply(FriendApply fa, Current current) {
		try {
			return BuddySOAAdapter.getInstance()
					.getBuddySOAManager(fa.getAccepter())
					.denyFriendApply(fa.getRpcData());
		} catch (Exception e) {
			e.printStackTrace();
		}
		return Constants.ERROR;
	}

	/**
	 * 删除好友
	 * 
	 * @param host
	 * @param guest
	 * @return
	 */
	public int deleteFriend(int host, int guest) {
		try {
			return BuddySOAAdapter.getInstance().getBuddySOAManager(host)
					.deleteFriend(host, guest);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return Constants.ERROR;
	}

	/**
	 * 用户主动推荐好友
	 * 
	 * @param matchmaker
	 * @param targetUserId
	 * @param recommendIdList
	 * @param isOneWay
	 * @param type
	 * @param current
	 * @return
	 */
	public int recommendFriend(int matchmaker, int targetUserId,
			List<Integer> recommendIdList, boolean isOneWay, int type,
			Current current) {
		try {
			return BuddySOAAdapter
					.getInstance()
					.getBuddySOAManager(matchmaker)
					.recommendFriend(matchmaker, targetUserId, recommendIdList,
							isOneWay, type);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return Constants.ERROR;
	}

	public int denyAllRecommendFriend(int host, Current current) {
		try {
			return BuddySOAAdapter.getInstance().getBuddySOAManager(host)
					.denyAllRecommendFriend(host);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return Constants.ERROR;
	}

	public int denyRecommendFriend(int host, int guest, Current current) {
		try {
			return BuddySOAAdapter.getInstance().getBuddySOAManager(host)
					.denyRecommendFriend(host, guest);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return Constants.ERROR;
	}

	public int addHotFriend(int host, int friend, Current current) {
		try {
			return BuddySOAAdapter.getInstance().getBuddySOAManager(host)
					.addHotFriend(host, friend);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return Constants.ERROR;
	}

	public int deleletHotFriend(int host, int friend, Current current) {
		try {
			return BuddySOAAdapter.getInstance().getBuddySOAManager(host)
					.deleletHotFriend(host, friend);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return Constants.ERROR;
	}

	public int updateHotFriendMemo(int host, int friend, String memo,
			Current current) {
		try {
			return BuddySOAAdapter.getInstance().getBuddySOAManager(host)
					.updateHotFriendMemo(host, friend, memo);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return Constants.ERROR;
	}

	public int setHotFriend(int host, List<Integer> friends, Current current) {
		try {
			return BuddySOAAdapter.getInstance().getBuddySOAManager(host)
					.setHotFriend(host, friends);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return Constants.ERROR;
	}

}
