package com.xiaonei.xce.userright;

import mop.hi.oce.domain.Markable;
import xce.userbase.UserRightData;

public class UserRightInfo extends Markable {
	public static final String ID = "ID";
	public static final String AUTH = "AUTH";
	public static final String RIGHTS = "COMMON_RIGHTS";
	protected int id;
	protected int auth;
	protected int rights;

	public UserRightInfo parse(UserRightData source) {
		id = source.uID;

		auth = source.auth;

		rights = source.rights;

		return this;
	}

	public int getAuth() {
		return auth;
	}

	public void setAuth(int auth) {
		this.auth = auth;
		mark(UserRightInfo.AUTH, String.valueOf(auth));
	}

	public int getRights() {
		return rights;
	}

	public void setRights(int rights) {
		this.rights = rights;
		mark(UserRightInfo.RIGHTS, String.valueOf(rights));
	}

	public int getId() {
		return id;
	}

	public void setId(int id) {
		this.id = id;
		mark(UserRightInfo.ID, String.valueOf(id));
	}
}
