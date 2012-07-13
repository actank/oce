package com.xiaonei.xce.test.classapplycounter;

import java.util.Arrays;
import java.util.Collection;
import java.util.Map;
import java.util.Vector;
import java.util.List;

import junit.framework.Assert;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import mop.hi.oce.adapter.ClassApplyCounterAdapter;
import mop.hi.oce.adapter.impl.ClassApplyCounterAdapterImpl;
import mop.hi.oce.domain.model.ClassApply;

import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;


@RunWith(Parameterized.class)
public class ClassApplyCounterTestCase {	
	
	private int _userId;
	ClassApplyCounterAdapter _adapter = new ClassApplyCounterAdapterImpl();	
	
	public ClassApplyCounterTestCase(int userId) {
		this._userId = userId;
	}
	
	@Parameters
	public static Collection<Object[]> setParameters(){
		return Arrays.asList(new Object[][]{
		      	{246996710},		//测试第一组
		});
	}
	
	@BeforeClass
	public static void setUpBeforeClass() throws Exception {		
	}
	@AfterClass
	public static void tearDownAfterClass() throws Exception {  
	}
	@Before
	public void setUpBefore() throws Exception {
	}
	@After
	public void tearDownAfter() throws Exception {  
	}
	@Test
	public void Test()  {
		ClassApply[] ca = _adapter.getCounterSeq(_userId);		
	}
}
