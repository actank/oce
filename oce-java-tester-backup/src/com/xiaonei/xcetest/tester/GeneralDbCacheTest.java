package com.xiaonei.xcetest.tester;

import junit.framework.TestCase;

import com.xiaonei.xce.dbcache.DbCacheNotifyConfigAdapter;

public class GeneralDbCacheTest extends TestCase {

	public void testHas() {
		int owner = 1;
		int app_id = 2;
		boolean result = DbCacheNotifyConfigAdapter.getInstance().has(owner,
				app_id);
		System.out.println(result);
	}

}
