package mop.hi.oce.domain.model;

import java.util.Date;

import xce.util.tools.DateFormatter;


public class Deposit {

	protected mop.hi.svc.model.Deposit _deposit;
	protected Deposit(mop.hi.svc.model.Deposit deposit)
	{
		_deposit = deposit;
	}
	public Deposit()
	{
		_deposit = new mop.hi.svc.model.Deposit();
	}
	public mop.hi.svc.model.Deposit get()
	{
		return _deposit;
	}
	
	public static Deposit parse(mop.hi.svc.model.Deposit deposit)
	{
		return new Deposit(deposit);
	}
	
	public int getFid()
	{
		return _deposit.fid;
	}
	
	public void setFid(int fid)
	{
		_deposit.fid = fid;
	}
	
	public int getId()
	{
		return _deposit.id;
	}
	
	public int getRMB()
	{
		return _deposit.rmb;
	}
	
	public void setRMB(int rmb)
	{
		_deposit.rmb = rmb;
	}
	
	public Date getTimestamp()
	{
		return DateFormatter.parse(_deposit.timestamp);
	}
	
	public void setTimestamp(Date time)
	{
		_deposit.timestamp = DateFormatter.format(time);
	}
	
	public int getXNB()
	{
		return _deposit.xnb;
	}
	
	public void setXNB(int xnb)
	{
		_deposit.xnb = xnb;
	}
	
	public int getUserId()
	{
		return _deposit.uid;
	}
	
	public void setUserId(int uid)
	{
		_deposit.uid = uid;
	}
	
}
