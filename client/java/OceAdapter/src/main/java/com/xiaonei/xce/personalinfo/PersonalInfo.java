package com.xiaonei.xce.personalinfo;

import java.util.Map;
import xce.personalinfo.*;
import mop.hi.oce.domain.Markable;

public class PersonalInfo extends Markable {
	protected int id;
	protected int linkStatus;
	protected int space_all;
	protected int source;
	protected long lastip_long;
	protected String invitecode;
	protected String link;

	public PersonalInfo parse(Map<String, String> info) {
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
			try {
				this.space_all = Integer.valueOf(info.get(CPISPACEALL.value));
			} catch (NumberFormatException e) {
				this.space_all = 0;
			}
			try {
				this.source = Integer.valueOf(info.get(CPISOURCE.value));
			} catch (NumberFormatException e) {
				this.source = 0;
			}
			try {
				this.lastip_long = Long.valueOf(info.get(CPILASTIPLONG.value));
			} catch (NumberFormatException e) {
				this.lastip_long = 0;
			}
			this.invitecode = info.get(CPIINVITECODE.value);
			this.link = info.get(CPILINK.value);
		}
		return this;
	}
	
	public String toString() {
		String result = "[";
		result += "id:" + id + ", ";
		result += "invitecode:" + invitecode + ", ";
		result += "link:" + link + ", ";
		result += "linkStatus:" + linkStatus + ", ";
		result += "space_all:" + space_all + ", ";
		result += "lastip_long:" + lastip_long + ", ";
		result += "source:" + source + "]";
		return result;
	}

	public int getId() {
		return this.id;
	}

	public int getLinkStatus() {
		return this.linkStatus;
	}
	
	public int getSpaceAll() {
		return this.space_all;
	}
	
	public int getSource() {
		return this.source;
	}
	
	public long getLastIpLong() {
		return this.lastip_long;
	}
	
	public String getLink() {
		return link;
	}
	
	public String getInvitecode() {
		return invitecode;
	}
	
	public void setId(int id) {
		this.id = id;
		mark(CPIID.value, String.valueOf(this.id));
	}

	public void setLinkStatus(int linkStatus) {
		this.linkStatus = linkStatus;
		mark(CPILINKSTATUS.value, String.valueOf(this.linkStatus));
	}
	
	public void setSpaceAll(int space_all) {
		this.space_all = space_all;
		mark(CPILINKSTATUS.value, String.valueOf(this.space_all));
	}

	public void setSource(int source) {
		this.source = source;
		mark(CPILINKSTATUS.value, String.valueOf(this.source));
	}

	public void setSource(long lastip_long) {
		this.lastip_long = lastip_long;
		mark(CPILINKSTATUS.value, String.valueOf(this.lastip_long));
	}

	public void setInvitecode(String invitecode) {
		this.invitecode = invitecode;
		mark(CPILINK.value, String.valueOf(this.invitecode));
	}

	public void setLink(String link) {
		this.link = link;
		mark(CPILINK.value, String.valueOf(this.link));
	}

}
