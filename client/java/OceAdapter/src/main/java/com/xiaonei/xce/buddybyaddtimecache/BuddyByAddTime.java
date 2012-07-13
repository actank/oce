package com.xiaonei.xce.buddybyaddtimecache;

public abstract class BuddyByAddTime {
	private int _userId;
	private long _time;

	public void setBase(int userId, int time){
		_userId = userId;
		_time = ((long)time)*1000;
	}
	
	public int getUserId(){
		return _userId;
	}
	public long getTime(){
		return _time;
	}
}
