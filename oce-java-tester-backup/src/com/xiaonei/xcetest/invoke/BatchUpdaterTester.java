package com.xiaonei.xcetest.invoke;

import junit.framework.TestCase;
import mop.hi.oce.adapter.AdapterFactory;
import mop.hi.oce.adapter.impl.BatchUpdaterMasterAdapterImpl;

public class BatchUpdaterTester extends TestCase {

	public void testMessage() {
		// AdapterFactory.getSelfBatchUpdaterAdapter().incMessageOutCount(id);
		//AdapterFactory.getSelfBatchUpdaterAdapter().setMessageInReadFlag(1,8);
		//AdapterFactory.getSelfBatchUpdaterAdapter().incBlogViewCount(210000236
		// , 400000300, 1);
		BatchUpdaterMasterAdapterImpl adapter = new BatchUpdaterMasterAdapterImpl();
		for (int i = 0; i < 1000; ++i) {
			AdapterFactory.getSelfBatchUpdaterAdapter().incMessageInCount(i);
			try {
				Thread.sleep(i);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			// adapter.getManagerOneway().incGameInviteCount(202911262, 1);
		}
	}
}
