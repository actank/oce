package com.xiaonei.xce.xfeed;

import java.util.Date;

import xce.util.tools.DateFormatter;

public class XFeedContent {
	private long id;
	private int type;
	private int stype;
	private int comment;
	private String xml;
	private boolean unread;
	private long time;
	private int weight;

	public long getId() {
		return id;
	}

	public void setId(long id) {
		this.id = id;
	}

	public String getXml() {
		return xml;
	}

	public void setXml(String xml) {
		this.xml = xml;
	}

	public String toString() {
		return id + "\t" + DateFormatter.format(new Date(time)) + "\t" + unread + "\t" + type + "\t" + stype + "\t" + weight
				+ "\t" + xml;
	}

	public int getComment() {
		return comment;
	}

	public void setComment(int comment) {
		this.comment = comment;
	}

	public boolean isUnread() {
		return unread;
	}

	public void setUnread(boolean unread) {
		this.unread = unread;
	}

	public int getType() {
		return type;
	}

	public void setType(int type) {
		this.type = type;
	}

	public int getStype() {
		return stype;
	}

	public void setStype(int stype) {
		this.stype = stype;
	}

	public long getTime() {
		return time;
	}

	public void setTime(long time) {
		this.time = time;
	}

	public int getWeight() {
		return weight;
	}

	public void setWeight(int weight) {
		this.weight = weight;
	}

	public void initialize(xce.xfeed.XFeedContent xfc) {
		this.setId(xfc.id);
		this.setType(xfc.type);
		this.setStype(xfc.stype);
		this.setComment(xfc.comment);
		this.setUnread(xfc.unread > 0);
		this.setXml(xfc.xml);
		this.setTime((long)xfc.time*1000);
		this.setWeight(xfc.weight);
	}
}
