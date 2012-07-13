package com.xiaonei.xce.usertime;

import java.util.Date;

import mop.hi.oce.domain.Markable;

import xce.userdesc.UserTimeData;
import xce.util.tools.DateFormatter;

public class UserTimeInfo extends Markable {

	protected int id;

	protected Date registerTime;

	protected Date activateTime;

	protected Date lastLoginTime;

	protected Date lastLogoutTime;

	public static final String UID = "ID";

	public static final String REGISTERTIME = "REGISTERTIME";

	public static final String ACTIVATETIME = "ACTIVATETIME";

	public static final String LASTLOGINTIME = "LASTLOGINTIME";

	public static final String LASTLOGOUTTIME = "LASTLOGOUTTIME";

	public UserTimeInfo parse(UserTimeData data) {
		this.id = data.id;
		this.registerTime = new Date(data.registerTime * 1000l);
		this.activateTime = new Date(data.activateTime * 1000l);
		this.lastLoginTime = new Date(data.lastLoginTime * 1000l);
		this.lastLogoutTime = new Date(data.lastLogoutTime * 1000l);
		return this;
	}

	public Date getActivateTime() {
		return activateTime;
	}

	public void setActivateTime(Date activateTime) {
		this.activateTime = activateTime;
		if (activateTime==null || activateTime.getTime() == 0) {
			return;
		}
		mark(UserTimeInfo.ACTIVATETIME, DateFormatter.format(activateTime));
	}

	public int getId() {
		return id;
	}

	public void setId(int id) {
		mark(UserTimeInfo.UID, String.valueOf(id));
		this.id = id;
	}

	public Date getLastLoginTime() {
		return lastLoginTime;
	}

	public void setLastLoginTime(Date lastLoginTime) {
		this.lastLoginTime = lastLoginTime;
		if (lastLoginTime == null||lastLoginTime.getTime() == 0) {
			return;
		}
		mark(UserTimeInfo.LASTLOGINTIME, DateFormatter.format(lastLoginTime));
	}

	public Date getLastLogoutTime() {
		return lastLogoutTime;
	}

	public void setLastLogoutTime(Date lastLogoutTime) {
		this.lastLogoutTime = lastLogoutTime;
		if (lastLogoutTime==null||lastLogoutTime.getTime() == 0) {
			return;
		}
		mark(UserTimeInfo.LASTLOGOUTTIME, DateFormatter.format(lastLogoutTime));
	}

	public Date getRegisterTime() {
		return registerTime;
	}

	public void setRegisterTime(Date registerTime) {
		this.registerTime = registerTime;
		if (registerTime==null||registerTime.getTime() == 0) {
			return;
		}
		mark(UserTimeInfo.REGISTERTIME, DateFormatter.format(registerTime));
	}
}
