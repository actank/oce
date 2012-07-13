package com.xiaonei.xce.pukfeedcache;

public class PUKFeedData {
	private int _friendId;
	private int _commonId;
	
	public PUKFeedData(int friendId, int commonId) {
		_friendId = friendId;
		_commonId = commonId;
	}
	
	public int getFriendId() {
		return _friendId;
	}
	
	public int getCommonId() {
		return _commonId;
	}
}
