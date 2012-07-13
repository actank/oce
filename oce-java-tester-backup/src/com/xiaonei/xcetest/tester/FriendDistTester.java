package com.xiaonei.xcetest.tester;

import junit.framework.TestCase;
import mop.hi.oce.adapter.impl.FriendDistAdapterImpl;
import mop.hi.svc.model.FriendCount;

public class FriendDistTester extends TestCase {
	FriendDistAdapterImpl adapter = new FriendDistAdapterImpl();

	public void test1() throws Exception {
		FriendCount[]  friendDist = adapter.getFriendDistSeq(1012);
		for (FriendCount fc : friendDist) {
			System.out.println(fc.networkId + " -> " + fc.count);
		}
		System.out.println("#############");
		
		adapter.incFriendDist(1012, 100);
	}
	
	
}
