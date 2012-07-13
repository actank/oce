package com.xiaonei.xce.userbaseview;

public interface UserBaseCommonViewFactory<T extends UserBaseCommonView<?, ?, ?, ?, ?>> {
	public T create();
}
