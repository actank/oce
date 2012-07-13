package mop.hi.oce.adapter;

import java.util.Map;

public interface Antispam2Adapter {
	public static final int TRIBE_TOPIC_TYPE =0;
	public static final int TRIBE_REPLY_TYPE =1;
	public static final int MAIL_TYPE =2;
	public static final int CLASS_TYPE =3;
	public static final int GOSSIP_TYPE =4;
	public static final int TRIBE_NAME_TYPE = 5;
	public static final int BLOG_TYPE = 6;
	public static final int REPLY_TYPE = 7;//不做封禁操作
	public static final int DOODLE_TYPE = 8;
	public static final int PHOTO_TYPE = 9;
	public static final int MARKETPLACE_TYPE = 10;
	
	public static final int ACTIVE_TYPE_FAST = 1;
	public static final int ACTIVE_TYPE_SAME = 2;
	public static final int ACTIVE_TYPE_FAST_COUNT_OVER = 4;
	public static final int ACTIVE_TYPE_SAME_COUNT_OVER = 8;
	public static final int ACTIVE_TYPE_SPAM = 16;
	public static final int ACTIVE_TYPE_BLOCK = 32;
	public static final int ACTIVE_TYPE_MAYBE_BLOCK = 64;
	public static final int ACTIVE_TYPE_AUDIT = 128;
	
	//--------control type;
	public static final String FAST_TIMESLOT = "FAST_TIMESLOT";
	public static final String FAST_OVER_COUNT = "FAST_OVER_COUNT"; 
	public static final String FAST_OVER_TIMESLOT ="FAST_OVER_TIMESLOT";
	public static final String SAME_TIMESLOT ="SAME_TIMESLOT";
	public static final String SAME_OVER_COUNT ="SAME_OVER_COUNT";
	public static final String SAME_OVER_TIMESLOT ="SAME_OVER_TIMESLOT";
	public static final String MAYBE_OVER_COUNT ="MAYBE_OVER_COUNT";
	public static final String FORBIDDEN_BY_BLOCK ="FORBIDDEN_BY_BLOCK";
	public static final String FORBIDDEN_BY_MAYBE ="FORBIDDEN_BY_MAYBE";
	public static final String FORBIDDEN_BY_SPAM ="FORBIDDEN_BY_SPAM";
	public static final String FORBIDDEN_BY_SAME_OVER ="FORBIDDEN_BY_SAME_OVER";
	public static final String FORBIDDEN_BY_FAST_OVER ="FORBIDDEN_BY_FAST_OVER";
	
	//----------------------dict word type
	public static final int STOP_TOKEN_END=1;
	public static final int WORD_END=2;
	public static final int ENWORD_END=4;
	public static final int NUM_END=8;
	public static final int MAYBE_BLOCK_WORD_END = 16;
	public static final int BLOCK_WORD_END=32;
	
	public class CheckRet {
		public int type;
		public double order;
	}
	public int check(int fromId, int toid, int type, String text);
	public CheckRet check2(int fromId, int toid, int type, String text);
	public void addNewSpam(String text);
//	public void stopCheck(int serverId,int stat);
//	public void changeTimeslot(int slot);
//	public void changeRepeatslot(int slot);
	public void addNewDictWord(String word, int type);
	public void control(Map<String, Integer> props);
}
