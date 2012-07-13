package com.xiaonei.xce.hatinfo;

import mop.hi.oce.domain.Markable;

import xce.vipinfo.HatData;

public class HatInfo extends Markable {
	
	protected int hatId;
	protected int status;
	protected String hideUrl;
	
	public static final String UID = "ID";
	public static final String STATUS = "STATUS";
	public static final String URL = "URL";
	
	public HatInfo parse(HatData data) {
		this.hatId = data.hatId;
		this.status = data.status;
		this.hideUrl  = data.hideUrl;
		return this;
	}
	
	public String toString() {
		String result = "HatInfo:";
		result += "\nhatId=" + this.hatId;
		result += "\nstatus=" + this.status;
		result += "\nhideUrl=" + this.hideUrl;
		return result;
	}

	public int getHatId() {
		return hatId;
	}

	public String getHideUrl() {
		return hideUrl;
	}

	public void setHatId(int hatId) {
		mark(HatInfo.UID, String.valueOf(hatId));
		this.hatId = hatId;
	}

	public void setStatus(int status) {
		mark(HatInfo.STATUS, String.valueOf(status));
		this.status = status;
	}

	public void setHideUrl(String hideUrl) {
		mark(HatInfo.URL, hideUrl);
		this.hideUrl = hideUrl;
	}

}
