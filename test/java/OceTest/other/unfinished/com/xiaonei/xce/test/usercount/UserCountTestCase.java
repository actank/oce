package com.xiaonei.xce.test.usercount;

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

import com.xiaonei.xce.usercount.UserCountAdapter;
	   

@RunWith (Parameterized.class)
public class UserCountTestCase {
	UserCountAdapter _adapter = UserCountAdapter.getInstance();	
	private static int userId, type;
	
	public UserCountTestCase(int userId, int type)  {
		this.userId = userId;
		this.type=type;
	}
	
	@Parameters
	public static Collection<Object[]> setParameters(){
		return Arrays.asList(new Object[][]{
			
		      	{222677625,1},		//测试第一组
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
		System.out.println(_adapter.get(userId, type));
	}

}
