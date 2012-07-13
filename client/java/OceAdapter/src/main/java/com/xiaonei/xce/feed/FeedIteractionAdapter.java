package com.xiaonei.xce.feed;

import java.util.Timer;

import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;
import xce.feed.FeedInteractionPrx;
import xce.feed.FeedInteractionPrxHelper;

public class FeedIteractionAdapter extends ServiceAdapter {
	private static FeedIteractionAdapter _instance = new FeedIteractionAdapter();
	private FeedInteractionPrx _prx = null;
	private static final String _endpoints = "@FeedInteraction";
	private Timer _timer = new Timer();

	public static FeedIteractionAdapter getInstance() {
		return _instance;
	}

	private FeedIteractionAdapter() {
		super(_endpoints, 0, Channel.newFeedChannel(_endpoints));
		_timer.schedule(new RelocateTimer(), 1000, 5000);
	}

	protected FeedInteractionPrx getManagerUdp() {
		if (_prx != null) {
			return _prx;
		} else {
			_prx = locateProxy("M", Channel.Invoke.Udp,
					FeedInteractionPrxHelper.class, 300);
			return _prx;
		}
	}

	protected void relocateUdpProxy() {
		try {
			_prx = locateProxy("M", Channel.Invoke.Udp,
					FeedInteractionPrxHelper.class, 300);
		} catch (Exception e) {
			System.err.print("relocate udp proxy err");
			e.printStackTrace();
		}
	}

	public void clickLog(String log){
		getManagerUdp().IncHitCount(log);
	}
	
	
	class RelocateTimer extends java.util.TimerTask {
		@Override
		public void run() {
			// TODO Auto-generated method stub
//			System.out.println("RelocateTimer");
			FeedIteractionAdapter.getInstance().relocateUdpProxy();
		}

	}
}