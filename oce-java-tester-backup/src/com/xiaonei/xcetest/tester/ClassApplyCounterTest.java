package com.xiaonei.xcetest.tester;

import junit.framework.TestCase;
import mop.hi.oce.adapter.AdapterFactory;
import mop.hi.oce.adapter.ClassApplyCounterAdapter;
import mop.hi.oce.domain.model.ClassApply;

public class ClassApplyCounterTest  extends TestCase {
	int admin = 224472295;//200301672;
	int applicant = 666;	
	int classId = 3409369;
	ClassApplyCounterAdapter adapter;
	public void setUp() {
		adapter = AdapterFactory.getClassApplyCounterAdapter();
		if(adapter.getCounterSeq(admin).length > 0)
			classId = adapter.getCounterSeq(admin)[0].getClassId();
	}
	
	public void tearDown() {
		ClassApply[] result = adapter.getCounterSeq(1000);
		for(ClassApply apply : result) {
			System.out.println(apply.getClassId()+ ", " + apply.getClassName() +", " + apply.getCount());
		}
	}
	
	public void testGet() {
		adapter.reloadClass(classId);
		adapter.reloadAdmin(admin);
		for(ClassApply c : adapter.getCounterSeq(admin)) {
			System.out.println(c.getClassId() + ", " + c.getClassName() + ", " + c.getCount());
		}
	}

	public void testGetTmp() {
		for(ClassApply c : adapter.getCounterSeq(33501183)) {
			System.out.println(c.getClassId() + ", " + c.getClassName() + ", " + c.getCount());
		}
	}
	
	public void testClassApplyCounter() throws Exception {
		testGet();
		
		System.out.println(11);
		adapter.reloadClass(classId);
		System.out.println(12);
		adapter.reloadAdmin(admin);
		System.out.println(13);
		Thread.sleep(1000);
		System.out.println(14);
		
		int oldLen = adapter.getCounterSeq(admin).length;
		System.out.println(15);
		if(adapter.getCounterSeq(admin).length == 0)
			return;
		System.out.println(16);
		int oldCount = adapter.getCounterSeq(admin)[0].getCount();
		System.out.println(17);
		
		ClassApply[] result = adapter.getCounterSeq(admin);
		System.out.println(18);
		ClassApply ca = result[0];
		System.out.println(ca.getClassId() + ", " + ca.getClassName() + ", " + ca.getCount());
		
		int count = ca.getCount();
		adapter.sendApply(applicant, ca.getClassId());
		System.out.println(19);
		Thread.sleep(1000);
		System.out.println(20);
		
		result = adapter.getCounterSeq(admin);
		System.out.println(21);
		ca = result[0];
		System.out.println(ca.getClassId() + ", " + ca.getClassName() + ", " + ca.getCount());
		System.out.println(22);
		assertEquals(ca.getCount(), count + 1);
		System.out.println(23);
		
		adapter.acceptApply(applicant, ca.getClassId());
		System.out.println(24);
		Thread.sleep(1000);
		System.out.println(25);
		assertEquals(adapter.getCounterSeq(admin)[0].getCount(), count);
		System.out.println(26);
		
		adapter.sendApply(applicant, ca.getClassId());
		System.out.println(27);
		Thread.sleep(1000);
		System.out.println(28);
		assertEquals(adapter.getCounterSeq(admin)[0].getCount(), count + 1);
		System.out.println(29);
		
		adapter.removeAdmin(admin, classId);
		System.out.println(30);
		Thread.sleep(1000);
		System.out.println(31);
		assertEquals(adapter.getCounterSeq(admin).length, oldLen-1);
		System.out.println(32);
		
		adapter.addAdmin(admin, classId);
		System.out.println(33);
		Thread.sleep(1000);
		System.out.println(34);
		assertEquals(adapter.getCounterSeq(admin).length, oldLen);
		System.out.println(35);
		
		System.out.println(1);
		adapter.reloadClass(classId);
		System.out.println(2);
		Thread.sleep(1000);
		System.out.println(3);
		assertEquals(adapter.getCounterSeq(admin)[0].getCount(), oldCount);
		System.out.println(4);
		
		adapter.reloadClass(classId);
		System.out.println(5);
		adapter.reloadAdmin(admin);
		System.out.println(6);
	}
	
}
