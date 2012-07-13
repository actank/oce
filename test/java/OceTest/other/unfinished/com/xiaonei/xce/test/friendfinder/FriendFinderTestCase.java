package com.xiaonei.xce.test.friendfinder;

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

import com.xiaonei.service.FriendFinderManagerPrx;
import com.xiaonei.service.FriendFinderManagerPrxHelper;

import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;


@RunWith (Parameterized.class)
public class FriendFinderTestCase extends ServiceAdapter{
	
	private static int userId1, userId2;

	public FriendFinderTestCase(int userId1, int userId2) {
		super("@FriendFinder5", 0, Channel.newOceChannel("FriendFinder"));
		this.userId1=userId1;
		this.userId2=userId2;
	}
	
	@Parameters
	public static Collection<Object[]> setParameters(){
		return Arrays.asList(new Object[][]{
			
		      	{222677625,241618718},		//测试第一组
		});
	}	
	
	 private FriendFinderManagerPrx getFriendFinderOneWay() {
         return locateProxy("FFM", Channel.Invoke.Oneway,
        		 FriendFinderManagerPrxHelper.class, 300);
	 }

	 private FriendFinderManagerPrx getFriendFinderTwoWay() {
         return locateProxy("FFM", Channel.Invoke.Twoway,
        		 FriendFinderManagerPrxHelper.class, 300);
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
			getFriendFinderTwoWay().getSharedFriendCount(userId1, userId2);
		}
	 

}
