package xce.buddy;

import java.util.List;

import com.xiaonei.xce.buddy.model.Constants;
import com.xiaonei.xce.buddyadapter.buddycount.BuddyCountAdapter;

import xce.buddy.logic.LogicFactory;
import xce.buddy.logic.impl.FriendLogicImpl;
import Ice.Current;

/**
 * 接受RPC请求的类
 * 
 * @author michael(liyong@opi-corp.com)
 * 
 */
public final class BuddySOAManagerI extends _BuddySOAManagerDisp {

	private static final long serialVersionUID = -6861495240411836800L;

	public BuddySOAManagerI() {
	}

	@Override
	public FriendApplyResult addFriendApply(FriendApply friendApply,
			Current current) {
		return LogicFactory.getFriendLogic().addFriendApply(friendApply);
	}

	@Override
	public FriendApplyResult acceptFriendApply(FriendApply friendApply,
			Current current) {
		return LogicFactory.getFriendLogic().acceptFriendApply(friendApply);
	}

	/**
	 * 业务action是否需要弹验证码 <br/>
	 * 我们只支持需要或不需要, 验证的过程,不是在这里做,在各个业务中做,多谢!
	 * 
	 */
	@Override
	public boolean needAuthCode(int userId, Operation op, Current current) {
		// TODO: 每种业务的数量限制应该写到一个配置文件中,不应该硬编码!!!
		int count = 0;
		switch (op.value()) {
		case Operation._AddFriendApply:
			try {
				count = BuddyCountAdapter.getInstance().get(userId, op.value());
			} catch (Exception e) {
				// TODO: Logger 一下
			}
			if (count < 15) {
				return false;
			}
			return true;
		case Operation._DeleteFriend:
			try {
				count = BuddyCountAdapter.getInstance().get(userId, op.value());
			} catch (Exception e) {
				// TODO: Logger 一下
			}
			if (count < 3) {
				return false;
			}
			return true;
		default:
			break;
		}
		return false;
	}

	@Override
	public int addBlock(int host, int guest, Current current) {
		return LogicFactory.getFriendLogic().addBlock(host, guest);
	}

	@Override
	public int removeBlock(int host, int guest, Current current) {
		return LogicFactory.getFriendLogic().removeBlock(host, guest);
	}

	@Override
	public int denyAllFriendApply(int host, Current current) {
		return LogicFactory.getFriendLogic().denyAllFriendApply(host);
	}

	@Override
	public int denyFriendApply(FriendApply fa, Current current) {
		return LogicFactory.getFriendLogic().denyFriendApply(fa);
	}

	@Override
	public int deleteFriend(int host, int guest, Current current) {
		return LogicFactory.getFriendLogic().deleteFriend(host, guest);
	}

	@Override
	public int recommendFriend(int matchmaker, int targetUserId,
			List<Integer> recommendIdList, boolean isOneWay, int type,
			Current current) {
		return LogicFactory.getFriendLogic().recommendFriend(matchmaker,
				targetUserId, recommendIdList, isOneWay, type);
	}

	@Override
	public int denyAllRecommendFriend(int host, Current current) {
		return LogicFactory.getFriendLogic().denyAllRecommendFriend(host);
	}

	@Override
	public int denyRecommendFriend(int host, int guest, Current current) {
		return LogicFactory.getFriendLogic().denyRecommendFriend(host, guest);
	}

	@Override
	public int addHotFriend(int host, int friend, Current current) {
		return LogicFactory.getFriendLogic().addHotFriend(host, friend);
	}

	@Override
	public int deleletHotFriend(int host, int friend, Current current) {
		return LogicFactory.getFriendLogic().deleletHotFriend(host, friend);
	}

	@Override
	public int updateHotFriendMemo(int host, int friend, String memo,
			Current current) {
		return LogicFactory.getFriendLogic().updateHotFriendMemo(host, friend,
				memo);
	}

	@Override
	public int setHotFriend(int host, List<Integer> friends, Current current) {
		return LogicFactory.getFriendLogic().setHotFriend(host, friends);
	}

}
