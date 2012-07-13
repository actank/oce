package com.xiaonei.xce.xfeed;

import java.util.Vector;

import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;
import xce.util.tools.IPAddress;
import xce.xfeed.XFeedMiniManagerPrx;
import xce.xfeed.XFeedMiniManagerPrxHelper;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.XceStat;

public class XFeedMiniAdapter extends ServiceAdapter {
	private static final String XFEED_MINI_MANAGER = "XFMM";
	private static  String endpoints = "@XFeedMiniN";
	private Vector<XFeedMiniManagerPrx> managers = new Vector<XFeedMiniManagerPrx>();

	private Vector<XFeedMiniManagerPrx> managersOneway = new Vector<XFeedMiniManagerPrx>();
	private XFeedFactory factory;

//	static {
//		if(IPAddress.isKaixin()){
//			endpoints = "@XFeedMini";
//		}
//	}
	
	public XFeedMiniAdapter(XFeedFactory factory) {
		super(endpoints, 100, Channel.newFeedChannel(endpoints));
		this.factory = factory;
	}

	public XFeedQuery getXFeedMiniSeq(int userId, boolean unread, int begin,
			int limit) throws Exception {
		return null;//return getXFeedMiniSeqByType(userId, null, unread, begin, limit);
	}

	public XFeedQuery getXFeedMiniSeqByType(int userId, int[] types,
			boolean unread, int begin, int limit) throws Exception {
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
		return null;
	}

	public XFeedQuery getXFeedMiniSeqBySType(int userId, int[] stypes,
			boolean unread, int begin, int limit) throws Exception {
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
		return null;
	}

	public void setAllXFeedMiniRead(int userId) {
//		getXFeedMiniManagerOneway(userId).setXFeedAllRead(userId);
	}

	public void setXFeedMiniRead(int userId, long feedId) {
//		getXFeedMiniManagerOneway(userId).setXFeedRead(userId, feedId);
	}

	private XFeedQuery buildQueryResult(xce.xfeed.XFeedQuery xfq,
			XFeedQuery result) {
//		result.setTotal(xfq.total);
//		result.setUnread(xfq.unread);
//		for (xce.xfeed.XFeedContent fc : xfq.data) {
//			XFeedContent xfc = factory.create();
//			xfc.initialize(fc);
//			result.addData(xfc);
//		}
//		return result;
		return null;
	}

	private XFeedMiniManagerPrx getXFeedMiniManager(int userId) {
		return locateProxy(managers, XFEED_MINI_MANAGER, userId,
				Channel.Invoke.Twoway, XFeedMiniManagerPrxHelper.class, 300);
	}

	private XFeedMiniManagerPrx getXFeedMiniManagerOneway(int userId) {
		return locateProxy(managersOneway, XFEED_MINI_MANAGER, userId,
				Channel.Invoke.Oneway, XFeedMiniManagerPrxHelper.class, 300);
	}

	public static void main(String[] args) throws Exception {
		XFeedMiniAdapter adapter = new XFeedMiniAdapter(null);
		xce.xfeed.XFeedQuery query = adapter.getXFeedMiniManager(202911262)
				.getXFeedSeqByType(202911262, null, true, 0, 10);
		for (xce.xfeed.XFeedContent c : query.data) {
			System.out.println(c.id + " " + c.type + " " + c.time + " " + c.xml);
		}
	}
}
