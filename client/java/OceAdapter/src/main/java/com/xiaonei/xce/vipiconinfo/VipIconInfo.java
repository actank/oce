package com.xiaonei.xce.vipiconinfo;

import xce.vipinfo.VipIconData;;

public class VipIconInfo {
	
	protected int id; //用户id
	protected int type; //标识vip用户类型
	protected int iconId; //图标代表的id，根据该id拼出用户的vip图标
	protected String hideUrl; //弹层隐藏的url
	protected int level; //用户等级
	protected int status; //用户状态

	public VipIconInfo parse(VipIconData data) {
		this.id = data.id;
		this.type = data.type;
		this.iconId = data.iconId;
		this.level = data.level;
		this.status = data.status;
		this.hideUrl = data.hideUrl;
		return this;
	}

	public int getId() {
		return id;
	}

	public int getType() {
		return type;
	}

	public int getIconId() {
		return iconId;
	}

	public String getHideUrl() {
		return hideUrl;
	}

	public int getLevel() {
		return level;
	}

	public int getStatus() {
		return status;
	}

}
