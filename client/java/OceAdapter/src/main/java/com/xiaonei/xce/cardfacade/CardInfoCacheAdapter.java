package com.xiaonei.xce.cardfacade;

import java.util.ArrayList;
import java.util.List;

import xce.cardcache.CardInfo;
import xce.cardcache.CardInfoCachePrx;
import xce.cardcache.CardInfoCachePrxHelper;
import xce.cardcache.PartOfCardInfo;
import xce.clusterstate.ReplicatedClusterAdapter;

public class CardInfoCacheAdapter extends ReplicatedClusterAdapter {

	private static final String ctr_endpoints = "ControllerCardInfoCache";
	private static int _interval = 120;

	public CardInfoCacheAdapter() {
		super(ctr_endpoints, _interval);
	}

	public CardInfoCachePrx getCardInfoCacheManager(int owner) {

		Ice.ObjectPrx prx0 = getValidProxy(owner,500);
		if (prx0 != null) {
			CardInfoCachePrx prx = CardInfoCachePrxHelper.uncheckedCast(prx0
					.ice_timeout(500).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	public PartOfCardInfo[] getPart(List<Integer> userIds) {
		return getCardInfoCacheManager(0).getPart(userIds);
	}
	

	public CardInfo[] get(List<Integer> userIds) {
		return getCardInfoCacheManager(0).get(userIds);
	}

	public boolean set(CardInfo cardInfo) {
		return getCardInfoCacheManager(0).set(new CardInfo[] { cardInfo });
	}

	public boolean setPrivacy(int userId, String privacy) {
		return getCardInfoCacheManager(0).setPrivacy(userId, privacy);
	}

	public String getPrivacy(int userId) {
		return getCardInfoCacheManager(0).getPrivacy(userId);
	}

	public void remove(int[] userIds) {
		getCardInfoCacheManager(0).remove(userIds);
	}

	public static void main(String[] args) {
		CardInfoCacheAdapter adapter = new CardInfoCacheAdapter();
		// int[] userIds = new int[] { 12345, 67067835, 123456789 };
		List<Integer> userIds = new ArrayList<Integer>();
		userIds.add(240857225);
		CardInfo[] res = adapter.get(userIds);
		for (CardInfo x : res) {
			System.out.println(x.privacy);
		}

		// int[] ids = new int[] { 11 };
		// adapter.remove(ids);

		CardInfo[] infos = new CardInfo[1];

		infos[0] = new CardInfo();
		infos[0].userId = 240857225;
		infos[0].address = "河北";
		infos[0].qq = "12345";
		infos[0].mobile = "1234512345";
		infos[0].email = "12345@163.com";
		infos[0].msn = "msn23456@hotmail.com";
		infos[0].extra = "测试1111111111";
		infos[0].privacy = "{h:99}";
		// boolean res1 = adapter.set(infos[0]);
		// System.out.println(res1);
		{
			CardInfo[] result = adapter.get(userIds);
			for (CardInfo x : result) {
				System.out.println(x.extra);
				System.out.println(x.address);
				System.out.println(x.privacy);
			}
		}

		System.exit(0);
	}
}
