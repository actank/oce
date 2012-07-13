package com.xiaonei.xce.fof.service.impl;

import java.util.List;

import com.xiaonei.xce.fof.service.AbstractNorbertRequest;

public class FriendOfFriendRequest implements AbstractNorbertRequest {
	public int uid;
	public List<Integer> friends;
	public boolean needCommonNum;
	public int minCommonLimit;
	public int getUid() {
		return uid;
	}
	public void setUid(int uid) {
		this.uid = uid;
	}
	public List<Integer> getFriends() {
		return friends;
	}
	public void setFriends(List<Integer> friends) {
		this.friends = friends;
	}
	public boolean isNeedCommonNum() {
		return needCommonNum;
	}
	public void setNeedCommonNum(boolean needCommonNum) {
		this.needCommonNum = needCommonNum;
	}
	public int getMinCommonLimit() {
		return minCommonLimit;
	}
	public void setMinCommonLimit(int minCommonLimit) {
		this.minCommonLimit = minCommonLimit;
	}
}
