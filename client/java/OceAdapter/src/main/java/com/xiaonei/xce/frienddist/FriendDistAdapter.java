package com.xiaonei.xce.frienddist;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Vector;
import java.util.Map.Entry;

import mop.hi.svc.model.FriendDistManagerPrx;
import mop.hi.svc.model.FriendDistManagerPrxHelper;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

public final class FriendDistAdapter<T extends FriendDistCount, W extends FriendDistWithTotal<T>>
		extends ServiceAdapter {
	private static final String endpoints = "@FriendDist";
	private FriendDistFactory<T, W> _factory;
	private Vector<FriendDistManagerPrx> managers = new Vector<FriendDistManagerPrx>();
	private Vector<FriendDistManagerPrx> managersOneway = new Vector<FriendDistManagerPrx>();

	protected FriendDistManagerPrx getFriendDistManager(int userId) {
		return locateProxy(managers, "FDM", userId, Channel.Invoke.Twoway,
				FriendDistManagerPrxHelper.class, 300);
	}

	protected FriendDistManagerPrx getFriendDistManagerOneway(int userId) {
		return locateProxy(managersOneway, "FDM", userId,
				Channel.Invoke.Oneway, FriendDistManagerPrxHelper.class, 300);
	}

	public FriendDistAdapter(FriendDistFactory<T, W> factory) {
		super(endpoints, 10, Channel.newOceChannel(endpoints));
		_factory = factory;
	}

	public W getFriendDistWithTotal(int userId) {
/*
		try {
			mop.hi.svc.model.FriendDistWithTotal ffc = getFriendDistManager(
					userId).getFriendDistWithTotal(userId);
			W result = _factory.createFriendDistWithTotal();
			result.setBase(ffc, _factory);
			return result;
		} catch (Ice.TimeoutException e) {
			System.err.println("getFriendDistWithTotal timeout " + userId);
		}
		return null;
*/
		return null;
	}

	public int getFriendDistByUniversity(int userId, int universityId) {
/*
		try {
			return getFriendDistManager(userId).getFriendDistByUniv(userId,
					universityId);
		} catch (Ice.TimeoutException e) {
			System.err.println("getFriendDistByUniversity timeout " + userId);
		}
		return 0;
*/
		return 0;
	}

	public List<T> getFriendDistListAll(int userId) {
/*
		List<T> result = new LinkedList<T>();
		try {
			Map<Integer, Integer> univ2count = getFriendDistManager(userId)
					.getFriendDist(userId);
			UNIVLOOP:
			for (Entry<Integer, Integer> entry : univ2count.entrySet()) {
				int univId = entry.getKey();
				int count = entry.getValue();
				for (int index = 0; index < result.size(); ++index) {
					if (result.get(index).getCount() < count) {
						T obj = _factory.createFriendDistCount();
						obj.setBase(univId, count);
						result.add(index, obj);
						continue UNIVLOOP;
					}
				}
				T obj = _factory.createFriendDistCount();
				obj.setBase(univId, count);
				result.add(obj);
			}
		} catch (Ice.TimeoutException e) {
			System.err.println("getFriendDistByUniversity timeout " + userId);
		}
		return result;
*/
		return new LinkedList<T>();
	}

	public void rebuildFriendDist(int userId) {
/*
		getFriendDistManagerOneway(userId).rebuildFriendDist(userId);
*/
	}
}
