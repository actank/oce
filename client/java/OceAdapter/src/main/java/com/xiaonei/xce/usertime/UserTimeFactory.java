package com.xiaonei.xce.usertime;

public interface UserTimeFactory<T extends UserTimeInfo> {
	public T create();
}
