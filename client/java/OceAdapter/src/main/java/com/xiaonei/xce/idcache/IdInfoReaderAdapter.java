package com.xiaonei.xce.idcache;

import idcache.IdCacheReaderPrx;
import idcache.IdCacheReaderPrxHelper;
import idcache.IdRelationInfo;
import xce.clusterstate.ReplicatedClusterAdapter;
import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;

import Ice.TimeoutException;

public final class IdInfoReaderAdapter extends
		ReplicatedClusterAdapter {

	private static final String ctr_endpoints = "ControllerIdCacheReader";

	private static final String service_name = "M";

	private static int _interval = 120;

	public IdInfoReaderAdapter() {
		super(ctr_endpoints, _interval);
	}

	protected IdCacheReaderPrx getIdCacheReaderPrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(Math.abs(userId), service_name, 300);
		if (prx0 != null) {
			IdCacheReaderPrx prx = IdCacheReaderPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}
	
	public IdRelationInfo getIdRelationInfo(int id){		
		TimeCost cost = TimeCost
				.begin("IdCacheReaderAdapter.getIdRelationInfo id: " + id);
		IdCacheReaderPrx prx = null;
		try {
			prx = getIdCacheReaderPrx(id);
			if (prx != null) {
				return prx.getIdRelationInfo(id);
			}
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getIdRelationInfo[ " + prx
							+ " ]id=" + id + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
		return null;
	}

	public int getRelatedUserId(int id) {
		TimeCost cost = TimeCost
				.begin("IdCacheReaderAdapter.getRelatedUserId id: " + id);
		IdCacheReaderPrx prx = null;
		try {
			prx = getIdCacheReaderPrx(id);
			if(prx != null)
			{
				return prx.getRelatedUserId(id);	
			}
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getRelatedUserId[ " + prx
							+ " ]id=" + id + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
		return 0;
	}
	public int getRelatedTransId(int id) {
		TimeCost cost = TimeCost
				.begin("IdCacheReaderAdapter.getRelatedTransId id: " + id);
		IdCacheReaderPrx prx = null;
		try {
			prx = getIdCacheReaderPrx(id);
			if(prx != null)
			{
				return prx.getRelatedTransId(id);	
			}
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getRelatedTransId[ " + prx
							+ " ]id=" + id + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
		return 0;
	}
	
	public boolean isVirtualId(int id) {
		TimeCost cost = TimeCost
				.begin("IdCacheReaderAdapter.isVirtualId id: " + id);
		IdCacheReaderPrx prx = null;
		try {
			prx = getIdCacheReaderPrx(id);
			if(prx != null)
			{
				return prx.isVirtualId(id);	
			}
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".isVirtualId[ " + prx
							+ " ]id=" + id + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
		return true;
	}

};
