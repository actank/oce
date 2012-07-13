package com.xiaonei.xce.cardfacade;

import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import xce.cardcache.PartNewInfo;
import xce.cardsearch.CardSearcherPrx;
import xce.cardsearch.CardSearcherPrxHelper;
import xce.cardsearch.FriendCardList;

import xce.clusterstate.ReplicatedClusterAdapter;

public class CardSearcherAdapter extends ReplicatedClusterAdapter {

	private static final String ctr_endpoints = "ControllerCardSearcher";
	private static int _interval = 120;

	public CardSearcherAdapter() {
		super(ctr_endpoints, _interval);
		// TODO Auto-generated constructor stub
	}

	public CardSearcherPrx getCardSearcherManager(int owner) {

		Ice.ObjectPrx prx0 = getProxy(owner, 500);

		// System.out.println(prx0.ice_toString());
		if (prx0 != null) {
			CardSearcherPrx prx = CardSearcherPrxHelper.uncheckedCast(prx0
					.ice_timeout(500).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	public Map<Integer, Integer> search(int userId, String query, int limit) {
		return getCardSearcherManager(userId).search(userId, query, limit);
	}

	public FriendCardList getFriendCard(int userId) {
		return getCardSearcherManager(userId).getFriendList(userId);
	}

	public List<Integer> getUnExchangeList(int userId) {
		return getCardSearcherManager(userId).getUnExchangeList(userId);
	}

	public PartNewInfo[] getFriendCardNew(int userId) {
		return getCardSearcherManager(userId).getFriendsList(userId);
	}
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		CardSearcherAdapter adapter = new CardSearcherAdapter();
		String query = "李";
		int limit = 10;
		Map<Integer, Integer> res = adapter.search(67067835, query, -1);
		for (Entry<Integer, Integer> entry : res.entrySet()) {
			System.out.println(entry.getKey() + " " + entry.getValue());
		}
		
		
		// [] idResultDatas = adapter.search(220840375, query, limit);
		// for (ResultData x : ids) {
		// System.out.println(x.userId + " " + x.type);
		// }
		// System.out.println();

		System.exit(0);
	}
}
