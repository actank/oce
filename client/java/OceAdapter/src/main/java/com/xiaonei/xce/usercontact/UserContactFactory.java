package com.xiaonei.xce.usercontact;

public interface UserContactFactory<T extends UserContactInfo> {
	public T create();
}
