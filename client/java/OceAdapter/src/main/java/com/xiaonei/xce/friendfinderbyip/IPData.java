package com.xiaonei.xce.friendfinderbyip;

public class IPData{
	private long _ip;
	private int _onlineCount;
	private int _awayCount;
	private int _lastLoginTime;

	public long getIP() {
		return _ip;
	}

	public int getOnlineCount() {
		return _onlineCount;
	}

	public int getAwayCount() {
		return _awayCount;
	}

	public int getLastOnlineTime() {
		return _lastLoginTime;
	}

	public void setIP(long ip) {
		_ip = ip;
	}

	public void setOnlineCount(int onlineCount) {
		this._onlineCount = onlineCount;
	}

	public void setAwayCount(int awayCount) {
		_awayCount = awayCount;
	}

	public void setLastOnlineTime(int lastLoginTime) {
		this._lastLoginTime = lastLoginTime;
	}

	protected void setBase(xce.friendfinder.IPNote base) {
		_ip = base.ip;
		_onlineCount = base.onlineCount;
		_awayCount = base.awayCount;
		_lastLoginTime = base.lastLoginTime;
	}
}
