package com.xiaonei.xce.buddysixdegrees;

import java.util.Vector;

import com.xiaonei.xce.log.Oce;

import xce.buddy.BuddySixDegreesManagerPrx;
import xce.buddy.BuddySixDegreesManagerPrxHelper;
import xce.buddy.RouteList;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

public class BuddySixDegreesAdapter extends ServiceAdapter {
	private static final String endpoints = "@BuddySixDegrees";
	private Vector<BuddySixDegreesManagerPrx> managers = new Vector<BuddySixDegreesManagerPrx>();
	private Vector<BuddySixDegreesManagerPrx> managersOneway = new Vector<BuddySixDegreesManagerPrx>();
	
	public BuddySixDegreesAdapter() {
		super(endpoints, 1 , Channel.newBuddyChannel(endpoints));
	}
	
	public void computeTaskStart(int host, int guest) {
		try {
			boolean  start = getBuddySixDegreesManager(host).computeTaskStart(host, guest);
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + "computeTaskStar host " + host, e);
		}
	}
	
	public RouteList getRouteList (int host, int guest, int begin, int limit) {
		RouteList result = new RouteList();
		try {
			result = getBuddySixDegreesManager(host).
							getRouteList(host, guest, begin, limit);		
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + "getRouteList host " + host, e);
		}
		return result;
	}
	
	public BuddySixDegreesManagerPrx getBuddySixDegreesManagerOneway(int id) {
		return locateProxy(managersOneway, "M", id,
				Channel.Invoke.Oneway, BuddySixDegreesManagerPrxHelper.class, 2000);
	}
	
	public BuddySixDegreesManagerPrx getBuddySixDegreesManager(int id) {
		return locateProxy(managers, "M", id,
				Channel.Invoke.Twoway, BuddySixDegreesManagerPrxHelper.class, 2000);
	}
}
