package com.xiaonei.xce.personalinfo;

public class DefaultLinkViewInfoFactory implements LinkViewInfoFactory<DefaultLinkViewInfo> {
	public DefaultLinkViewInfo create() {
		return new DefaultLinkViewInfo();
	}
}