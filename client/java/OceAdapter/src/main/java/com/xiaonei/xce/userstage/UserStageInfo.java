package com.xiaonei.xce.userstage;


import mop.hi.oce.domain.Markable;
import xce.userbase.UserStageData;

public class UserStageInfo extends Markable {

	public static final String ID = "ID";
	public static final String UNIV = "UNIV";
	public static final String STAGE = "STAGE";

	protected int id;
	protected int univ;
	protected int stage;

	public UserStageInfo parse(UserStageData source) {
		this.id = source.id;
		this.univ=source.univ;
		this.stage=source.stage;
		return this;
	}

	public int getId() {
		return id;
	}

	public void setId(int id) {
		this.id = id;
		mark(UserStageInfo.ID, String.valueOf(id));
	}

	public int getUniv() {
		return univ;
	}

	public void setUniv(int univ) {
		this.univ = univ;
		mark(UserStageInfo.UNIV, String.valueOf(univ));
	}

	public int getStage() {
		return stage;
	}

	public void setStage(int stage) {
		this.stage = stage;
		mark(UserStageInfo.STAGE, String.valueOf(stage));
	}

}
