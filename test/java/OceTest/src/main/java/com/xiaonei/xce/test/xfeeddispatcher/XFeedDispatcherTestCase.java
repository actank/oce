package com.xiaonei.xce.test.xfeeddispatcher;

import java.util.Arrays;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;

import mop.hi.oce.adapter.impl.BuddyCoreAdapterImpl;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Assert;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import com.xiaonei.xce.xfeed.XFeedContent;
import com.xiaonei.xce.xfeed.XFeedDispatcherAdapter;
import com.xiaonei.xce.xfeed.XFeedFactory;
import com.xiaonei.xce.xfeed.XFeedMiniAdapter;
import com.xiaonei.xce.xfeed.XFeedNewsAdapter;

@RunWith(Parameterized.class)
public class XFeedDispatcherTestCase {

	class MyXFeedFactory implements XFeedFactory {
		public XFeedContent create() {
			return new XFeedContent();
		}
	}

	private static XFeedDispatcherAdapter _dispatcherAdapter = XFeedDispatcherAdapter
			.getInstance();
	private static XFeedNewsAdapter _newsAdapter = new XFeedNewsAdapter(
			new XFeedTestFactory());
	private static XFeedMiniAdapter _miniAdapter = new XFeedMiniAdapter(
			new XFeedTestFactory());

	private static String xml = "<f><title>saaaaa</title><dID>359396730</dID><head>http://hd45.xiaonei.com/photos/hd45/20080805/01/38/tiny_9345d169.jpg</head><new>1</new><json>{&quot;replyList&quot;:[{&quot;id&quot;:325076224,&quot;replyer_tinyurl&quot;:&quot;http://hd14.xiaonei.com/photos/hd14/20080312/23/36/tiny_7506e107.jpg&quot;,&quot;replyContent&quot;:&quot;aaaaaa&quot;,&quot;ubname&quot;:&quot;\u5eb7\u4f1f.kw&quot;,&quot;replyTime&quot;:&quot;03-24 12:43&quot;,&quot;type&quot;:0,&quot;reply_layer&quot;:0,&quot;ubid&quot;:241610785}],&quot;ownerid&quot;:242332421}</json><count>1</count><time>1237869838861</time><type>506</type><from><fID>242332421</fID><fName>??</fName></from></f>";
	private static Map<String, String> map = new HashMap<String, String>();

	private static int userId = 610000055;
//	private static int userId = 200600528;
	private static int buddyId = 700000003;

	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] { {}, });
	}

	@BeforeClass
	public static void setUpBeforeClass() throws Exception {
		_miniAdapter.setAllXFeedMiniRead(userId);
		_miniAdapter.setAllXFeedMiniRead(buddyId);
		_newsAdapter.setAllXFeedNewsRead(buddyId);
		_newsAdapter.setAllXFeedNewsRead(userId);
		BuddyCoreAdapterImpl buddyCoreAdapter = new BuddyCoreAdapterImpl();
		buddyCoreAdapter.addFriend(userId, buddyId);

		map.put("actor", String.valueOf(userId));
		map.put("type", "5");
		map.put("expr", "u(" + userId + ")");
		map.put("resource", "266026923");
		map.put("weight", "1000");
		map.put("renew", "1");
		map.put("unread", "1");
		map.put("merge", "1");
		map.put("stype", "506");
		map.put("news", "1");
		map.put("mini", "1");

		Thread.sleep(2000);
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
			this.assertSelfGotMiniFeed();
			this.assertFriendGotNewsFeed();
			this.assertMergeTimeIsUpdated();
			this.assertDispatchAgainRight();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	private void assertMergeTimeIsUpdated() throws Exception {
		int[] types = { 506 };

		long oldMiniTime = _miniAdapter.getXFeedMiniSeqBySType(userId, types,
				true, 0, -1).getData().get(0).getTime();
		long oldNewsTime = _newsAdapter.getXFeedNewsSeqBySType(buddyId, types,
				true, 0, -1).getData().get(0).getTime();

		map.put("expr", "f(" + userId + ")");
		_dispatcherAdapter.dispatch(xml, map);
		Thread.sleep(2000);

		long NewMiniTime = _miniAdapter.getXFeedMiniSeqBySType(userId, types,
				true, 0, -1).getData().get(0).getTime();
		long newNewsTime = _newsAdapter.getXFeedNewsSeqBySType(buddyId, types,
				true, 0, -1).getData().get(0).getTime();
		
		Assert.assertTrue(NewMiniTime > oldMiniTime);
		Assert.assertTrue(newNewsTime > oldNewsTime);
	}

	private void assertSelfGotMiniFeed() throws Exception {
		map.put("expr", "u(" + userId + ")");
		_dispatcherAdapter.dispatch(xml, map);
		Thread.sleep(2000);

		int[] types = { 506 };
		Assert.assertEquals(1, _miniAdapter.getXFeedMiniSeqBySType(userId,
				types, true, 0, -1).getTotal());
		Assert.assertEquals(1, _newsAdapter.getXFeedNewsSeqBySType(userId,
				types, true, 0, -1).getTotal());
	}

	private void assertFriendGotNewsFeed() throws Exception {
		int[] types = { 506 };
		int oldSize = _newsAdapter.getXFeedNewsSeqBySType(buddyId, types, true,
				0, -1).getTotal();

		map.put("expr", "f(" + userId + ")");
		_dispatcherAdapter.dispatch(xml, map);
		Thread.sleep(3000);

		int newSize = _newsAdapter.getXFeedNewsSeqBySType(buddyId, types, true,
				0, -1).getTotal();

		Assert.assertEquals(oldSize + 1, newSize);
	}
	
	private void assertDispatchAgainRight() throws Exception {
		_miniAdapter.setAllXFeedMiniRead(userId);
		_miniAdapter.setAllXFeedMiniRead(buddyId);
		_newsAdapter.setAllXFeedNewsRead(buddyId);
		_newsAdapter.setAllXFeedNewsRead(userId);
		Thread.sleep(3000);
		
		map.put("expr", "f(" + userId + ")");
		_dispatcherAdapter.dispatch(xml, map);
		Thread.sleep(3000);

		int[] types = { 506 };
		Assert.assertEquals(1, _miniAdapter.getXFeedMiniSeqBySType(userId,
				types, true, 0, -1).getTotal());
		long xfeedId = _miniAdapter.getXFeedMiniSeqBySType(userId,
				types, true, 0, -1).getData().get(0).getId();
		_miniAdapter.setAllXFeedMiniRead(userId);
		_miniAdapter.setAllXFeedMiniRead(buddyId);
		_newsAdapter.setAllXFeedNewsRead(buddyId);
		_newsAdapter.setAllXFeedNewsRead(userId);
		Thread.sleep(3000);
		
		Assert.assertEquals(0, _newsAdapter.getXFeedNewsSeqBySType(buddyId, types, true,
				0, -1).getTotal());
		map.put("expr", "f(" + userId + ")");
		_dispatcherAdapter.dispatchAgain(xfeedId, xfeedId+1);
		Thread.sleep(3000);
		
		Assert.assertEquals(1, _newsAdapter.getXFeedNewsSeqBySType(buddyId, types, true,
				0, -1).getTotal());
		Assert.assertEquals(xfeedId, _newsAdapter.getXFeedNewsSeqBySType(buddyId, types, true,
				0, -1).getData().get(0).getId());
	}
}
