package com.xiaonei.xcetest.tester;

import junit.framework.TestCase;
import mop.hi.oce.adapter.AdapterFactory;

public class BatchUpdaterTester extends TestCase {
	int id = 200353733;

	public void testMessage() {
		// AdapterFactory.getSelfBatchUpdaterAdapter().incMessageOutCount(id);
		// AdapterFactory.getSelfBatchUpdaterAdapter().setMessageInReadFlag(1,8);
		//AdapterFactory.getSelfBatchUpdaterAdapter().incBlogViewCount(210000236, 400000300, 1);
		for(int i = 0 ;i < 1000; ++i)
		{
			//AdapterFactory.getSelfBatchUpdaterAdapter().incMessageInCount(111);
			try {
				Thread.sleep(1);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			AdapterFactory.getSelfBatchUpdaterAdapter().incBlogViewCount(i, i, 1);
		}
	}
}
