package com.xiaonei.xcetest.tester;

import java.io.ByteArrayOutputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.net.URL;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import junit.framework.TestCase;
import mop.hi.oce.adapter.AdapterFactory;
import mop.hi.oce.adapter.FeedAdapter;
import mop.hi.oce.domain.feed.FeedFilter;
import mop.hi.oce.domain.feed.FeedPublishHolder;
import mop.hi.oce.domain.feed.FeedQueryHolder;
import mop.hi.oce.domain.feed.FeedQuerySetHolder;
import mop.hi.oce.domain.feed.FeedTypeHelper;

public class FeedTester extends TestCase {

	FeedAdapter adapter = AdapterFactory.getFeedAdapter();

	public void testCodec() throws Exception {
		URL url = new URL("来源");
		System.out.println(url);
	}

	public void testFeedNewsGet() throws Exception {
//		for (int i = 0; ; ++i) {
//			try {
//				FeedQuerySetHolder fqs = adapter.getFeedNewsSeq(200000027 + i*10,
//						"", false, 0, -1);
//				System.out.println(fqs.getTotal() + "\t" + fqs.getUnknowns());
//
//				for (FeedQueryHolder one : fqs.getFeeds()) {
//					System.out.println(one);
//				}
////				Thread.sleep(10);
//			} catch (Exception e) {
//				System.out.println("EEE:" + i);
//			}
//		}
		FeedQuerySetHolder fqs = adapter.getFeedNewsSeq(241610785, "", false, 0,
				100);
		System.out.println(fqs.getTotal() + "\t" + fqs.getUnknowns() + "\t"
				+ fqs.getFeeds().size());
		for (FeedQueryHolder one : fqs.getFeeds()) {
			System.out.println(one);
		}
	}

	public void testFeedMiniGet() throws Exception {
		// System.out.println(adapter.getFeedMiniCount(200353733, "5", false));
		FeedQuerySetHolder fqs = adapter.getFeedMiniSeq(251764629, "", false, 0,
				-1);
//		FeedQuerySetHolder fqs = adapter.getFeedMiniSeq(223256709, "", false, 0,
//				-1);
		System.out.println(fqs.getTotal() + "\t" + fqs.getUnknowns());
		for (FeedQueryHolder one : fqs.getFeeds()) {
			System.out.println(one);
		}
	}

	public void testFeedNewsRead() {
		adapter.setFeedNewsKnown(85432256, 366393559);
	}

	public void testFeedMiniRead() {
		adapter.setFeedMiniKnown(200353733, 13808640);
	}

	public void testFeedNewsAllRead() {
		adapter.setAllFeedNewsKnown(80578957);
	}

	public void testFeedMiniAllRead() {
		adapter.setAllFeedMiniKnown(200353733);
	}

	public void testFeedDispatch() throws Exception {
		FeedPublishHolder feed = new FeedPublishHolder();
		feed.setFromId(1004);//ren
		feed.setFromName("");
		feed.setOwnerId(80578957);
		feed.setOwnerName("quhao");
		feed.setResource(242139870);
		// feed.setType(FeedTypeHelper.FEED_TYPE_MARKET_MSG);
		feed.setType("50.5.0");
		feed.setTimestamp(new Date());
		feed.setTitle("");
		// feed.setContent("content");

		Content c = new Content();
		c.type = 1;
		c.content = "中国话fdsjkfla74r2q432kl;fjdas7884932743012af'fds]]\\''f'dsafds";
		c.link = "";
		c.list = new ArrayList<String>();
		c.list.add("美国");
		ByteArrayOutputStream bos = new ByteArrayOutputStream();
		ObjectOutputStream os = new ObjectOutputStream(bos);
		os.writeObject(c);
		byte[] rfByte = bos.toByteArray();

		// feed.setContent(rfByte);
		System.out.println(feed);
		adapter.dispatch(feed);
	}

	public void testFeedDispatchAgain() throws Exception {
		AdapterFactory.getFeedAdapter().dispatchAgain(206699516, 206855209);
	}

	public void testFeedFilter() throws Exception {
		FeedFilter filter = new FeedFilter(5, 1);
		System.out.println(filter);
		System.out.println(filter.yAxisOf(8));

		filter.mark(1, 2);
		System.out.println(filter);
		System.out.println(filter.yAxisOf(1));
		filter.mark(7, 3);
		System.out.println(filter);
		System.out.println(filter.yAxisOf(7));

		filter.mark(8, 4);
		System.out.println(filter);
		System.out.println(filter.yAxisOf(8));

		filter.mark(5, 9);
		System.out.println(filter);
		System.out.println(filter.yAxisOf(8));

		filter.mark(4, 0);
		System.out.println(filter);
		System.out.println(filter.yAxisOf(8));

		filter.mark(-1, -2);
		System.out.println(filter);
		System.out.println(filter.yAxisOf(-1));

		System.out.println();
		System.out.println(FeedTypeHelper.getTopCategory("13.7.1"));
	}

	public void testFeedType() throws Exception {
		for (int i = 0; i < 20; ++i) {
			for (int j = 0; j < 20; ++j) {
				for (int k = 0; k < 20; ++k) {
					String type = i + "." + j + "." + k;
					System.out.println(FeedTypeHelper.getX(type) + " - "
							+ FeedTypeHelper.getY(type) + " - "
							+ FeedTypeHelper.getZ(type));
				}
			}
		}
	}

	public static void main(String[] args) {
		FeedAdapter adapter = AdapterFactory.getFeedAdapter();
		for (int i = 1001; ; ++i) {
			Date begin = new Date();
			try {
				FeedQuerySetHolder fqs = adapter.getFeedNewsSeq(i, "", false,
						0, 30);
				long latency = System.currentTimeMillis() - begin.getTime();
				System.out.print("Feeds for " + i + "\t" + fqs.getTotal()
						+ "\t" + fqs.getUnknowns() + "\t" + latency + "\t" + begin + "\t");
				if (latency >= 300) {
					System.out.println("timeout");
				} else {
					System.out.println("ok");
				}
			} catch (Exception e) {
				System.out.println("Feeds for " + i + "\t" + begin + "\tfailed\t" + e.getMessage());
			}
//			try {
//				Thread.sleep(10);
//			} catch (Exception e) {
//
//			}
		}
	}
}

class Content implements Serializable {
	public static final long serialVersionUID = 1;
	public int type;
	public String content;
	public String link;
	public List<String> list;
}
