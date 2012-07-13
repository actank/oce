package com.xiaonei.xce.test.activetrackfacade;

import java.util.Arrays;
import java.util.Collection;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import com.xiaonei.xce.activetrackfacade.ActiveTrackFacadeAdapter;


@RunWith(Parameterized.class)
public class ActiveTrackFacadeTestCase {	
	
	private int _userId1, _userId2;	
	ActiveTrackFacadeAdapter _adapter = ActiveTrackFacadeAdapter.getInstance();	
	
	public ActiveTrackFacadeTestCase(int userId1, int userId2) {
		this._userId1 = userId1;
		this._userId2 = userId2;
	}
	
	@Parameters
	public static Collection<Object[]> setParameters(){
		return Arrays.asList(new Object[][]{
		      	{700000001, 700000002},		//测试第一组
//		      	{700000004, 700000005},		//测试第二组
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
	}
}
