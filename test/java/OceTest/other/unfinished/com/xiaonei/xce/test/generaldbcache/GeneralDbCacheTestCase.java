package com.xiaonei.xce.test.generaldbcache;

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

import xce.generaldbcache.GeneralDbCacheManagerPrx;
import xce.generaldbcache.GeneralDbCacheManagerPrxHelper;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

import com.xiaonei.service.FriendFinderManagerPrx;
import com.xiaonei.service.FriendFinderManagerPrxHelper;

@RunWith (Parameterized.class)
public class GeneralDbCacheTestCase extends ServiceAdapter{
	
	private static int userId1;

	public GeneralDbCacheTestCase(int userId1) {
		super("@DbCacheNotifyConfig0", 0, Channel.newOceChannel("DbCacheNotifyConfig"));
		this.userId1=userId1;
	}
	
	@Parameters
	public static Collection<Object[]> setParameters(){
		return Arrays.asList(new Object[][]{
			
		      	{222677625},		//测试第一组
		});
	}	
	
	 private GeneralDbCacheManagerPrx getGeneralDbCacheOneWay() {
         return locateProxy("M", Channel.Invoke.Oneway,
        		 GeneralDbCacheManagerPrxHelper.class, 300);
	 }

	 private GeneralDbCacheManagerPrx getGeneralDbCacheTwoWay() {
         return locateProxy("M", Channel.Invoke.Twoway,
        		 GeneralDbCacheManagerPrxHelper.class, 300);
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
			getGeneralDbCacheTwoWay().getDbRows(userId1);
		}
	 

}
