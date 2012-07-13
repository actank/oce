package com.xiaonei.xce.userstate;

public class DefaultUserStateFactory implements UserStateFactory<DefaultUserStateInfo> {
	public DefaultUserStateInfo create() {
		return new DefaultUserStateInfo();
	}
}
