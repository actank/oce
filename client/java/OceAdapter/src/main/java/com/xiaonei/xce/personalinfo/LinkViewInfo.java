package com.xiaonei.xce.personalinfo;

import java.util.Map;
import xce.personalinfo.*;
import mop.hi.oce.domain.Markable;

public class LinkViewInfo extends Markable {
	protected int id;
	protected int linkStatus;
	protected String link;

	public LinkViewInfo parse(Map<String, String> info) {
		if(info != null) {
			try {
				this.id = Integer.valueOf(info.get(CPIID.value));
			} catch (NumberFormatException e) {
				this.id = 0;
			}
			try {
				this.linkStatus = Integer.valueOf(info.get(CPILINKSTATUS.value));
			} catch (NumberFormatException e) {
				this.linkStatus = 0;
			}
			this.link = info.get(CPILINK.value);
		}
		return this;
	}
	
	public String toString() {
		String result = "[";
		result += "id:" + id + ", ";
		result += "link:" + link + ", ";
		result += "linkStatus:" + linkStatus + "]";
		return result;
	}

	public int getId() {
		return this.id;
	}

	public int getLinkStatus() {
		return this.linkStatus;
	}
	
	public String getLink() {
		return link;
	}
	
	public void setId(int id) {
		this.id = id;
		mark(CPIID.value, String.valueOf(this.id));
	}

	public void setLinkStatus(int linkStatus) {
		this.linkStatus = linkStatus;
		mark(CPILINKSTATUS.value, String.valueOf(this.linkStatus));
	}
	
	public void setLink(String link) {
		this.link = link;
		mark(CPILINK.value, String.valueOf(this.link));
	}

}
