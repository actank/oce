package com.xiaonei.xce.cardfacade;

import xce.cardcache.PartInfo;
import xce.cardcache.PartNewInfo;

public class CardShowInfo {

	@Override
	public String toString() {
		return "CardShowInfo [userId=" + userId + ", privacy=" + privacy
				+ ", qq=" + qq + ", mobile=" + mobile + ", name=" + name
				+ ", type=" + type + ", email=" + email + ", msn=" + msn + "]";
	}

	private int userId;
	private String privacy;
	private String qq;
	private String mobile;
	private String name;
	private int type;
	private String msn;
	private String email;

	public CardShowInfo() {

	}

	public CardShowInfo(PartInfo info, int type) {
		this.userId = info.userId;
		this.mobile = info.mobile;
		this.name = info.name;
		this.qq = info.qq;
		this.privacy = info.privacy;
		this.type = type;
	}

	public CardShowInfo(PartNewInfo info) {
		this.userId = info.userId;
		this.mobile = info.mobile;
		this.name = info.name;
		this.qq = info.qq;
		this.privacy = info.privacy;
		this.type = info.type;
	}

	public int getUserId() {
		return userId;
	}

	public void setUserId(int userId) {
		this.userId = userId;
	}

	public void setType(int type) {
		this.type = type;
	}

	public int getType() {
		return type;
	}

	public void setName(String name) {
		this.name = name;
	}

	public String getName() {
		return name;
	}

	public void setPrivacy(String privacy) {
		this.privacy = privacy;
	}

	public String getPrivacy() {
		return privacy;
	}

	public void setQq(String qq) {
		this.qq = qq;
	}

	public String getQq() {
		return qq;
	}

	public void setMobile(String mobile) {
		this.mobile = mobile;
	}

	public String getMobile() {
		return mobile;
	}

	public void setMsn(String msn) {
		this.msn = msn;
	}

	public String getMsn() {
		return msn;
	}

	public void setEmail(String email) {
		this.email = email;
	}

	public String getEmail() {
		return email;
	}

	//TODO：等web更新后把一下四个方法改为private
	public boolean isQQMatch(String query) {
		if (query == null || query.isEmpty())
			return false;
		return this.qq.startsWith(query) ? true : false;

	}

	public boolean isMSNMatch(String query) {
		if (query == null || query.isEmpty())
			return false;
		return this.msn.startsWith(query) ? true : false;
	}

	public boolean isMobileMatch(String query) {
		if (query == null || query.isEmpty())
			return false;
		return this.mobile.startsWith(query) ? true : false;
	}

	public boolean isEmailMatch(String query) {
		if (query == null || query.isEmpty())
			return false;
		return this.email.startsWith(query) ? true : false;
	}

	/**
	 * 判断匹配字段，并返回int数值，从底到高第一位表示手机、第二位表示qq、第三位表示msn、第四位表示邮箱 1表示匹配上改字段，0表示未匹配上
	 */
	public int matchResult(String query) {
		int result = 0;
		if (isMobileMatch(query)) {
			result = result | 1;
		}
		if (isQQMatch(query)) {
			result = result | 2;
		}
		if (isMSNMatch(query)) {
			result = result | 4;
		}
		if (isEmailMatch(query)) {
			result = result | 8;
		}
		return result;
	}

	public static void main(String[] args) {
		CardShowInfo showInfos = new CardShowInfo();
		showInfos.setMobile("123456789");
		showInfos.setQq("897561");
		showInfos.setMsn("1young3005@hotmail.com");
		showInfos.setEmail("1young3005@gmail.com");

		// System.out.println("QQ:" + showInfos.isQQMatch(""));
		// System.out.println("MSN:" + showInfos.isMSNMatch(""));
		// System.out.println("Email:" + showInfos.isEmailMatch(""));
		// System.out.println("Mobile:" + showInfos.isMobileMatch(""));
		System.out.println("match result:" + showInfos.matchResult(""));
		System.out.println("match result:" + showInfos.matchResult("y"));
		System.out.println("match result:" + showInfos.matchResult("1"));
		System.out.println("match result:" + showInfos.matchResult("abc"));
	}

}
