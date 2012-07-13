package com.xiaonei.xce.userurl;

import mop.hi.oce.domain.Markable;

import xce.userbase.UserUrlData;

public class UserUrlInfo extends Markable {

	protected int id;

	protected String largeUrl;

	protected String mainUrl;

	protected String headUrl;

	protected String tinyUrl;

	public static final String UID = "ID";

	public static final String LARGEURL = "LARGEURL";

	public static final String MAINURL = "MAINURL";

	public static final String HEADURL = "HEADURL";

	public static final String TINYURL = "TINYURL";

	public UserUrlInfo parse(UserUrlData data) {
		this.id = data.id;
		this.largeUrl = data.largeUrl;
		this.mainUrl = data.mainUrl;
		this.headUrl = data.headUrl;
		this.tinyUrl = data.tinyUrl;
		return this;
	}

	public String getHeadUrl() {
		return headUrl;
	}

	public void setHeadUrl(String headUrl) {
		mark(UserUrlInfo.HEADURL, headUrl);
		this.headUrl = headUrl;
	}

	public int getId() {
		return id;
	}

	public void setId(int id) {
		mark(UserUrlInfo.UID, String.valueOf(id));
		this.id = id;
	}

	public String getLargeUrl() {
		return largeUrl;
	}

	public void setLargeUrl(String largeUrl) {
		mark(UserUrlInfo.LARGEURL, largeUrl);
		this.largeUrl = largeUrl;
	}

	public String getMainUrl() {
		return mainUrl;
	}

	public void setMainUrl(String mainUrl) {
		mark(UserUrlInfo.MAINURL, mainUrl);
		this.mainUrl = mainUrl;
	}

	public String getTinyUrl() {
		return tinyUrl;
	}

	public void setTinyUrl(String tinyUrl) {
		mark(UserUrlInfo.TINYURL, tinyUrl);
		this.tinyUrl = tinyUrl;
	}
}
