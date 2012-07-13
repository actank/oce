package com.xiaonei.xcetest.invoke;

import junit.framework.TestCase;
import mop.hi.oce.adapter.impl.BuddyCoreAdapterImpl;
import mop.hi.oce.adapter.impl.UserAdapterImpl;
import mop.hi.oce.adapter.impl.UserBasicAdapterImpl;
import mop.hi.oce.adapter.impl.UserViewAdapterImpl;
import mop.hi.oce.domain.buddy.BuddyApply;
import mop.hi.oce.domain.buddy.BuddyApplyWithInfo;
import mop.hi.oce.domain.user.User;
import mop.hi.svc.model.UserException;

public class UserTester extends TestCase {
	public UserAdapterImpl adapter = new UserAdapterImpl();

	public void testGetUser() throws Exception {
		System.out.println(adapter.get(241644435).getSelected());
		System.out.println(adapter.get(252937970).getSelected());
	}

	public void testHas() throws UserException{
		adapter.getUserManager('1').setUserIdByEmail("13581774429", -1);
		System.out.println(adapter.getUserManager('1').getUserIdByEmail("13581774429"));
	}
	
	public void testGetUserBasic() throws Exception {
		System.out.println(new UserBasicAdapterImpl().get(243368827));
	}

	public void testGetUserView() throws Exception {
		System.out.println(new UserViewAdapterImpl().getUserHomeView(80578957));
	}

	public void testGetEmail() throws Exception {
		System.out.println(adapter.get("15935405447"));
	}

	public void testEmail() {
		String email = "15935405447";
		System.out.println((int) email.charAt(0));
	}

	public void testUserConfig() throws UserException {
	}

	public void testReloadUser() {
	}

	public static void main(String[] args) throws Exception {
		UserTester t = new UserTester();
		t.testGetUser();
		while (true) {
			try {
				Thread.sleep(60 * 1000);
			} catch (Exception e) {

			}
		}
	}

	public void setPasswd() throws Exception {
		for (int i = 21; i <= 50; i++) {
			String email = "gm_" + i + "@dudu-inc.com";
			int id = adapter.get(email).getId();
			User user = new User();
			user.setId(id);
			user.setPassword("b500452c3d170a79e1970347bb763175");
			adapter.set(user);
		}
	}

	public void testRelation() throws Exception {
		int applicant = 245877297;
		int acceptor = 251497247;
		for (int i = 0; i < 10; ++i) {
			BuddyApplyWithInfo request = new BuddyApplyWithInfo(applicant,
					acceptor);

			new BuddyCoreAdapterImpl().addApply(request);
			// Thread.sleep(1000);

			assertTrue(new BuddyCoreAdapterImpl().getRelation(applicant,
					acceptor).isApply());
			assertFalse(new BuddyCoreAdapterImpl().getRelation(acceptor,
					applicant).isApply());

			new BuddyCoreAdapterImpl().denyApply(request);
			// Thread.sleep(1000);

			assertFalse(new BuddyCoreAdapterImpl().getRelation(applicant,
					acceptor).isApply());
			assertFalse(new BuddyCoreAdapterImpl().getRelation(acceptor,
					applicant).isApply());
		}
	}
}
