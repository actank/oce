package mop.hi.oce.adapter.impl;

import mop.hi.oce.adapter.MasterBatchUpdaterAdapter;
import mop.hi.svc.model.BatchUpdaterManagerPrx;
import mop.hi.svc.model.BatchUpdaterManagerPrxHelper;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

public class BatchUpdaterMasterAdapterImplOld extends ServiceAdapter implements
		MasterBatchUpdaterAdapter {
	private static final String endpoints = "@BatchUpdater";
	private BatchUpdaterManagerPrx managerOneway;

	public BatchUpdaterMasterAdapterImplOld() {
		super(endpoints, 0, Channel.newOceChannel(endpoints));
		managerOneway = locateProxy("M", Channel.Invoke.Oneway,
				BatchUpdaterManagerPrxHelper.class, 300);
	}

	public void incUserViewCount(int id) {
		managerOneway.incUserViewCount(id, 1);

	}

	public void incUserViewCount(long id) {
		managerOneway.incLong(new long[] { id }, 1, "USERVIEWCOUNTER");
	}
	
	public static void main(String[] args) {
		BatchUpdaterMasterAdapterImplOld test = new BatchUpdaterMasterAdapterImplOld();
		
		int idInt=700007661;
	
		long idLong=700007661l;
	
		test.incUserViewCount(idInt);
		test.incUserViewCount(idLong);
		
		System.exit(0);
	}
}
