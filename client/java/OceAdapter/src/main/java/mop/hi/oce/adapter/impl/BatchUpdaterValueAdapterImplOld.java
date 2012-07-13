package mop.hi.oce.adapter.impl;

import mop.hi.oce.adapter.BatchUpdaterValueAdapter;
import mop.hi.svc.model.BatchUpdaterManagerPrx;
import mop.hi.svc.model.BatchUpdaterManagerPrxHelper;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

public class BatchUpdaterValueAdapterImplOld extends ServiceAdapter implements
		BatchUpdaterValueAdapter {
	private static final String endpoints = "@BatchUpdaterValue";
	private BatchUpdaterManagerPrx managerOneway;

	public BatchUpdaterValueAdapterImplOld() {
		super(endpoints, 0, Channel.newOceChannel(endpoints));
		managerOneway = locateProxy("M", Channel.Invoke.Oneway,
				BatchUpdaterManagerPrxHelper.class, 300);
	}

	public void incTribePageViewCount(int tribeId) {
		managerOneway.incTribePageViewCount(tribeId, 1);
	}

	public void incFlyerUserAdCount(int userId, int adId) {
		managerOneway.incFlyerUserAdCount(userId, adId, 1);

	}
	
	public void incTribePageViewCount(long tribeId) {
		managerOneway.incLong(new long[] {tribeId }, 1, "TRIBEPAGEVIEWCOUNTER");
		
	}
	
	public void incFlyerUserAdCount(int userId, long adId) {
		managerOneway.incLong(new long[] {userId, adId }, 1, "FLYERUSERADCOUNTER");
		
	}
	
	public static void main(String[] args) {
		BatchUpdaterValueAdapterImplOld test = new BatchUpdaterValueAdapterImplOld();
		
		int tribeIdInt=258475070;
	
		long tribeIdLong=258475070l;	
	
		test.incTribePageViewCount(tribeIdInt);
		test.incTribePageViewCount(tribeIdLong);
		
//		test.incFlyerUserAdCount(userId, adId);
//		test.incFlyerUserAdCount(userId, adId);
		
		System.exit(0);
	}
}
