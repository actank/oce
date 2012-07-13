package mop.hi.oce.adapter.impl;

import Ice.ConnectTimeoutException;
import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;

import mop.hi.oce.adapter.BatchUpdaterLeavewordAdapter;
import mop.hi.svc.model.BatchUpdaterManagerPrx;
import mop.hi.svc.model.BatchUpdaterManagerPrxHelper;
import xce.clusterstate.ReplicatedClusterAdapter;

public class BatchUpdaterLeavewordAdapterImpl extends ReplicatedClusterAdapter
		implements BatchUpdaterLeavewordAdapter {

	private static final String ctr_endpoints = "ControllerBatchUpdater";
	private static int _interval = 120;

	// private BatchUpdaterManagerPrx managerOneway;

	public BatchUpdaterLeavewordAdapterImpl() {
		super(ctr_endpoints, _interval);

		// Ice.ObjectPrx prx0 = getValidProxy(0);

		// managerOneway =
		// BatchUpdaterManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
	}

	private BatchUpdaterManagerPrx getBatchUpdaterManager() {

		Ice.ObjectPrx prx0 = getValidProxy(0, 300);
		if (prx0 != null) {
			BatchUpdaterManagerPrx prx = BatchUpdaterManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}

	public void incLeavewordCount(int id) {
		try {
//			TimeCost cost = TimeCost.begin("incLeavewordCount(int)" + id + "]");
			getBatchUpdaterManager().incLeavewordCount(id, 1);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incLeavewordCount(int)" + " Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incLeavewordCount(int)" + " Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".incLeavewordCount(int)", e);
		}
	}

	public void incLeavewordCount(long id) {
		try {
	//		TimeCost cost = TimeCost.begin("incLeavewordCount(long)" + id + "]");
			getBatchUpdaterManager().incLong(new long[] { id }, 1,
					"LEAVEWORDNAME");
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incLeavewordCount(long)" + " Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incLeavewordCount(long)" + " Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".incLeavewordCount(long)", e);
		}

	}

	// public static void main(String[] args) {
	// BatchUpdaterLeavewordAdapterImpl test = new
	// BatchUpdaterLeavewordAdapterImpl();
	// int idInt=700012608;
	// long idLong=700012608l;
	// test.incLeavewordCount(idInt);
	// test.incLeavewordCount(idLong);
	// System.exit(0);
	// }

}
