package com.xiaonei.xce.userpassport;

public class DefaultUserPassportFactory implements UserPassportFactory<DefaultUserPassportInfo> {
	public DefaultUserPassportInfo create() {
		return new DefaultUserPassportInfo();
	}
}
