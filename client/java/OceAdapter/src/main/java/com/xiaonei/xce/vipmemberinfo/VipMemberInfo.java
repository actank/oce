package com.xiaonei.xce.vipmemberinfo;

import mop.hi.oce.domain.Markable;
import xce.vipinfo.VipMemberDataN;

public class VipMemberInfo extends Markable {
	
	protected int id;
	protected int type;
	protected int status;
	protected int iconId;
	protected int hatId;	
	protected int level;
	protected int intInfo1;
	protected int intInfo2;
	protected String charInfo1;
	protected String charInfo2;
	
	protected boolean isTypeMarked = false;

	public static final String UID = "UID";
	public static final String TYPE = "TYPE";
	public static final String STATUS = "STATUS";
	public static final String ICONID = "ICON_ID";
	public static final String HATID = "HAT_ID";
	public static final String LEVEL = "LEVEL";
	public static final String INTINFO1 = "INT_INFO1";
	public static final String INTINFO2 = "INT_INFO2";
	public static final String CHARINFO1 = "CHAR_INFO1";
	public static final String CHARINFO2 = "CHAR_INFO2";
	
	public static final int TYPE_VIP = 0;
	public static final int TYPE_DISNEY = 1;
	public static final int TYPE_GIFT = 2;
	public static final int TYPE_WIRELESS = 3;

	public VipMemberInfo parse(VipMemberDataN data) {
		this.id = data.id;
		this.type  = data.type;
		this.status = data.status;
		this.iconId = data.iconId;
		this.hatId = data.hatId;
		this.level = data.level;
		this.intInfo1 = data.intInfo1;
		this.intInfo2 = data.intInfo2;
		this.charInfo1 = data.charInfo1;
		this.charInfo2 = data.charInfo2;
		return this;
	}
	
	public String toString() {
		String result = "";
		result += "\nid = " + this.id;
		result += "\ntype = " + this.type;
		result += "\nstatus = " + this.status;
		result += "\niconId = " + this.iconId;
		result += "\nhatId = " + this.hatId;
		result += "\nlevel = " + this.level;
		result += "\nintInfo1 = " + this.intInfo1;
		result += "\nintInfo2 = " + this.intInfo2;
		result += "\ncharInfo1 = " + this.charInfo1;
		result += "\ncharInfo2 = " + this.charInfo2;
		return result;
	}
	
	public boolean isTypeMarked() {
		return isTypeMarked;
	}

	public int getId() {
		return id;
	}

	public void setId(int id) {
		mark(VipMemberInfo.UID, String.valueOf(id));
		this.id = id;
	}

	public int getType() {
		return type;
	}

	public void setType(int type) {
		mark(VipMemberInfo.TYPE, String.valueOf(type));
		isTypeMarked = true;
		this.type = type;
	}

	public int getStatus() {
		return status;
	}

	public void setStatus(int status) {
		mark(VipMemberInfo.STATUS, String.valueOf(status));
		this.status = status;
	}

	public int getIconId() {
		return iconId;
	}

	public void setIconId(int iconId) {
		mark(VipMemberInfo.ICONID, String.valueOf(iconId));
		this.iconId = iconId;
	}

	public int getHatId() {
		return hatId;
	}

	public void setHatId(int hatId) {
		mark(VipMemberInfo.HATID, String.valueOf(hatId));
		this.hatId = hatId;
	}

	public int getLevel() {
		return level;
	}

	public void setLevel(int level) {
		mark(VipMemberInfo.LEVEL, String.valueOf(level));
		this.level = level;
	}

	public int getIntInfo1() {
		return intInfo1;
	}

	public void setIntInfo1(int intInfo1) {
		mark(VipMemberInfo.INTINFO1, String.valueOf(intInfo1));
		this.intInfo1 = intInfo1;
	}

	public int getIntInfo2() {
		return intInfo2;
	}

	public void setIntInfo2(int intInfo2) {
		mark(VipMemberInfo.INTINFO2, String.valueOf(intInfo2));
		this.intInfo2 = intInfo2;
	}

	public String getCharInfo1() {
		return charInfo1;
	}

	public void setCharInfo1(String charInfo1) {
		mark(VipMemberInfo.CHARINFO1, charInfo1);
		this.charInfo1 = charInfo1;
	}

	public String getCharInfo2() {
		return charInfo2;
	}

	public void setCharInfo2(String charInfo2) {
		mark(VipMemberInfo.CHARINFO2, charInfo2);
		this.charInfo2 = charInfo2;
	}
	
}
