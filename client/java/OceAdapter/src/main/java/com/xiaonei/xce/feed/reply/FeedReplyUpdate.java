package com.xiaonei.xce.feed.reply;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;

import com.xiaonei.xce.feed.FeedLogicAdapter;



public class FeedReplyUpdate {

	private Map<String,String> newReplyMap = new HashMap<String,String>();
	private Map<String,String> oldReplyMap = new HashMap<String,String>();
	
	private int replyCount = 0;
	private SimpleDateFormat formatter = new SimpleDateFormat(
			"yyyy-MM-dd HH:mm");

	
	private String getStringDate(Date date) {
		return formatter.format(date);
	}
	
	 /**
	   *@param newReplyId :新回复id
	   */
	  public void SetReplyCount( int  replyCount){
	    	       
	    newReplyMap.put("reply.count", String.valueOf(replyCount));
	    this.replyCount = replyCount;
	  }
	
	 /**
	   *@param newReplyId :新回复id
	   */
	  public void SetNewReplyId( long  newReplyId){
	    	       
	    newReplyMap.put("reply.id", String.valueOf(newReplyId));
	  
	  }
	  
	  /**
	   *@param newReplyBody :新回复body
	   */
	  public void SetNewReplyBody( String  newReplyBody){
	    	       
	    newReplyMap.put("reply.body", newReplyBody);
	  
	  }
	  
	  /**
	   *@param newReplyBody :新回复body
	   */
	  public void SetNewReplyImBody( String  newReplyImBody){
	    	       
	    newReplyMap.put("reply.im.body", newReplyImBody);
	  
	  }
	  
	  
	  /**
	   *@param newReplyTinyImg :新回复头像地址
	   */
	  public void SetNewReplyTinyImg( String  newReplyTinyImg){
	    	       
	    newReplyMap.put("reply.tinyimg", newReplyTinyImg);
	  
	  }
	  
	  
	  /**
	   *@param newReplyFromId :新回复人id
	   */
	  public void SetNewReplyFromId( long  newReplyFromId){
	    	       
	    newReplyMap.put("reply.from.id", String.valueOf(newReplyFromId));
	  
	  }
	  
	  /**
	   *@param newReplyFromId :新回复人名字 
	   */
	  public void SetNewReplyFromName( String  newReplyFromName){
	    	       
	    newReplyMap.put("reply.from.name", newReplyFromName);
	  
	  }
	  
	  /**
	   *@param newReplyFromId :新回复时间
	   */
	  public void SetNewReplyTime( Date  newReplyTime){
	   
		String str = getStringDate(newReplyTime);
	    newReplyMap.put("reply.time", str);
	  
	  }
	  
	  
	  
	  

	  /**
	   *@param oldReplyId :老回复id
	   */
	  public void SetOldReplyId( long  oldReplyId){
	    	       
	    oldReplyMap.put("reply.id", String.valueOf(oldReplyId));
	  
	  }
	  
	  /**
	   *@param oldReplyBody :老回复body
	   */
	  public void SetOldReplyBody( String  oldReplyBody){
	    	       
	    oldReplyMap.put("reply.body", oldReplyBody);
	  
	  }
	  
	  /**
	   *@param oldReplyBody :老回复body
	   */
	  public void SetOldReplyImBody( String  oldReplyImBody){
	    	       
	    oldReplyMap.put("reply.im.body", oldReplyImBody);
	  
	  }
	  
	  
	  /**
	   *@param oldReplyTinyImg :老回复头像地址
	   */
	  public void SetOldReplyTinyImg( String  oldReplyTinyImg){
	    	       
	    oldReplyMap.put("reply.tinyimg", oldReplyTinyImg);
	  
	  }
	  
	  
	  /**
	   *@param oldReplyFromId :老回复人id
	   */
	  public void SetOldReplyFromId( long  oldReplyFromId){
	    	       
	    oldReplyMap.put("reply.from.id", String.valueOf(oldReplyFromId));
	  
	  }
	  
	  /**
	   *@param oldReplyFromId :老回复人名字 
	   */
	  public void SetOldReplyFromName( String  oldReplyFromName){
	    	       
	    oldReplyMap.put("reply.from.name", oldReplyFromName);
	  
	  }
	  
	  /**
	   *@param oldReplyFromId :老回复时间
	   */
	  public void SetOldReplyTime( Date  oldReplyTime){
	    	       
	    oldReplyMap.put("reply.time", getStringDate(oldReplyTime));
	  
	  }
	  
	  /* 
	  public void updateFeedReply(long source, int stype, int actor, long replyId, boolean isDel) {
		// TODO Auto-generated method stub
			//FeedReplyUpdate fr = new FeedReplyUpdate();
			
			//System.out.println("ss:" + dis.ll);
//			FeedReplyObj rp = new FeedReplyObj();
//			rp.setContent("ccccccccccc");
//			rp.setFromId(225092473);
//			rp.setFromName("啊迪");
//			rp.setReplyId(111);
//			rp.setTime(new Date());
//			
//			
//			FeedReplyObj rp2 = new FeedReplyObj();
//			rp2.setContent("bbbbbbbb");
//			rp2.setFromId(225092473);
//			rp2.setFromName("啊迪");
//			rp2.setReplyId(112);
//			rp2.setTime(new Date());
			
//			rp.actor=327580522;
//			rp.count=1;
//			rp.source=2180336678L;
//			rp.sType=502;
//			rp.newFromId=123;
//			rp.newContent="abb<baab";
//			rp.newTime=new Date();
//			rp.newReplyId=111;
			FeedLogicAdapter.getInstance().updateFeedReply(source, stype, actor, newReplyMap, oldReplyMap, replyId, isDel);
			//dis.addFeedReply(rp,rp2, 3, 2180336678L, 502, 327580522);
		//	dis.removeFeedReply(null, null, 3, 111, 2180336678L, 502, 327580522);
	  }
         */
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		FeedReplyUpdate fr = new FeedReplyUpdate();
		fr.SetNewReplyBody("xxxxxxxxxxx");
		fr.SetNewReplyFromId(225092473L);
		fr.SetNewReplyId(123);
		fr.SetNewReplyFromName("ddddd");
		fr.SetNewReplyId(33333333);
		fr.SetNewReplyTime(new Date());
		
		
		
//		fr.SetOldReplyBody("yyyyyyyyy");
//		fr.SetOldReplyFromId(225092473L);
//		fr.SetOldReplyId(1234);
//		fr.SetOldReplyFromName("qqqq");
//		fr.SetOldReplyId(456);
//		fr.SetOldReplyTime(new Date());
		
		//fr.SetReplyCount(1);
		//fr.updateFeedReply(2238211380L, 502, 240570432, 33333333, true);
	}

}
