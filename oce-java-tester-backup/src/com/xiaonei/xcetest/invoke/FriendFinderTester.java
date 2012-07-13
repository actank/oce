package com.xiaonei.xcetest.invoke;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import junit.framework.TestCase;
import mop.hi.oce.adapter.FriendFinderAdapter;
import mop.hi.oce.adapter.impl.FriendFinderAdapterImpl;
import mop.hi.oce.domain.factory.FriendFinderFactory;
import mop.hi.oce.domain.model.FriendCommon;
import mop.hi.oce.domain.model.FriendShare;

public class FriendFinderTester extends TestCase {


	private final FriendFinderAdapterImpl _adapter;

	public FriendFinderTester() {
		_adapter = new FriendFinderAdapterImpl(new com.xiaonei.xcetest.model.FriendFinderFactoryI());
	}

	@Override
	protected void setUp() throws Exception {
	}


	public void test1() {
		int id = 700000025;
		// FriendFinderChannel.getInstance().getFindFriendsManager(id)
		// .reloadCommonFriend(id);
		long startTime = System.currentTimeMillis();
		List<FriendCommon> p = _adapter.getCommonFriendsRandom(id, 100);
		for (FriendCommon a : p) {
//			if(a.getUserId()== 222479639){
//				System.err.println("NONO");
//			}
			System.out.println(a.getUserId());
			System.out.print("       ");
			for (int o : a.getShares()) {
				System.out.print(o + " ");
			}
			System.out.println();
		}
		long finishTime = System.currentTimeMillis();
		System.out.println(p.size() + "TimeCost: " + (finishTime - startTime));

	}

	public void test2() {
		int userId1 = 200353733;
		int userId2 = 202911262;
		List<FriendShare> p = _adapter
				.getSharedFriends(userId1, userId2, 0, -1);
		for (FriendShare s : p) {
			System.out.println(s.getUserId());
		}
		System.out.println("---- Count:" + p.size());
	}

	private static Map<String, String> imRebuild = new HashMap<String, String>();

	static {
		imRebuild.put("REBUILD_IMMEDIATELY", "1");
	}

	public void test3() {
		int id = 221988163;
//		id = 230659121;
		_adapter.getFindFriendsManager(id).rebuildCommonFriend(id, imRebuild);
		_adapter.getFindFriendsManager(id).reloadCommonFriend(id);
	}
	
	public void test4(){
		int id1=202911262;
		int id2=150894670;
		_adapter.removeCommonFriendWithBlock(id1, id2);
		_adapter.removeCommonFriendWithBlock(id2, id1);
	}
}
