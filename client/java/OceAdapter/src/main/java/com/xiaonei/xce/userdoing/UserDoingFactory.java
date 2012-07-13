package com.xiaonei.xce.userdoing;

import com.xiaonei.xce.userdoing.UserDoingInfo;

public interface UserDoingFactory<T extends UserDoingInfo> {
	public T create();
}
