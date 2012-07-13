package com.xiaonei.xce.userpassport;

public interface UserPassportFactory<T extends UserPassportInfo> {
	public T create();
}
