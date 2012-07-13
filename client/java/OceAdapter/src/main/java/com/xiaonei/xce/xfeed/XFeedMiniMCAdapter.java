package com.xiaonei.xce.xfeed;

import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import xce.util.tools.IPAddress;
import xce.xfeed.XFeedMiniManagerPrx;
import xce.xfeed.XFeedMiniManagerPrxHelper;
import Ice.ObjectPrx;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.XceStat;

public class XFeedMiniMCAdapter extends AbstractModProxyHolder<Integer, XFeedMiniManagerPrx> {
	private static final String XFEED_MINI_MANAGER = "XFMM";
	private static  String endpoints = "@XFeedMiniN";
//	private Vector<XFeedMiniManagerPrx> managers = new Vector<XFeedMiniManagerPrx>();
//
//	private Vector<XFeedMiniManagerPrx> managersOneway = new Vector<XFeedMiniManagerPrx>();
	private XFeedFactory factory;

//	static {
//		if(IPAddress.isKaixin()){
//			endpoints = "@XFeedMini";
//		}
//	}
	
	public XFeedMiniMCAdapter(XFeedFactory factory) {
//		super(endpoints, 100, Channel.newFeedChannel(endpoints));
		super(ChannelFactory.getChannel(ChannelType.FEED),
				XFEED_MINI_MANAGER+endpoints, 100);
		this.factory = factory;
	}
	@Override
	protected int mod(Integer key, int size) {
		return key % size;
	}

	@Override
	public XFeedMiniManagerPrx uncheckedCast(ObjectPrx baseProxy) {
		return XFeedMiniManagerPrxHelper.uncheckedCast(baseProxy);
	}

	@Override
	protected ObjectPrx initProxy(ObjectPrx baseProxy) {
		return baseProxy;
	}
	
	
	

	public XFeedQuery getXFeedMiniSeq(int userId, boolean unread, int begin,
			int limit) throws Exception {
		return null;//getXFeedMiniSeqByType(userId, null, unread, begin, limit);
	}

	public XFeedQuery getXFeedMiniSeqByType(int userId, int[] types,
			boolean unread, int begin, int limit) throws Exception {
		return null;
//		XFeedQuery result = new XFeedQuery();
//		TimeCost cost = TimeCost
//				.begin(XceStat.getLogger(), "getMini " + userId);
//		try {
//			xce.xfeed.XFeedQuery xfq = getXFeedMiniManager(userId)
//					.getXFeedSeqByType(userId, types, unread, begin, limit);
//			cost.endStep("build done");
//			return buildQueryResult(xfq, result);
//		} catch (Ice.TimeoutException e) {
//			Oce.getLogger().error("MiniFeedTimeout: userId=" + userId + " types="
//					+ types + " unread=" + unread + " begin=" + begin
//					+ " limit=" + limit);
//		} finally {
//			cost.endFinally();
//		}
//		return result;
	}

	public XFeedQuery getXFeedMiniSeqBySType(int userId, int[] stypes,
			boolean unread, int begin, int limit) throws Exception {
		return null;
//		XFeedQuery result = new XFeedQuery();
//		try {
//			xce.xfeed.XFeedQuery xfq = getXFeedMiniManager(userId)
//					.getXFeedSeqBySType(userId, stypes, unread, begin, limit);
//			return buildQueryResult(xfq, result);
//		} catch (Ice.TimeoutException e) {
//			Oce.getLogger().error("MiniFeedTimeout: userId=" + userId + " stypes="
//					+ stypes + " unread=" + unread + " begin=" + begin
//					+ " limit=" + limit);
//		}
//		return result;
	}

	public void setAllXFeedMiniRead(int userId) {
		getXFeedMiniManagerOneway(userId).setXFeedAllRead(userId);
	}

	public void setXFeedMiniRead(int userId, long feedId) {
		getXFeedMiniManagerOneway(userId).setXFeedRead(userId, feedId);
	}

	private XFeedQuery buildQueryResult(xce.xfeed.XFeedQuery xfq,
			XFeedQuery result) {
		return null;
//		result.setTotal(xfq.total);
//		result.setUnread(xfq.unread);
//		for (xce.xfeed.XFeedContent fc : xfq.data) {
//			XFeedContent xfc = factory.create();
//			xfc.initialize(fc);
//			result.addData(xfc);
//		}
//		return result;
	}

	private XFeedMiniManagerPrx getXFeedMiniManager(int userId) {
//		return locateProxy(managers, XFEED_MINI_MANAGER, userId,
//				Channel.Invoke.Twoway, XFeedMiniManagerPrxHelper.class, 300);
		return getProxy(userId);
	}

	private XFeedMiniManagerPrx getXFeedMiniManagerOneway(int userId) {
//		return locateProxy(managersOneway, XFEED_MINI_MANAGER, userId,
//				Channel.Invoke.Oneway, XFeedMiniManagerPrxHelper.class, 300);
		
		return uncheckedCast(getProxy(userId).ice_oneway());
	}

	

}
