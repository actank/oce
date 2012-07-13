package mop.hi.oce.adapter.impl;

import java.util.HashMap;
import java.util.Map;

import mop.hi.oce.adapter.PassportAdapter;
import mop.hi.svc.passport.PassportManagerPrx;
import mop.hi.svc.passport.PassportManagerPrxHelper;
import xce.util.cache.Factory;
import xce.util.cache.TimedEvictor;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

public class PassportWapAdapterImpl extends ServiceAdapter implements
		PassportAdapter {
	private static final String endpoints = "@PassportWap";
	private PassportManagerPrx manager;
	private PassportManagerPrx managerOneway;
	private TimedEvictor userIdEvictor = new TimedEvictor(
			new ObjectFactoryImpl(), 60 * 30);

	public PassportWapAdapterImpl() {
		super(endpoints, 0, Channel.newOceChannel(endpoints));
		manager = locateProxy("Manager", Channel.Invoke.Twoway,
				PassportManagerPrxHelper.class, 300);
		managerOneway = locateProxy("Manager", Channel.Invoke.Oneway,
				PassportManagerPrxHelper.class, 300);
	}

	public String createTicket(int userId, boolean isPresistent) {
		String ticket = null;
		if (isPresistent) {
			Map<String, String> ctx = new HashMap<String, String>();
			ctx.put("PERSISTENT", "1");
			ticket = manager.createTicket(userId, ctx);
		} else {
			ticket = manager.createTicket(userId);
		}
		userIdEvictor.add(userId, ticket);

		return ticket;
	}

	public void destroyTicket(String ticket) {
		userIdEvictor.remove(ticket);
		managerOneway.destroyTicket(ticket);
	}

	public void impersistence(String ticket) {
		Map<String, String> ctx = new HashMap<String, String>();
		ctx.put("PERSISTENT", "1");
		userIdEvictor.remove(ticket);
		managerOneway.destroyTicket(ticket, ctx);
	}

	public Integer verifyTicket(String ticket) {
		Integer userId = (Integer) userIdEvictor.locate(ticket);
		if (userId != null) {
			return userId;
		}

		return null;
	}

	public String queryTicket(int userId) {
		return manager.queryTicket(userId);
	}

	class ObjectFactoryImpl implements Factory {
		public Object create(Object obj) {
			Integer userId = new Integer(manager.verifyTicket((String) obj));
			if (userId.intValue() > 0) {
				return userId;
			}
			return null;
		}
	}
}
