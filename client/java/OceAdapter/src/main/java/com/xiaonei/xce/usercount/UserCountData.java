package com.xiaonei.xce.usercount;

import xce.usercount.UserCounter;

public final class UserCountData {

	public int types[];
	public int values[];

	public UserCountData() {
		types = new int[] {};
		values = new int[] {};
	}

	public UserCountData(UserCounter counter) {

		types = counter.types;
		values = counter.values;
	}
}
