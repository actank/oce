package com.xiaonei.talk.test;

import junit.framework.TestCase;

import com.xiaonei.talk.WebEventAdapter;

public class WEventTest extends TestCase{
	
	public void testAppNotice()
	{
		WebEventAdapter.instance().deliverAppNotice(128487631, 1000, "apptest", "http://a.jpg", "msg");
	}
}
