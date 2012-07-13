package com.xiaonei.xce.feed.reply;

import java.util.Date;

public class FeedReplyObj {
	private long replyId = 0; //
	private int replyType = 0;
	private Date time = new Date();
	private String content = "";
	private String imContent = "";
	private long fromId = 0;
	private String fromName = "";
	private String tinyImg = "";    
	private String isShowIcon = ""; //可能没用了
	
	public long getReplyId() {
		return replyId;
	}
	public void setReplyId(long replyId) {
		this.replyId = replyId;
	}
	public int getReplyType() {
		return replyType;
	}
	public void setReplyType(int replyType) {
		this.replyType = replyType;
	}
	public Date getTime() {
		return time;
	}
	public void setTime(Date time) {
		this.time = time;
	}
	public String getContent() {
		return content;
	}
	public void setContent(String content) {
		this.content = content;
	}
	public String getImContent() {
		return imContent;
	}
	public void setImContent(String imContent) {
		this.imContent = imContent;
	}
	public long getFromId() {
		return fromId;
	}
	public void setFromId(long fromId) {
		this.fromId = fromId;
	}
	public String getFromName() {
		return fromName;
	}
	public void setFromName(String fromName) {
		this.fromName = fromName;
	}
	public String getTinyImg() {
		return tinyImg;
	}
	public void setTinyImg(String tinyImg) {
		this.tinyImg = tinyImg;
	}
	public String getIsShowIcon() {
		return isShowIcon;
	}
	public void setIsShowIcon(String isShowIcon) {
		this.isShowIcon = isShowIcon;
	}
	
}
