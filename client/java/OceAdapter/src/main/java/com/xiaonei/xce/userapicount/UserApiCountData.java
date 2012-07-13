package com.xiaonei.xce.userapicount;

import xce.userapicount.UserApiCounter;

public final class UserApiCountData {

	public int types[];
	public int values[];

	public UserApiCountData() {
		types = new int[] {};
		values = new int[] {};
	}

	public UserApiCountData(UserApiCounter counter) {

		types = counter.types;
		values = counter.values;
	}
}
