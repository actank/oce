package com.xiaonei.xce.buddyadapter.buddyhotcache;
import java.util.Date;

/**
 * @author yuyang
 *
 */
public class HotFriendData {
	
	public static final String DEFAULT_MEMO = "";
	
	private int hotfriend;
	private int position;
	private int timestamp;
	private String memo;
	
	/**
	 * @param hotfriend
	 * @param position
	 * @param timestamp
	 * @param memo
	 */
	public HotFriendData(int hotfriend,
			int position,
			long timestamp,
			String memo){
		this.hotfriend = hotfriend;	
		this.position = position;
		this.timestamp = (int) timestamp;
		this.memo = memo;
	}
	
	/**
	 * @param hotfriend
	 * @param memo
	 */
	public HotFriendData(int hotfriend,
			String memo){
		this.hotfriend = hotfriend;
		Date date = new Date();
		this.timestamp = (int) date.getTime();
		this.memo = memo;
		this.position = 0;
	}
	
	/**
	 * @param data
	 */
	public HotFriendData(xce.buddy.HotData data){
		this.hotfriend = data.hotfriend;	
		this.position = data.position;
		this.timestamp = data.timestamp;
		this.memo = data.memo;
	}
	
	/**
	 * @param hotfriend
	 */
	public void setHotFriend(int hotfriend){
		this.hotfriend = hotfriend;
	}
	
	/**
	 * @return
	 */
	public int getHotFriend(){
		return this.hotfriend;
	}
	
	/**
	 * @param timestamp
	 */
	public void setTimestamp(int timestamp){
		this.timestamp = timestamp;
	}
	
	/**
	 * @return
	 */
	public int getTimestamp(){
		return this.timestamp;
	}
	
	/**
	 * @param position
	 */
	public void setPosition(int position){
		this.position = position;
	}
	
	/**
	 * @return
	 */
	public int getPosition(){
		return this.position;
	}
	
	/**
	 * @param memo
	 */
	public void setMemo(String memo){
		this.memo = memo;
	}
	
	/**
	 * @return
	 */
	public String getMemo(){
		return this.memo;
	}

	/**
	 * @param data
	 */
	public void setBase(xce.buddy.HotData data){
		this.hotfriend = data.hotfriend;
		this.memo = data.memo;
		this.position = data.position;
	}
}
