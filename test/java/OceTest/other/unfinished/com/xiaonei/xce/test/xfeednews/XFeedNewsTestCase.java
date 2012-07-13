package com.xiaonei.xce.test.xfeednews;

import java.util.Arrays;
import java.util.Collection;
import java.util.Map;
import java.util.Vector;
import java.util.List;

import junit.framework.Assert;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import com.xiaonei.xce.xfeed.XFeedNewsAdapter;
import com.xiaonei.xce.xfeed.XFeedFactory;
import com.xiaonei.xce.xfeed.XFeedContent;
import com.xiaonei.xce.xfeed.XFeedQuery;

import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;


@RunWith(Parameterized.class)
public class XFeedNewsTestCase {	
	
	private int _userId;
	XFeedFactory _factory = new XFeedFactoryI();
	XFeedNewsAdapter _adapter = new XFeedNewsAdapter(_factory);	
	
	public XFeedNewsTestCase(int userId) {
		this._userId = userId;
	}
	
	@Parameters
	public static Collection<Object[]> setParameters(){
		return Arrays.asList(new Object[][]{
		      	{246996710},		//测试第一组
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
		try {
			XFeedQuery query = _adapter.getXFeedNewsSeq(_userId, false, 0, 10);
			System.out.println(query.getTotal());
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}

class XFeedFactoryI implements XFeedFactory {
	public XFeedContent create() {
		return new XFeedContent();
	}
}
