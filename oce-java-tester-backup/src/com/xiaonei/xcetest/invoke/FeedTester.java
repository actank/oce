package com.xiaonei.xcetest.invoke;

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
import mop.hi.oce.adapter.impl.FeedNewsAdapterImpl;
import mop.hi.oce.domain.feed.FeedFilter;
import mop.hi.oce.domain.feed.FeedPublishHolder;
import mop.hi.oce.domain.feed.FeedQueryHolder;
import mop.hi.oce.domain.feed.FeedQuerySetHolder;
import mop.hi.oce.domain.feed.FeedTypeHelper;

public class FeedTester extends TestCase {

	public void testCodec() throws Exception {
		URL url = new URL("来源");
		System.out.println(url);
	}

	public void testFeedNewsGet() throws Exception {
		FeedAdapter adapter = AdapterFactory.getFeedAdapter();
		// while (true) {
		try {
			FeedQuerySetHolder fqs = adapter.getFeedNewsSeq(244689277,  "",
					false, 0, 500);
			System.out.println(fqs.getTotal() + "\t" + fqs.getUnknowns() + "\t"
					+ fqs.getFeeds().size());
			for (FeedQueryHolder one : fqs.getFeeds()) {
				System.out.println(one);
			}
		} catch (Exception e) {
			e.printStackTrace();
		}

		// Thread.sleep(5 * 1000);
		// }
	}

	public void testFeedMiniGet() throws Exception {
		FeedAdapter adapter = AdapterFactory.getFeedAdapter();
		FeedQuerySetHolder fqs = adapter.getFeedMiniSeq(223256709, "", false,
				0, -1);
		System.out.println(fqs.getTotal() + "\t" + fqs.getUnknowns());
		for (FeedQueryHolder one : fqs.getFeeds()) {
			System.out.println(one);
		}
	}

	public void testFeedNewsRead() {
		FeedAdapter adapter = AdapterFactory.getFeedAdapter();
		adapter.setFeedNewsKnown(85432256, 366393559);
	}

	public void testFeedMiniRead() {
		FeedAdapter adapter = AdapterFactory.getFeedAdapter();
		adapter.setFeedMiniKnown(200353733, 13808640);
	}

	public void testFeedNewsAllRead() {
		FeedAdapter adapter = AdapterFactory.getFeedAdapter();
		adapter.setAllFeedNewsKnown(80578957);
	}

	public void testFeedMiniAllRead() {
		FeedAdapter adapter = AdapterFactory.getFeedAdapter();
		adapter.setAllFeedMiniKnown(200353733);
	}

	public void testFeedDispatch() throws Exception {
		FeedAdapter adapter = AdapterFactory.getFeedAdapter();
		FeedPublishHolder feed = new FeedPublishHolder();
		feed.setFromId(143914418);// ren
		feed.setFromName("tt");
		feed.setOwnerId(143914418);
		feed.setOwnerName("tt");
		feed.setResource(143914418);
		// feed.setType(FeedTypeHelper.FEED_TYPE_MARKET_MSG);
		feed.setType("19.5.3");
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
		FeedAdapter adapter = AdapterFactory.getFeedAdapter();
		AdapterFactory.getFeedAdapter().dispatchAgain(1170834918, 1170842421);
	}

	public void testFeedFilter() throws Exception {
		FeedAdapter adapter = AdapterFactory.getFeedAdapter();
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

	public void testSupply() {
		AdapterFactory.getFeedAdapter().deliverSupply(80578957, 250112769);
	}

/*	public static void main(String[] args) {
		FeedAdapter adapter = AdapterFactory.getFeedAdapter();
		for (int i = 1001;; ++i) {
			Date begin = new Date();
			try {
				FeedQuerySetHolder fqs = adapter.getFeedNewsSeq(i, "", false,
						0, 30);
				long latency = System.currentTimeMillis() - begin.getTime();
				System.out.print("Feeds for " + i + "\t" + fqs.getTotal()
						+ "\t" + fqs.getUnknowns() + "\t" + latency + "\t"
						+ begin + "\t");
				if (latency >= 300) {
					System.out.println("timeout");
				} else {
					System.out.println("ok");
				}
			} catch (Exception e) {
				System.out.println("Feeds for " + i + "\t" + begin
						+ "\tfailed\t" + e.getMessage());
			}
			// try {
			// Thread.sleep(10);
			// } catch (Exception e) {
			//
			// }
		}
	}*/
}

class Content implements Serializable {
	public static final long serialVersionUID = 1;
	public int type;
	public String content;
	public String link;
	public List<String> list;
}
