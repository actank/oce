package com.xiaonei.xce.test.xfeedcontent;

import java.util.Arrays;
import java.util.Collection;
import java.util.Map;
import java.util.Random;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Assert;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import xce.util.channel.Channel;
import xce.xfeed.XFeedContent;
import xce.xfeed.XFeedContentManagerPrx;
import xce.xfeed.XFeedContentManagerPrxHelper;
import Ice.ObjectPrx;

@RunWith(Parameterized.class)
public class XFeedContentTestCase {
	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {{},});
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
		Random random = new Random(System. currentTimeMillis());
		int count=random.nextInt();
		
		XFeedContent content=new XFeedContent();
		content.id=6813;
		content.comment=count;
		content.xml="<f><title>saaaaa</title><dID>359396730</dID><head>http://hd45.xiaonei.com/photos/hd45/20080805/01/38/tiny_9345d169.jpg</head><new>1</new><json>{&quot;replyList&quot;:[{&quot;id&quot;:325076224,&quot;replyer_tinyurl&quot;:&quot;http://hd14.xiaonei.com/photos/hd14/20080312/23/36/tiny_7506e107.jpg&quot;,&quot;replyContent&quot;:&quot;aaaaaa&quot;,&quot;ubname&quot;:&quot;\u5eb7\u4f1f.kw&quot;,&quot;replyTime&quot;:&quot;03-24 12:43&quot;,&quot;type&quot;:0,&quot;reply_layer&quot;:0,&quot;ubid&quot;:241610785}],&quot;ownerid&quot;:242332421}</json><count>1</count><time>1237869838861</time><type>506</type><from><fID>242332421</fID><fName>??</fName></from></f>";
		
		ObjectPrx object=Channel.newFeedChannel("XFeedContent").getCommunicator().stringToProxy("XFCM@XFeedContent3");
		XFeedContentManagerPrx manager=XFeedContentManagerPrxHelper.uncheckedCast(object);
		
		manager.deliverXFeed(content);
		long[] ids=new long[1];
		ids[0]=6813;
		Map<Long,XFeedContent> map=manager.getXFeedDict(ids);
		Assert.assertEquals(6813,map.get(ids[0]).id);
		Assert.assertEquals(count,map.get(ids[0]).comment);
		Assert.assertTrue(content.xml.equals(map.get(ids[0]).xml));
		
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		manager.decXFeedComment(6813, 2);
		map=manager.getXFeedDict(ids);
		Assert.assertEquals(6813,map.get(ids[0]).id);
		Assert.assertEquals(count-2,map.get(ids[0]).comment);
		Assert.assertTrue(content.xml.equals(map.get(ids[0]).xml));
		
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		manager.incXFeedComment(6813, 2);
		map=manager.getXFeedDict(ids);
		Assert.assertEquals(6813,map.get(ids[0]).id);
		Assert.assertEquals(count,map.get(ids[0]).comment);
		Assert.assertTrue(content.xml.equals(map.get(ids[0]).xml));
	}
}
