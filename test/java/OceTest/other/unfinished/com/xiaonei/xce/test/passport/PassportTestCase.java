package com.xiaonei.xce.test.passport;

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

import mop.hi.oce.adapter.PassportAdapter;
import mop.hi.oce.adapter.impl.PassportAdapterImpl;


import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;


@RunWith(Parameterized.class)
public class PassportTestCase {	
	
	private int _userId;
	PassportAdapter _adapter = new PassportAdapterImpl();	
	
	public PassportTestCase(int userId) {
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
		String ticket = _adapter.queryTicket(_userId);
		System.out.println("Ticket of " + String.valueOf(_userId) + " : " + ticket);		
	}
}
