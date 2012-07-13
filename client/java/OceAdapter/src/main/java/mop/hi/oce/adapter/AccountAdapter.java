package mop.hi.oce.adapter;

import mop.hi.svc.model.RollbackException;

public interface AccountAdapter {
	
	public void inc(mop.hi.oce.domain.model.Deposit deposit) throws RollbackException;
	public void dec(mop.hi.oce.domain.model.Expenditure expenditure) throws RollbackException;
	public mop.hi.oce.domain.model.Account getAccount(int userId);
}
