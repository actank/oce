package mop.hi.oce.adapter.impl;

import java.util.Vector;

import com.xiaonei.xce.log.Oce;

import mop.hi.oce.domain.feed.FeedQueryHolder;
import mop.hi.oce.domain.feed.FeedQuerySetHolder;
import mop.hi.svc.feed.FeedMiniManagerPrx;
import mop.hi.svc.feed.FeedMiniManagerPrxHelper;
import mop.hi.svc.feed.FeedQuery;
import mop.hi.svc.feed.FeedQueryResultSet;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

public class FeedMiniAdapterImpl extends ServiceAdapter {
	private static final String endpoints = "@FeedMini";
	private Vector<FeedMiniManagerPrx> managers = new Vector<FeedMiniManagerPrx>();

	private Vector<FeedMiniManagerPrx> managersOneway = new Vector<FeedMiniManagerPrx>();

	public FeedMiniAdapterImpl() {
		super(endpoints, 10, Channel.newOceChannel(endpoints));
	}

	public int getFeedMiniCount(int userId, String type, boolean unknown)
			throws Exception {
		return getFeedMiniManager(userId).getFeedCount(userId, type, unknown);
	}

	public FeedQuerySetHolder getFeedMiniSeq(int userId, String type,
			boolean unknown, int begin, int limit) throws Exception {
		FeedQueryResultSet rs = null;
		FeedQuerySetHolder result = new FeedQuerySetHolder();
    FeedMiniManagerPrx prx=null;
		try {
      prx = getFeedMiniManager(userId);
			rs = prx.getFeedSeq(userId, type, unknown,
					begin, limit);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error("MiniFeedTimeout: userId=" + userId + " type="
					+ type + " unknown=" + unknown + " begin=" + begin
					+ " limit=" + limit+" "+prx);
			return result;
		}
		result.setTotal(rs.total);
		result.setUnknowns(rs.unknowns);
		for (FeedQuery feed : rs.data) {
			result.getFeeds().add(new FeedQueryHolder(feed));
		}
		return result;
	}

	public void setAllFeedMiniKnown(int userId) {
		getFeedMiniManagerOneway(userId).setAllFeedKnown(userId);
	}

	public void setFeedMiniKnown(int userId, int feedId) {
		getFeedMiniManagerOneway(userId).setFeedKnown(userId, feedId);
	}

	public FeedMiniManagerPrx getFeedMiniManager(int userId) {
		return locateProxy(managers, "FM", userId, Channel.Invoke.Twoway,
				FeedMiniManagerPrxHelper.class, 300);
	}

	public FeedMiniManagerPrx getFeedMiniManagerOneway(int userId) {
		return locateProxy(managersOneway, "FM", userId, Channel.Invoke.Oneway,
				FeedMiniManagerPrxHelper.class, 300);
	}
}
