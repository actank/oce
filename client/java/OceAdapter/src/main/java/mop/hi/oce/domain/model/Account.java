package mop.hi.oce.domain.model;

public class Account {
	protected mop.hi.svc.model.Account _account;
	protected Account(mop.hi.svc.model.Account account)
	{
		_account = account;
	}
	public Account()
	{
		_account = new mop.hi.svc.model.Account();
	}
	
	public mop.hi.svc.model.Account get()
	{
		return _account;
	}
	
	public static Account parse(mop.hi.svc.model.Account account)
	{
		return new Account(account);
	}
	
	public int getXNB()
	{
		return _account.xnb;
	}
	public void setXNB(int xnb)
	{
		_account.xnb = xnb;
	}
}
