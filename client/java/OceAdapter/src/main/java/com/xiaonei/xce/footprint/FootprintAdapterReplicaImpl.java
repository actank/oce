package com.xiaonei.xce.footprint;

import java.util.Date;
import java.util.HashMap;
import java.util.Map;

import com.xiaonei.xce.footprint.FootprintNewAdapter;
import mop.hi.oce.domain.model.FootprintNewView;
import mop.hi.oce.domain.model.FootprintResult;

import xce.fptn.Footprint;
import xce.fptn.FootprintInfo;
import xce.fptn.FootprintManagerPrx;
import xce.fptn.FootprintManagerPrxHelper;
import xce.fptn.FootprintFilterManagerPrx;
import xce.fptn.FootprintFilterManagerPrxHelper;
import xce.fptn.FootprintTiny;

import xce.clusterstate.ReplicatedClusterAdapter;
import xce.util.channel.Channel;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;

public class FootprintAdapterReplicaImpl extends ReplicatedClusterAdapter implements
		FootprintNewAdapter {

	private FootprintFilterManagerPrx filterManager = null;
	private FootprintFilterManagerPrx filterManagerOneway = null;
	private Channel channel;
	
	private static final String ctr_endpoints = "ControllerFootprint";
	private static int _interval = 120;
	private int _cluster = 10;

	public FootprintAdapterReplicaImpl(String endpoints, int cluster, boolean filter) {
		super(ctr_endpoints,_interval);
			
		channel = Channel.newOceChannel(ctr_endpoints);
		
		if (filter) {
			filterManager = channel.locate("FFM", "@FootprintFilterN",
					Channel.Invoke.Twoway,
					FootprintFilterManagerPrxHelper.class, 300);
			filterManagerOneway = channel.locate("FFM", "@FootprintFilterN",
					Channel.Invoke.Oneway,
					FootprintFilterManagerPrxHelper.class, 300);
		}
	}

	public FootprintResult getAll(int hostId) {		
			return getAll(hostId, 0, 6);		
	}

	public FootprintResult getAll(int hostId, int begin, int limit) {	
		
		FootprintResult result = null;
		FootprintManagerPrx prx = null;
		try {
			prx = getFootprintManager(hostId);
			// TimeCost cost = TimeCost.begin("FootprintAdapterReplicaImpl.getAll id: " + hostId + " begin: " + begin + " limit : " + limit);
			FootprintInfo info = prx.getAll(hostId, begin, limit);
			result = new FootprintResult(hostId, info.fpts, info.visitcount);
			// cost.endFinally();	
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".getAll [" + prx.toString() + "]"  + hostId + " Ice.TimeoutException");
			result = new FootprintResult(hostId, new xce.fptn.FootprintTiny[0], 0);
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".getAll [" + prx.toString() + "]"  + hostId, e);
			result = new FootprintResult(hostId, new xce.fptn.FootprintTiny[0], 0);			
		}
		
		return result;
	}

	public int getSize(int hostId){
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
	
	public int getUserVisitCount(int hostId) {
		FootprintManagerPrx prx = null;
		try {
			prx = getFootprintManager(hostId);
			return prx.getUserVisitCount(hostId);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".getUserVisitCount [" + prx.toString() + "]" + hostId + " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".getUserVisitCount [" + prx.toString() + "]" + hostId, e);
		}
		return 0;
	}	
	
	public void setUserVisitCount(int hostId, int count) {
		FootprintManagerPrx prx = null; 
		try {
			prx = getFootprintManager(hostId);
			prx.setUserVisitCount(hostId, count);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".setUserVisitCount [" + prx.toString() + "]" + hostId + " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".setUserVisitCount [" + prx.toString() + "]" + hostId, e);
		}
	}

	public void visitHomepage(FootprintNewView fpt) {
		visitHomepage(fpt, false, "UNKNOWN", false, false);
	}
	
	public void visitHomepage(FootprintNewView fpt, boolean friendFlag) {
		visitHomepage(fpt, friendFlag, "UNKNOWN", false, false);
	}

	public void visitHomepage(FootprintNewView fpt, boolean friendFlag, String ref) {
		visitHomepage(fpt, friendFlag, ref, false, false);
	}	
	
	// 无返回值的就改为Oneway调用
	public void visitHomepage(FootprintNewView fpt, boolean friendFlag, String ref, boolean nocount) {
		visitHomepage(fpt, friendFlag, ref, nocount, false);
	}

	// 无返回值的就改为Oneway调用
	public void visitHomepage(FootprintNewView fpt, boolean friendFlag, String ref, boolean nocount, boolean ignoreLimitFlag) {
		
		Map<String, String> ctx = new HashMap<String, String>();		
		if (friendFlag) {
			ctx.put("friendstatus", "yes");
		} else {
			ctx.put("friendstatus", "no");
		}
		if (ignoreLimitFlag) {
			ctx.put("ignorelimitstatus", "yes");
		} else {
			ctx.put("ignorelimitstatus", "no");
		}
		if (nocount) {
			ctx.put("NOCOUNT", "YES");
		}
		ctx.put("REF", ref);
		try {			
			if (filterManagerOneway != null) {			
				filterManagerOneway.visitHomepageOneway(fpt.getFootprint(), ctx);							
			} else {				
				getFootprintManagerOneway(fpt.getHost()).visitHomepageOneway(fpt.getFootprint());				
			}		
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".visitHomepage " + fpt.getHost() + " Ice.TimeoutException");
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".visitHomepage " + fpt.getHost(), e);			
		}
	}
	
	public FootprintResult visitHomepageAndGet(FootprintNewView fpt, boolean friendFlag, String ref) {
		return visitHomepageAndGet(fpt, friendFlag, ref, false, false);
	}
	
	public FootprintResult visitHomepageAndGet(FootprintNewView fpt, boolean friendFlag, String ref, boolean nocount) {
		return visitHomepageAndGet(fpt, friendFlag, ref, nocount, false);
	}
		
	public FootprintResult visitHomepageAndGet(FootprintNewView fpt, boolean friendFlag, String ref, boolean nocount, boolean ignoreLimitFlag) {	
		Map<String, String> ctx = new HashMap<String, String>();		
		
		if (friendFlag) {
			ctx.put("friendstatus", "yes");
		} else {
			ctx.put("friendstatus", "no");
		}
		if (ignoreLimitFlag) {
			ctx.put("ignorelimitstatus", "yes");
		} else {
			ctx.put("ignorelimitstatus", "no");
		}
		if (nocount) {
			ctx.put("NOCOUNT", "YES");
		}
		ctx.put("REF", ref);

		try {
			FootprintInfo info;
			if (filterManager != null) {			
				// TimeCost cost = TimeCost.begin("FootprintAdapterReplicaImpl.VisitHomepage by filter. host: "	+ fpt.getHost() + " guest : " + fpt.getGuest());			
				info = filterManager.visitHomepage(fpt.getFootprint());			
				// cost.endFinally();			
			} else {				
				// TimeCost cost = TimeCost.begin("FootprintAdapterReplicaImpl.VisitHomepage by footprint direct. host: " + fpt.getHost() + " guest : " + fpt.getGuest());
				info = getFootprintManager(fpt.getHost()).visitHomepage(fpt.getFootprint(), 0, 9, ctx);
				// cost.endFinally();
			}
			FootprintResult result = new FootprintResult(fpt.getHost(), info.fpts, info.visitcount);
			return result;
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".visitHomepageAndGet " + fpt.getHost() + " Ice.TimeoutException");			
			FootprintResult result = new FootprintResult(fpt.getHost(), new FootprintTiny[0], 0);
			return result;
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".visitHomepageAndGet " + fpt.getHost(), e);			
			FootprintResult result = new FootprintResult(fpt.getHost(), new FootprintTiny[0], 0);
			return result;
		}
	}

	public FootprintManagerPrx getFootprintManager(int host) {
		
		Ice.ObjectPrx prx0 = getValidProxy(host, 300);
		if ( prx0 != null ) {
			FootprintManagerPrx prx = FootprintManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	public FootprintManagerPrx getFootprintManagerOneway(int host) {

		Ice.ObjectPrx prx0 = getValidProxy(host, 300);
		if ( prx0 != null ) {
			FootprintManagerPrx prx = FootprintManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}	
	}
	
	/* public void reloadCache(int host) {public FootprintResult visitHomepageAndGet(FootprintNewView fpt, boolean friendflag, String ref);
		int mod = host % _cluster;
		int category = 0;
		ObjectCachePrx objPrx = channel.locate("SC", "@FootprintN"+mod,
				Channel.Invoke.Twoway,
				ObjectCachePrxHelper.class, 300);
		objPrx.reloadObject(category, host);
	} */
	
	public static void main(String[] args) {
		
		FootprintAdapterReplicaImpl test = new FootprintAdapterReplicaImpl("", 10, true);		
		int hostId = 222525090;
		// hostId = 222790535;
		// hostId = 229194345;	
		// hostId = 245265173;		// 人气暴涨
		// hostId = 238557753;		// 人气不涨
		// hostId = 302160619;		// 人气不涨
		// hostId = 260852116;	// ly
		hostId = 222677625;
		hostId = 25;
		// hostId = 125;
		int guestId = 24496255;	// yx
		guestId = 222525098;
		guestId = 257234596;	// 特别好友
		guestId = 10;	// 特别好友
		
		// 设置人气(慎用！！)
		
		FootprintResult result = null;
		//   踩脚印		
		System.out.println("VisitCount of " + hostId + " : " + test.getUserVisitCount(hostId));	
		//  莫文蔚, 蔡健雅, SHE, 游鸿明, 周迅, 张信哲, 光良, 陈奕迅, 李宇春 
		// int[] guests = new int[]{600002664, 600002252, 600002300, 600002216, 600002221, 600002349, 600002233, 600002342, 600002235};
		// int[] guests = new int[]{};		
		int[] guests = new int[]{guestId};
		for ( int i = 0; i < guests.length; ++i) {
			FootprintNewView fpnv=new FootprintNewView();
			fpnv.setHost(hostId);
			fpnv.setGuest(guests[i]);
			// test.visitHomepage(fpnv, false, "UNKNOWN");						
			// result = test.visitHomepageAndGet(fpnv, false, "UNKNOWN");
		} 
		System.out.println("VisitCount of " + hostId + " : " + test.getUserVisitCount(hostId)); 
		
		// 取脚印
		result = test.getAll(hostId, 0, 10);
		System.out.println("Visit Count of " + hostId + " is :" + result.getVisitCount());
		System.out.println(test.getSize(hostId) + " footprint(s)\n[");		
		for ( int i = 0; i < result.getFootprintViews().size(); ++i) {
			System.out.println(result.getFootprintViews().get(i).getHost() + ":"
					+ result.getFootprintViews().get(i).getGuest() + ":"
					+ result.getFootprintViews().get(i).getTime()	);
		} 
		System.out.println("]\n");
		
		System.exit(0); 
	}
}
