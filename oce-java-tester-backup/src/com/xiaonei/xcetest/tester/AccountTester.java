package com.xiaonei.xcetest.tester;

import java.util.Date;

import junit.framework.TestCase;
import mop.hi.oce.adapter.AccountAdapter;
import mop.hi.oce.adapter.impl.AccountAdapterImpl;
import mop.hi.oce.domain.model.Deposit;
import mop.hi.oce.domain.model.Expenditure;
import mop.hi.svc.model.RollbackException;

public class AccountTester  extends TestCase{
	public AccountAdapter adapter = new AccountAdapterImpl();
	public void test1() throws RollbackException
	{
		int [] fids = {  -1
};
		int [] uids = {230105116
};
		int [] rmb = {  100
};
		
		for(int i = 0; i< fids.length; i++){
		Deposit dep = new Deposit();
		dep.setFid(fids[i]);
		dep.setRMB(rmb[i]);
		dep.setTimestamp(new Date());
		dep.setUserId(uids[i]);
		dep.setXNB(rmb[i]);
		adapter.inc(dep);
		System.out.println(uids[i]);
		}
	}
	public void test2() throws RollbackException
	{
		Expenditure exp = new Expenditure();
		exp.setDescription("-1");
		exp.setProductName("-1");
		exp.setProductType("-1");
		exp.setTimestamp(new Date());
		exp.setUserId(200080602);
		exp.setXNB(87);
		adapter.dec(exp);
	}
	
	public void test3()
	{
		int [] uids = {200080602  
};
		for(int i = 0 ; i< uids.length; i++)
		{
			System.out.print(uids[i]);
			System.out.print("=");
			System.out.println(adapter.getAccount(uids[i]).getXNB());
		}
	}
}
