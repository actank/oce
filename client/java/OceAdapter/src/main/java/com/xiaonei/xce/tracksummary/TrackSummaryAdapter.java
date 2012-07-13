package com.xiaonei.xce.tracksummary;

import java.util.Map;
import xce.activetrack.TrackSummaryPrx;
import xce.activetrack.TrackSummaryPrxHelper;

import xce.clusterstate.ReplicatedClusterAdapter;

public final class TrackSummaryAdapter extends ReplicatedClusterAdapter {	
	
	private static final String ctr_endpoints = "ControllerTrackSummary";
	private static final int _interval = 120;
	private static TrackSummaryAdapter _instance = new TrackSummaryAdapter();
	

	private TrackSummaryAdapter() {
		super(ctr_endpoints, _interval);						
	}
	
	public static TrackSummaryAdapter getInstance(){
		return _instance;
	}
	
	public Map<String, String> get(int userId){
		return getTrackSummary(userId).get(userId);
	}

	protected TrackSummaryPrx getTrackSummary(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			TrackSummaryPrx prx = TrackSummaryPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());			
			return prx;
		} else {
			return null;
		}		
	}

	protected TrackSummaryPrx getTrackSummaryOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			TrackSummaryPrx prx = TrackSummaryPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}		
	}
	
	public static void main(String[] args) {
		int userId = 701306920;
		userId = 718143563;
		System.out.println(TrackSummaryAdapter.getInstance().get(userId));
		System.exit(0);
	}
}
