package com.xiaonei.xce.xfeed;

import java.util.Vector;

import xce.util.channel.Channel;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import xce.util.service.ServiceAdapter;
import xce.util.tools.IPAddress;
import xce.xfeed.XFeedMiniManagerPrx;
import xce.xfeed.XFeedMiniManagerPrxHelper;
import xce.xfeed.XFeedNewsManagerPrx;
import xce.xfeed.XFeedNewsManagerPrxHelper;

import Ice.ObjectPrx;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;

public class XFeedNewsMCAdapter   extends AbstractModProxyHolder<Integer, XFeedNewsManagerPrx> {
	private static final String XFEED_NEWS_MANAGER = "XFNM";
	private static String endpoints = "@XFeedNewsN";
//	private Vector<XFeedNewsManagerPrx> managers = new Vector<XFeedNewsManagerPrx>();
//
//	private Vector<XFeedNewsManagerPrx> managersOneway = new Vector<XFeedNewsManagerPrx>();

	private XFeedFactory factory;
//	static {
//		if(IPAddress.isKaixin()){
//			endpoints = "@XFeedNews";
//		}
//	}
	public XFeedNewsMCAdapter(XFeedFactory factory) {
//		super(endpoints, 100, Channel.newFeedChannel(endpoints));
		super(ChannelFactory.getChannel(ChannelType.FEED),
				XFEED_NEWS_MANAGER +endpoints, 100);
		this.factory = factory;
	}

	
	@Override
	protected int mod(Integer key, int size) {
		return key % size;
	}

	@Override
	public XFeedNewsManagerPrx uncheckedCast(ObjectPrx baseProxy) {
		return XFeedNewsManagerPrxHelper.uncheckedCast(baseProxy);
	}

	@Override
	protected ObjectPrx initProxy(ObjectPrx baseProxy) {
		return baseProxy;
	}
	
	
	public XFeedQuery getXFeedNewsSeq(int userId, boolean unread, int begin,
			int limit) throws Exception {
	return null;
		//	return getXFeedNewsSeqByType(userId, null, unread, begin, limit);
	}

	public XFeedQuery getXFeedNewsSeqByType(int userId, int[] types,
			boolean unread, int begin, int limit) throws Exception {
//		XFeedQuery result = new XFeedQuery();
//		TimeCost cost = TimeCost.begin("getXFeedNewsSeqByType " + userId);
//		try {
//			xce.xfeed.XFeedQuery xfq = getXFeedNewsManager(userId)
//					.getXFeedSeqByType(userId, types, unread, begin, limit);
//			cost.endStep("before buildQuery");
//			return buildQueryResult(xfq, result);
//		} catch (Ice.TimeoutException e) {
//			cost.endException(e);
//			Oce.getLogger().error("NewsFeedTimeout: userId=" + userId + " types="
//					+ types + " unread=" + unread + " begin=" + begin
//					+ " limit=" + limit);
//		}catch(Throwable e){
//			cost.endException(e);
//			Oce.getLogger().error("NewsFeed Exception: userId=" + userId + " types="
//					+ types + " unread=" + unread + " begin=" + begin
//					+ " limit=" + limit);
//			//e.printStackTrace();
//		}finally{
//			cost.endFinally();
//		}
//
//		return result;
		return null;
	}

	public XFeedQuery getXFeedNewsSeqBySType(int userId, int[] stypes,
			boolean unread, int begin, int limit) throws Exception {
//		XFeedQuery result = new XFeedQuery();
//		TimeCost cost = TimeCost.begin("getXFeedNewsSeqBySType " + userId);
//		try {
//			xce.xfeed.XFeedQuery xfq = getXFeedNewsManager(userId)
//					.getXFeedSeqBySType(userId, stypes, unread, begin, limit);
//			cost.endStep("before buildQuery");
//			return buildQueryResult(xfq, result);
//		} catch (Ice.TimeoutException e) {
//			cost.endException(e);
//			Oce.getLogger().error("NewsFeedTimeout: userId=" + userId + " stypes="
//					+ stypes + " unread=" + unread + " begin=" + begin
//					+ " limit=" + limit);
//		} catch (Throwable e) {
//			cost.endException(e);
//			Oce.getLogger().error("NewsFeedTimeout: userId=" + userId + " stypes="
//					+ stypes + " unread=" + unread + " begin=" + begin
//					+ " limit=" + limit);
//			//e.printStackTrace();
//		}finally{
//			cost.endFinally();
//		}
//
//		return result;
		return null;
	}

	public void setAllXFeedNewsRead(int userId) {
//		getXFeedNewsManagerOneway(userId).setXFeedAllRead(userId);
	}

	public void setXFeedNewsRead(int userId, long xfeedId) {
//		getXFeedNewsManagerOneway(userId).setXFeedRead(userId, xfeedId);
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

	private XFeedNewsManagerPrx getXFeedNewsManager(int userId) {
//		return locateProxy(managers, XFEED_NEWS_MANAGER, userId,
//				Channel.Invoke.Twoway, XFeedNewsManagerPrxHelper.class, 1500);
		return getProxy(userId);
	}

	private XFeedNewsManagerPrx getXFeedNewsManagerOneway(int userId) {
//		return locateProxy(managersOneway, XFEED_NEWS_MANAGER, userId,
//				Channel.Invoke.Oneway, XFeedNewsManagerPrxHelper.class, 300);
		return uncheckedCast(getProxy(userId).ice_oneway());
	}

}
