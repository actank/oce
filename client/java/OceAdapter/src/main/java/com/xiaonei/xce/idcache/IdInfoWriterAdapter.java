package com.xiaonei.xce.idcache;

import idcache.IdCacheWriterPrx;
import idcache.IdCacheWriterPrxHelper;
import xce.clusterstate.ReplicatedClusterAdapter;
import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;

import Ice.TimeoutException;

public final class IdInfoWriterAdapter extends
		ReplicatedClusterAdapter {

	private static final String ctr_endpoints = "ControllerIdCacheWriter";

	private static int _interval = 120;

	public IdInfoWriterAdapter() {
		super(ctr_endpoints, _interval);
	}

	protected IdCacheWriterPrx getIdCacheWriter10sPrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(Math.abs(userId), 10*1000);
		if (prx0 != null) {
			IdCacheWriterPrx prx = IdCacheWriterPrxHelper
					.uncheckedCast(prx0.ice_timeout(10*1000).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	public void createIdRelation(int id, int transId, int virtualId) {
		TimeCost cost = TimeCost
				.begin("IdCacheWriterAdapter.createIdRelation id: " + id + " trandId:" + transId + " virtualId:" + virtualId);
		IdCacheWriterPrx prx = null;
		try {
			prx = getIdCacheWriter10sPrx(id);
			if(prx != null)
			{
				prx.createIdRelation(id, transId, virtualId);	
			}
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".createIdRelation[ " + prx
							+ " ]id=" + id + "transId=" + transId + " virtualId=" + virtualId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}
	
	public void deleteIdRelation(int id, int virtualId) {
		TimeCost cost = TimeCost
				.begin("IdCacheWriterAdapter.deleteIdRelation id: " + id + " virtualId:" + virtualId);
		IdCacheWriterPrx prx = null;
		try {
			prx = getIdCacheWriter10sPrx(id);
			if(prx != null)
			{
				prx.deleteIdRelation(id, virtualId);	
			}
		} catch (TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".deleteIdRelation[ " + prx
							+ " ]id=" + id  + " virtualId=" + virtualId + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

};
