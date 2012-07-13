package com.xiaonei.xce.cardfacade;

import java.util.Map;

import xce.cardcache.CardRelationCachePrx;
import xce.cardcache.CardRelationCachePrxHelper;
import xce.cardcache.RelationInfo;
import xce.clusterstate.ReplicatedClusterAdapter;
import java.util.Date;

public class CardRelationCacheAdapter extends ReplicatedClusterAdapter {

	private static final String ctr_endpoints = "ControllerCardRelationCache";
	private static int _interval = 120;

	public CardRelationCacheAdapter() {
		super(ctr_endpoints, _interval);
		// TODO Auto-generated constructor stub
	}

	public CardRelationCachePrx getCardRelationCacheManager(int owner) {

		Ice.ObjectPrx prx0 = getProxy(owner, 1500);

		// System.out.println(prx0.ice_toString());
		if (prx0 != null) {
			CardRelationCachePrx prx = CardRelationCachePrxHelper
					.uncheckedCast(prx0.ice_timeout(1500).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	public int[] get(int userId, int begin, int limit, int state) {
		return getCardRelationCacheManager(userId).get(userId, begin, limit,
				state);
	}

	public int set(RelationInfo info) {
		return getCardRelationCacheManager(0).set(info);
	}

	public void setBatch(RelationInfo[] infos) {
		getCardRelationCacheManager(0).setBatch(infos);
	}

	public int getSize(int userId, int state) {
		return getCardRelationCacheManager(userId).getSize(userId, state);
	}

	public int getRelation(int userId, int friendId) {
		return (getCardRelationCacheManager(userId)).getRelation(userId,
				friendId);
	}

	public int[] getReceive(int userId, int begin, int limit) {
		return getCardRelationCacheManager(userId).getReceive(userId, begin,
				limit);
	}

	public boolean ignoreReceive(int userId, int[] friendIds) {
		return getCardRelationCacheManager(userId).ignoreReceive(userId,
				friendIds);
	}

	public boolean ignoreAll(int userId) {
		return getCardRelationCacheManager(userId).ignoreAll(userId);
	}

	public Map<Integer, int[]> getAccept(long time, int lastId, int limit) {
		int time1 = (int) (time / 1000);
		return getCardRelationCacheManager(0).getAccept(time1, lastId, limit);
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		CardRelationCacheAdapter adapter = new CardRelationCacheAdapter();
		int userId = 1001;
		int state = 0;

		RelationInfo[] infos = new RelationInfo[4];
		infos[0] = new RelationInfo();
		infos[0].userId = 10002;
		infos[0].friendId = 2002;
		infos[0].requestFlag = 2;
		infos[1] = new RelationInfo();
		infos[1].userId = 21002;
		infos[1].friendId = 2002;
		infos[1].requestFlag = 2;
		infos[2] = new RelationInfo();
		infos[2].userId = 31002;
		infos[2].friendId = 2002;
		infos[2].requestFlag = 2;
		infos[3] = new RelationInfo();
		infos[3].userId = 41002;
		infos[3].friendId = 3002;
		infos[3].requestFlag = 2;
		//
		RelationInfo info = new RelationInfo();
		info.userId = 61001;
		info.friendId = 2002;
		info.requestFlag = 0;

		// System.out.println("set");
		// int x = adapter.set(info);
		// System.out.println(x);
		// System.out.println("setBatch");
		// adapter.setBatch(infos);
		// System.out.println(adapter.getSize(userId, state));

		// int[] fids = { 10001, 2001, 61001 };
		// adapter.ignoreReceive(2002, fids);

		// System.out.println(adapter.ignoreAll(2002));

		// System.out.println("get");
		// int xs[] = adapter.get(2002, 0, -1, 2);
		// for (int i : xs) {
		// System.out.println(i);
		// }
		// System.out.println("getRelation");
		// System.out.println(adapter.getRelation(10001, 2002));
		//
		// System.out.println("getReceive");
		// int[] uid = adapter.getReceive(2002, 0, -1);
		// for (int i : uid) {
		// System.out.println(i);
		// }
		//
		// System.out.println("getSize " + adapter.getSize(2002, 1));
		//
		System.out.println("getAccept");
		long t1 = System.currentTimeMillis();
		System.out.println(t1);
		Map<Integer, int[]> acceptInfo = adapter.getAccept(t1, 1, 8000);
		System.out.println("size:" + acceptInfo.size());
		for (Map.Entry<Integer, int[]> entry : acceptInfo.entrySet()) {
			System.out.print("userid:" + entry.getKey() + ":");
			for (int fid : entry.getValue()) {
				System.out.print(fid + " ");
			}
			System.out.println("");
		}

		// System.out.println(adapter.ignoreAll(3002));
		// System.out.println(adapter.getRelation(308553345, 224131964));

		System.exit(0);
	}
}
