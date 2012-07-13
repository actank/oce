package com.xiaonei.xce.cardfacade;

import java.util.Vector;

import xce.cardcache.CardImportCachePrx;
import xce.cardcache.ImportEmail;
import xce.cardcache.ImportIdImportDetail;
import xce.cardcache.ImportInfo;
import xce.cardcache.CardImportCachePrxHelper;
import xce.cardcache.ImportInfoNew;
import xce.cardcache.ImportValue;
import xce.cardcache.ImportValueII;
import xce.cardcache.ImportValueNew;
import xce.clusterstate.ReplicatedClusterAdapter;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

public class CardImportCacheAdapter extends ReplicatedClusterAdapter {

	// private static CardImportCacheAdapter _instance = new
	// CardImportCacheAdapter();
	// private static final String endpoints = "@CardImportCache";
	//
	// private Vector<CardImportCachePrx> _managers = new
	// Vector<CardImportCachePrx>();
	// private Vector<CardImportCachePrx> _managersOneway = new
	// Vector<CardImportCachePrx>();
	//
	// public static CardImportCacheAdapter getInstance() {
	// return _instance;
	// }
	//
	// protected CardImportCacheAdapter() {
	// super(endpoints, 1, Channel.newSearchFoFChannel(endpoints));
	//
	// }
	//
	// protected CardImportCachePrx getCardImportCacheManager(int userId) {
	// return CardImportCachePrxHelper.uncheckedCast(locateProxy(_managers,
	// "CIMCS", userId, Channel.Invoke.Twoway,
	// CardImportCachePrxHelper.class, 300));
	// }
	//
	// protected CardImportCachePrx getCardImportCacheManagerOneway(int userId)
	// {
	// return CardImportCachePrxHelper.uncheckedCast(locateProxy(
	// _managersOneway, "CIMCS", userId, Channel.Invoke.Oneway,
	// CardImportCachePrxHelper.class, 300));
	// }

	private static final String ctr_endpoints = "ControllerCardImportCache";
	private static int _interval = 120;

	public CardImportCacheAdapter() {
		super(ctr_endpoints, _interval);
	}

	public CardImportCachePrx getCardImportCacheManager(int owner) {

		Ice.ObjectPrx prx0 = getValidProxy(owner, 500);
		if (prx0 != null) {
			CardImportCachePrx prx = CardImportCachePrxHelper
					.uncheckedCast(prx0.ice_timeout(500).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	public ImportInfo[] get(int userId, int limit) {
		return getCardImportCacheManager(0).get(userId, limit);
	}

	public ImportInfo getDetail(int userId, byte importWay, String importDetail) {
		return getCardImportCacheManager(0).getDetail(userId, importWay,
				importDetail);
	}

	public ImportValue[] getList(int userId) {
		return getCardImportCacheManager(0).getList(userId);
	}

	public boolean set(ImportInfo[] importInfos) {
		return getCardImportCacheManager(0).set(importInfos);
	}

	public boolean load(ImportInfo[] importInfos) {
		return getCardImportCacheManager(0).load(importInfos);
	}

	public void remove(ImportInfo[] importInfos) {
		getCardImportCacheManager(0).remove(importInfos);
	}

	public ImportInfoNew[] getNew(int userId, int limit) {
		return getCardImportCacheManager(0).getNew(userId, limit);
	}

	public ImportInfoNew getDetailNew(int userId, byte importWay,
			String importDetail) {
		return getCardImportCacheManager(0).getDetailNew(userId, importWay,
				importDetail);
	}

	public ImportValueNew[] getListNew(int userId) {
		return getCardImportCacheManager(0).getListNew(userId);
	}

	public boolean setNew(ImportInfoNew[] importInfos) {
		return getCardImportCacheManager(0).setNew(importInfos);
	}

	public boolean loadNew(ImportInfoNew[] importInfos) {
		return getCardImportCacheManager(0).loadNew(importInfos);
	}

	public ImportInfoNew[] loadII(ImportInfoNew[] importInfos) {
		return getCardImportCacheManager(0).loadII(importInfos);
	}

	public ImportIdImportDetail[] getIdImportDetail(int userId) {
		return getCardImportCacheManager(0).getIdImportDetail(userId);
	}

	public boolean setSubscribeFlag(ImportInfoNew[] infos) {
		return getCardImportCacheManager(0).setSubscribeFlag(infos);
	}

	public boolean isContact(int userId, String email, int eType) {
		return getCardImportCacheManager(0).isContact(userId, email, eType);
	}

	public ImportValueII[] getListII(int userId) {
		return getCardImportCacheManager(0).getListII(userId);
	}

	public int[] getImportUser(int userId) {
		return getCardImportCacheManager(0).getImportUser(userId);
	}

	public ImportEmail[] getImportEmail(int offset, int limit) {
		return getCardImportCacheManager(0).getImportEmail(offset, limit);
	}

	public boolean isImported(int userId) {
		return getCardImportCacheManager(0).isImported(userId);
	}

	public int[] friendsImported(int[] friendIds) {
		return getCardImportCacheManager(0).friendsImported(friendIds);
	}

	public ImportInfoNew getDetailByEmail(int userId, String email) {
		return getCardImportCacheManager(0).getDetailByEmail(userId, email);
	}

	public int[] getImportUserId(int begin, int limit) {
		return getCardImportCacheManager(0).getImportUserId(begin, limit);
	}

	public static void main(String[] args) {
		CardImportCacheAdapter adapter = new CardImportCacheAdapter();
		int userId = 67067835;

		ImportInfo[] loadinfos = new ImportInfo[1];
		loadinfos[0] = new ImportInfo();
		loadinfos[0].userId = 67067835;
		loadinfos[0].importWay = 33;
		loadinfos[0].importDetail = "odd@gmail.com";
		loadinfos[0].email = "liwu@gmail.com";
		loadinfos[0].msn = "occ@gmail.com";
		loadinfos[0].name = "李五";
		loadinfos[0].mobile = "o3";
		loadinfos[0].qq = "o3";
		loadinfos[0].address = "o3";
		loadinfos[0].extra = "o3";
		loadinfos[0].requestCount = 10;

		ImportInfoNew[] details = adapter.getNew(67067835, -1);
		for (ImportInfoNew detail : details) {
			System.out.println("id=" + detail.id);
			System.out.println("userId=" + detail.userId);
			System.out.println("importWay=" + detail.importWay);
			System.out.println("importDetail=" + detail.importDetail);
			System.out.println("email=" + detail.email);
			System.out.println("msn=" + detail.msn);
			System.out.println("name=" + detail.name);
			System.out.println("mobile=" + detail.mobile);
			System.out.println("qq=" + detail.qq);
			System.out.println("address=" + detail.address);
			System.out.println("extra=" + detail.extra);
			System.out.println("requestCount=" + detail.requestCount);
			System.out.println("subscribeFlag=" + detail.subscribeFlag);
			System.out.println();
		}

		ImportInfo[] infos = adapter.get(99999, -1);
		for (ImportInfo info : infos) {
			System.out.println("id=" + info.importDetail);

		}

		System.exit(0);
	}
}
