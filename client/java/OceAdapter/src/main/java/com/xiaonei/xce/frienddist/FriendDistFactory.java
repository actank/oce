package com.xiaonei.xce.frienddist;

public interface FriendDistFactory<T extends FriendDistCount, W extends FriendDistWithTotal<T>> {
	public W createFriendDistWithTotal();
	public T createFriendDistCount();
}
