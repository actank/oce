package com.xiaonei.xce.userfav;

public interface UserFavFactory<T extends UserFavInfo> {
	public T create();
}
