package com.xiaonei.xce.xfeed;

import java.util.Vector;
import com.xiaonei.xce.log.Oce;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;
import xce.xfeed.XFeedDispatcherPrx;
import xce.xfeed.XFeedDispatcherPrxHelper;
import xce.util.tools.IPAddress;

/**
 * The {@code XFeedDispatcherAdapter} class is the adapter of XFeedDispatcher 
 * service.  
 * 
 * @author Quhao
 */
public class XFeedDispatcherAdapter extends ServiceAdapter {
	private static XFeedDispatcherAdapter instance;
	private static String endpoints = "@XFeedDispatcherN";
	private Vector<XFeedDispatcherPrx> dispatchersOneway = new Vector<XFeedDispatcherPrx>();
	private Vector<XFeedDispatcherPrx> dispatchers = new Vector<XFeedDispatcherPrx>();

//	static {
//		if(IPAddress.isKaixin()){
//			endpoints = "@XFeedDispatcher";
//		}
//	}
	/**
	 * Constructs an singleton instance of this class.
	 * 
	 */
	public static XFeedDispatcherAdapter getInstance() {
		if (instance == null) {
			instance = new XFeedDispatcherAdapter();
		}
		return instance;
	}
	
	private XFeedDispatcherAdapter() {
		
		super(endpoints, 10, Channel.newFeedChannel(endpoints));
	}
	
	/**
	 * Send xfeedXml and xfeedProp to the background service
	 * 
	 * @param xfeedXml
	 *            the xml data of feed
	 * @param xfeedProp
	 *            the properties for dispatching, it should contain the 
	 *            following keys:<br/> 
	 *            
	 *            actor -> who performs this action.<br/>
	 *            type -> which is the first level type of this action.<br/>
	 *            resource -> which is the unique key for merging.<br/>
	 *            expr -> which is an expression or id that represent feed targets.<br/>
	 *            	example: f(xx)+u(xx)+u(xx)-u(xx)&n(xx)&a(xx)-a(xx)|c(xx).<br/>
	 *            	+ plus rhs to lhs.<br/>
	 *            	- sub the rhs from lhs.<br/>
	 *            	| plus rhs to lhs, like +.<br/>
	 *            	& keep the common targets between lhs and rhs.<br/>
	 *            	f(xx) evaluate all friends of xx.<br/>
	 *            	n(xx) only be used in & operator, evaluate the common  
	 *            		network members with another collection.<br/>
	 *            	a(xx) only be used in & operator, evaluate the common
	 *            		app members with another collection.<br/>
	 *            	c(xx) evaluate all class members of xx.<br/>
	 *            	u(xx) xx only.<br/>            	
	 *            
	 *            weight -> which is the weight of this action when feeding.<br/>
	 *            merge -> which is policy when merging.<br/>
	 *            	-1 remove,<br/>
	 *            	0 none ,<br/>
	 *            	1 replace,<br/>
	 *            	2 append.<br/>
	 *            renew -> renew the feed's time when merging if it is 1.<br/>
	 *            unread -> reset the feed's unread when merging if it is 1.<br/>
	 */
	public void dispatch(String xfeedXml, java.util.Map xfeedProp) {
//		try {
//			getXFeedDispatcherOneway(
//					Integer.parseInt(xfeedProp.get("actor").toString()))
//					.dispatch(xfeedXml, xfeedProp);			
//		} catch (Exception e) {
//			Oce.getLogger().error(this.getClass().getName() + " " + e);
//		}
	}

	/**
	 * Dispatch the original feeds again between startXFeedId and endXFeedId
	 * 
	 * @param startXFeedId
	 *            the start feed id
	 * @param endXFeedId
	 *            the end feed id
	 */
	public void dispatchAgain(long startXFeedId, long endXFeedId) {
//		int mod=(int)(startXFeedId%cluster);
//		getXFeedDispatcherOneway(mod).dispatchAgain(startXFeedId,
//				endXFeedId);
	}

	/**
	 * Increase feed's comment count
	 * 
	 * @param feedId
	 *            the feed id to increase
	 * @param step
	 *            the count to increase on the feed
	 */
	public void incCommentCount(int feedId, int step) {
//		getXFeedDispatcherOneway(feedId).incCommentCount(feedId, step);
	}

	/**
	 * Decrease feed's comment count
	 * 
	 * @param feedId
	 *            the feed id to increase
	 * @param step
	 *            the count to increase on the feed
	 */
	public void decCommentCount(int feedId, int step) {
//		getXFeedDispatcherOneway(feedId).decCommentCount(feedId, step);
	}

	private XFeedDispatcherPrx getXFeedDispatcherOneway(int userId) {
		return locateProxy(dispatchersOneway, "XFD", userId,
				Channel.Invoke.Oneway, XFeedDispatcherPrxHelper.class, 300);
	}

	private XFeedDispatcherPrx getXFeedDispatcher(int userId) {
		return locateProxy(dispatchers, "XFD", userId, Channel.Invoke.Twoway,
				XFeedDispatcherPrxHelper.class, 300);
	}
}
