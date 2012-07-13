package com.xiaonei.xce.test.svcdescriptor;

import java.util.Arrays;
import java.util.Collection;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runners.Parameterized.Parameters;

import MyUtil.SvcDescriptorPrx;
import MyUtil.SvcDescriptorPrxHelper;

import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

public class SvcDescriptorTestCase extends ServiceAdapter{
	
	private static String name;

	public SvcDescriptorTestCase(String name) {
		super("@SvcDescriptor", 0, Channel.newOceChannel("SvcDescriptor"));
		this.name=name;	
	}
	
	@Parameters
	public static Collection<Object[]> setParameters(){
		return Arrays.asList(new Object[][]{
			
		      	{"yanangao"},		//测试第一组
		});
	}	
	
	 private SvcDescriptorPrx getSvcDescriptorOneWay() {
         return locateProxy("SD", Channel.Invoke.Oneway,
        		 SvcDescriptorPrxHelper.class, 300);
	 }

	 private SvcDescriptorPrx getSvcDescriptorTwoWay() {
         return locateProxy("SD", Channel.Invoke.Twoway,
        		 SvcDescriptorPrxHelper.class, 300);
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
			System.out.println(name);
			//getSvcDescriptorTwoWay().getSvcInstance(name);
		}
	 

}
