package com.xiaonei.xce.passport;

import java.util.Map;

import Ice.ConnectTimeoutException;
import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;

import passport.TicketManagerPrx;
import passport.TicketManagerPrxHelper;
import xce.clusterstate.ReplicatedClusterMCAdapter;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;

public class TicketWebImpl implements Ticket {
	private static TicketWebImpl _instance = new TicketWebImpl();

	private class TicketManagerHolderById extends
	ReplicatedClusterMCAdapter {
		private final static String ctr_endpoints = "ControllerTicket";
		private static final int _interval = 120;

		public TicketManagerHolderById() {
			super(ChannelFactory.getChannel(ChannelType.PASSPORT), ctr_endpoints, _interval);
		}
		
		public TicketManagerPrx getProxy(int userId) {
            Ice.ObjectPrx prx0 = getValidProxy(userId, -1);
            if (prx0 != null) {
                    TicketManagerPrx prx = TicketManagerPrxHelper
                                    .uncheckedCast(prx0.ice_twoway().ice_timeout(-1));
                    return prx;
            } else {
                    return null;
            }
		}
	}

	private class TicketManagerHolderByTicket extends ReplicatedClusterMCAdapter {
		private final static String ctr_endpoints = "ControllerTicket";
		private static final int _interval = 120;

		public TicketManagerHolderByTicket() {
			super(ChannelFactory.getChannel(ChannelType.PASSPORT), ctr_endpoints, _interval);
		}
		
		
		
		public TicketManagerPrx getProxy(String ticket) {
            Ice.ObjectPrx prx0 = getValidProxy(ticket.charAt(32) - '0', 300);
            if (prx0 != null) {
                    TicketManagerPrx prx = TicketManagerPrxHelper
                                    .uncheckedCast(prx0.ice_twoway().ice_timeout(300));
                    return prx;
            } else {
                    return null;
            }
		}
	}

	private TicketManagerHolderById proxyHolderById = new TicketManagerHolderById();
	private TicketManagerHolderByTicket proxyHolderByTicket = new TicketManagerHolderByTicket();

	protected TicketWebImpl() {
		// prevent outside new
	}

	public static TicketWebImpl getInstance() {
		return _instance;
	}

	protected TicketManagerPrx getTicketManager(int userId) {
		return proxyHolderById.getProxy(userId);
	}

	protected TicketManagerPrx getTicketManager(String ticket) {
		return proxyHolderByTicket.getProxy(ticket);
	}

	public String createTicket(int userId, Map<String, String> m) {
		String infos = "";
		if (m != null) {
			for (Map.Entry<String, String> entry : m.entrySet()) {
				infos += entry.getKey() + "_" + entry.getValue();
			}
		}
		TicketManagerPrx prx = null;
		try {
			prx = getTicketManager(userId);
			return prx.createTicket(userId, infos, TraceMe.createStackTraceContext(TraceMe.getStackTrack()));
		} catch(ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".createTicket [" + prx
					+ "] userId=" + userId + " Ice.ConnectTimeoutException");
			throw e;
	     } catch (TimeoutException e) {
	    	 Oce.getLogger().error(
	    			 this.getClass().getName() + ".createTicket [" + prx
	                      + "] userId=" + userId + " Ice.TimeoutException");
	    	 throw e;
	    }
	}

	public String queryTicket(int userId) {
		TicketManagerPrx prx = null;
		try {
			prx = getTicketManager(userId);
			return prx.queryTicket(userId, TraceMe.createStackTraceContext(TraceMe.getStackTrack()));
		} catch(ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".queryTicket [" + prx
					+ "] userId=" + userId + " Ice.ConnectTimeoutException");
			throw e;
	     } catch (TimeoutException e) {
	    	 Oce.getLogger().error(
	    			 this.getClass().getName() + ".queryTicket [" + prx
	                      + "] userId=" + userId + " Ice.TimeoutException");
	    	 throw e;
	    }
	}

	public int verifyTicket(String ticket, Map<String, String> m) {
		if (ticket.length() == 33) {
			String infos = "";
			if (m != null) {
				for (Map.Entry<String, String> entry : m.entrySet()) {
					infos += entry.getKey() + "_" + entry.getValue();
				}
			}
			TicketManagerPrx prx = null;
			try {
				prx = getTicketManager(ticket);
				return prx.verifyTicket(ticket, infos, TraceMe.createStackTraceContext(TraceMe.getStackTrack()));
			} catch(ConnectTimeoutException e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".verifyTicket [" + prx
						+ "] ticket=" + ticket + " Ice.ConnectTimeoutException");
				throw e;
		     } catch (TimeoutException e) {
		    	 Oce.getLogger().error(
		    			 this.getClass().getName() + ".verifyTicket [" + prx
		                      + "] ticket=" + ticket + " Ice.TimeoutException");
		    	 throw e;
		    }
		} else {
			return -1;
		}
	}

	public boolean destroyTicket(String ticket) {
		if (ticket.length() == 33) {
			TicketManagerPrx prx = null;
			try {
				prx = getTicketManager(ticket);
				return prx.destroyTicket(ticket, TraceMe.createStackTraceContext(TraceMe.getStackTrack()));
			} catch(ConnectTimeoutException e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".destroyTicket [" + prx
						+ "] ticket=" + ticket + " Ice.ConnectTimeoutException");
				throw e;
		     } catch (TimeoutException e) {
		    	 Oce.getLogger().error(
		    			 this.getClass().getName() + ".destroyTicket [" + prx
		                      + "] ticket=" + ticket + " Ice.TimeoutException");
		    	 throw e;
		    }
		} else {
			return false;
		}
	}
}
