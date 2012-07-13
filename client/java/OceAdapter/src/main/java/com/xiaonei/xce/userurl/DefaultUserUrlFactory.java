package com.xiaonei.xce.userurl;

public class DefaultUserUrlFactory implements UserUrlFactory<DefaultUserUrlInfo> {
	public DefaultUserUrlInfo create() {
		return new DefaultUserUrlInfo();
	}
}
