package com.xiaonei.xce.cardfacade;

import xce.cardcache.CardInfo;

public class CardDetailInfo {
	private int userId;
	private String privacy;
	private int type;

	public CardDetailInfo() {

	}

	public CardDetailInfo(CardInfo info) {
		this.userId = info.userId;
		this.email = info.email;
		this.mobile = info.mobile;
		this.msn = info.msn;
		this.qq = info.qq;
		this.address = info.address;
		this.extra = info.extra;
		this.privacy = info.privacy;
	}

	public CardInfo getCardInfo() {
		CardInfo info = new CardInfo();
		info.userId = this.userId;
		info.email = this.email;
		info.mobile = this.mobile;
		info.msn = this.msn;
		info.qq = this.qq;
		info.address = this.address;
		info.extra = this.extra;
		info.privacy = this.privacy;
		return info;
	}

	public int getUserId() {
		return userId;
	}

	public void setUserId(int userId) {
		this.userId = userId;
	}

	public String getMobile() {
		return mobile;
	}

	public void setMobile(String mobile) {
		this.mobile = mobile;
	}

	public String getMsn() {
		return msn;
	}

	public void setMsn(String msn) {
		this.msn = msn;
	}

	public String getQq() {
		return qq;
	}

	public void setQq(String qq) {
		this.qq = qq;
	}

	public String getAddress() {
		return address;
	}

	public void setAddress(String address) {
		this.address = address;
	}

	public String getExtra() {
		return extra;
	}

	public void setExtra(String extra) {
		this.extra = extra;
	}

	public void setType(int type) {
		this.type = type;
	}

	public int getType() {
		return type;
	}

	public void setPrivacy(String privacy) {
		this.privacy = privacy;
	}

	public String getPrivacy() {
		return privacy;
	}

	public void setEmail(String email) {
		this.email = email;
	}

	public String getEmail() {
		return email;
	}

	private String mobile;
	private String email;
	private String msn;
	private String qq;
	private String address;
	private String extra;

	@Override
	public String toString() {
		return "CardDetailInfo [userId=" + userId + ", privacy=" + privacy
				+ ", type=" + type + ", mobile=" + mobile + ", email=" + email
				+ ", msn=" + msn + ", qq=" + qq + ", address=" + address
				+ ", extra=" + extra + "]";
	}

}
