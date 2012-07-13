package com.xiaonei.xce.friendfindernewcomer;

public interface NewComerFactory<T extends NewComer> {
	public T create();
}
