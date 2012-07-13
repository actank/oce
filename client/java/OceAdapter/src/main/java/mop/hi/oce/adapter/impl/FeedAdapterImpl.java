package mop.hi.oce.adapter.impl;

import mop.hi.oce.adapter.FeedAdapter;
import mop.hi.oce.domain.feed.FeedPublishHolder;
import mop.hi.oce.domain.feed.FeedQuerySetHolder;

public class FeedAdapterImpl implements FeedAdapter {
	private FeedDispatcherAdapterImpl feedDispatcherAdapter = new FeedDispatcherAdapterImpl();
	private FeedMiniAdapterImpl feedMiniAdapter = new FeedMiniAdapterImpl();

	public void dispatch(FeedPublishHolder feed) {
		feedDispatcherAdapter.dispatch(feed);
	}

	public void dispatchAgain(int startId, int endId) {
		feedDispatcherAdapter.dispatchAgain(startId, endId);
	}

	public void deliver(FeedPublishHolder feed, int[] acceptors) {
		feedDispatcherAdapter.deliver(feed, acceptors);
	}

	public void deliverSupply(int supplier, int acceptor) {
		feedDispatcherAdapter.deliverSupply(supplier, acceptor);
	}

	public int getFeedMiniCount(int userId, String type, boolean unknown)
			throws Exception {
		return feedMiniAdapter.getFeedMiniCount(userId, type, unknown);
	}

	public FeedQuerySetHolder getFeedMiniSeq(int userId, String type,
			boolean unknown, int begin, int limit) throws Exception {
		return feedMiniAdapter.getFeedMiniSeq(userId, type, unknown, begin,
				limit);
	}

	public void setAllFeedMiniKnown(int userId) {
		feedMiniAdapter.setAllFeedMiniKnown(userId);
	}

	public void setFeedMiniKnown(int userId, int feedId) {
		feedMiniAdapter.setFeedMiniKnown(userId, feedId);
	}
}
