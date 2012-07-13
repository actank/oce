package com.xiaonei.xce.friendrank;

public interface RankFactory<T extends Rank> {
	public T create();
}
