package com.xiaonei.xce.userbasic;

public class DefaultUserBasicFactory implements UserBasicFactory<DefaultUserBasicInfo> {
	public DefaultUserBasicInfo create() {
		return new DefaultUserBasicInfo();
	}
}
