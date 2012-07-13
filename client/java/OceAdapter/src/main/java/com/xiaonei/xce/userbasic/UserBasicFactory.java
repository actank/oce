package com.xiaonei.xce.userbasic;

public interface UserBasicFactory<T extends UserBasicInfo> {
	public T create();
}
