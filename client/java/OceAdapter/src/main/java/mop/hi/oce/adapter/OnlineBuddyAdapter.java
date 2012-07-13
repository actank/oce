package mop.hi.oce.adapter;


import talk.online.UserOnlineType;



public interface OnlineBuddyAdapter {

	public static int WEB_ONLINE = 2;
	public static int IM_ONLINE = 4;
	//public static int OLD_IM_ONLINE = 6;
	//public static int XNTALK_ONLINE = 10;
	
	public UserOnlineType[] getBuddyStatSeq(int userId);
	public int getBuddyCount(int userId);
	public boolean isWebOnline(int stat);
	public boolean isIMOnline(int stat);

}
