package com.xiaonei.xcetest.tester;

import junit.framework.TestCase;
import mop.hi.oce.adapter.impl.UserValueAddedAdapterImpl;

public class UserValueAddedTester  extends TestCase{
	public UserValueAddedAdapterImpl adapter = new UserValueAddedAdapterImpl();
	
	public void testClearUserNickName() throws Exception {
		adapter.reload(234431374);
		System.out.println(adapter.get(234431374));
	}
	
}

