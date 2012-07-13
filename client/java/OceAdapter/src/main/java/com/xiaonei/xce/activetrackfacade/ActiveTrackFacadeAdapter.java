package com.xiaonei.xce.activetrackfacade;

import xce.activetrack.ActiveTrackFacadePrx;
import xce.activetrack.ActiveTrackFacadePrxHelper;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

public final class ActiveTrackFacadeAdapter extends ServiceAdapter {
	private static ActiveTrackFacadeAdapter _instance = new ActiveTrackFacadeAdapter();

	public static ActiveTrackFacadeAdapter getInstance() {
		return _instance;
	}

	private ActiveTrackFacadeAdapter() {
		super(endpoints, 0, Channel.newActiveTrackChannel(endpoints));
	}
	
	private static final String endpoints = "@ActiveTrackFacade";
	
	public void access(int userId, String ip, String url){
		getActiveTrackFacadeUDP().access(userId, ip, url);
	}

	public void action(int userId, String[] paths, String action, String object) {
		getActiveTrackFacadeUDP().action(userId, paths, action, object);
	}

	protected ActiveTrackFacadePrx getActiveTrackFacadeUDP() {
		return locateProxy("ATF", Channel.Invoke.Udp, ActiveTrackFacadePrxHelper.class, 300);
	}
}
