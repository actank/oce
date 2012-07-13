package com.xiaonei.talk;

import java.util.Vector;

import talk.wevent.WEventManagerPrx;
import talk.wevent.WEventManagerPrxHelper;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

public class WebEventAdapter extends ServiceAdapter {
	private static final String endpoints = "@WEvent";
	private Vector<WEventManagerPrx> managersOneway = new Vector<WEventManagerPrx>();;

	private static WebEventAdapter instance = new WebEventAdapter();

	public static WebEventAdapter instance() {
		return instance;
	}

	WebEventAdapter() {
		super(endpoints, 10, Channel.newTalkChannel(endpoints));
//		managerOneway = locateProxy("M", Channel.Invoke.Oneway,
//				WEventManagerPrxHelper.class);
//		managerOneway = locateProxy("M", Channel.Invoke.Twoway,
//				WEventManagerPrxHelper.class);
	}
	
	public WEventManagerPrx getManagerOneway(int id) {
		return locateProxy(managersOneway, "M", id, Channel.Invoke.Oneway,
				WEventManagerPrxHelper.class, 300);
	}
	
	public void notify(int userid, String title, String msg) {
		getManagerOneway(userid).deliverNotify(userid, title, msg);
	}
	
	public void deliverAppNotice(int toid,int appid,String appName, String appIcon, String msg){
		getManagerOneway(toid).deliverAppNotice(toid, appid, appName, appIcon, msg);
	}

}
