package com.xiaonei.xce.buddyapplycache;

public interface BuddyApplyCacheFactoryN<T extends BuddyApplyCacheDataN, W extends BuddyApplyCacheWithTotalN<T>> {
	public T createBuddyApplyCacheDataN();
	public W createBuddyApplyCacheWithTotalN();
}
