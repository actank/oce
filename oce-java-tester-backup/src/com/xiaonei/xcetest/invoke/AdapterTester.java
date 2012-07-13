package com.xiaonei.xcetest.invoke;

import junit.framework.TestCase;
import mop.hi.oce.adapter.AdapterFactory;
import mop.hi.oce.domain.event.Event;

public class AdapterTester extends TestCase {
	public void testAdapter() {
		Object o = new java.util.Map[10];
		Event ev = new Event();
		AdapterFactory.getReplyEventAdapter().publishEvent(ev);
	}

	public void test1() {
		String a = "jiangwen_0012@sina.com";
		String b = "wang9255@163.com";
		String c = "gsws1127@mail.sdu.edu.cn";
		String d = "zmyshufe@163.com";
		String e = "10041530104@student.ecnu.edu.cn";
		System.out.print(Math.abs(e.hashCode()) % 5 + 1);
	}
}
