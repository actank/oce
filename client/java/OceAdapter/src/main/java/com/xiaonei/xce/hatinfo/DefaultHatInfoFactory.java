package com.xiaonei.xce.hatinfo;

public class DefaultHatInfoFactory implements HatInfoFactory<DefaultHatInfo>{
	public DefaultHatInfo create() {
		return new DefaultHatInfo();
	}

}
