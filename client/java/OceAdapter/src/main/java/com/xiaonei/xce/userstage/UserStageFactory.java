package com.xiaonei.xce.userstage;

import com.xiaonei.xce.userstage.UserStageInfo;

public interface UserStageFactory<T extends UserStageInfo> {
	public T create();
}
