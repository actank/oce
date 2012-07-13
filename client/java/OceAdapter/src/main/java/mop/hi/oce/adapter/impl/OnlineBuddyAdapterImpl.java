package mop.hi.oce.adapter.impl;

import java.util.Vector;

import mop.hi.oce.adapter.OnlineBuddyAdapter;
import talk.online.OnlineManagerPrx;
import talk.online.OnlineManagerPrxHelper;
import talk.online.UserOnlineType;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;


public class OnlineBuddyAdapterImpl extends ServiceAdapter implements
		OnlineBuddyAdapter {
	private static final String endpoints = "@OnlineCenter";
	private Vector<OnlineManagerPrx> managers = new Vector<OnlineManagerPrx>();

	private Vector<OnlineManagerPrx> managersOneway = new Vector<OnlineManagerPrx>();

	public OnlineBuddyAdapterImpl() {
		super(endpoints, 10, Channel.newTalkChannel(endpoints));
	}

	public UserOnlineType[] getBuddyStatSeq(int userId) {
		return getManager(userId).getBuddyStatSeq(userId);
	}

	public boolean isIMOnline(int stat) {
		return (stat & OnlineBuddyAdapter.IM_ONLINE) > 0;
	}

	public boolean isWebOnline(int stat) {
		return (stat & OnlineBuddyAdapter.WEB_ONLINE) > 0;
	}

	public int getBuddyCount(int userId) {
		return getManager(userId).getBuddyCount(userId);
	}

	public OnlineManagerPrx getManager(int userId) {
		return locateProxy(managers, "M", userId, Channel.Invoke.Twoway,
				OnlineManagerPrxHelper.class, 300);
	}

	public OnlineManagerPrx getManagerOneway(int userId) {
		return locateProxy(managersOneway, "M", userId, Channel.Invoke.Oneway,
				OnlineManagerPrxHelper.class, 300);
	}

	public static void main(String[] args) {
		OnlineBuddyAdapterImpl adapter = new OnlineBuddyAdapterImpl();
		UserOnlineType[] result = adapter.getBuddyStatSeq(128487631);
		for (UserOnlineType os : result) {
			System.out.println(os.userId + "\t" + os.onlineType);
		}
	}
}
