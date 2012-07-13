package mop.hi.oce.adapter.impl;

import java.util.HashMap;
import java.util.Map;

import com.xiaonei.xce.log.Oce;

import mop.hi.oce.adapter.PassportAdapter;
import mop.hi.svc.passport.PassportManagerPrx;
import mop.hi.svc.passport.PassportManagerPrxHelper;
import xce.util.ng.channel.ChannelFactory;
import xce.util.ng.channel.ChannelFactory.ChannelType;
import xce.util.ng.proxyholder.AbstractModProxyHolder;
import xce.util.ng.tools.PerformanceReporter;
import Ice.ObjectPrx;

public class PassportAdapterMCImpl implements PassportAdapter {

	private class PassportManagerByIdTwowayHolder extends
			AbstractModProxyHolder<Integer, PassportManagerPrx> {

		public PassportManagerByIdTwowayHolder() {
			super(ChannelFactory.getChannel(ChannelType.OCE),
					"Manager@Passport", 10);
		}

		@Override
		protected int mod(Integer key, int size) {
			return key % size;
		}

		@Override
		public PassportManagerPrx uncheckedCast(ObjectPrx baseProxy) {
			return PassportManagerPrxHelper.uncheckedCast(baseProxy);
		}

		@Override
		protected ObjectPrx initProxy(ObjectPrx baseProxy) {
			return baseProxy;
		}
	}

	private class PassportManagerByTicketTwowayHolder extends
			AbstractModProxyHolder<String, PassportManagerPrx> {

		public PassportManagerByTicketTwowayHolder() {
			super(ChannelFactory.getChannel(ChannelType.OCE),
					"Manager@Passport", 10);
		}

		@Override
		protected int mod(String key, int size) {
			return (key.charAt(key.length() - 1) - '0') % size;
		}

		@Override
		public PassportManagerPrx uncheckedCast(ObjectPrx baseProxy) {
			return PassportManagerPrxHelper.uncheckedCast(baseProxy);
		}

		@Override
		protected ObjectPrx initProxy(ObjectPrx baseProxy) {
			return baseProxy;
		}
	}

	private PassportManagerByIdTwowayHolder _twoway_id = new PassportManagerByIdTwowayHolder();
	private PassportManagerByTicketTwowayHolder _twoway_ticket = new PassportManagerByTicketTwowayHolder();

	private PerformanceReporter _createPerformance = new PerformanceReporter(
			"CreateTicket");
	private PerformanceReporter _destoryPerformance = new PerformanceReporter(
			"DestoryTicket");
	private PerformanceReporter _verifyPerformance = new PerformanceReporter(
			"VerifyTicket");

	public PassportAdapterMCImpl() {
		_createPerformance.printRepeat(System.out, 60 * 1000);
		_destoryPerformance.printRepeat(System.out, 60 * 1000);
		_verifyPerformance.printRepeat(System.out, 60 * 1000);
	}

	@Override
	public String createTicket(int userId, boolean isPersistent) {
		Map<String, String> ctx = null;
		if (isPersistent) {
			ctx = new HashMap<String, String>();
			ctx.put("PERSISTENT", "1");

		}
		long bTime = System.currentTimeMillis();
		try {
			return _twoway_id.getProxy(userId).createTicket(userId, ctx);
		} finally {
			_createPerformance.record(1, System.currentTimeMillis() - bTime);
		}
	}

	@Override
	public void destroyTicket(String ticket) {
		if (ticket == null) {
			Oce.getLogger().error("PassportAdapterImpl destory null");
			return;
		}
		if (ticket.length() != 33) {
			Oce.getLogger().error("PassportAdapterImpl destory wrong ticket: "
					+ ticket);
		}
		long bTime = System.currentTimeMillis();
		try {
			_twoway_ticket.getProxy(ticket).destroyTicket(ticket);
		} finally {
			_destoryPerformance.record(1, System.currentTimeMillis() - bTime);
		}
	}

	@Override
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
		long bTime = System.currentTimeMillis();
		try {
			_twoway_ticket.getProxy(ticket).destroyTicket(ticket, ctx);
		} finally {
			_destoryPerformance.record(1, System.currentTimeMillis() - bTime);
		}
	}

	@Override
	public String queryTicket(int userId) {
		return _twoway_id.getProxy(userId).queryTicket(userId);
	}

	@Override
	public Integer verifyTicket(String ticket) {
		long bTime = System.currentTimeMillis();
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
			userId = new Integer(_twoway_ticket.getProxy(ticket).verifyTicket(
					ticket));
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
		}finally{
			_verifyPerformance.record(1, System.currentTimeMillis() - bTime);
		}
		return null;
	}

}
