package com.xiaonei.xce.fanspage;

import java.util.ArrayList;
import java.util.Vector;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.log.XceStat;

import xce.buddy.FansPageReloaderPrx;
import xce.buddy.FansPageReloaderPrxHelper;
import xce.buddy.PageEntry;
import xce.clusterstate.ReplicatedClusterAdapter;

public class FansPageReloaderAdapter extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerFansPageReloader";
	private static int _interval = 120;	
	private static FansPageReloaderAdapter instance = new FansPageReloaderAdapter();

	public static FansPageReloaderAdapter instance() {
		if(instance == null) {
			instance = new FansPageReloaderAdapter();
		}
		return instance;
	}
	
	public FansPageReloaderAdapter() {
		super(ctr_endpoints,_interval);
	}
	
	public void updateState(int pageId, int userId, short state) {
		FansPageReloaderPrx prx = null;
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FansPageReloader " + userId);
		try {
			prx = getFansPageReloaderManagerOneway(userId);
			prx.updateState(pageId, userId, state);
		} catch (Ice.ConnectTimeoutException e) {
            Oce.getLogger().error( this.getClass().getName() + ".updateState prx = " + prx + " userId = " + userId + " Ice.ConnectTimeoutException ");
            stat.endException(e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error( this.getClass().getName() + ".updateState prx = " + prx + " userId = " + userId + " Ice.TimeoutException ");
			stat.endException(e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(this.getClass().getName() + ".updateState prx = " + prx + " userId = " + userId, e);
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
	}
	
	public void insert(PageEntry page) {
		int userId = page.userId;
		FansPageReloaderPrx prx = null;
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FansPageReloader " + userId);
		try {
			prx = getFansPageReloaderManagerOneway(userId);
			prx.insert(page);
		} catch (Ice.ConnectTimeoutException e) {
            Oce.getLogger().error( this.getClass().getName() + ".insert prx = " + prx + " userId = " + userId + " Ice.ConnectTimeoutException ");
            stat.endException(e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error( this.getClass().getName() + ".insert prx = " + prx + " userId = " + userId + " Ice.TimeoutException ");
			stat.endException(e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(this.getClass().getName() + ".insert prx = " + prx + " userId = " + userId, e);
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
	}
	
	public void insertBatch(PageEntry[] list) {
		FansPageReloaderPrx prx = null;
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FansPageReloader");
		try {
			prx = getFansPageReloaderManagerOneway(0);
			prx.insertBatch(list);
		} catch (Ice.ConnectTimeoutException e) {
            Oce.getLogger().error( this.getClass().getName() + ".insertBatch prx = " + prx + " Ice.ConnectTimeoutException ");
            stat.endException(e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error( this.getClass().getName() + ".insertBatch prx = " + prx + " Ice.TimeoutException ");
			stat.endException(e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(this.getClass().getName() + ".insertBatch prx = " + prx, e);
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
	}
	
	public void deleteByUidPid(int pageId, int userId) {
		FansPageReloaderPrx prx = null;
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FansPageReloader");
		try {
			prx = getFansPageReloaderManagerOneway(userId);
			prx.deleteByUidPid(pageId, userId);
		} catch (Ice.ConnectTimeoutException e) {
            Oce.getLogger().error( this.getClass().getName() + ".deleteByUidPid prx = " + prx + " Ice.ConnectTimeoutException ");
            stat.endException(e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error( this.getClass().getName() + ".deleteByUidPid prx = " + prx + " Ice.TimeoutException ");
			stat.endException(e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(this.getClass().getName() + ".deleteByUidPid prx = " + prx, e);
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
	}
	
	public void deleteByUid(int userId) {
		FansPageReloaderPrx prx = null;
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FansPageReloader");
		try {
			prx = getFansPageReloaderManagerOneway(userId);
			prx.deleteByUid(userId);
		} catch (Ice.ConnectTimeoutException e) {
            Oce.getLogger().error( this.getClass().getName() + ".deleteByUid prx = " + prx + " Ice.ConnectTimeoutException ");
            stat.endException(e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error( this.getClass().getName() + ".deleteByUid prx = " + prx + " Ice.TimeoutException ");
			stat.endException(e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(this.getClass().getName() + ".deleteByUid prx = " + prx, e);
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
	}
	
	public void loadFansPageByUid(int userId) {
		FansPageReloaderPrx prx = null;
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FansPageReloader");
		try {
			prx = getFansPageReloaderManagerOneway(userId);
			prx.loadFansPageByUid(userId);
		} catch (Ice.ConnectTimeoutException e) {
            Oce.getLogger().error( this.getClass().getName() + ".loadFansPageByUid prx = " + prx + " Ice.ConnectTimeoutException ");
            stat.endException(e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error( this.getClass().getName() + ".loadFansPageByUid prx = " + prx + " Ice.TimeoutException ");
			stat.endException(e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(this.getClass().getName() + ".loadFansPageByUid prx = " + prx, e);
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
	}
	
	public void loadFansPageByUidPid(int pageId, int userId) {
		FansPageReloaderPrx prx = null;
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FansPageReloader");
		try {
			prx = getFansPageReloaderManagerOneway(userId);
			prx.loadFansPageByUidPid(pageId, userId);
		} catch (Ice.ConnectTimeoutException e) {
            Oce.getLogger().error( this.getClass().getName() + ".loadFansPageByUidPid prx = " + prx + " Ice.ConnectTimeoutException ");
            stat.endException(e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error( this.getClass().getName() + ".loadFansPageByUidPid prx = " + prx + " Ice.TimeoutException ");
			stat.endException(e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(this.getClass().getName() + ".loadFansPageByUidPid prx = " + prx, e);
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
	}
	
	public void loadFansPageByUidState(int userId, short state) {
		FansPageReloaderPrx prx = null;
		TimeCost stat = TimeCost.begin(XceStat.getLogger(), "FansPageReloader");
		try {
			prx = getFansPageReloaderManagerOneway(userId);
			prx.loadFansPageByUidState(userId, state);
		} catch (Ice.ConnectTimeoutException e) {
            Oce.getLogger().error( this.getClass().getName() + ".loadFansPageByUidState prx = " + prx + " Ice.ConnectTimeoutException ");
            stat.endException(e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error( this.getClass().getName() + ".loadFansPageByUidState prx = " + prx + " Ice.TimeoutException ");
			stat.endException(e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(this.getClass().getName() + ".loadFansPageByUidState prx = " + prx, e);
			stat.endException(e);
		} finally {
			stat.endFinally();
		}
	}
	
	protected FansPageReloaderPrx getFansPageReloaderManagerOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			FansPageReloaderPrx prx = FansPageReloaderPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}
	
	protected FansPageReloaderPrx getFansPageReloaderManagerTwoway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			FansPageReloaderPrx prx = FansPageReloaderPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}
}
