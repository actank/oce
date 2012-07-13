package com.xiaonei.xcetest.invoke;

import mop.hi.oce.adapter.AdapterFactory;
import junit.framework.TestCase;

public class OnlineBuddyTest extends TestCase {

	public void testBuddyList() {

		System.out.println(AdapterFactory.getOnlineBuddyAdapter()
				.getBuddyCount(200000002));
		for (int i = 0; i < AdapterFactory.getOnlineBuddyAdapter()
				.getBuddyStatSeq(200000002).length; ++i)
			System.out.println(AdapterFactory.getOnlineBuddyAdapter()
					.getBuddyStatSeq(200000002)[i].stat);

	}
}
