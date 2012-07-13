package com.xiaonei.xce.test.xcestorm;

import java.util.Arrays;
import java.util.Collection;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;

import junit.framework.Assert;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import xce.storm.ObserverPrx;

import com.xiaonei.xce.storm.XceStormAdapter;
import com.xiaonei.xce.storm.XceStormListener;

@RunWith(Parameterized.class)
public class XceStormTestCase {

	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] { {}, });
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
		try {
			System.out.println("start testing ...");
			XceStormListenerExample listener = new XceStormListenerExample();
			int original = 0;
			ObserverPrx[] observers = XceStormAdapter.getInstance().query(
					"Test").get("Test");
			if (observers != null) {
				original = observers.length;
			}
			XceStormAdapter.getInstance().subscribe("Test", listener);
			Thread.sleep(1000);
			Assert.assertEquals(original + 1, XceStormAdapter.getInstance()
					.query("Test").get("Test").length);

			Map<String, String> properties = new HashMap<String, String>();
			Date now = new Date();
			properties.put("key", now.toString());
			XceStormAdapter.getInstance().notify("Test", properties);
			Thread.sleep(1000);
			listener.assertMessageSentAndReceived(now.toString());

			XceStormAdapter.getInstance().unsubscribe("Test", listener);
			Thread.sleep(1000);
			Assert.assertEquals(original, XceStormAdapter.getInstance().query(
					"Test").get("Test").length);

			XceStormAdapter.getInstance().subscribe("Test", listener);
			Thread.sleep(1000);
			Assert.assertEquals(original + 1, XceStormAdapter.getInstance()
					.query("Test").get("Test").length);
			XceStormAdapter.getInstance().destroy();
			XceStormAdapter.getInstance().notify("Test", properties);
			Thread.sleep(3 * 60 * 1000);
			Assert.assertEquals(original, XceStormAdapter.getInstance().query(
					"Test").get("Test").length);
			// Thread.sleep(30 * 60 * 1000);
			// Assert.assertEquals(original, XceStormAdapter.getInstance()
			// .query("Test").get("Test").length);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}

class XceStormListenerExample extends XceStormListener {
	private String messageReceived;

	public void handle(Map<String, String> properties) {
		messageReceived = properties.get("key");
	}

	public void assertMessageSentAndReceived(String messageSent) {
		Assert.assertEquals(messageReceived, messageSent);
	}
}
