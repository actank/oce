package com.xiaonei.xcetest.invoke;

import java.util.List;
import java.util.Vector;

import junit.framework.TestCase;
import mop.hi.oce.adapter.AdapterFactory;
import mop.hi.oce.adapter.CountCacheAdapter;

public class CountCacheTest extends TestCase {
	CountCacheAdapter adapter = AdapterFactory.getCountCacheAdapter();

	/*
	 * public void test1() throws InterruptedException { // int id = 128487631; //
	 * adapter.incImLeavewordCount(id, 1); // Thread.sleep(1000); //
	 * System.out.println(adapter.getImLeavewordCount(id)); //
	 * adapter.incImLeavewordCount(id, 1); // Thread.sleep(1000);
	 * 
	 * System.out.println(adapter.getRegistApplyCount(128487631));
	 * adapter.incImLeavewordCount(128487631, 1);
	 * System.out.println(adapter.getImLeavewordCount(128487631)); }
	 * 
	 * public void testPoke() throws Throwable { int id = 200353733; int a =
	 * adapter.getPokeCount(id); Thread.sleep(1000); int b =
	 * adapter.getPokeCount(id); System.out.println(a + " " + b); }
	 * 
	 * public void testClassInvite() throws InterruptedException { int id =
	 * 1255; int a = adapter.getClassInviteCount(id); System.out.println(a);
	 * Thread.sleep(1000); adapter.incClassInviteCount(id, 1);
	 * Thread.sleep(1000); a = adapter.getClassInviteCount(id);
	 * System.out.println(a); }
	 */
	public void testTeamInvite() throws InterruptedException {
		int id = 1259;
		int a = adapter.getTeamInviteCount(id);
		System.out.println("TeamInviteCount for " + id + " : " + a);
		Thread.sleep(1000);
		adapter.incTeamInviteCount(id, 2);
		Thread.sleep(1000);
		a = adapter.getTeamInviteCount(id);
		System.out.println("TeamInviteCount for " + id + " : " + a);
		adapter.decTeamInviteCount(id, 1);
		Thread.sleep(1000);
		a = adapter.getTeamInviteCount(id);
		System.out.println("TeamInviteCount for " + id + " : " + a);
	}

	public void testGuestRequest() throws InterruptedException {
		int id = 1050;
		int a = adapter.getGuestRequestCount(id);
		System.out.println("GuestRequestCount for " + id + " : " + a);
		Thread.sleep(3000);
		// adapter.incGuestRequestCount(id, 2);
		Thread.sleep(3000);
		a = adapter.getGuestRequestCount(id);
		System.out.println("GuestRequestCount for " + id + " : " + a);
		// adapter.decGuestRequestCount(id, 1);
		Thread.sleep(3000);
		a = adapter.getGuestRequestCount(id);
		System.out.println("GuestRequestCount for " + id + " : " + a);
	}

	public void testReload() {
		int id = 42050380;
		adapter.reload(id);
	}

	private List<Thread> _threads = new Vector<Thread>();
	private static int i = 1;

	private synchronized int getNextInt() {
		return i += 10;
	}

	public void testTask() throws InterruptedException {
		for (int c = 0; c < 40; ++c) {
			Thread t = new Thread(new Runnable() {
				public void run() {
					while (true) {
						adapter.getAll(getNextInt());
					}
				}
			});
			t.start();
			_threads.add(t);
			System.out.println(_threads.size());
		}
		Thread.sleep(60 * 1000);
	}

	public void testGetAll() {
		int id = 700000012;
//		adapter.incGuestRequestCount(id, 10);
		System.out.println(adapter.getAll(id).getNotify());
		// adapter.incNotifyCount(id, 1);
		// System.out.println(adapter.getAll(id).getNotify());
		// System.out.println(adapter.getNotifyCount(id));
	}

	public void testInc() {
		int id = 202911262;;
		// id = 200190004;
		adapter.incNotifyCount(id, 2);
	}

	public void testRecommendFriend() {
		int id = 240548033;
		int _type = 11;
		adapter.incRecommendFriend(id, 3);
	}

	public void testintFriend() {
		int id = 200525706;
		int _type = 11;
		adapter.incRecommendFriend(id, 3);
	}

	public void testincMessage() {
		int id = 200525706;
		int _type = 5;
		adapter.incNewMsgCount(id, 3);
	}

	public void testgetRecommendFriend() {
		int id = 240548033;
		System.out.println(adapter.getRecommendFriend(id));
	}

	public void incNotify() {
		adapter.incNotifyCount(200190004, 3);
	}

	public void decNotify() {
		adapter.decNotifyCount(200190004, 3);
	}

	public void getNotify() {
		System.out.println(adapter.getNotifyCount(200190004));
	}
};
