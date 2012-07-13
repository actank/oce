package com.xiaonei.xcetest.invoke;

import junit.framework.TestCase;
import mop.hi.oce.adapter.AdapterFactory;
import mop.hi.oce.adapter.WebOnlineAdapter;
import mop.hi.oce.domain.user.UserHomeView;

public class OnlineTester extends TestCase {
	WebOnlineAdapter adapter = AdapterFactory.getWebOnlineAdapter();

	public void testLogin() {
		for (int a = 0; a < 60; a++) {
			adapter.login(a);
		}
	}

	public void testOnline() {
		System.out.println("=============Online=============");
		adapter.keepOnline(200011104);

		System.out.println(adapter.getOnlineUsers(0, -1));
		// adapter.keepOnline(200011089);

		System.out.println(adapter.getOnlineUsers(0, -1));
	}

	public void testGetStats() throws Exception {
		System.out.println("=============GetStats=============");
		int userId = 123440860;
		int[] users = { userId };
		while (true) {
			System.out.print(adapter.getOnlineStatus(users));

			UserHomeView uhv = AdapterFactory.getUserViewAdapter()
					.getUserHomeView(userId);
			// UserDesc userdesc =
			// AdapterFactory.getUserDescAdapter().get(userId);
			System.out.print(uhv.getUserOnlineStatus());
			System.out.print(uhv.getUserdesc().getLastLoginTime());
			System.out.println(uhv.getUserdesc().getLastLogoutTime());
			Thread.sleep(2000);
		}
	}

	public void testLogout() {
		System.out.println("=============Logout=============");
		for (int a = 0; a < 20; a++) {
			adapter.logout(200353733);
		}
	}

	public void testOnlineSize() throws Exception {
		// AdapterFactory.getWebOnlineAdapter().getOnlineSize();
		System.out.println(adapter.getOnlineSize());
		// System.out.println(adapter.getOnlineUsers(0,-1));
	}

}
