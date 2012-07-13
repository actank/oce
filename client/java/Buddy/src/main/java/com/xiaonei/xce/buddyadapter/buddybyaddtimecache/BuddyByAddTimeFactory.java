package com.xiaonei.xce.buddyadapter.buddybyaddtimecache;

/**
 * @author yuyang
 *
 * @param <T>
 */
public interface BuddyByAddTimeFactory<T extends BuddyByAddTime> {
	public T create();
}
