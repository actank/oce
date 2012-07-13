package com.xiaonei.xce.test.aiguidecache;


import java.util.Arrays;
import java.util.Collection;

import junit.framework.Assert;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import com.xiaonei.xce.aiguidecache.AIGuideCacheAdapter;

@RunWith (Parameterized.class)
public class AIGuideCacheTestCase {
	
	private static int _userId;
	public static AIGuideCacheAdapter _adapter = AIGuideCacheAdapter.getInstance();
	
	public AIGuideCacheTestCase(int userId)  {
		this._userId = userId;		
	}
	
	@Parameters
	public static Collection<Object[]> setParameters(){
		return Arrays.asList(new Object[][]{
		      	{700000002},		//测试第一组
		      	{700000007},		//测试第二组
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
	public void test() {	
		
		AIGuideCacheAdapter.AppType appId = AIGuideCacheAdapter.AppType.APP_FARMGAME;			
		
		// 设置状态0
		_adapter.set(_userId, appId, false);
		boolean status = _adapter.get(_userId, appId);
		Assert.assertFalse(status);
		
		// 设置状态1
		_adapter.set(_userId, appId, true);
		status = _adapter.get(_userId, appId);
		Assert.assertTrue(status);
		
		// 再设置状态0
		_adapter.set(_userId, appId, false);
		status = _adapter.get(_userId, appId);
		Assert.assertFalse(status);
	}
}