package com.xiaonei.xce.friendfinderbycommon;

public class CommonFriendData{
	private int _userId;
	private int _shareCount;

	public int getUserId() {
		return _userId;
	}

	public int getShareCount() {
		return _shareCount;
	}

	protected void setBase(xce.friendfinder.CommonFriendItem base) {
		_userId = base.userId;
		_shareCount = base.shareCount;
	}
}
