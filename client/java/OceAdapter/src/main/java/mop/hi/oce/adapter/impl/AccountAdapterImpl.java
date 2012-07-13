package mop.hi.oce.adapter.impl;

import mop.hi.oce.adapter.AccountAdapter;
import mop.hi.oce.domain.model.Account;
import mop.hi.oce.domain.model.Deposit;
import mop.hi.oce.domain.model.Expenditure;
import mop.hi.svc.model.AccountManagerPrx;
import mop.hi.svc.model.AccountManagerPrxHelper;
import mop.hi.svc.model.RollbackException;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;


public class AccountAdapterImpl extends ServiceAdapter implements
		AccountAdapter {
	private static final String endpoints = "@Account";
	private AccountManagerPrx manager;

	private AccountManagerPrx managerOneway;

	public AccountAdapterImpl() {
		super(endpoints, 0, Channel.newOceChannel(endpoints));
		manager = locateProxy("M", Channel.Invoke.Twoway,
				AccountManagerPrxHelper.class, 10000);
		managerOneway = locateProxy("M", Channel.Invoke.Oneway,
				AccountManagerPrxHelper.class, 10000);
	}

	public void dec(Expenditure expenditure) throws RollbackException {
		manager.dec(expenditure.get());
	}

	public Account getAccount(int userId) {
		return Account.parse(manager.getAccount(userId));
	}

	public void inc(Deposit deposit) throws RollbackException {
		manager.inc(deposit.get());
	}
}
