package com.xiaonei.xce.userstate;

public interface UserStateFactory<T extends UserStateInfo> {
	public T create();
}
