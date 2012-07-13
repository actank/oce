package com.xiaonei.xce.frienddist;

public abstract class FriendDistCount {
	private int _networkId;
	private int _count;

	protected void setBase(int networkId, int count) {
		_networkId = networkId;
		_count = count;
	}

	public int getNetworkId() {
		return _networkId;
	}

	public int getCount() {
		return _count;
	}
	
	@Override
	public String toString() {
		return "[FriendDistCount:networkId="+_networkId+";count="+_count+"]";
	}
}