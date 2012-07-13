package com.renren.xce.socialgraph.block;

import java.util.List;

import xce.socialgraph.recommend.DbUtil;
import xce.socialgraph.recommend.FriendListUtil;

import com.renren.xce.socialgraph.common.BlockFetcher;

/**
 * Get blocklist with host's friends
 * @author Administrator
 *
 */
public class BlockFetchWithOwnerFriends implements BlockFetcher {

	public List<Long> getBlockList(int hostId) {
		// TODO Auto-generated method stub
		List<Long> sgBlockList = DbUtil.getInstance().getSgBlockList(hostId);
		List<Integer> friendList = FriendListUtil.getInstance().getFriendList(hostId, -1);
		for (Integer friendId : friendList) {
			sgBlockList.add((long)friendId);
		}
		return sgBlockList;
	}

}
