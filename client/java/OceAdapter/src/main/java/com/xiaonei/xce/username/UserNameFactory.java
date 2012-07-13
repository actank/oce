package com.xiaonei.xce.username;

public interface UserNameFactory<T extends UserNameInfo> {
	public T create();
}
