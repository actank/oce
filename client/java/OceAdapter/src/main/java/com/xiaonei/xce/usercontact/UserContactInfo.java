package com.xiaonei.xce.usercontact;

import mop.hi.oce.domain.Markable;
import xce.userdesc.UserContactData;
import xce.userdesc.UserContactDataN;

public class UserContactInfo extends Markable {

	protected int id;

	protected String msn;

	protected String mobilePhone;

	protected String fixPhone;

	protected long qq;

	protected String homepage;

	public static final String UID = "ID";

	public static final String MSN = "MSN";

	public static final String MOBILEPHONE = "MOBILEPHONE";

	public static final String FIXPHONE = "FIXPHONE";

	public static final String QQ = "QQ";

	public static final String HOMEPAGE = "HOMEPAGE";

	public String getFixPhone() {
		return fixPhone;
	}

	public void setFixPhone(String fixPhone) {
		mark(UserContactInfo.FIXPHONE, fixPhone);
		this.fixPhone = fixPhone;
	}

	public String getHomepage() {
		return homepage;
	}

	public void setHomepage(String homepage) {
		mark(UserContactInfo.HOMEPAGE, homepage);
		this.homepage = homepage;
	}

	public int getId() {
		return id;
	}

	public void setId(int id) {
		mark(UserContactInfo.UID, String.valueOf(id));
		this.id = id;
	}

	public String getMobilePhone() {
		return mobilePhone;
	}

	public void setMobilePhone(String mobilePhone) {
		mark(UserContactInfo.MOBILEPHONE, mobilePhone);
		this.mobilePhone = mobilePhone;
	}

	public String getMsn() {
		return msn;
	}

	public void setMsn(String msn) {
		mark(UserContactInfo.MSN, msn);
		this.msn = msn;
	}

	public long getQq() {
		return qq;
	}

	public void setQq(long qq) {
		mark(UserContactInfo.QQ, String.valueOf(qq));
		this.qq = qq;
	}
	
	/**
	 * 功能：将Ice对象UserContactData解析为UserContactInfo对象
	 * UserContactData中qq成员变量是int类型，所以本方法即将弃用
	 * @param data
	 * @return
	 */
	@Deprecated
	public UserContactInfo parse(UserContactData data) {
		this.id = data.id;
		this.msn = data.msn;
		this.mobilePhone = data.mobilePhone;
		this.fixPhone = data.fixPhone;
		this.qq = data.qq;
		this.homepage = data.homepage;
		return this;
	}

	/**
	 * 功能：将Ice对象UserContactDataN解析为UserContactInfo对象
	 * UserContactDataN中qq成员变量是long类型
	 * @param data
	 * @return
	 */
	public UserContactInfo parse(UserContactDataN data) {
		this.id = data.id;
		this.msn = data.msn;
		this.mobilePhone = data.mobilePhone;
		this.fixPhone = data.fixPhone;
		this.qq = data.qq;
		this.homepage = data.homepage;
		return this;
	}
	
	public String toString() {
		String result = "UserContactInfo:";
		result += "\nid=" + this.id;
		result += "\nmsn=" + this.msn;
		result += "\nmobilePhone=" + this.mobilePhone;
		result += "\nfixPhone=" + this.fixPhone;
		result += "\nqq=" + this.qq;
		result += "\nhomepage=" + this.homepage;
		return result;
	}

}
