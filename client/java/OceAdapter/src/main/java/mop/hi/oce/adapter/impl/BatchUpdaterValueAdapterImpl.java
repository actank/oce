package mop.hi.oce.adapter.impl;

import Ice.ConnectTimeoutException;
import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;

import mop.hi.oce.adapter.BatchUpdaterValueAdapter;
import mop.hi.svc.model.BatchUpdaterManagerPrx;
import mop.hi.svc.model.BatchUpdaterManagerPrxHelper;
import xce.clusterstate.ReplicatedClusterAdapter;

public class BatchUpdaterValueAdapterImpl extends ReplicatedClusterAdapter implements
		BatchUpdaterValueAdapter {
	
	private static final String ctr_endpoints = "ControllerBatchUpdaterValue";
	private static int _interval = 120;	
	
//	private BatchUpdaterManagerPrx managerOneway;

	public BatchUpdaterValueAdapterImpl() {
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

	public void incTribePageViewCount(int tribeId) {
		BatchUpdaterManagerPrx prx=null;
		try {
	//		TimeCost cost = TimeCost.begin("incTribePageViewCount(int)" + tribeId + "]");
			prx=getBatchUpdaterManager();
			prx.incTribePageViewCount(tribeId, 1);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incTribePageViewCount [ " + prx + " ] Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incTribePageViewCount [ " + prx + " ] Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".incTribePageViewCount", e);
		}	
	}

	public void incFlyerUserAdCount(int userId, int adId) {
		BatchUpdaterManagerPrx prx=null;
		try {
	//		TimeCost cost = TimeCost.begin("incFlyerUserAdCount(int)" + userId + "]");
			prx=getBatchUpdaterManager();
			prx.incFlyerUserAdCount(userId, adId, 1);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incFlyerUserAdCount [ " + prx + " ] Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incFlyerUserAdCount [ " + prx + " ] Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".incFlyerUserAdCount", e);
		}
	}
	
//	public static void main(String[] args) {
//		BatchUpdaterValueAdapterImpl test = new BatchUpdaterValueAdapterImpl();
//		int tribeIdInt=258475070;
//		
//		long tribeIdLong=258475070l;	
//	
//		test.incTribePageViewCount(tribeIdInt);
//		test.incTribePageViewCount(tribeIdLong);
//		
//		System.exit(0);
//	}

	public void incFlyerUserAdCount(int userId, long adId) {
		BatchUpdaterManagerPrx prx=null;
		try {
	//		TimeCost cost = TimeCost.begin("incFlyerUserAdCount(long)" + userId + "]");
			prx=getBatchUpdaterManager();
			prx.incLong(new long[] {userId, adId }, 1, "FLYERUSERADCOUNTER");
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incFlyerUserAdCount [ " + prx + " ] Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incFlyerUserAdCount [ " + prx + " ] Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".incFlyerUserAdCount", e);
		}
	}

	public void incTribePageViewCount(long tribeId) {
		BatchUpdaterManagerPrx prx=null;
		try {
	//		TimeCost cost = TimeCost.begin("incTribePageViewCount(long)" + tribeId + "]");
			prx=getBatchUpdaterManager();
			prx.incLong(new long[] {tribeId }, 1, "TRIBEPAGEVIEWCOUNTER");
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incTribePageViewCount [ " + prx + " ] Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incTribePageViewCount [ " + prx + " ] Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".incTribePageViewCount", e);
		}		
	}
	
}
