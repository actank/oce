package com.xiaonei.xce.userurl;

public interface UserUrlFactory<T extends UserUrlInfo> {
	public T create();
}
