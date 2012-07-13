package com.xiaonei.xce.buddyadapter.buddybyaddtimecache;

/**
 * @author yuyang
 *
 */
public abstract class BuddyByAddTime {
	private int _userId;
	private long _time;

	/**
	 * @param userId
	 * @param time
	 */
	public void setBase(int userId, int time){
		_userId = userId;
		_time = ((long)time)*1000;
	}
	
	/**
	 * @return
	 */
	public int getUserId(){
		return _userId;
	}
	
	/**
	 * @return
	 */
	public long getTime(){
		return _time;
	}
}
