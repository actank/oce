package com.xiaonei.xcetest.tester;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import xce.usercache.UserCacheManagerPrx;
import xce.usercache.UserCacheManagerPrxHelper;
import xce.util.channel.Channel;
import xce.util.channel.CoreChannel;
import xce.util.storm.Topic;

import junit.framework.TestCase;

import Ice.Endpoint;

import com.xiaonei.xce.usercache.DefaultUserCache;
import com.xiaonei.xce.usercache.DefaultUserCacheFactory;
import com.xiaonei.xce.usercache.UserCacheAdapter;
import com.xiaonei.xce.usercache.UserCache;

public class UserCacheTester extends TestCase {

	private UserCacheAdapter<DefaultUserCache> _uca = new UserCacheAdapter<DefaultUserCache>(
			new DefaultUserCacheFactory());

	public void testGetUserCache() {
		int userId = 200000002;
		UserCache result = _uca.getUserCache(userId);
		System.out.println(result);
	}

	public void testGetUserCacheList() {
		List<Integer> userIds = new ArrayList<Integer>();
		userIds.add(700000027);
		while (true) {
			List<DefaultUserCache> results = _uca.getUserCacheList(userIds);

			for (UserCache i : results) {
				System.out.println(i.isOnline());
			}
		}
	}

	public void tesReplicaEndpoints() {
		Channel channel = CoreChannel.newOceChannel("ReplicaEndpoints");

		UserCacheManagerPrx prx = UserCacheManagerPrxHelper.checkedCast(channel
				.getCommunicator().stringToProxy("M@UserCache0"));
		System.out.println(prx.ice_getEndpointSelection());
		prx.getUserCache(202911262);
		Endpoint[] es = prx.ice_getEndpoints();
		System.out.println("Size:" + es.length);
		for (Endpoint e : es) {
			System.out.println(e.toString());
		}

		IceGrid.QueryPrx query = IceGrid.QueryPrxHelper.checkedCast(channel
				.getCommunicator().stringToProxy("XiaoNei/Query"));
		Ice.ObjectPrx[] prxs = query.findAllReplicas(prx);
		for (Ice.ObjectPrx p : prxs) {
			System.out.println(p);
		}
	}

	public void testPublish() {
		Topic topic = new Topic("UserStorm", "UserCache");
		UserCacheManagerPrx prx = UserCacheManagerPrxHelper.uncheckedCast(topic
				.getPublisher());
		// prx.load(new int[] { 202911262, 200353733 });
		Map<Integer, Integer> map = new HashMap<Integer, Integer>();
		map.put(200000780, 1);
		prx.setOnline(map); 
	}
}
