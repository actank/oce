package com.xiaonei.xce.friendfindernewcomer;

public abstract class NewComer {
	private int _userId;
	private long _time;

	public void setBase(int userId, int time){
		_userId = userId;
		_time = time*1000;
	}
	
	public int getUserId(){
		return _userId;
	}
	public long getTime(){
		return _time;
	}
	
}
