package com.xiaonei.xce.footprintunite;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.Scanner;
import java.util.Vector;
import java.util.Map.Entry;

import xce.clusterstate.ReplicatedClusterAdapter;
import xce.footprint.FootprintFacadeManagerPrx;
import xce.footprint.FootprintFacadeManagerPrxHelper;
import xce.footprint.FootprintInfo;
import xce.footprint.IpFootprintInfo;
import xce.footprint.HomeFootprintCachePrx;
import xce.footprint.HomeFootprintCachePrxHelper;
import xce.footprint.HomeInfo;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

import Ice.ConnectTimeoutException;
import Ice.TimeoutException;

import com.xiaonei.services.FriendInput.*;
import com.xiaonei.xce.log.Oce;

public class FootprintFacadeAdapter extends ReplicatedClusterAdapter {

	private static final String ctr_endpoints = "ControllerFootprintFacade";
	private static int _interval = 120;
	private int _cluster = 10;

	public FootprintFacadeAdapter() {

		super(ctr_endpoints, _interval);
	}

	private FootprintFacadeManagerPrx getFootprintFacadeManager(int owner) {
		Ice.ObjectPrx prx0 = getProxy(owner, 300);

		// System.out.println(prx0.ice_toString());
		if (prx0 != null) {
			FootprintFacadeManagerPrx prx;
			prx = FootprintFacadeManagerPrxHelper.uncheckedCast(prx0
					.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	private FootprintFacadeManagerPrx getFootprintFacadeManagerOneWay(int owner) {
		Ice.ObjectPrx prx0 = getProxy(owner, 300);

		// System.out.println(prx0.ice_toString());
		if (prx0 != null) {
			FootprintFacadeManagerPrx prx;
			prx = FootprintFacadeManagerPrxHelper.uncheckedCast(prx0
					.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}

	public void visit(FootprintInfo fpt) {
		getFootprintFacadeManagerOneWay(fpt.owner).visit(fpt);
	}

	public void visitByIp(IpFootprintInfo fpt) {
		getFootprintFacadeManagerOneWay(fpt.owner).visitByIp(fpt);
	}

	public void visitBatch(ArrayList<FootprintInfo> fpts) {
		HashMap<Integer, ArrayList<FootprintInfo>> bucket = new HashMap<Integer, ArrayList<FootprintInfo>>();
		for (FootprintInfo fpt : fpts) {

			ArrayList<FootprintInfo> list = bucket.get(fpt.owner % _cluster);
			if (list == null) {
				list = new ArrayList<FootprintInfo>();
				bucket.put(fpt.owner % _cluster, list);
			}
			list.add(fpt);
		}
		for (Entry<Integer, ArrayList<FootprintInfo>> entry : bucket.entrySet()) {
			getFootprintFacadeManagerOneWay(entry.getKey()).visitBatch(
					entry.getValue().toArray(
							new FootprintInfo[entry.getValue().size()]));
		}
	}

	public HomeInfo visitAndGet(FootprintInfo fpt) {
		return getFootprintFacadeManager(fpt.owner).visitAndGet(fpt);
	}
	
	public void removeAllByVisitor(int visitor) {
		getFootprintFacadeManager(visitor).removeAllByVisitor(visitor);
	}
}
