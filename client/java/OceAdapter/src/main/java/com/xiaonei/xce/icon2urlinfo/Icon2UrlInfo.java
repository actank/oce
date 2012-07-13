package com.xiaonei.xce.icon2urlinfo;

import mop.hi.oce.domain.Markable;

import xce.vipinfo.Icon2UrlData;;

public class Icon2UrlInfo extends Markable {
	
	protected int iconId;
	protected String hideUrl;
	
	public static final String UID = "ID";
	public static final String URL = "URL";
	
	public Icon2UrlInfo parse(Icon2UrlData data) {
		this.iconId = data.iconId;
		this.hideUrl  = data.hideUrl;
		return this;
	}

	public int getIconId() {
		return iconId;
	}

	public String getHideUrl() {
		return hideUrl;
	}

	public void setIconId(int iconId) {
		mark(Icon2UrlInfo.UID, String.valueOf(iconId));
		this.iconId = iconId;
	}

	public void setHideUrl(String hideUrl) {
		mark(Icon2UrlInfo.URL, hideUrl);
		this.hideUrl = hideUrl;
	}

}
