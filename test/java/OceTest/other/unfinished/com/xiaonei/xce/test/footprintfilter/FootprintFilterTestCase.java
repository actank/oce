package com.xiaonei.xce.test.footprintfilter;

import java.util.Arrays;
import java.util.Collection;

import mop.hi.svc.fpt.Footprint;
import mop.hi.svc.fpt.FootprintFilterManagerPrx;
import mop.hi.svc.fpt.FootprintFilterManagerPrxHelper;

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
public class FootprintFilterTestCase extends ServiceAdapter{
	
	private static Footprint ftp;

	public FootprintFilterTestCase(Footprint ftp) {
		super("@FootprintFilter", 0, Channel.newOceChannel("FootprintFilter"));
		this.ftp=ftp;
	}
	
	@Parameters
	public static Collection<Object[]> setParameters(){
		Footprint ftp = new Footprint();

		ftp.guest=222677625;
		ftp.host=241618718;
		ftp.id=700000001;
		ftp.guestName="yanangao";
		ftp.guestHead="gao";
		ftp.time=700000008;
		
		return Arrays.asList(new Object[][]{
			
		      	{ftp},		//测试第一组
		});
	}	
	
	 private FootprintFilterManagerPrx getFootprintFilterOneWay() {
         return locateProxy("FFM", Channel.Invoke.Oneway,
        		 FootprintFilterManagerPrxHelper.class, 300);
	 }

	 private FootprintFilterManagerPrx getFootprintFilterTwoWay() {
         return locateProxy("FFM", Channel.Invoke.Twoway,
        		 FootprintFilterManagerPrxHelper.class, 300);
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
			getFootprintFilterOneWay().visitHomepage(ftp);
		}
	 

}