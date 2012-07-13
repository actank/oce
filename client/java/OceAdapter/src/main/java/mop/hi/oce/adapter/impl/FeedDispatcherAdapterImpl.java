package mop.hi.oce.adapter.impl;

import java.util.Vector;

import mop.hi.oce.domain.feed.FeedPublishHolder;
import mop.hi.svc.feed.FeedDispatcherPrx;
import mop.hi.svc.feed.FeedDispatcherPrxHelper;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

public class FeedDispatcherAdapterImpl extends ServiceAdapter {
	private static final String endpoints = "@FeedDispatcher";
	private Vector<FeedDispatcherPrx> managersOneway = new Vector<FeedDispatcherPrx>();

	public FeedDispatcherAdapterImpl() {
//		super(endpoints, 10, Channel.newFeedChannel(endpoints));
		super(endpoints, 10, Channel.newOceChannel(endpoints));
	}

	public void dispatch(FeedPublishHolder feed) {
		if (feed.getType() == "") {
			// invoke NewbieFeeder to dispatch the guy's registering news
		} else {
			getFeedDispatcherOneway(feed.getFromId()).dispatch(feed.getFeed());
		}
	}

	public void dispatchAgain(int startId, int endId) {
		getFeedDispatcherOneway(startId).dispatchAgain(startId, endId);
	}

	public void deliver(FeedPublishHolder feed, int[] acceptors) {
		getFeedDispatcherOneway(feed.getFromId()).deliver(feed.getFeed(),
				acceptors);
	}

	public void deliverSupply(int supplier, int acceptor) {
		getFeedDispatcherOneway(supplier).deliverSupply(supplier, acceptor);
	}

	private FeedDispatcherPrx getFeedDispatcherOneway(int userId) {
		return locateProxy(managersOneway, "FD", userId, Channel.Invoke.Oneway,
				FeedDispatcherPrxHelper.class, 300);
	}
}
