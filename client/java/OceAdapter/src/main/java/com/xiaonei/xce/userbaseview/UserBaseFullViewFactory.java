package com.xiaonei.xce.userbaseview;

public interface UserBaseFullViewFactory<T extends UserBaseFullView<?, ?, ?, ?, ?, ?>> {
	public T create();
}
