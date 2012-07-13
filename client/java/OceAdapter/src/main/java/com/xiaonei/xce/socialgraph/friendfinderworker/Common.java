package com.xiaonei.xce.socialgraph.friendfinderworker;

import java.util.ArrayList;
import java.util.List;

import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Item;

/**
 * 二度好友返回的数据结构
 * @author zhangnan
 *
 */
public class Common {
	private int userId;
	private int shareCount;
	List<Integer> shares = new ArrayList<Integer>();
	
	public Common(Item comm) {
		// TODO Auto-generated constructor stub
		this.userId = comm.getId();
		this.shareCount = comm.getField();
		this.shares.addAll(comm.getFieldsList());
	}
	/**
	 * 获取推荐用户的id
	 * @return
	 */
	public int getUserId() {
		return userId;
	}
	
	/**
	 * 获取共同好友数
	 * @return
	 */
	public int getShareCount() {
		return shareCount;
	}
	
	/**
	 * 获取共同好友
	 * @return
	 */
	public List<Integer> getShares() {
		return shares;
	}
}
