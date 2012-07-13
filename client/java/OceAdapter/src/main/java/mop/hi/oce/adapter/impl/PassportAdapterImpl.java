package mop.hi.oce.adapter.impl;

import java.util.HashMap;
import java.util.Map;
import java.util.Vector;

import com.xiaonei.xce.log.Oce;

import mop.hi.oce.adapter.PassportAdapter;
import mop.hi.svc.passport.PassportManagerPrx;
import mop.hi.svc.passport.PassportManagerPrxHelper;
import xce.util.cache.Factory;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

public class PassportAdapterImpl extends ServiceAdapter implements
		PassportAdapter {
	private static final String endpoints = "@Passport";
	private Vector<PassportManagerPrx> managers = new Vector<PassportManagerPrx>();
	private Vector<PassportManagerPrx> managersOneway = new Vector<PassportManagerPrx>();

	public PassportAdapterImpl() {
		super(endpoints, 10, Channel.newOceChannel(endpoints));
	}

	public String createTicket(int userId, boolean isPresistent) {
		String ticket = null;
		if (isPresistent) {
			Map<String, String> ctx = new HashMap<String, String>();
			ctx.put("PERSISTENT", "1");
			ticket = getManager(userId).createTicket(userId, ctx);
		} else {
			ticket = getManager(userId).createTicket(userId);
		}
		return ticket;
	}

	public void destroyTicket(String ticket) {
		if (ticket == null) {
			Oce.getLogger().error("PassportAdapterImpl destory null");
			return;
		}
		if (ticket.length() != 33) {
			Oce.getLogger().error("PassportAdapterImpl destory wrong ticket: "
					+ ticket);
		}
		getManagerOneway(ticket).destroyTicket(ticket);
	}

	public void impersistence(String ticket) {
		if (ticket == null) {
			Oce.getLogger().error("PassportAdapterImpl impersistence null");
			return;
		}
		Map<String, String> ctx = new HashMap<String, String>();
		ctx.put("PERSISTENT", "1");
		if (ticket.length() != 33) {
			Oce.getLogger().error("PassportAdapterImpl impersistence wrong ticket: "
							+ ticket);
		}
		getManagerOneway(ticket).destroyTicket(ticket, ctx);
	}

	public Integer verifyTicket(String ticket) {
		try {
			if (ticket == null) {
				Oce.getLogger().error("PassportAdapterImpl verify null");
				return -1;
			}
			Integer userId = null;
			if (ticket.length() != 33) {
				Oce.getLogger().error("PassportAdapterImpl verify wrong ticket: "
						+ ticket);
			}
			userId = new Integer(getManager(ticket).verifyTicket(ticket));
			if (userId != null && userId != -1) {
				return userId;
			}
		} catch (Ice.ConnectionTimeoutException e) {
			Oce.getLogger().error("Passport ConnectTimeoutException " + ticket);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error("Passport TimeoutException " + ticket);
		} catch (Throwable e) {
			Oce.getLogger().error("Passport Throwable. rethrow it. " + ticket);
			//Oce.getLogger().error(this.getClass().getName() + e);
		}
		return null;
	}

	public String queryTicket(int userId) {
		return getManager(userId).queryTicket(userId);
	}

	private int ticket2position(String ticket) {
		return ticket.charAt(ticket.length() - 1) - '0';
	}

	public PassportManagerPrx getManager(int userId) {
		return locateProxy(managers, "Manager", userId, Channel.Invoke.Twoway,
				PassportManagerPrxHelper.class, 300);
	}

	public PassportManagerPrx getManager(String ticket) {
		return locateProxy(managers, "Manager", ticket2position(ticket),
				Channel.Invoke.Twoway, PassportManagerPrxHelper.class, 300);
	}

	public PassportManagerPrx getManagerOneway(int userId) {
		return locateProxy(managersOneway, "Manager", userId,
				Channel.Invoke.Oneway, PassportManagerPrxHelper.class, 300);
	}

	public PassportManagerPrx getManagerOneway(String ticket) {
		return locateProxy(managersOneway, "Manager", ticket2position(ticket),
				Channel.Invoke.Oneway, PassportManagerPrxHelper.class, 300);
	}

	class ObjectFactoryImpl implements Factory {
		public Object create(Object obj) {
			String ticket = (String) obj;
			if (ticket == null) {
				Oce.getLogger().info("PassportAdapterImpl create null");
				return null;
			}
			Integer userId = null;
			if (ticket.length() != 33) {
				Oce.getLogger().info("PassportAdapterImpl create wrong ticket: "
						+ ticket);
			}
			userId = new Integer(getManager(ticket).verifyTicket(ticket));
			if (userId.intValue() > 0) {
				return userId;
			}
			return null;
		}
	}
}
