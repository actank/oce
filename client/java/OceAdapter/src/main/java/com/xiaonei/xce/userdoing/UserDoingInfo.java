package com.xiaonei.xce.userdoing;


import java.util.Date;

import com.xiaonei.xce.usertime.UserTimeInfo;

import mop.hi.oce.domain.Markable;
import xce.userbase.UserDoingData;
import xce.util.tools.DateFormatter;

public class UserDoingInfo extends Markable {

	public static final String ID = "ID";
	public static final String DOING = "DOING";
	public static final String DOINGTIME = "DOINGTIME";

	protected int id;
	protected String doing;
	protected Date doingTime;

	public UserDoingInfo parse(UserDoingData source) {
		this.id = source.id;
		this.doing=source.doing;
		this.doingTime = new Date(source.doingTime * 1000l);
		return this;
	}

	public int getId() {
		return id;
	}

	public void setId(int id) {
		this.id = id;
		mark(UserDoingInfo.ID,String.valueOf(id));
	}

	public String getDoing() {
		return doing;
	}

	public void setDoing(String doing) {
		this.doing = doing;
		mark(UserDoingInfo.DOING,doing);
	}

	public Date getDoingTime() {
		return doingTime;
	}

	public void setDoingTime(Date doingTime) {
		this.doingTime = doingTime;
		if (doingTime==null||doingTime.getTime() == 0) {
			return;
		}
		mark(UserDoingInfo.DOINGTIME, DateFormatter.format(doingTime));
	}

}
