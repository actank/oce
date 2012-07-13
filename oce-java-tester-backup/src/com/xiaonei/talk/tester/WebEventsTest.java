package com.xiaonei.talk.tester;

import junit.framework.TestCase;

public class WebEventsTest extends TestCase{
	public void notifyTest(){
		//mop.hi.oce.adapter.AdapterFactory.getWebEventsAdapter().notify(240599893,"aaa12313123", "<a href=\"www.sohu.com\" >www</a>");
		WebEventAdapterImpl a = new WebEventAdapterImpl();
		a.notify(200000002, "<a href='www.sohu.com'>sohu</a>hehe", "<a href='www.sohu.com'>sohu</a>hehe");
		
	}
}
