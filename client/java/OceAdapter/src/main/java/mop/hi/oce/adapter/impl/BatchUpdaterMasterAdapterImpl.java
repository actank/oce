package mop.hi.oce.adapter.impl;

import Ice.ConnectTimeoutException;
import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;

import mop.hi.oce.adapter.MasterBatchUpdaterAdapter;
import mop.hi.svc.model.BatchUpdaterManagerPrx;
import mop.hi.svc.model.BatchUpdaterManagerPrxHelper;
import xce.clusterstate.ReplicatedClusterAdapter;

public class BatchUpdaterMasterAdapterImpl extends ReplicatedClusterAdapter implements
		MasterBatchUpdaterAdapter {
	
	private static final String ctr_endpoints = "ControllerBatchUpdater";
	private static int _interval = 120;	
	
//	private BatchUpdaterManagerPrx managerOneway;

	public BatchUpdaterMasterAdapterImpl() {
		super(ctr_endpoints,_interval);
		
//		Ice.ObjectPrx prx0 = getValidProxy(0);
		
//		managerOneway = BatchUpdaterManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());

	}
	
	private BatchUpdaterManagerPrx getBatchUpdaterManager() {
		
		Ice.ObjectPrx prx0 = getValidProxy(0, 300);
		if ( prx0 != null ) {
			BatchUpdaterManagerPrx prx = BatchUpdaterManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}

	public void incUserViewCount(int id) {	
		BatchUpdaterManagerPrx prx= null;
		try {
//			TimeCost cost = TimeCost.begin("incUserViewCount(int)" + id + "]");
			prx=getBatchUpdaterManager();
			prx.incUserViewCount(id, 1);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incUserViewCount(int) [ " + prx + " ]  Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incUserViewCount(int) [ " + prx + " ]  Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".incUserViewCount(int)", e);
		}
	}

	public void incUserViewCount(long id) {
		BatchUpdaterManagerPrx prx= null;
		try {
	//		TimeCost cost = TimeCost.begin("incUserViewCount(long)" + id + "]");
			prx=getBatchUpdaterManager();
			prx.incLong(new long[] { id }, 1, "USERVIEWCOUNTER");
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incUserViewCount(long) [ " + prx + " ]  Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incUserViewCount(long) [ " + prx + " ]  Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".incUserViewCount(long)", e);
		}
	}
	
//	public static void main(String[] args) {
//		BatchUpdaterMasterAdapterImpl test = new BatchUpdaterMasterAdapterImpl();
//		int idInt=700003485;
//		long idLong=700003485l;
//		test.incUserViewCount(idInt);
//		test.incUserViewCount(idLong);
//		System.exit(0);
//	}
}
