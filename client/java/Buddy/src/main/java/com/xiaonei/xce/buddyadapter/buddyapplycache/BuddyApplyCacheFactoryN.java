package com.xiaonei.xce.buddyadapter.buddyapplycache;

/**
 * @author yuyang
 *
 * @param <T>
 * @param <W>
 */
public interface BuddyApplyCacheFactoryN<T extends BuddyApplyCacheDataN, W extends BuddyApplyCacheWithTotalN<T>> {
	public T createBuddyApplyCacheDataN();
	public W createBuddyApplyCacheWithTotalN();
}
