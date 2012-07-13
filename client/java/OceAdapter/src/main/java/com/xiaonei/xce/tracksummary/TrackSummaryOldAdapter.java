package com.xiaonei.xce.tracksummary;

import java.util.Map;
import java.util.Vector;

import xce.activetrack.TrackSummaryPrx;
import xce.activetrack.TrackSummaryPrxHelper;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

public final class TrackSummaryOldAdapter extends ServiceAdapter {
	private static final String endpoints = "@TrackSummary";
	private static TrackSummaryOldAdapter _instance = new TrackSummaryOldAdapter();

	private TrackSummaryOldAdapter() {
		super(endpoints, 100, Channel.newActiveTrackChannel(endpoints));
	}
	
	public static TrackSummaryOldAdapter getInstance(){
		return _instance;
	}
	
	public Map<String, String> get(int userId){
		return getTrackSummary(userId).get(userId);
	}

	private Vector<TrackSummaryPrx> managers = new Vector<TrackSummaryPrx>();
	private Vector<TrackSummaryPrx> managersOneway = new Vector<TrackSummaryPrx>();

	protected TrackSummaryPrx getTrackSummary(int userId) {
		return locateProxy(managers, "TS", userId, Channel.Invoke.Twoway,
				TrackSummaryPrxHelper.class, 300);
	}

	protected TrackSummaryPrx getTrackSummaryOneway(int userId) {
		return locateProxy(managersOneway, "TS", userId, Channel.Invoke.Oneway,
				TrackSummaryPrxHelper.class, 300);
	}
	
	public static void main(String[] args) {
		int userId = 701306920;		
		System.out.println(TrackSummaryOldAdapter.getInstance().get(userId));
		System.exit(0);
	}
}
