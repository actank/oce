package xce.buddy.logic;

import java.util.List;

import xce.buddy.FriendApply;
import xce.buddy.FriendApplyResult;

/**
 * 好友逻辑接口定义
 * 
 * @author michael(liyong@opi-corp.com)
 * 
 */
public interface FriendLogic {

	/**
	 * 发送好友申请接口
	 * 
	 * @param friendApply
	 * @return
	 */
	public FriendApplyResult addFriendApply(FriendApply friendApply);

	/**
	 * 接受好友申请接口
	 * 
	 * @param friendApply
	 * @return
	 */
	public FriendApplyResult acceptFriendApply(FriendApply friendApply);

	/**
	 * 加入黑名单
	 * 
	 * @param host
	 * @param guest
	 * @return
	 */
	public int addBlock(int host, int guest);

	/**
	 * 移出黑名单
	 * 
	 * @param host
	 * @param guest
	 * @return
	 */
	public int removeBlock(int host, int guest);

	/**
	 * 忽略所有好友申请
	 * 
	 * @param host
	 * @return
	 */
	public int denyAllFriendApply(int host);

	/**
	 * 忽略好友申请
	 * 
	 * @param fa
	 * @return
	 */
	public int denyFriendApply(FriendApply fa);

	/**
	 * 删除好友
	 * 
	 * @param host
	 * @param guest
	 * @return
	 */
	public int deleteFriend(int host, int guest);

	/**
	 * 推荐好友接口
	 * 
	 * @param matchmaker
	 * @param targetUserId
	 * @param recommendIdList
	 * @param isOneWay
	 * @param type
	 * @return
	 */
	public int recommendFriend(int matchmaker, int targetUserId,
			List<Integer> recommendIdList, boolean isOneWay, int type);

	/**
	 * 双向忽略好友推荐
	 * 
	 * @param host
	 * @param guest
	 * @return
	 */
	public int denyRecommendFriend(int host, int guest);

	/**
	 * 忽略所有好友推荐
	 * 
	 * @param host
	 * @return
	 */
	public int denyAllRecommendFriend(int host);

	/**
	 * 添加特别好友<br/>
	 * 前提:host 与 friend 好友关系，并且host是星级用户!
	 * 
	 * @param host
	 * @param friend
	 * @return
	 */
	public int addHotFriend(int host, int friend);

	/**
	 * 更新特别好友描述<br/>
	 * 前提:host 与 friend 好友关系，并且host是星级用户!
	 * 
	 * @param host
	 * @param friend
	 * @param memo
	 * @return
	 */
	public int updateHotFriendMemo(int host, int friend, String memo);

	/**
	 * 删除特别好友<br/>
	 * 前提:host 与 friend 好友关系，并且host是星级用户!
	 * 
	 * @param host
	 * @param friend
	 * @return
	 */
	public int deleletHotFriend(int host, int friend);

	/**
	 * 用户自定义特别好友排序<br/>
	 * vip:最多10个特别好友
	 * 
	 * @param host
	 * @param friends
	 * @return
	 */
	public int setHotFriend(int host, List<Integer> friends);

}
