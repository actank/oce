package mop.hi.oce.adapter.impl;

import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import mop.hi.oce.adapter.WebOnlineAdapter;
import mop.hi.svc.online.OnlineManagerPrx;
import mop.hi.svc.online.OnlineManagerPrxHelper;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

@Deprecated
public class WebOnlineAdapterImpl extends ServiceAdapter implements
		WebOnlineAdapter {
	private static final String endpoints = "@UserOnline";
	private OnlineManagerPrx manager;
	private OnlineManagerPrx managerOneway;
	// private TalkLogicAdapterImpl talkLogicAdapter = new
	// TalkLogicAdapterImpl();
	private int OT_WEB = 2;

	public void login(int userId) {
		// talkLogicAdapter.login(userId);
		managerOneway.setStat(userId, OT_WEB, true);
	}

	public void logout(int userId) {
		// talkLogicAdapter.logout(userId);
		managerOneway.setStat(userId, OT_WEB, false);
	}

	public void keepOnline(int userId) {
		// talkLogicAdapter.keepAlive(userId);
		managerOneway.setStat(userId, OT_WEB, true);
	}

	public List<Integer> getOnlineUsers(int begin, int limit) {
		int[] wss = manager.getStats(begin, limit, 0);// 0 is onlineType. not
		// used at this time.
		List<Integer> result = new LinkedList<Integer>();
		for (Integer ws : wss) {
			result.add(ws);
		}
		return result;
	}

	public int getOnlineSize() {
		return manager.size(2);// 0 is onlineType. not used at
	}

	public Map<Integer, Integer> getOnlineStatus(int[] users) {
		return manager.checkStats(users);
	}

	public int getOnlineStatus(int user) {
		return manager.getStat(user);
	}

	public WebOnlineAdapterImpl() {
		super(endpoints, 0, Channel.newOceChannel(endpoints));
		manager = locateProxy("Manager", Channel.Invoke.Twoway,
				OnlineManagerPrxHelper.class, 300);
		managerOneway = locateProxy("Manager", Channel.Invoke.Oneway,
				OnlineManagerPrxHelper.class, 300);
	}
}
