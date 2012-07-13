package com.xiaonei.xce.userpassport;

import mop.hi.oce.domain.Markable;
import xce.userbase.UserPassportData;

public class UserPassportInfo extends Markable {

	public static final String ID = "ID";
	public static final String STATUS = "STATUS";
	public static final String SAFESTATUS = "SAFE_STATUS";
	public static final String ACCOUNT = "ACCOUNT";
	public static final String DOMAIN = "DOMAIN";
	public static final String PASSWORD_MD5 = "PASSWORD_MD5";

	protected int id;
	protected int status;
	protected int safeStatus;
	protected String account;
	protected String domain;
	protected String passwordMd5;

	protected void setKeyValue(String strKey, String strValue) {
		if (strKey.equals(ACCOUNT)) {
			account = strValue;
			mark(UserPassportInfo.ACCOUNT, account);
		} else if (strKey.equals(DOMAIN)) {
			domain = strValue;
			mark(UserPassportInfo.DOMAIN, domain);
		} else if (strKey.equals(PASSWORD_MD5)) {
			passwordMd5 = strValue;
			mark(UserPassportInfo.PASSWORD_MD5, passwordMd5);
		}
	}

	protected void setKeyValue(String strKey, int iValue) {
		if (strKey.equals(ID)) {
			id = iValue;
			mark(UserPassportInfo.ID, String.valueOf(id));
		} else if (strKey.equals(STATUS)) {
			status = iValue;
			mark(UserPassportInfo.STATUS, String.valueOf(status));
		}
	}

	public UserPassportInfo parse(UserPassportData source) {
		id = source.uID;

		status = source.status;

		safeStatus = source.safeStatus;

		account = source.account;

		domain = source.domain;

		passwordMd5 = source.passwordmd5;

		return this;
	}

	public String getAccount() {
		return account;
	}

	public void setAccount(String account) {
		this.account = account;
		mark(UserPassportInfo.ACCOUNT, account);
	}

	public String getDomain() {
		return domain;
	}

	public void setDomain(String domain) {
		this.domain = domain;
		mark(UserPassportInfo.DOMAIN, domain);
	}

	public String getPasswordMd5() {
		return passwordMd5;
	}

	public void setPasswordMd5(String passwordMd5) {
		this.passwordMd5 = passwordMd5;
		mark(UserPassportInfo.PASSWORD_MD5, passwordMd5);
	}

	public int getStatus() {
		return status;
	}

	public void setStatus(int status) {
		this.status = status;
		mark(UserPassportInfo.STATUS, String.valueOf(status));
	}


	public int getSafeStatus() {
		return safeStatus;
	}

	public void setSafeStatus(int safeStatus) {
		this.safeStatus = safeStatus;
		mark(UserPassportInfo.SAFESTATUS, String.valueOf(safeStatus));
	}

	public int getId() {
		return id;
	}

	public void setId(int id) {
		this.id = id;
		mark(UserPassportInfo.ID, String.valueOf(id));
	}
}
