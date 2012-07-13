package com.xiaonei.xce.userright;

public interface UserRightFactory<T extends UserRightInfo> {
	public T create();
}
