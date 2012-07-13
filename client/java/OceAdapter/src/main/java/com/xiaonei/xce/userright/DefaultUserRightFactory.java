package com.xiaonei.xce.userright;

public class DefaultUserRightFactory implements UserRightFactory<DefaultUserRightInfo> {
	public DefaultUserRightInfo create() {
		return new DefaultUserRightInfo();
	}
}
