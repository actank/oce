package com.xiaonei.talk;

import java.util.HashMap;
import java.util.Map;

import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

import com.xiaonei.talk.ProxyManagerPrx;
import com.xiaonei.talk.ProxyManagerPrxHelper;
import com.xiaonei.talk.common.Jid;

public class TalkProxyAdapter extends ServiceAdapter {
	private static final String endpoints = "@TalkProxy";
	private Map<String, ProxyManagerPrx> prxMap = new HashMap<String, ProxyManagerPrx>();

	private static TalkProxyAdapter instance = new TalkProxyAdapter();

	public static TalkProxyAdapter instance() {
		return instance;
	}

	TalkProxyAdapter() {
		super(endpoints, 0, Channel.newTalkChannel(endpoints));
	}

	public boolean checkTalkTicket(String ticket) {
		String[] list = ticket.split("\\$");
		if (list.length != 5) {
			return false;
		}

		Jid jid = new Jid();
		jid.userId = Integer.parseInt(list[0]);
		jid.endpoint = list[1];
		jid.index = Long.parseLong(list[2]);

		return getManager(jid).checkTicket(jid, ticket);

	}

	synchronized public ProxyManagerPrx getManager(Jid jid) {
		ProxyManagerPrx prx = prxMap.get(jid.endpoint);
		if (prx == null) {
			prx = channel.locate("M@", jid.endpoint, Channel.Invoke.Twoway,
					ProxyManagerPrxHelper.class, 300);
			prxMap.put(jid.endpoint, prx);
		}
		;
		return prx;
	}
}
