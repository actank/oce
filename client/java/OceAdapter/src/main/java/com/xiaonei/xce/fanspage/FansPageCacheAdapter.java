package com.xiaonei.xce.fanspage;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.XceStat;

import xce.buddy.FansPageCachePrx;
import xce.buddy.FansPageCachePrxHelper;
import xce.buddy.PageEntry;
import xce.clusterstate.ReplicatedClusterAdapter;

public class FansPageCacheAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerFansPageCache";
	private static int _interval = 120;	
	private static FansPageCacheAdapter instance = new FansPageCacheAdapter();

	public static FansPageCacheAdapter instance() {
		if(instance == null) {
			instance = new FansPageCacheAdapter();
		}
		return instance;
	}
	public FansPageCacheAdapter() {
		super(ctr_endpoints,_interval);
	}
	
	public PageEntry[] getPagesOfUser(int userId, int[] pageIds) {
		FansPageCachePrx prx = null;
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FansPageCache " + userId);
		try {
			prx = getFansPageCacheManagerTwoway(userId);
			prx.getPagesOfUser(userId, pageIds);
		} catch (Ice.ConnectTimeoutException e) {
            Oce.getLogger().error( this.getClass().getName() + ".getPagesOfUser prx = " + prx + " userId = " + userId + " Ice.ConnectTimeoutException ");
            stat.endException(e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error( this.getClass().getName() + ".getPagesOfUser prx = " + prx + " userId = " + userId + " Ice.TimeoutException ");
			stat.endException(e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(this.getClass().getName() + ".getPagesOfUser prx = " + prx + " userId = " + userId, e);
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return new PageEntry[0];
	}
	
	public int getNormalPageCount(int userId) {
		FansPageCachePrx prx = null;
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FansPageCache " + userId);
		try {
			prx = getFansPageCacheManagerTwoway(userId);
			return prx.getNormalPageCount(userId);
		} catch (Ice.ConnectTimeoutException e) {
            Oce.getLogger().error( this.getClass().getName() + ".getNormalPageCount prx = " + prx + " userId = " + userId + " Ice.ConnectTimeoutException ");
            stat.endException(e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error( this.getClass().getName() + ".getNormalPageCount prx = " + prx + " userId = " + userId + " Ice.TimeoutException ");
			stat.endException(e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(this.getClass().getName() + ".getNormalPageCount prx = " + prx + " userId = " + userId, e);
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return 0;
	}
	
	public int[] getAllUserPages(int userId) {
		FansPageCachePrx prx = null;
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FansPageCache " + userId);
		try {
			prx = getFansPageCacheManagerTwoway(userId);
		    return prx.getAllUserPages(userId);
		} catch (Ice.ConnectTimeoutException e) {
            Oce.getLogger().error( this.getClass().getName() + ".getAllUserPages prx = " + prx + " userId = " + userId + " Ice.ConnectTimeoutException ");
            stat.endException(e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error( this.getClass().getName() + ".getAllUserPages prx = " + prx + " userId = " + userId + " Ice.TimeoutException ");
			stat.endException(e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(this.getClass().getName() + ".getAllUserPages prx = " + prx + " userId = " + userId, e);
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return new int[0];
	}
	
	public PageEntry[] getLimitPages(int userId, int id, int size) {
		FansPageCachePrx prx = null;
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FansPageCache " + userId);
		try {
			prx = getFansPageCacheManagerTwoway(userId);
			return prx.getLimitPages(userId, id, size);
		} catch (Ice.ConnectTimeoutException e) {
            Oce.getLogger().error( this.getClass().getName() + ".getLimitPages prx = " + prx + " userId = " + userId + " Ice.ConnectTimeoutException ");
            stat.endException(e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error( this.getClass().getName() + ".getLimitPages prx = " + prx + " userId = " + userId + " Ice.TimeoutException ");
			stat.endException(e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(this.getClass().getName() + ".getLimitPages prx = " + prx + " userId = " + userId, e);
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return new PageEntry[0];
	}
	public PageEntry[] getPagesByUid(int userId) {
		FansPageCachePrx prx = null;
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FansPageCache " + userId);
		try {
			prx = getFansPageCacheManagerTwoway(userId);
			return prx.getPagesByUid(userId);
		} catch (Ice.ConnectTimeoutException e) {
            Oce.getLogger().error( this.getClass().getName() + ".getPagesByUid prx = " + prx + " userId = " + userId + " Ice.ConnectTimeoutException ");
            stat.endException(e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error( this.getClass().getName() + ".getPagesByUid prx = " + prx + " userId = " + userId + " Ice.TimeoutException ");
			stat.endException(e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(this.getClass().getName() + ".getPagesByUid prx = " + prx + " userId = " + userId, e);
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return new PageEntry[0];
	}
	
	public PageEntry[] getPagesByUidState(int userId, short state) {
		FansPageCachePrx prx = null;
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FansPageCache " + userId);
		try {
			prx = getFansPageCacheManagerTwoway(userId);
			return prx.getPagesByUidState(userId, state);
		} catch (Ice.ConnectTimeoutException e) {
            Oce.getLogger().error( this.getClass().getName() + ".getPagesByUidState prx = " + prx + " userId = " + userId + " Ice.ConnectTimeoutException ");
            stat.endException(e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error( this.getClass().getName() + ".getPagesByUidState prx = " + prx + " userId = " + userId + " Ice.TimeoutException ");
			stat.endException(e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(this.getClass().getName() + ".getPagesByUidState prx = " + prx + " userId = " + userId, e);
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return new PageEntry[0];
	}
	
	public int getPagesCountByState(int userId, short state) {
		FansPageCachePrx prx = null;
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FansPageCache " + userId);
		try {
			prx = getFansPageCacheManagerTwoway(userId);
			return prx.getPagesCountByState(userId, state);
		} catch (Ice.ConnectTimeoutException e) {
            Oce.getLogger().error( this.getClass().getName() + ".getPagesCountByState prx = " + prx + " userId = " + userId + " Ice.ConnectTimeoutException ");
            stat.endException(e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error( this.getClass().getName() + ".getPagesCountByState prx = " + prx + " userId = " + userId + " Ice.TimeoutException ");
			stat.endException(e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(this.getClass().getName() + ".getPagesCountByState prx = " + prx + " userId = " + userId, e);
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
		return 0;
	}
	
	protected FansPageCachePrx getFansPageCacheManagerOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			FansPageCachePrx prx = FansPageCachePrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}
	
	protected FansPageCachePrx getFansPageCacheManagerTwoway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			FansPageCachePrx prx = FansPageCachePrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}
}
