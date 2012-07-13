package com.xiaonei.xce.test.onlineventmediator;

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

import xce.mediator.onlineevent.OnlineEventMediatorManagerPrx;
import xce.mediator.onlineevent.OnlineEventMediatorManagerPrxHelper;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

@RunWith (Parameterized.class)
public class OnlineEventMediatorTestCase extends ServiceAdapter{
	
	private static int userId, onlineType;

	public OnlineEventMediatorTestCase(int userId, int onlineType) {
		super("@OnlineEventMediator1", 0, Channel.newOceChannel("OnlineEventMediator"));
		this.userId=userId;
		this.onlineType=onlineType;	
	}
	
	@Parameters
	public static Collection<Object[]> setParameters(){
		return Arrays.asList(new Object[][]{
			
		      	{700000001,1},		//测试第一组
		});
	}	
	
	 private OnlineEventMediatorManagerPrx getOnlineEventMediatorOneWay() {
         return locateProxy("M", Channel.Invoke.Oneway,
        		 OnlineEventMediatorManagerPrxHelper.class, 300);
	 }

	 private OnlineEventMediatorManagerPrx getOnlineEventMediatorTwoWay() {
         return locateProxy("M", Channel.Invoke.Twoway,
        		 OnlineEventMediatorManagerPrxHelper.class, 300);
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
			getOnlineEventMediatorOneWay().sendOnlineEventSingle(userId, onlineType);
		}
	 

}
