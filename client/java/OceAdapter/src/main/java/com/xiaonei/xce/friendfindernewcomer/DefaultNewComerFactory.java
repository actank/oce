package com.xiaonei.xce.friendfindernewcomer;

public class DefaultNewComerFactory implements NewComerFactory<DefaultNewComer> {

	@Override
	public DefaultNewComer create() {
		return new DefaultNewComer();
	}

}
