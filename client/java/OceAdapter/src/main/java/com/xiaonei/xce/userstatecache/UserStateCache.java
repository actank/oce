package com.xiaonei.xce.userstatecache;

import com.xiaonei.xce.usercache.UserCache;

public class UserStateCache {
	private final int VIPMEMBER = 1;
	private final int EVERVIPMEMBER = 4;
	
	private int id;
	private int state;
	private int level;
	private String name;
	public UserStateCache(UserCache data) {
		id = data.getId();
		state = data.getState();
		name = data.getName();
		level = data.getLevel();
	}
	public int getId() {
		return id;
	}
	public String getName() {
		return name;
	}
	public int getState() {
		return state;
	}
	
	public int getLevel(){
		return level;
	}
	
	public boolean isEverVipMember() {
		return 0 != (state & (1 << EVERVIPMEMBER));
	}
	public boolean isVipMember() {
		return 0 != (state & (1 << VIPMEMBER));
	}
}
