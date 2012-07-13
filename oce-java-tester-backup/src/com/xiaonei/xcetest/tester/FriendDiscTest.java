package com.xiaonei.xcetest.tester;

import java.util.Map;

import junit.framework.TestCase;
import mop.hi.oce.adapter.AdapterFactory;
import mop.hi.oce.adapter.FriendDistAdapter;

public class FriendDiscTest extends TestCase  {
	
	FriendDistAdapter adapter = AdapterFactory.getFriendDistAdapter();
	public void test1() throws Exception
	{
		Map<Integer, Integer> map = adapter.getFriendDist(128487631);
	
		System.out.println(map.size());
	}

}
