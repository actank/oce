package com.xiaonei.xce.charity;

import java.util.Vector;

import xce.charity.CharityManagerPrx;
import xce.charity.CharityManagerPrxHelper;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;

import Ice.TimeoutException;

public class CharityAdapter extends ServiceAdapter {
	private static final String endpoints = "@Charity";

	private Vector<CharityManagerPrx> managers = new Vector<CharityManagerPrx>();
	private Vector<CharityManagerPrx> managersOneway = new Vector<CharityManagerPrx>();


	public CharityAdapter() {
		super(endpoints, 1, Channel.newWServiceChannel(endpoints));
	}

	public int count() {
		TimeCost cost = TimeCost.begin("CharityAdapter.count ");
		CharityManagerPrx prx = null;
		try {
			prx = getCharityManager();
			return prx.count();
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".count prx = " + prx + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public int next(int begin){
		TimeCost cost = TimeCost.begin("CharityAdapter.next begin =  "+ begin);
		CharityManagerPrx prx = null;
		try {
			prx = getCharityManager();
			return prx.next(begin);
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".next prx = " + prx + " begin=" + begin + " Ice.TimeoutException");
		} finally {
			cost.endFinally();
		}
		return 0;
	}

	public Boolean set(int pos, Boolean flag) {
		TimeCost cost = TimeCost.begin("CharityAdapter.set pos = "+ pos + "flag = " + flag);
		CharityManagerPrx prx = null;
		try {
			prx = getCharityManager();
			return prx.set(pos,flag);
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".set prx = " + prx + " pos = " + pos + " flag = " + flag + " Ice.TimeoutException");
		} finally {
			cost.endFinally();
		}
		return false;
	}

	protected CharityManagerPrx getCharityManager() {
		return locateProxy(managers, "M", 0, Channel.Invoke.Twoway,
				CharityManagerPrxHelper.class, 300);
	}

	protected CharityManagerPrx getCharityManagerOneWay() {
		return locateProxy(managersOneway, "M", 0,
				Channel.Invoke.Oneway, CharityManagerPrxHelper.class, 300);
	}
}
