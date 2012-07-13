package com.xiaonei.xce.username;

public class DefaultUserNameFactory implements UserNameFactory<DefaultUserNameInfo> {
	public DefaultUserNameInfo create() {
		return new DefaultUserNameInfo();
	}
}
