package com.xiaonei.xce.test.buddyapplycache;

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

import xce.buddyapplycache.BuddyApplyCacheManagerPrx;
import xce.buddyapplycache.BuddyApplyCacheManagerPrxHelper;

import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

@RunWith (Parameterized.class)
public class BuddyApplyCacheTestCase extends ServiceAdapter{

	public BuddyApplyCacheTestCase() {
		super("@BuddyApplyCache0", 0, Channel.newOceChannel("BuddyApplyCache"));
	}
	
	@Parameters
	public static Collection<Object[]> setParameters(){
		return Arrays.asList(new Object[][]{
			
		      	{},		//测试第一组
		});
	}	
	
	 private BuddyApplyCacheManagerPrx getBuddyApplyCacheOneWay() {
         return locateProxy("M", Channel.Invoke.Oneway,
        		 BuddyApplyCacheManagerPrxHelper.class, 300);
	 }

	 private BuddyApplyCacheManagerPrx getBuddyApplyCacheTwoWay() {
         return locateProxy("M", Channel.Invoke.Twoway,
        		 BuddyApplyCacheManagerPrxHelper.class, 300);
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
			System.out.println(getBuddyApplyCacheTwoWay().getHardLimit());
		}
	 

}
