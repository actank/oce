package mop.hi.oce.adapter.impl;

import mop.hi.oce.adapter.BatchUpdaterLeavewordAdapter;
import mop.hi.svc.model.BatchUpdaterManagerPrx;
import mop.hi.svc.model.BatchUpdaterManagerPrxHelper;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

public class BatchUpdaterLeavewordAdapterImplOld extends ServiceAdapter implements
		BatchUpdaterLeavewordAdapter {
	private static final String endpoints = "@BatchUpdater";
	private BatchUpdaterManagerPrx managerOneway;

	public BatchUpdaterLeavewordAdapterImplOld() {
		super(endpoints, 0, Channel.newOceChannel(endpoints));
		managerOneway = locateProxy("M", Channel.Invoke.Oneway,
				BatchUpdaterManagerPrxHelper.class, 300);
	}

	public void incLeavewordCount(int id) {
		managerOneway.incLeavewordCount(id, 1);
	}

	public void incLeavewordCount(long id) {
		managerOneway.incLong(new long[] { id }, 1, "LEAVEWORDNAME");
	}
	
	public static void main(String[] args) {
		BatchUpdaterLeavewordAdapterImplOld test = new BatchUpdaterLeavewordAdapterImplOld();
		
		int idInt=700068280;
	
		long idLong=700068280l;
	
		test.incLeavewordCount(idInt);
		
		test.incLeavewordCount(idLong);
		
		System.exit(0);
	}
}
