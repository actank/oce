package com.xiaonei.xcetest.invoke;

import java.util.Date;

import junit.framework.TestCase;
import mop.hi.oce.adapter.AdapterFactory;

public class PassportTester extends TestCase {
	public void testPassport() throws Exception {
		//String ticket = AdapterFactory.getPassportAdapter().createTicket(142443436, false);
		String ticket = "6649e25f614b368c4c92fbbc94c528d08";
		System.out.println(AdapterFactory.getPassportAdapter().verifyTicket(ticket));
//		while (true) {
//			super.assertEquals(227366242, AdapterFactory.getPassportAdapter().verifyTicket(ticket).intValue());
//			System.out.println(227366242 + " -> " + ticket);
//			Thread.sleep(1000);
//		}
//		String[] items = { "7ed65f6e3f35a50c42967fd1c5869b60|hostid:228669683" };
//		for (int i = 0; i < items.length; ++i) {
//			String item = items[i];
//			String userId = item.substring(item.indexOf(":") + 1);
//			String ticket = item.substring(0, item.indexOf("|"));
//			System.out.println("Testing " + ticket + " -> " + userId + " ? ");
//			System.out.println("UserId about " + ticket + " -> "
//					+ AdapterFactory.getPassportAdapter().verifyTicket(ticket));
//			System.out.println("Ticket about " + userId + " -> "
//					+ AdapterFactory.getPassportAdapter().queryTicket(Integer.parseInt(userId)));
//			System.out.println("--------------------------------------------------------\n");
//		}
		// System.out.println("UserId: "
		// + AdapterFactory.getPassportAdapter().verifyTicket(
		// "802fc7987811434ece50694e3c8595dd"));
		// System.out.println("Ticket: "
		// + AdapterFactory.getPassportAdapter().queryTicket(200556978));
	}

	public void testCreatePersistent() {
		int userId = (int) Math.round(Math.random() * Integer.MAX_VALUE);
		System.out.println(AdapterFactory.getPassportAdapter().createTicket(userId, true));
	}

	public void testCreateNormal() {
		int userId = (int) Math.round(Math.random() * Integer.MAX_VALUE);
		System.out.println(AdapterFactory.getPassportAdapter().createTicket(userId, false));
	}

	public void testVerify() {
		// String ticket=
		// AdapterFactory.getPassportAdapter().createTicket(userId, true);
		String ticket = "a8ad28e63a3a26ce1f8d2e298c3c59c2";
		// while(true){
		System.out.println(AdapterFactory.getPassportAdapter().verifyTicket(ticket));
		// }

	}

	public void testQuery(){
		int id = 79512511;
		System.out.println(AdapterFactory.getPassportAdapter().queryTicket(id));
	}
	public void destoryTicket() {
		String ticket = "8fc700dd6681809c8e4623f960121acd";
		AdapterFactory.getPassportAdapter().destroyTicket(ticket);
	}

	public void impersistentTicket() {
		String ticket = "4b568fef90ddd917de5c9bca34966446";
		AdapterFactory.getPassportAdapter().impersistence(ticket);
	}

	public static void main(String[] args) throws Exception {
		for (int i = 0;; i++) {
			Thread.sleep(1000);
			int userId = 10020 + i;
			try {
				String ticket = AdapterFactory.getPassportAdapter().createTicket(userId, false);
				System.out.println(new Date() + "\t" + userId + "\t"
						+ AdapterFactory.getPassportAdapter().verifyTicket(ticket));
			} catch (Ice.TimeoutException e) {
				System.out.println(new Date() + "\t" + userId + "\tTimeout");
			} catch (Exception e) {
				System.out.println(new Date() + "\t" + userId + "\tException");
			}
		}
	}
}
