package com.xiaonei.xce.test.batchupdate;

import java.util.Arrays;
import java.util.Collection;
import java.util.Map;
import java.util.Vector;

import junit.framework.Assert;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import mop.hi.oce.adapter.BatchUpdaterSelfAdapter;
import mop.hi.oce.adapter.impl.BatchUpdaterSelfAdapterImpl;

import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;


@RunWith(Parameterized.class)
public class BatchUpdateTestCase {	
	
	private int _userId;
	BatchUpdaterSelfAdapter _adapter = new BatchUpdaterSelfAdapterImpl();	
	
	public BatchUpdateTestCase(int userId) {
		this._userId = userId;
	}
	
	@Parameters
	public static Collection<Object[]> setParameters(){
		return Arrays.asList(new Object[][]{
		      	{700000001},		//测试第一组
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
		_adapter.incMessageInCount(_userId);
	}
}
