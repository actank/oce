package mop.hi.oce.adapter.impl;

import java.util.HashMap;
import java.util.Map;

import com.xiaonei.xce.log.Oce;

import mop.hi.oce.adapter.PassportAdapter;
import mop.hi.svc.passport.PassportManagerPrx;
import mop.hi.svc.passport.PassportManagerPrxHelper;
import xce.clusterstate.ReplicatedClusterAdapter;
import xce.util.cache.Factory;

public class PassportAdapterReplicaImpl extends ReplicatedClusterAdapter implements
		PassportAdapter {
	
	private static final String ctr_endpoints = "ControllerPassport";
	private static int _interval = 120;	

	public PassportAdapterReplicaImpl() {
		super(ctr_endpoints,_interval);
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
			Oce.getLogger().error("PassportAdapterReplicaImpl destory null");
			return;
		}
		if (ticket.length() != 33) {
			Oce.getLogger().error("PassportAdapterReplicaImpl destory wrong ticket: "
					+ ticket);
		}
		getManagerOneway(ticket).destroyTicket(ticket);
	}

	public void impersistence(String ticket) {
		if (ticket == null) {
			Oce.getLogger().error("PassportAdapterReplicaImpl impersistence null");
			return;
		}
		Map<String, String> ctx = new HashMap<String, String>();
		ctx.put("PERSISTENT", "1");
		if (ticket.length() != 33) {
			Oce.getLogger().error("PassportAdapterReplicaImpl impersistence wrong ticket: "
							+ ticket);
		}
		getManagerOneway(ticket).destroyTicket(ticket, ctx);
	}

	public Integer verifyTicket(String ticket) {
		try {
			if (ticket == null) {
				Oce.getLogger().error("PassportAdapterReplicaImpl verify null");
				return -1;
			}
			Integer userId = null;
			if (ticket.length() != 33) {
				Oce.getLogger().error("PassportAdapterReplicaImpl verify wrong ticket: "
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
		
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			PassportManagerPrx prx = PassportManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	public PassportManagerPrx getManager(String ticket) {
		int userId=ticket2position(ticket);
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			PassportManagerPrx prx = PassportManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	public PassportManagerPrx getManagerOneway(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			PassportManagerPrx prx = PassportManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}

	public PassportManagerPrx getManagerOneway(String ticket) {
		int userId=ticket2position(ticket);
		Ice.ObjectPrx prx0 = getValidProxy(userId, 300);
		if ( prx0 != null ) {
			PassportManagerPrx prx = PassportManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}

	class ObjectFactoryImpl implements Factory {
		public Object create(Object obj) {
			String ticket = (String) obj;
			if (ticket == null) {
				Oce.getLogger().error("PassportAdapterReplicaImpl create null");
				return null;
			}
			Integer userId = null;
			if (ticket.length() != 33) {
				Oce.getLogger().error("PassportAdapterReplicaImpl create wrong ticket: "
						+ ticket);
			}
			userId = new Integer(getManager(ticket).verifyTicket(ticket));
			if (userId.intValue() > 0) {
				return userId;
			}
			return null;
		}
	}
	
	public static void main(String[] args) {
		PassportAdapterReplicaImpl test = new PassportAdapterReplicaImpl();
		test.queryTicket(68126);
		System.exit(0);
	}
}
