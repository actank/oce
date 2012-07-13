package com.xiaonei.talk;

import java.util.ArrayList;
import java.util.List;
import java.util.Vector;

import talk.online.OnlineManagerPrx;
import talk.online.OnlineManagerPrxHelper;
import talk.online.UserOnlineType;
import talk.wevent.WEventManagerPrx;
import talk.wevent.WEventManagerPrxHelper;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

public class OnlineCenterAdapter extends ServiceAdapter {
	private static final String endpoints = "@OnlineCenter";
	private Vector<OnlineManagerPrx> managersOneway = new Vector<OnlineManagerPrx>();;

	private static OnlineCenterAdapter instance = new OnlineCenterAdapter();

	public static OnlineCenterAdapter instance() {
		return instance;
	}

	OnlineCenterAdapter() {
		super(endpoints, 10, Channel.newTalkChannel(endpoints));
//		managerOneway = locateProxy("M", Channel.Invoke.Oneway,
//				WEventManagerPrxHelper.class);
//		managerOneway = locateProxy("M", Channel.Invoke.Twoway,
//				WEventManagerPrxHelper.class);
	}
	
	public OnlineManagerPrx getManager(int id) {
		return locateProxy(managersOneway, "M", id, Channel.Invoke.Twoway,
				OnlineManagerPrxHelper.class, 300);
	}
	
	public List<UserOnlineType> getRandomOnlineUsers(int count){
		UserOnlineType[] onlineType = getManager((int)((System.currentTimeMillis()/1000) % 10)).getRandomOnlineUsers(count);
		List<UserOnlineType> list = new ArrayList<UserOnlineType>();
		for(int i = 0 ;i < onlineType.length; ++i){
			list.add(onlineType[i]);
		}
		return list;
	}

	public List<UserOnlineType> getBuddyOnlineTypeSeq(int userId) {
		List<UserOnlineType> list = new ArrayList<UserOnlineType>();
		UserOnlineType[] types =  getManager(userId%10).getBuddyOnlineTypeSeq(userId);
		for(int i = 0 ;i < types.length; ++i){
			list.add(types[i]);
		}
		return list;
		
	}
}
