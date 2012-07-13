package com.xiaonei.xce.searchcache;

import java.util.ArrayList;
import java.util.List;

import com.xiaonei.xce.cardfacade.CardInfoCacheAdapter;

import xce.cardcache.CardInfo;
import xce.clusterstate.ReplicatedClusterAdapter;
import xce.searchcache.SearchCacheManagerPrx;
import xce.searchcache.SearchCacheManagerPrxHelper;
import xce.searchcache.UpdateUnit;

public class DistSearchLogicAdapter extends ReplicatedClusterAdapter {
	private static final String ctrEndpoints = "ControllerDistSearchLogic";
	private static int interval = 120;

	public DistSearchLogicAdapter() {
		super(ctrEndpoints, interval);
	}

	private SearchCacheManagerPrx getCardInfoCacheManager(int owner) {
		Ice.ObjectPrx prx0 = getValidProxy(owner, 500);
		if (prx0 != null) {
			SearchCacheManagerPrx prx = SearchCacheManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(500).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	public void doUpdate(UpdateUnit[] updateUnits) {
		SearchCacheManagerPrx prx = getCardInfoCacheManager(0);
		if (null != prx) {
			prx.update(updateUnits);
		}
	}

	public static void main(String[] args) {
		DistSearchLogicAdapter adapter = new DistSearchLogicAdapter();
		UpdateUnit[] updateUnits = new UpdateUnit[1];
		UpdateUnit tmpUpdateUnit = new UpdateUnit();
		tmpUpdateUnit.id = 278514210;
		updateUnits[0] = tmpUpdateUnit;
		adapter.doUpdate(updateUnits);

		System.exit(0);
	}
}
