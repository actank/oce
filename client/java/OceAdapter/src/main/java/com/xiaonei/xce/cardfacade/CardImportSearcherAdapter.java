package com.xiaonei.xce.cardfacade;

import java.util.Vector;

import xce.cardsearch.CardImportSearcherPrx;
import xce.cardcache.ImportInfo;
import xce.cardcache.ImportValueNew;
import xce.cardsearch.CardImportSearcherPrxHelper;
import xce.cardcache.ImportValue;
import xce.clusterstate.ReplicatedClusterAdapter;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

public class CardImportSearcherAdapter extends ReplicatedClusterAdapter {

	// private static CardImportSearcherAdapter _instance = new
	// CardImportSearcherAdapter();
	// private static final String endpoints = "@CardImportSearcher";
	//
	// private Vector<CardImportSearcherPrx> _managers = new
	// Vector<CardImportSearcherPrx>();
	// private Vector<CardImportSearcherPrx> _managersOneway = new
	// Vector<CardImportSearcherPrx>();
	//
	// public static CardImportSearcherAdapter getInstance() {
	// return _instance;
	// }
	//
	// protected CardImportSearcherAdapter() {
	// super(endpoints, 1, Channel.newSearchFoFChannel(endpoints));
	//
	// }
	//
	// protected CardImportSearcherPrx getCardImportSearcherManager(int userId)
	// {
	// return CardImportSearcherPrxHelper.uncheckedCast(locateProxy(_managers,
	// "Manager", userId, Channel.Invoke.Twoway,
	// CardImportSearcherPrxHelper.class, 300));
	// }
	//
	// protected CardImportSearcherPrx getCardImportSearcherManagerOneway(int
	// userId) {
	// return CardImportSearcherPrxHelper.uncheckedCast(locateProxy(
	// _managersOneway, "Manager", userId, Channel.Invoke.Oneway,
	// CardImportSearcherPrxHelper.class, 300));
	// }

	private static final String ctr_endpoints = "ControllerCardImportSearcher";
	private static int _interval = 120;

	public CardImportSearcherAdapter() {
		super(ctr_endpoints, _interval);
	}

	public CardImportSearcherPrx getCardImportSearcherManager(int owner) {

		Ice.ObjectPrx prx0 = getValidProxy(owner, 500);
		if (prx0 != null) {
			CardImportSearcherPrx prx = CardImportSearcherPrxHelper
					.uncheckedCast(prx0.ice_timeout(500).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	public ImportValue[] searchImport(int userId, String query, int limit) {
		return getCardImportSearcherManager(0).searchImport(userId, query,
				limit);
	}

	public ImportValueNew[] searchImportNew(int userId, String query, int limit) {
		return getCardImportSearcherManager(0).searchImportNew(userId, query,
				limit);
	}

	public static void main(String[] args) {
		CardImportSearcherAdapter adapter = new CardImportSearcherAdapter();
		int userId = 67067835;
		String query = "li";
		int limit = -1;
		ImportValue[] res = adapter.searchImport(userId, query, limit);

		for (ImportValue x : res) {
			System.out.println(x.importWay);
			System.out.println(x.importDetail);
			System.out.println(x.name);
		}

		ImportValueNew[] resnew = adapter.searchImportNew(67067835, "zh",
				limit);
		if (resnew == null) {
			System.out.println("NULL");
		}
		System.out.println("size:" + resnew.length);
		for (ImportValueNew x : resnew) {
			System.out.println(x.id);
			System.out.println(x.importWay);
			System.out.println(x.importDetail);
			System.out.println(x.name);
		}

		System.exit(0);
	}
}
