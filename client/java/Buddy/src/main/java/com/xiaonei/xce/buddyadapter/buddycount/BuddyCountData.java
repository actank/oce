package com.xiaonei.xce.buddyadapter.buddycount;

import xce.buddy.BuddyCounter;

/**
 * @author yuyang
 * 
 */
public final class BuddyCountData {

	public int types[];
	public int values[];

	/**
	 * 
	 */
	public BuddyCountData() {
		types = new int[] {};
		values = new int[] {};
	}

	/**
	 * @param counter
	 */
	public BuddyCountData(BuddyCounter counter) {

		types = counter.types;
		values = counter.values;
	}
}
