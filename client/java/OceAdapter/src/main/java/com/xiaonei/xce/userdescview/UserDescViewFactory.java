package com.xiaonei.xce.userdescview;

public interface UserDescViewFactory<T extends UserDescView<?, ?, ?>> {
	public T create();
}
