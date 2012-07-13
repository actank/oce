package com.xiaonei.xce.xfeed;

import java.util.ArrayList;
import java.util.List;

public class XFeedQuery {
	private int total;
	private int unread;
	private List<XFeedContent> data = new ArrayList<XFeedContent>();

	public int getTotal() {
		return this.total;
	}

	public void setTotal(int total) {
		this.total = total;
	}

	public int getUnread() {
		return this.unread;
	}

	public void setUnread(int unread) {
		this.unread = unread;
	}

	public List<XFeedContent> getData() {
		return data;
	}

	public void setData(List<XFeedContent> xfeeds) {
		this.data = xfeeds;
	}

	public void addData(XFeedContent xfc) {
		data.add(xfc);
	}
	
	public String toString() {
		StringBuffer s = new StringBuffer();
		s.append("total: " + total + "\n").append("unread: " + unread + "\n");
		for (XFeedContent xfc : data) {
			s.append(xfc + "\n");
		}
		s.append("############\n");
		return s.toString();
	}

}
