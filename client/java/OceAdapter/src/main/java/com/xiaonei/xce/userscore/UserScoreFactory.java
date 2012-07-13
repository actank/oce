package com.xiaonei.xce.userscore;

public interface UserScoreFactory<T extends UserScore> {
	public T create(int userId);
}
