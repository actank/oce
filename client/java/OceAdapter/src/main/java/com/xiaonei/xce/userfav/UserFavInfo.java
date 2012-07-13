package com.xiaonei.xce.userfav;

import mop.hi.oce.domain.Markable;
import xce.userdesc.UserFavData;

public class UserFavInfo extends Markable {

	protected int id;

	protected String fav;

	protected int favFlag;

	public static final String UID = "ID";

	public static final String FAV = "FAV";

	public static final String FAVFLAG = "FAV_FLAG";

	public String getFav() {
		return fav;
	}

	public void setFav(String fav) {
		mark(UserFavInfo.FAV, fav);
		this.fav = fav;
	}

	public int getFavFlag() {
		return favFlag;
	}

	public void setFavFlag(int favFlag) {
		mark(UserFavInfo.FAVFLAG, String.valueOf(favFlag));
		this.favFlag = favFlag;
	}

	public int getId() {
		return id;
	}

	public void setId(int id) {
		mark(UserFavInfo.UID, String.valueOf(id));
		this.id = id;
	}

	public UserFavInfo parse(UserFavData data) {
		this.id = data.id;
		this.fav = data.fav;
		this.favFlag = data.favFlag;
		return this;
	}
}
