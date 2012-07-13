package com.xiaonei.xce.footprint;

import java.util.Date;
import java.util.HashMap;
import java.util.Map;

import com.xiaonei.xce.footprint.FootprintNewAdapter;
import com.xiaonei.xce.log.Oce;

import mop.hi.oce.domain.model.FootprintNewView;
import mop.hi.oce.domain.model.FootprintResult;

import xce.fptn.FootprintInfo;
import xce.fptn.FootprintManagerPrx;
import xce.fptn.FootprintManagerPrxHelper;
import xce.fptn.FootprintTiny;

import xce.clusterstate.ReplicatedClusterAdapter;

public class AppFootprintAdapterImpl extends ReplicatedClusterAdapter
		implements FootprintNewAdapter {

	private static final String ctr_endpoints = "ControllerAppFootprint";

	private static int _interval = 120;

	public AppFootprintAdapterImpl(String endpoints, int cluster) {
		super(ctr_endpoints, _interval);
	}

	public FootprintResult getAll(int hostId) {
		return getAll(hostId, 0, 6);
	}

	public FootprintResult getAll(int hostId, int begin, int limit) {

		FootprintResult result = null;
		FootprintManagerPrx prx = null; 
		try {
			prx = getFootprintManager(hostId);
			FootprintInfo info = prx.getAll(hostId, begin, limit);
			result = new FootprintResult(hostId, info.fpts, info.visitcount);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".getAll [" + prx.toString() + "]" + hostId + " Ice.TimeoutException");
			result = new FootprintResult(hostId, new xce.fptn.FootprintTiny[0], 0);
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".getAll [" + prx.toString() + "]" + hostId, e);
			result = new FootprintResult(hostId, new xce.fptn.FootprintTiny[0], 0);	
		}
		
		return result;
	}

	public int getSize(int hostId) {
		int size = 0;
		FootprintManagerPrx prx = null;
		try {
			prx = getFootprintManager(hostId);
			size = prx.getSize(hostId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".getSize [" + prx.toString() + "]" + hostId + " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".getSize [" + prx.toString() + "]" + hostId, e);	
		}
		return size;
	}

	/*
	 * @brief	AppFootprint没有人气(UserVisitCount)的设置
	 */
	@Deprecated	
	public int getUserVisitCount(int hostId) {		
		return 0;
	}

	
	/*
	 * @brief	AppFootprint没有人气(UserVisitCount)的设置
	 */
	@Deprecated
	public void setUserVisitCount(int hostId, int count) {		
	}

	public void visitHomepage(FootprintNewView fpt) {
		visitHomepage(fpt, false, "UNKNOWN", false);
	}

	public void visitHomepage(FootprintNewView fpt, boolean friendFlag) {
		visitHomepage(fpt, friendFlag, "UNKNOWN", false);
	}

	public void visitHomepage(FootprintNewView fpt, boolean friendFlag,	String ref) {
		visitHomepage(fpt, friendFlag, ref, false);
	}

	/* @brief Oneway踩脚印
	 * (non-Javadoc)
	 * @see com.xiaonei.xce.footprint.FootprintNewAdapter#visitHomepage(mop.hi.oce.domain.model.FootprintNewView, boolean, java.lang.String, boolean)
	 */
	public void visitHomepage(FootprintNewView fpt, boolean friendFlag,	String ref, boolean nocount) {
		
		Map<String, String> ctx = new HashMap<String, String>();
		ctx.put("friendstatus", "yes");
		ctx.put("ignorelimitstatus", "yes");
		ctx.put("REF", ref);		
		try {
			getFootprintManagerOneway(fpt.getHost()).visitHomepageOneway(fpt.getFootprint(), ctx);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".visitHomepageAndGet " + fpt.getHost() + " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".visitHomepageAndGet " + fpt.getHost(), e);		
		}
	}
	
	public FootprintResult visitHomepageAndGet(FootprintNewView fpt, boolean friendFlag, String ref) {
		return visitHomepageAndGet(fpt, friendFlag, ref, false);
	}
		
	public FootprintResult visitHomepageAndGet(FootprintNewView fpt, boolean friendFlag, String ref, boolean nocount) {
		
		Map<String, String> ctx = new HashMap<String, String>();
		ctx.put("friendstatus", "yes");
		ctx.put("ignorelimitstatus", "yes");
		ctx.put("REF", ref);		
		FootprintResult result = null;
		FootprintManagerPrx prx = null;
		try {
			prx = getFootprintManager(fpt.getHost());
			FootprintInfo info = prx.visitHomepage(fpt.getFootprint(), 0,
					9, ctx);
			result = new FootprintResult(fpt.getHost(), info.fpts, info.visitcount);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".visitHomepageAndGet [" + prx.toString() + "]" + fpt.getHost() + " Ice.TimeoutException");
			result = new FootprintResult(fpt.getHost(), new FootprintTiny[0], 0);
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".visitHomepageAndGet [" + prx.toString() + "]" + fpt.getHost(), e);
			result = new FootprintResult(fpt.getHost(), new FootprintTiny[0], 0);
		}
		return result;
	}

	public FootprintManagerPrx getFootprintManager(int host) {

		Ice.ObjectPrx prx0 = getValidProxy(host, 300);		
		if (prx0 != null) {
			FootprintManagerPrx prx = FootprintManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());			
			return prx;
		} else {
			return null;
		}
	}

	public FootprintManagerPrx getFootprintManagerOneway(int host) {

		Ice.ObjectPrx prx0 = getValidProxy(host, 300);
		if (prx0 != null) {
			FootprintManagerPrx prx = FootprintManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}

	public static void main(String[] args) {

		AppFootprintAdapterImpl test = new AppFootprintAdapterImpl("", 10);
		int hostId = 222525090;
		int guestId = 2002;
//		guestId = 80625915;

		FootprintNewView fpnv = new FootprintNewView();
		fpnv.setHost(hostId);
		fpnv.setGuest(guestId);
		test.visitHomepage(fpnv);
		System.out.println("--------------");

		FootprintResult result = test.getAll(hostId, 0, 6);//		
		System.out.println("\n[\n");
		for (int i = 0; i < result.getFootprintViews().size(); ++i) {
			System.out.println(result.getFootprintViews().get(i).getHost()
					+ ":" + result.getFootprintViews().get(i).getGuest() + ":"
					+ result.getFootprintViews().get(i).getTime());
		}
		System.out.println("]\n");

		System.exit(0);
	}
}
