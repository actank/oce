package com.xiaonei.xce.userborn;

public interface UserBornFactory<T extends UserBornInfo> {
	public T create();
}
