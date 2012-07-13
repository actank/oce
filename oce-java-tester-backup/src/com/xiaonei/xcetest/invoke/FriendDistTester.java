package com.xiaonei.xcetest.invoke;

import com.xiaonei.xce.frienddist.DefaultFriendDistFactory;
import com.xiaonei.xce.frienddist.FriendDistAdapter;

import junit.framework.TestCase;
import mop.hi.oce.adapter.impl.FriendDistAdapterImpl;
import mop.hi.svc.model.FriendCount;

public class FriendDistTester extends TestCase {
	FriendDistAdapter adapter = new FriendDistAdapter(new DefaultFriendDistFactory());
	int userId = 231859122;
	
	public void test1(){
		System.out.println(adapter.getFriendDistWithTotal(userId));
		
	}
	
}
