package com.xiaonei.xce.vipfullinfo;

import com.xiaonei.xce.vipmemberinfo.VipMemberInfo;

public abstract class VipFullInfo<T extends VipMemberInfo> {
	
	protected int id;
	protected T memberInfo;
	protected String iconUrl;
	protected String hatUrl;
	protected int hatStatus;
	
	public int getId() {
		return id;
	}
	
	public T getMemberInfo() {
		return memberInfo;
	}
	
	public void setId(int id) {
		this.id = id;
	}
	
	public void setMemberInfo(T memberInfo) {
		this.memberInfo = memberInfo;
	}
	
	public void setIconUrl(String iconUrl) {
		this.iconUrl = iconUrl;
	}
	
	public String getIconUrl() {
		return iconUrl;
	}

	public String getHatUrl() {
		return hatUrl;
	}

	public void setHatUrl(String hatUrl) {
		this.hatUrl = hatUrl;
	}

	public int getHatStatus() {
		return hatStatus;
	}

	public void setHatStatus(int hatStatus) {
		this.hatStatus = hatStatus;
	}

}
